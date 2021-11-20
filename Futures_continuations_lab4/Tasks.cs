using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Futures_continuations_lab4
{
    public static class Tasks
    {
        private static List<string> hosts;

        public static void run(List<string> hostsList)
        {
            hosts = hostsList;
            List<Task> tasks = new List<Task>();
            foreach (var host in hosts)
            {
                tasks.Add(Task.Factory.StartNew(StartTask, host));
            }
            Task.WaitAll(tasks.ToArray());
            Thread.Sleep(10000);
            Console.WriteLine("Done !");
        }

        private static void StartTask(object hostObj)
        {
            var host = (string)hostObj;
            Console.WriteLine("Start {0}", host);
            string hostname = host.Split('/')[0];
            
            // get the DNS IP address associated with the host.
            var ipAddress = Dns.GetHostEntry(hostname).AddressList[0];

            // create the endpoint for the host on HTTP port
            IPEndPoint hostEndPoint = new IPEndPoint(ipAddress, HttpUtils.HTTP_PORT);

            // create a socket for the client
            Socket clientSocket = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            // store the state of the connection
            State state = new State
            {
                socket = clientSocket,
                hostname = hostname,
                endpoint = host.Contains('/') ? host.Substring(host.IndexOf('/')) : "/",
                remoteEndpoint = hostEndPoint
            };

            // connect to the host endpoint
            Task<Socket> futureConnect = ConnectWrapper(state);
            state.socket = futureConnect.Result;

            Console.WriteLine(state.socket.Connected);

            // send the request
            Task<int> futureSend = SendGetRequest(state);

        }
        private static Task<Socket> ConnectWrapper(State state)
        {
            TaskCompletionSource<Socket> promise = new TaskCompletionSource<Socket>();

            state.socket.BeginConnect(state.remoteEndpoint, 
                (IAsyncResult ar) => {
                    state.socket.EndConnect(ar);
                    promise.SetResult(state.socket);
                }
            , state);

            Console.WriteLine("Socket connected to {0} IP: {1}", state.hostname, state.endpoint);

            return promise.Task;
        }

        private static Task<int> SendGetRequest(State state)
        {
            // create the HTTP GET request
            string getRequestString = HttpUtils.createGetRequestString(state.hostname, state.endpoint);
            byte[] getRequestBytes = Encoding.ASCII.GetBytes(getRequestString);

            TaskCompletionSource<int> promise = new TaskCompletionSource<int>();
            
            Socket clientSocket = state.socket;
            clientSocket.BeginSend(getRequestBytes, 0, getRequestBytes.Length, 0, 
                (IAsyncResult ar) => promise.SetResult(clientSocket.EndSend(ar)), state);

            Console.WriteLine("Sent {0} bytes to {1}.", promise.Task.Result, state.hostname);

            return promise.Task;
        }

        private static void ReceiveResponseWrapper(Task<int> future)
        {
            Console.WriteLine("Received");
        }

        private static void ReceiveResponse()
        {

        }

        private static void ReceivingContent(IAsyncResult ar)
        {
            // get the state of the connection
            State state = (State)ar.AsyncState;
            var hostname = state.hostname;

            // get the socket
            Socket clientSocket = state.socket;

            // end the receive call
            var bytesReceived = clientSocket.EndReceive(ar);
            state.responseContent.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesReceived));

            if (!HttpUtils.hasEntireResponseHeader(state.responseContent.ToString()))
            {
                // get the rest of the response header
                clientSocket.BeginReceive(state.buffer, 0, State.BUFFER_SIZE, 0, ReceivingContent, state);
            }
            else
            {
                // response header was entirely received
                int contentLength = HttpUtils.getContentLengthFromHeader(state.responseContent.ToString());
                Console.WriteLine("Received {0} bytes from {1} -> content-length = {2}", bytesReceived, hostname, contentLength);
            }

            clientSocket.Close();
        }
    }
}

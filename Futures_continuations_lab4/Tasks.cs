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
                // start a task for each host
                tasks.Add(Task.Factory.StartNew(StartTask, host));
            }
            // wait for all tasks to complete
            Task.WaitAll(tasks.ToArray());
            Thread.Sleep(60000);
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
            
            // update the socket (now it is connected) 
            state.socket = futureConnect.Result;
            Console.WriteLine("Socket connected to {0} IP: {1}", state.hostname, state.socket.RemoteEndPoint);

            // send the GET request
            Task<int> futureSend = SendGetRequest(state);

            // receive the response from the host
            futureSend.ContinueWith((Task<int> f) => ReceiveResponseWrapper(state, f));

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

            return promise.Task;
        }

        private static Task<int> SendGetRequest(State state)
        {
            // create the HTTP GET request
            string getRequestString = HttpUtils.createGetRequestString(state.hostname, state.endpoint);
            byte[] getRequestBytes = Encoding.ASCII.GetBytes(getRequestString);

            TaskCompletionSource<int> promise = new TaskCompletionSource<int>();

            // send the GET request
            state.socket.BeginSend(getRequestBytes, 0, getRequestBytes.Length, 0, 
                (IAsyncResult ar) => promise.SetResult(state.socket.EndSend(ar)), state);

            Console.WriteLine("Sent {0} bytes to {1}.", promise.Task.Result, state.hostname);

            return promise.Task;
        }

        private static void ReceiveResponseWrapper(State state, Task<int> future)
        {
            TaskCompletionSource<int> promise = new TaskCompletionSource<int>();

            // receive a fixed-length response
            state.socket.BeginReceive(state.buffer, 0, State.BUFFER_SIZE, 0, 
                (IAsyncResult ar) => promise.SetResult(state.socket.EndReceive(ar)), state);

            Task<int> futureReceive = promise.Task;

            // attempt to receive the remaining of the response (if needed)
            futureReceive.ContinueWith((Task<int> f) => ReceivingContent(state, f));
        }

        private static void ReceivingContent(State state, Task<int> future)
        {
            // get the number of received bytes so far
            int bytesReceived = future.Result;

            // save the received content
            state.responseContent.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesReceived));
            Console.WriteLine("Received {0} bytes from {1}", bytesReceived, state.hostname);

            if (!HttpUtils.hasEntireResponseHeader(state.responseContent.ToString()))
            {
                // get the rest of the response header
                ReceiveResponseWrapper(state, future);
            }
            else
            {
                // response header was entirely received
                int contentLength = HttpUtils.getContentLengthFromHeader(state.responseContent.ToString());
                Console.WriteLine("Received {0} bytes from {1} -> content-length = {2}", bytesReceived, state.hostname, contentLength);
            }

            state.socket.Close();
        }
    }
}

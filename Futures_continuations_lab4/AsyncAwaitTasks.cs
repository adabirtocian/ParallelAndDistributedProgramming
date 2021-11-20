using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
namespace Futures_continuations_lab4
{
    public static class AsyncAwaitTasks
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

        private static async void StartTask(object hostObj)
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
            Socket socket = await ConnectWrapper(state);
            
            // update the socket (now it is connected) 
            state.socket = socket;
            Console.WriteLine("Socket connected to {0} IP: {1}", state.hostname, state.socket.RemoteEndPoint);

            // send the GET request
            int bytesSent = await SendGetRequest(state);
            Console.WriteLine("Sent {0} bytes to {1}.", bytesSent, state.hostname);

            // receive the response from the host
            int bytesReceived = await ReceiveResponseWrapper(state);
            int contentLength = await ReceivingContent(state, bytesReceived);

            Console.WriteLine("Content-length = {0}", contentLength);

            state.socket.Close();

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

            return promise.Task;
        }

        private static Task<int> ReceiveResponseWrapper(State state)
        {
            TaskCompletionSource<int> promise = new TaskCompletionSource<int>();

            // receive a fixed-length response
            state.socket.BeginReceive(state.buffer, 0, State.BUFFER_SIZE, 0,
                (IAsyncResult ar) => promise.SetResult(state.socket.EndReceive(ar)), state);

            return promise.Task;
        }

        private static async Task<int> ReceivingContent(State state, int bytesReceived)
        {
            // save the received content
            state.responseContent.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesReceived));
            Console.WriteLine("Received {0} bytes from {1}", bytesReceived, state.hostname);

            while (!HttpUtils.hasEntireResponseHeader(state.responseContent.ToString()))
            {
                // get the rest of the response header
                bytesReceived = await ReceiveResponseWrapper(state);
                state.responseContent.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesReceived));
                Console.WriteLine("Received {0} bytes from {1}", bytesReceived, state.hostname);
            }
            
            // response header was entirely received
            int contentLength = HttpUtils.getContentLengthFromHeader(state.responseContent.ToString());

            return contentLength;
        }
    }
}

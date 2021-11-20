using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace Futures_continuations_lab4
{
    public static class Callbacks
    {
        private static List<string> hosts;

        public static void run(List<string> hostsList)
        {
            hosts = hostsList;
            foreach(var host in hosts)
            {
                Start(host);
             
            }
            Thread.Sleep(30000);
        }

        private static void Start(string host)
        {
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
                endpoint = host.Contains('/') ? host.Substring(host.IndexOf('/')) : "/"
            };

            // connect to the host endpoint
            clientSocket.BeginConnect(hostEndPoint, (IAsyncResult ar) => SendGetRequest(ar), state);
        }

        private static void SendGetRequest(IAsyncResult ar)
        {
            // get the state of the connection
            State state = (State) ar.AsyncState;
            string hostname = state.hostname;
            string endpoint = state.endpoint;

            // get the socket on which the connection started in BeginConnect call
            Socket clientSocket = state.socket;

            // end the connection on the socket
            clientSocket.EndConnect(ar);
            Console.WriteLine("Socket connected to {0} IP: {1}", hostname, clientSocket.RemoteEndPoint);

            // create the HTTP GET request
            string getRequestString = HttpUtils.createGetRequestString(hostname, endpoint);
            byte[] getRequestBytes = Encoding.ASCII.GetBytes(getRequestString);

            // send the request
            clientSocket.BeginSend(getRequestBytes, 0, getRequestBytes.Length, 0, (IAsyncResult ar) => ReceiveResponse(ar), state);

        }

        private static void ReceiveResponse(IAsyncResult ar)
        {
            // get the state of the connection
            State state = (State)ar.AsyncState;
            var hostname = state.hostname;

            // get the socket
            Socket clientSocket = state.socket;

            // end sending the data to host
            var bytesSent = clientSocket.EndSend(ar);
            Console.WriteLine("Sent {0} bytes to {1}.", bytesSent, hostname);

            // receive the response
            clientSocket.BeginReceive(state.buffer, 0, State.BUFFER_SIZE, 0, (IAsyncResult ar) => ReceivingContent(ar), state);
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

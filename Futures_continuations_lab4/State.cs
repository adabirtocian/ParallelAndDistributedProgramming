using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace Futures_continuations_lab4
{
    public class State
    {
        public Socket socket = null;

        public string hostname;

        public string endpoint;

        // size of receive buffer
        public const int BUFFER_SIZE = 512;

        // received response header  
        public byte[] buffer = new byte[BUFFER_SIZE];

        // received response data 
        public StringBuilder responseContent = new StringBuilder();

    }
}

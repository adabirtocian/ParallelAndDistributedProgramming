// A server that accepts pairs of numbers, transmitted as text and separated by whitespace, and sends back their sums

using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;

namespace Futures_continuations_lab4
{
    public static class Program
    {
        // adding 3 hosts, each returning a response in different format:
        private static readonly List<string> HOSTS = new List<string> {
            "www.cs.ubbcluj.ro/~rlupsa/edu/pdp/",
            "www.google.com",
            "www.scs.ubbcluj.ro"
        };

        public static void Main(string[] args)
        {
            //Callbacks.run(HOSTS);
            Tasks.run(HOSTS);
        }
    }
}

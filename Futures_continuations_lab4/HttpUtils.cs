using System;
using System.Collections.Generic;
using System.Text;

namespace Futures_continuations_lab4
{
    public class HttpUtils
    {
        public static int HTTP_PORT = 80;

        public static string createGetRequestString(string hostname, string endpoint)
        {
            return "GET " + endpoint + " HTTP/1.1\r\n" +
                   "Host: " + hostname + "\r\n" +
                   "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36\r\n" +
                   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,#1#*;q=0.8\r\n" +
                   "Accept-Language: en-US,en;q=0.9,ro;q=0.8\r\n" +
                   "Accept-Encoding: gzip, deflate\r\n" +
                   "Connection: keep-alive\r\n" +
                   "Upgrade-Insecure-Requests: 1\r\n" +
                   "Pragma: no-cache\r\n" +
                   "Cache-Control: no-cache\r\n" +
                   "Content-Length: 0\r\n\r\n";
        }

        public static bool hasEntireResponseHeader(string response)
        {
            // response header ends with 2 empty lines
            return response.Contains("\r\n\r\n");
        }

        public static int getContentLengthFromHeader(string response)
        {

            var lines = response.Split('\n', '\r');
            foreach(var responseLine in lines)
            {
                var pair = responseLine.Split(':');

                if(pair[0] == "Content-Length")
                {
                    return int.Parse(pair[1]);
                }
            }

            return 0;
        }
    }
}

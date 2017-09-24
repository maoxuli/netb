/*
 * Copyright (C) 2013, Maoxu Li. Email: maoxu@lebula.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TcpSocket.hpp"
#include "HttpMessage.hpp"

// HTTP client
int main(const int argc, char* argv[])
{
    const char* host = nullptr;
    unsigned short port = 8080; // By default 8080
    assert(argc >= 2);
    if(argc == 2) // httpc 8090
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = (unsigned short)n;
        }
    }
    else if(argc == 3) // httpc 192.168.1.5 8090
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = (unsigned short)n;
        }
    }

    netb::TcpSocket client;
    if(!client.Connect(netb::SocketAddress(host, port), nullptr))
    {
        std::cout << "HTTP client failed to connect: " << host << ":" << port << "\n";
        return 0;
    }

    // Send out a request
    netb::HttpRequest request("GET", "google.com");
    netb::StreamBuffer buf;
    if(!request.ToBuffer(&buf))
    {
        std::cout << "Packing request to buffer failed.\n";
        return 0;
    }
    if(client.Send(buf) <= 0)
    {
        std::cout << "Send request failed.\n";
        return 0;
    }
    std::cout << "Request: " << request.String();

    // Receive response
    buf.Clear();
    if(client.Receive(&buf) <= 0) // receive with timeout
    {
        std::cout << "Receive response failed.\n";
        return 0;
    }
    netb::HttpResponse response;
    if(!response.FromBuffer(&buf))
    {
        std::cout << "Unpacking response from buffer failed.\n";
    }
    std::cout << "Response: " << response.String();
    return 0;
}

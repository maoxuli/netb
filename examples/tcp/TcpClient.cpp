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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Socket.hpp"

// TCP client
int main(const int argc, char* argv[])
{
    // Service host and port, by default local and 9000
    // Given by: tcpc 127.0.0.1 9000
    std::string host;
    unsigned short port = 9000;
    if(argc == 2) // tcpc 9001
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    else if(argc == 3) // tcpc 192.168.1.1 9001
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    // Check return error
    netb::Error e;
    // Open TCP socket
    netb::Socket s(AF_INET, SOCK_STREAM, IPPROTO_TCP, &e);
    if(!s.Valid() && e)
    {
        std::cout << "Open socket failed: " << e.ToString() << "\n";
        return -1;
    }
    // Connect
    if(!s.Connect(netb::SocketAddress(host, port), &e))
    {
        std::cout << "Connect faild: " << e.ToString() << "\n";
    }
    return 0;
}

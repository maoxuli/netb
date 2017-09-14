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

using namespace netb;

// UDP echo client
int main(const int argc, char* argv[])
{
    // Service host and port, by default local and 9000
    // Given by: udpc 127.0.0.1 9000
    std::string host;
    unsigned short port = 9000;
    if(argc == 2) // udpc 9001
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    else if(argc == 3) // udpc 192.168.1.1 9001
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    // UDP echo client
    // Error handling with return values and error object
    Error e;
    Socket udpc;
    if(!udpc.Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &e))
    {
        std::cout << e.Report() << std::endl;
        return 0;
    }
    // I/O
    SocketAddress addr(host, port, AF_INET);
    std::string msg = "Hello";
    size_t len = msg.length() * 2;
    char* buf = new char[len];
    ssize_t ret = 0;
    if((ret = udpc.SendTo(msg.data(), msg.length(), &addr, 0, &e)) <= 0 ||
       (ret = udpc.ReceiveFrom(buf, len, &addr, 0, &e)) <= 0)
    {
        std::cout << e.Report() << std::endl;
    }
    else
    {
        std::cout << "Received [" << ret << "][" << std::string(buf, ret) << "]" << std::endl;
    }
    delete [] buf;
    return 0;
}

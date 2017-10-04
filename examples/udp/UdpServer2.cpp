/*
 * Copyright (C) 2013, Maoxu Li. http://maoxuli.com/dev
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

// UDP echo server
int main(const int argc, char* argv[])
{
    // Service port, default 9000
    // Given port: udps 9001
    unsigned short port = 9000;
    if(argc == 2)
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }
    // UDP echo server
    // Error handling with return values and error object
    Error e;
    Socket udps;
    if(!udps.Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &e) || 
       !udps.Bind(SocketAddress(port, AF_INET), &e))
    {
        std::cout << e.Report() << std::endl;
        return 0;
    }
    std::cout << "Opened on: " << udps.Address().String() << std::endl;
    // I/O
    SocketAddress addr;
    char* buf = new char[2048];
    ssize_t ret;
    while((ret = udps.ReceiveFrom(buf, 2048, &addr)) > 0)
    {
        if((ret = udps.SendTo(buf, ret, addr)) > 0)
        {
            std::cout << "Echo [" << ret << "][" << addr.String() << "]" << std::endl;
        }
    }
    delete [] buf;
    return 0;
}

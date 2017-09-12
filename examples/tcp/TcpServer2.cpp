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

#include "TcpAcceptor.hpp"

// TCP server
int main(const int argc, char* argv[])
{
    // Service port, default 9000
    // Given port: tcps 9001
    unsigned short port = 9000;
    if(argc == 2)
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }
    netb::Error e;
    // Open a TCP socket
    netb::TcpAcceptor acceptor(netb::SocketAddress(port, AF_INET, NULL));
    if(!acceptor.Open(&e))
    {
        std::cout << "Acceptor open failed: " << e.ToString() << "\n";
        return -1;
    }
    std::cout << "Open at: " << acceptor.Address().ToString() << "\n";
    // Accept incomming connections
    while(true)
    {
        netb::SOCKET in = acceptor.Accept(nullptr, &e);
        if(in == netb::INVALID_SOCKET && e)
        {
            std::cout << "Accept failed. " << e.ToString() << "]\n";
        }
        std::cout << "Accept socket: " << in << "\n";
    }
    return 0;
}

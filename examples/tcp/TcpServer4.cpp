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
#include "TcpSocket.hpp"

using namespace netb;

// TCP echo server
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
    // TCP echo server
    // Error handling with return values and error object
    Error e;
    TcpAcceptor tcps(SocketAddress(port, AF_INET, nullptr));
    if(!tcps.Open(&e))
    {
        std::cout << "Error: " << e.Report() << std::endl;
        return 0;
    }

    TcpSocket conn;
    char* buf = new char[2048];
    while(conn.Connected(tcps.Accept(&e), nullptr, &e))
    {
        int ret;
        while((ret = conn.Receive(buf, 2048)) > 0)
        {
            if(conn.Send(buf, ret) > 0)
            {
                std::cout << "Receive and send back: " << ret << " bytes.\n";
            }
        }
    }
    delete [] buf;
    std::cout << "Error: " << e.Report() << std::endl;
    return 0;
}

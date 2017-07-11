/*
 * Copyright (C) 2017, Maoxu Li. Email: maoxu@lebula.com
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
#include <iostream>
#include <cassert>

// RFC 862
// TCP listen on port 7
class EchoServer 
{
public: 
    EchoServer(unsigned short port = 7)  // By default, echo service on port 7
    : mSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
    , mPort(port)
    {
        assert(mSocket.Valid());
        mSocket.Block(true);
    }

    ~EchoServer() 
    {

    }

    bool Open()
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = PF_INET;
        addr.sin_port = htons(mPort);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(mSocket.Bind((sockaddr*)&addr, sizeof(addr)) && mSocket.Listen())
        {
            std::cout << "EchoSever opened on port " << mPort << ".\n";
            return true;
        }
        return false;
    }

    void Run()
    {
        netbase::Socket s;
        char* buf = new char[2048]; // enough for a MTU
        ssize_t ret = 0;
        while(s.Attach(mSocket.Accept()).Valid())
        {
            s.Block(true);
            while((ret = s.Receive(buf, 2048)) > 0)
            {
                std::cout << "Received " << ret << " bytes.\n";
                s.Send(buf, ret);
            }
        }
    }

private: 
    netbase::Socket mSocket;
    unsigned short mPort;
};

int main(const int argc, char* argv[])
{
    // By default echo server on port 7
    unsigned short port = 7;
    if(argc == 2) // echoserver 9007
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }

    EchoServer server(port); 
    if(!server.Open())
    {
        std::cout << "EchoServer failed to open on port " << port << ".\n";
        return false;
    }
    server.Run(); 
    return true;
}
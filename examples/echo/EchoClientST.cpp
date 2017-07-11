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
// TCP echo on port 7
class EchoClient
{
public: 
    EchoClient() 
    : mSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
    , mConnected(false)
    {
        assert(mSocket.Valid());
        mSocket.Block(true);
    }

    ~EchoClient() 
    {

    }

    bool Connect(const char* host, unsigned short port = 7) // By default, echo service on port 7
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = PF_INET;
        addr.sin_port = htons(port);
        if(host != NULL) inet_pton(AF_INET, host, &addr.sin_addr);
        mConnected = mSocket.Connect((sockaddr*)&addr, sizeof(addr));
        return mConnected;
    }

    void Run()
    {
        if(!mConnected) return;

        std::cout << "Please input a message, exit to quit.\n";
        std::string msg;
        char* buf = new char[2048];
        ssize_t ret = 0;
        while(true)
        {
            std::cout << "<";
            std::cin >> msg;
            if(msg.empty())
            {
                continue;
            }
            if(msg == "exit")
            {
                break;
            }

            mSocket.Send(msg.data(), msg.length());
            if((ret = mSocket.Receive(buf, 2048)) > 0)
            {
                std::cout << ">" << std::string(buf, ret) << "\n"; 
            }
        }
    }

private: 
    netbase::Socket mSocket;
    bool mConnected;
};

int main(const int argc, char* argv[])
{
    const char* host = NULL;
    unsigned short port = 7; // By default, port is 7
    if(argc == 2) // echoclient 9007
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    else if(argc == 3) // echoclient 192.168.1.1 9007
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }

    EchoClient client;
    if(!client.Connect(host, port))
    {
        std::cout << "EchoClient failed to connect to " << host << ":" << port << ".\n";
        return 0;
    }
    client.Run();
    return 0;
}
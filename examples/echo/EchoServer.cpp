/*
 * Copyright (C) 20113, Maoxu Li. Email: maoxu@lebula.com
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

#include "EventLoop.hpp"
#include "TcpListener.hpp"
#include "TcpConnection.hpp"
#include "ByteBuffer.hpp"
#include <iostream>
#include <string>
#include <map>
#include <cstdlib>


using namespace netbase;
using namespace std::placeholders;

// RFC 862
// TCP listen on port 7
class EchoServer 
{
public: 
    EchoServer(EventLoop* loop, unsigned short port = 7)  // By default, echo service on port 7
    : mLoop(loop)
    , mListener(mLoop, NULL, port)
    {
        mListener.SetConnectedCallback(std::bind(&EchoServer::OnConnected, this, _1));
    }

    ~EchoServer() 
    {

    }

    bool Open()
    {
        if(mListener.Listen())
        {
            
            std::cout << "EchoSever opened on " << mListener.Address().ToString() << ".\n";
            return true;
        }
        return false;
    }

private: 
    void OnConnected(TcpConnection* conn)
    {
        std::cout << "Incoming connection from " << conn->RemoteAddress().ToString() << ".\n";
        conn->SetReceivedCallback(std::bind(&EchoServer::OnReceived, this, _1, _2));
    }

    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        std::cout << "Received " << buf->Readable() << " bytes.\n";
        std::string msg((const char*)buf->Peek(), buf->Peekable());
        std::cout << msg << "\n";
        conn->Send(buf);
        assert(buf->Empty());
    }

private: 
    EventLoop* mLoop;
    TcpListener mListener;
};

int main(const int argc, char* argv[])
{
    // By default echo server on port 7
    unsigned short port = 7;
    if(argc > 1) // echoserver 9007
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }

    EventLoop loop; // IO on current thread, single thread mode
    EchoServer server(&loop, port); 
    if(!server.Open())
    {
        std::cout << "EchoServer failed to open on port " << port << ".\n";
        return false;
    }
    loop.Run(); 
    return true;
}
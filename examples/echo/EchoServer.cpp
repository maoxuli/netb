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
#include "StreamBuffer.hpp"
#include "StreamPeeker.hpp"
#include <iostream>
#include <string>
#include <map>
#include <cstdlib>


using namespace netbase;
using namespace std::placeholders;

class EchoServer 
{
public: 
    EchoServer(EventLoop* loop, unsigned short port) 
    : mLoop(loop)
    , mListener(loop, NULL, port)
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
        conn->SetReceivedCallback(std::bind(&EchoServer::OnReceived, this, _1, _2));
        std::cout << "Incoming connection from " << conn->RemoteAddress().ToString() << ".\n";
    }

    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        std::cout << "Received " << buf->Readable() << " bytes.\n";
        std::string msg;
        StreamPeeker(buf).SerializeString(msg, "\r\n");
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
    EventLoop loop; // Current thread
    EchoServer server(&loop, 9001);
    if(!server.Open())
    {
        std::cout << "EchoServer failed to open.\n";
    }
    loop.Run(); 
    return true;
}
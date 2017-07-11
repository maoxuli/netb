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

#include "EventLoop.hpp"
#include "EventLoopThread.hpp"
#include "TcpConnector.hpp"
#include "TcpConnection.hpp"
#include "StreamBuffer.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace netbase;
using namespace std::placeholders;

// RFC 862
// TCP echo on port 7
class EchoClient
{
public: 
    EchoClient(EventLoop* loop) 
    : mLoop(loop)
    , mConnector(mLoop)
    , mConnection(NULL)
    {
        mConnector.SetConnectedCallback(std::bind(&EchoClient::OnConnected, this, _1));
    }

    ~EchoClient() 
    {

    }

    bool Connect(const char* host, unsigned short port = 7) // By default, echo service on port 7
    {
        assert(mConnection == NULL);
        return mConnector.Connect(host, port);
    }

    void Run()
    {
        std::cout << "Please input message, exit to quit.\n";
        std::cout << "<";
        std::string msg;
        while(true)
        {
            std::cin >> msg;
            if(msg.empty())
            {
                continue;
            }
            if(msg == "exit")
            {
                break;
            }
            if(mConnection == NULL)
            {
                std::cout << "Not connected, please try again.\n";
            }
            mConnection->Send(msg.data(), msg.length());
        }
    }

private: 
    void OnConnected(TcpConnection* conn)
    {
        assert(mConnection == NULL);
        mConnection = conn;
        mConnection->SetReceivedCallback(std::bind(&EchoClient::OnReceived, this, _1, _2));
        mConnection->SetClosedCallback(std::bind(&EchoClient::OnClosed, this, _1, _2));
        std::cout << "Outgoing connection: " << mConnection->LocalAddress().ToString() << "-"
                                             << mConnection->RemoteAddress().ToString() << ".\n";
    }

    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        assert(conn == mConnection);
        std::string msg((const char*)buf->Read(), buf->Readable());
        std::cout << ">" << msg << ".\n";
        buf->Clear();  
        std::cout << "<";      
    }

    void OnClosed(TcpConnection* conn, bool keepReceiving)
    {
        if(conn == mConnection)
        {
            mConnection = NULL;
        }
    }

private: 
    EventLoop* mLoop;
    TcpConnector mConnector;
    TcpConnection* mConnection;
    StreamBuffer mBuffer;
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

    EventLoopThread thread; // IO on a separate thread
    EventLoop* loop = thread.Start();
    EchoClient client(loop);
    if(!client.Connect(host, port))
    {
        std::cout << "EchoClient failed to connect to " << host << ":" << port << ".\n";
        return 0;
    }
    client.Run(); // Current thread is for UI
    return 0;
}
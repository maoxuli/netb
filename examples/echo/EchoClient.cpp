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
#include "SocketAddress.hpp"
#include "StreamBuffer.hpp"
#include "EventLoop.hpp"
#include "EventLoopThread.hpp"
#include "TcpConnector.hpp"
#include "TcpConnection.hpp"
#include <string>
#include <cstdlib>

// RFC 862/UDP
// Echo client
class EchoClient
{
public:
    virtual ~EchoClient() { }
    virtual bool Connect(const char* host, unsigned short port = 9007) = 0;
    virtual void Run() = 0;
};

// RFC 862/UDP
// Based on simple socket API
class EchoClientSU : public EchoClient
{
public: 
    EchoClientSU() 
    : mSocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)
    , mConnected(false)
    {
        assert(mSocket.Valid());
        mSocket.Block(true);
    }

    bool Connect(const char* host, unsigned short port = 9007) // By default on 9007
    {
        netbase::SocketAddress addr(host, port);
        mConnected = mSocket.Connect(addr.SockAddr(), addr.Length());
        return mConnected;
    }

    void Run()
    {
        if(!mConnected) return;

        std::cout << "Please input a message, exit to quit.\n";
        std::string msg;
        ssize_t ret = 0;
        char* buf = new char[2048];
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

// RFC 862/TCP
// Based on simple socket API
class EchoClientST : public EchoClient
{
public: 
    EchoClientST() 
    : mSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
    , mConnected(false)
    {
        assert(mSocket.Valid());
        mSocket.Block(true);
    }

    bool Connect(const char* host, unsigned short port = 9007) // By default, echo service on port 9007
    {
        netbase::SocketAddress addr(host, port);
        mConnected = mSocket.Connect(addr.SockAddr(), addr.Length());
        return mConnected;
    }

    void Run()
    {
        if(!mConnected) return;

        std::cout << "Please input a message, exit to quit.\n";
        std::string msg;
        ssize_t ret = 0;
        char* buf = new char[2048];
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

// RFC 862/TCP
// Based on async socket API
using namespace netbase;
using namespace std::placeholders;
class EchoClientAT : public EchoClient
{
public: 
    EchoClientAT() 
    : mLoopThread()
    , mLoop(mLoopThread.Start())
    , mConnector(mLoop)
    , mConnection(NULL)
    {
        mConnector.SetConnectedCallback(std::bind(&EchoClientAT::OnConnected, this, _1));
    }

    bool Connect(const char* host, unsigned short port = 9007) // By default, echo service on port 9007
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
        mConnection->SetReceivedCallback(std::bind(&EchoClientAT::OnReceived, this, _1, _2));
        mConnection->SetClosedCallback(std::bind(&EchoClientAT::OnClosed, this, _1, _2));
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
    netbase::EventLoopThread mLoopThread;
    netbase::EventLoop* mLoop;
    netbase::TcpConnector mConnector;
    netbase::TcpConnection* mConnection;
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
    EchoClient* client = new EchoClientAT();
    if(!client->Connect(host, port))
    {
        std::cout << "EchoClient failed to connect to " << host << ":" << port << ".\n";
        return 0;
    }
    client->Run();
    return 0;
}
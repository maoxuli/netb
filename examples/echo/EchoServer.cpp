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

#include "SocketAddress.h"
#include "SocketSelector.h"
#include "StreamBuffer.h"
#include "Socket.h"
#include "TcpAcceptor.h"
#include "TcpSocket.h"
#include "UdpSocket.h"
#include "AsyncTcpAcceptor.h"
#include "AsyncTcpSocket.h"
#include "AsyncUdpSocket.h"
#include <string>
#include <map>
#include <cstdlib>

// RFC 862
// Echo server
class EchoServer 
{
public:
    virtual ~EchoServer() { }
    virtual bool Open(unsigned short port) = 0;
    virtual bool Open(unsigned short port, Error* e) = 0; 
}

// RFC 862/UDP
// Using simple socket API
class EchoServerSU
{
public: 
    EchoServerSU(unsigned short port = 9007)  // By default, echo service on port 9007
    : mPort(port)
    {

    }

    // return false if failed to open, and return details in e
    bool Open(Error* e = NULL)
    {
        if(!mSocket.Create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, e);
        {
            return false;
        }
        if(!mSocket.Bind(SocketAddress(mPort)), e)
        { 
            return false;
        }
        return false;
    }

    void Run()
    {
        ssize_t ret = 0;
        char* buf = new char[2048]; // enough for a MTU
        netbase::SocketAddress addr;
        socklen_t addrlen = addr.Length();
        while((ret = mSocket.ReceiveFrom(buf, 2048, (sockaddr*)&addr, &addrlen)) > 0)
        {
            mSocket.SendTo(buf, ret, addr.SockAddr(), addr.Length());
        }
    }

private: 
    netbase::Socket mSocket;
    unsigned short mPort;
};

// RFC 862/TCP
// Based on simple socket API
class EchoServerST : public EchoServer
{
public: 
    EchoServerST(unsigned short port = 9007)  // By default, echo service on port 9007
    : mSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
    , _address(NULL, port)
    {
        assert(mSocket.Valid());
        mSocket.Block(true);
    }

    bool Open()
    {
        if(mSocket.Bind(_address.SockAddr(), _address.Length()) && mSocket.Listen())
        {
            std::cout << "EchoSever opened on " << _address.ToString() << ".\n";
            return true;
        }
        return false;
    }

    void Run()
    {
        netbase::Socket s;
        ssize_t ret = 0;
        char* buf = new char[2048]; // enough for a MTU
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
    netbase::SocketAddress _address;
};

// RFC 862/TCP
// Based on async socket API
using namespace netbase;
using namespace std::placeholders;
class EchoServerAT : public EchoServer
{
public: 
    EchoServerAT(unsigned short port = 9007)  // By default, echo service on port 9007
    : _loop()
    , mListener(&_loop, NULL, port)
    {
        mListener.SetConnectedCallback(std::bind(&EchoServerAT::OnConnected, this, _1));
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

    void Run()
    {
        _loop.Run();
    }

private: 
    void OnConnected(TcpConnection* conn)
    {
        conn->SetReceivedCallback(std::bind(&EchoServerAT::OnReceived, this, _1, _2));
        std::cout << "Incoming connection from " << conn->RemoteAddress().ToString() << ".\n";
    }

    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        std::cout << "Received " << buf->Addressable() << " bytes.\n";
        conn->Send(buf);
    }

private: 
    EventLoop _loop;
    TcpListener mListener;
};

// Open echo server on given port, by default 9007
int main(const int argc, char* argv[])
{
    // Service port, by default 9007
    unsigned short port = 9007;
    if(argc == 2) // echoserver 9017
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }

    // Default server
    EchoServer server(port);
    Error e;
    if(!server.Open(&e))
    {
        std::cout << "Echo server open failed on " << port << ".\n";
        std::cout << e.Class().Name() << ":" << e.Code() << ":" << e.Infor() << "\n";
        return -1;
    }
    if(!server.Run(e))
    {
        std::cout << Echo server 
    }

    // 

    return 0;
}
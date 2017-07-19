/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#include "TcpSocket.h"
#include "EventLoop.h"
#include "EventHandler.h"
#include <cassert>

NET_BASE_BEGIN

// Open a socket internally
TcpSocket::TcpSocket(int domain)
: mSocket(domain, SOCK_STREAM, IPPROTO_TCP)
{

}

// Base class close the SOCKET
TcpSocket::~TcpSocket()
{

}

bool TcpSocket::Bind(const char* host, unsigned short port)
{
    SocketAddress addr(host, port, mSocket.Family());
    return mSocket.Bind(addr.SockAddr(), addr.Length());
}

bool TcpSocket::Bind(const SocketAddress& addr)
{
    return mSocket.Bind(addr.SockAddr(), addr.Length());
}

bool TcpSocket::Listen(int backlog)
{
    return mSocket.Listen(backlog);
}

SOCKET TcpSocket::Accept()
{
    return mSocket.Accept();
}

SOCKET TcpSocket::Accept(SocketAddress* addr)
{
    assert(addr != NULL);
    socklen_t addrlen = addr->Length();
    return mSocket.Accept(addr->SockAddr(), &addrlen);
}

bool TcpSocket::Connect(const char* host, unsigned short port)
{
    SocketAddress addr(host, port, mSocket.Family());
    return mSocket.Connect(addr.SockAddr(), addr.Length());
}

bool TcpSocket::Connect(const SocketAddress& addr)
{
    return mSocket.Connect(addr.SockAddr(), addr.Length());
}

bool TcpSocket::LocalAddress(SocketAddress* addr) const
{
    assert(addr != NULL);
    socklen_t addrlen = addr->Length();
    return mSocket.LocalAddress(addr->SockAddr(), &addrlen);
}

bool TcpSocket::RemoteAddress(SocketAddress* addr) const 
{
    assert(addr != NULL);
    socklen_t addrlen = addr->Length();
    return mSocket.RemoteAddress(addr->SockAddr(), &addrlen);
}

ssize_t TcpSocket::Send(const void* p, size_t n)
{
    return mSocket.Send(p, n);
}

ssize_t TcpSocket::Send(StreamBuffer* buf)
{
    assert(buf != NULL);
    ssize_t ret = mSocket.Send(buf->Read(), buf->Readable());
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

ssize_t TcpSocket::Receive(void* p, size_t n)
{
    return mSocket.Receive(p, n);
}

ssize_t TcpSocket::Receive(StreamBuffer* buf)
{
    assert(buf != NULL);
    ssize_t ret = 0;
    if(buf->Writable(2048))
    {
        ssize_t ret = mSocket.Receive(buf->Write(), buf->Writable());
        if(ret > 0)
        {
            buf->Write(ret);
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////

bool TcpSocket::Block() const 
{
    return mSocket.Block();
}

bool TcpSocket::Block(bool block)
{
    return mSocket.Block(block);
}

bool TcpSocket::ReuseAddress() const 
{
    return mSocket.ReuseAddress();
}

bool TcpSocket::ReuseAddress(bool reuse)
{
    return mSocket.ReuseAddress(reuse);
}

bool TcpSocket::NoDelay() const 
{
    return mSocket.NoDelay();
}

bool TcpSocket::NoDelay(bool no)
{
    return mSocket.NoDelay(no);
}

bool TcpSocket::KeepAlive() const 
{
    return mSocket.KeepAlive();
}

bool TcpSocket::KeepAlive(bool keep)
{
    return mSocket.KeepAlive(keep);
}

bool TcpSocket::SendBuffer(size_t size)
{
    return mSocket.SendBuffer(size);
}

size_t TcpSocket::SendBuffer() const
{
    return mSocket.SendBuffer();
}

bool TcpSocket::ReceiveBuffer(size_t size)
{
    return mSocket.ReceiveBuffer(size);
}

size_t TcpSocket::ReceiveBuffer() const
{
    return mSocket.ReceiveBuffer();
}

NET_BASE_END
NET_BASE_BEGIN

// Create an unbound, unconnected TCP socket
// The domain of the socket is not determined before calling Connect()
TcpSocket::TcpSocket()
: Socket()
, mAddress()
, mConnectedAddress()
, mConnected(false)
{

}

// Create a unbound, unconnected TCP socket
// The domain of the socket is given
TcpSocket::TcpSocket(int domain)
: Socket(domain)
, mAddress((sa_family_t)domain)
, mConnectedAddress()
, mConnected(false)
{

}

// Create a TCP socket bound to given local address
// The domain of the socket is determined by the address family
// See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
TcpSocket::TcpSocket(const SocketAddress& addr, bool reuseaddr, bool reuseport)
: Socket(addr.Family())
, mAddress(addr)
, mConnectedAddress()
, mConnected(false)
{

}

// Create a TCP socket with externally established connection
TcpSocket::TcpSocket(SOCKET s, const SocketAddress* conencted, const SocketAddress* local)
: Socket(s)
, mAddress()
, mConnectedAddress()
, mConnected(false)
{
    if(local != NULL) mAddress = *local;
    if(connected != NULL) mConnectedAddress = *connected;
}

// Destructor, deriviation is allowed for extension
TcpSocket::~TcpSocket()
{

}

// Called only once when the connection is established
// by TcpListener or TcpConnector
bool TcpScoket::Connected()
{
    if(mAddress.Empty())
    {
        socklen_t addrlen = mAddress.Length();
        Socket::Address((sockaddr*)&mAddress, &addrlen);
    }
    if(mConnectedAddress.Empty())
    {
        socklen_t addrlen = mConnectedAddress.Length();
        Socket::ConnectedAddress((sockaddr*)&mRemoteAddress, &addrlen);
    }
    mConnected = true;
}

bool TcpSocket::Connect(const SocketAddress& addr)
{
    mConnected = false;
    return false;
}

void TcpSocket::Disconnect()
{
    mConnected = false;
}

ssize_t TcpSocket::Send(const void* p, size_t n, int flags)
{
    return Socket::Send(p, n, flags);
}

ssize_t TcpSocket::Send(StreamBuffer* buf, int flags)
{
    return Socket::Send(buf, flags);
}

ssize_t TcpSocket::Receive(void* p, size_t n, int flags)
{
    return Socket::Receive(p, n, flags);
}

ssize_t TcpSocket::Receive(StreamBuffer* buf, int flags)
{
    return Socket::Receive(buf, flags);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool TcpSocket::Block() const 
{
    return Socket::Block();
}

bool TcpSocket::Block(bool block)
{
    return Socket::Block(block);
}

bool TcpSocket::NoDelay(bool no)
{
    int flag = no ? 1 : 0;
    return Socket::SetOption(IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int));
}

size_t Socket::KeepAlive(size_t keep)
{
    int flag = keep > 0 ? 1 : 0;
    Socket::SetOption(SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(int));
    if(keep > 0)
    {
        Socket::SetOption(IPPROTO_TCP, TCP_KEEPALIVE, &keep, sizeof(int));
    }
    return true;
}

bool Socket::SendBuffer(size_t size)
{
    return Socket::SetOption(SOL_SOCKET, SO_SNDBUF, &size, sizeof(int)));
}

size_t Socket::SendBuffer() const
{
    int size;
    socklen_t len = sizeof(int);
    if(Socket::GetOption(SOL_SOCKET, SO_SNDBUF, &size, &len))
    {
        return size;
    }
    return 0;
}

bool Socket::ReceiveBuffer(size_t size)
{
    return Socket::SetOption(SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
}

size_t Socket::ReceiveBuffer() const
{
    int size;
    socklen_t len = sizeof(int);
    if(Socket::GetOption(SOL_SOCKET, SO_RCVBUF, &size, &len))
    {
        return size;
    }
    return 0;
}

NET_BASE_END
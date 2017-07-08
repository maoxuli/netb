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

#include "UdpSocket.hpp"
#include <cassert>

NET_BASE_BEGIN

// Open a socket internally
UdpSocket::UdpSocket(int domain)
: mSocket(domain, SOCK_DGRAM, IPPROTO_UDP)
{

}

UdpSocket::~UdpSocket()
{

}

bool UdpSocket::Bind(const char* host, unsigned short port)
{
    SocketAddress addr(host, port);
    return mSocket.Bind(addr.SockAddr(), addr.SockAddrLen());
}

bool UdpSocket::Bind(const SocketAddress& addr)
{
    return mSocket.Bind(addr.SockAddr(), addr.SockAddrLen());
}

ssize_t UdpSocket::Send(void* p, size_t n, const SocketAddress& addr)
{
    return mSocket.SendTo(p, n, addr.SockAddr(), addr.SockAddrLen());
}

ssize_t UdpSocket::Send(StreamBuffer* buf, const SocketAddress& addr)
{
    ssize_t ret = mSocket.SendTo(buf->Read(), buf->Readable(), addr.SockAddr(), addr.SockAddrLen());
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

ssize_t UdpSocket::Receive(void* p, size_t n, SocketAddress* addr)
{
    socklen_t addrlen = addr->SockAddrLen();
    return mSocket.ReceiveFrom(p, n, addr->SockAddr(), &addrlen);
}

ssize_t UdpSocket::Receive(StreamBuffer* buf, SocketAddress* addr)
{
    socklen_t addrlen = addr->SockAddrLen();
    ssize_t ret = 0;
    if(buf->Writable(2048))
    {
        ret = mSocket.ReceiveFrom(buf->Write(), buf->Writable(), addr->SockAddr(), &addrlen);
        if(ret > 0)
        {
            buf->Write(ret);
        }
    }
    return ret;
}

bool UdpSocket::Connect(const char* host, unsigned short port)
{
    SocketAddress addr(host, port);
    return mSocket.Connect(addr.SockAddr(), addr.SockAddrLen());
}

bool UdpSocket::Connect(const SocketAddress& addr)
{
    return mSocket.Connect(addr.SockAddr(), addr.SockAddrLen());
}

bool UdpSocket::LocalAddress(SocketAddress* addr) const
{
    socklen_t addrlen = addr->SockAddrLen();
    return mSocket.LocalAddress(addr->SockAddr(), &addrlen);
}

bool UdpSocket::RemoteAddress(SocketAddress* addr) const 
{
    socklen_t addrlen = addr->SockAddrLen();
    return mSocket.RemoteAddress(addr->SockAddr(), &addrlen);
}

ssize_t UdpSocket::Send(void* p, size_t n)
{
    return mSocket.Send(p, n);
}

ssize_t UdpSocket::Send(StreamBuffer* buf)
{
    ssize_t ret = mSocket.Send(buf->Read(), buf->Readable());
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

ssize_t UdpSocket::Receive(void* p, size_t n)
{
    return mSocket.Receive(p, n);
}

ssize_t UdpSocket::Receive(StreamBuffer* buf)
{
    assert(buf != NULL);
    ssize_t ret = 0;
    if(buf->Writable(2048))
    {
        ret = mSocket.Receive(buf->Write(), buf->Writable());
        if(ret > 0)
        {
            buf->Write(ret);
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////

bool UdpSocket::Block() const 
{
    return mSocket.Block();
}

bool UdpSocket::Block(bool block) 
{
    return mSocket.Block(block);
}

bool UdpSocket::ReuseAddress() const 
{
    return mSocket.ReuseAddress();
}

bool UdpSocket::ReuseAddress(bool reuse)
{
    return mSocket.ReuseAddress(reuse);
}

size_t UdpSocket::SendBuffer() const 
{
    return mSocket.SendBuffer();
}

bool UdpSocket::SendBuffer(size_t size)
{
    return mSocket.SendBuffer(size);
}

size_t UdpSocket::ReceiveBuffer() const 
{
    return mSocket.ReceiveBuffer();
}

bool UdpSocket::ReceiveBuffer(size_t size)
{
    return mSocket.ReceiveBuffer(size);
}

NET_BASE_END 

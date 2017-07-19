/*
 * Copyright (C) 2010-2016, Maoxu Li. Email: maoxu@lebula.com
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

#include "TcpAcceptor.h"

NET_BASE_BEGIN
(

// Create an unbound TCP socket working as TCP server
TcpAcceptor::TcpAcceptor()
: mSocket()
, mAddress()
, mOpened(false)
{

}

// Create an unbound TCP socket working as TCP server
// The socket is only for given family
TcpAcceptor::TcpAcceptor(int domain)
: mSocket(domain, SOCK_STREAM, IPPROTO_TCP)
, mAddress(NULL, 0, (sa_family_t)family)
, mOpened(false)
{

}

// Create a TCP socket that bound to give local address
// The family of the socket is determined by address
// See SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
TcpAcceptor::TcpAcceptor(const SocketAddress& addr, bool reuseaddr, bool reuseport)
: mSocket(addr.Family(), SOCK_STREAM, IPPROTO_TCP)
, mAddress(addr)
, mOpened(false)
{
    mSocket.ReuseAddress(reuseaddr);
    mSocket.ReusePort(reuseport);
}

TcpAcceptor::~TcpAcceptor()
{

}

// Open on address pased in on initialization
bool TcpAcceptor::Open(int backlog)
{
    if(!mAddress.Empty())
    {
        mAddress.Reset(AF_INET);
    }
    if(mSocket.Invalid())
    {
        mSocket.Create(mAddress,Family(), SOCK_STREAM, IPPROTO_TCP);
    }
    assert(!mSocket.Invalid());
    assert(mSocket.Family() == mAddress.Familiy());

    return Open(mAddress, backlog);
}

// Open on given address
bool TcpAcceptor::Open(const SocketAddress& addr, int backlog)
{
    if(mSocket.Invalid())
    {
        mSocket.Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP);
    }
    assert(!mSocket.Invalid());
    assert(mSocket.Family() == addr.Family());

    if(mSocket.Bind(addr.SockAddr(), addr.Length()) && mSocket.Listen(backlog))
    {
        SocketAddress addr;
        socklen_t addrlen = addr.Length()
        if(mSocket.Address((sockaddr*)&addr, &addrlen))
        {
            mAddress = addr;
        }
        mOpened = true;
        return true;
    }
    return false;
}

// Open on given address and use given options
bool TcpAcceptor::Open(const SocketAddress& addr, bool reuseaddr, bool reuseport, int backlog)
{
    if(mSocket.Invalid())
    {
        mSocket.Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP);
    }
    assert(!mSocket.Invalid());
    assert(mSocket.Family() == addr.Family());
    mSocket.ReuseAddress(reuseaddr);
    mSocket.ReusePort(reuseport);

    return Open(addr, backlog);
}

// Accept a connection
SOCKET TcpAcceptor::Accept()
{
    return mSocket.Accept();
}

// Accept a connection
SOCKET TcpAcceptor::Accept(SocketAddress* addr)
{
    if(addr == NULL)
    {
        return mSocket.Accept();
    }
    socklen_t addrlen = addr->Length();
    return mSocket.Accept((sockaddr*)addr, &addrlen);
}

/////////////////////////////////////////////////////////////////////////////

bool TcpAcceptor::Block() const 
{
    return Socket::Block(); 
}

bool TcpAcceptor::Block(bool block)
{
    return Socket::Block(block);
}

NET_BASE_END

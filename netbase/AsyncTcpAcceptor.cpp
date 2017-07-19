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

#include "AsyncTcpAcceptor.h"

NET_BASE_BEGIN

using namespace std::placeholders;

// Create an unbound TCP socket as TCP server
// The domain of the socket is not determined before calling of Open()
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop)
: TcpAcceptor()
, mLoop(loop)
, mHandler(NULL)
{
    assert(mLoop != NULL);
}

// Create an unbound TCP socket as TCP server
// The domain of the socket is given
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop, int domain)
: TcpAcceptor(domain)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler.SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
}

// Create an TCP socket bound to given local address
// The domain of the socket is determined by address family
// See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr, bool reuseaddr, bool reuseport)
: TcpAcceptor(addr, reuseaddr, reuseport)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler->SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
}

// Destructor, deriviation is allowed for extension
AsyncTcpAcceptor::~AsyncTcpAcceptor()
{
    if(mHandler != NULL)
    {
        delete mHandler;
    }
}

bool AsyncTcpAcceptor::EnableReading()
{
    assert(!TcpAcceptor::Invalid());
    if(mHandler == NULL)
    {
        mHandler = new EventHandler(mLoop, GetSocket());
        mHandler->SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
    }
    assert(mHandler != NULL);
    mHandler->EnableReading();
}

// Open on address passed in on initialization
// If offset is none zero, try contiguous ports limited by the offset until success
bool AsyncTcpAcceptor::Open(int backlog)
{
    return (TcpAcceptor::Open(backlog) && EnableReading())
}

// Set backlog option
bool AsyncTcpAcceptor::Open(const SocketAddress& addr, int backlog)
{
    return (TcpAcceptor::Open(addr, backlog) && EnableReading())
}

// EventHandler::ReadCallbck
// Called when TCP socket is ready to accept a incomming connection
void AsyncTcpAcceptor::OnRead(SOCKET s)
{
    assert(s == mSocket.GetSocket());
    SOCKET sock = TcpAcceptor::Accept(false);
    assert(mAcceptCallback);
    if(sock != INVALID_SOCKET && mAcceptedCallback)
    { 
        mAcceptedCallback(this, sock);
    }
}

NET_BASE_END

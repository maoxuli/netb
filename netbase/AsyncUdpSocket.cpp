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

#include "AsyncUdpSocket.h"
#include <cassert>

NET_BASE_BEGIN

// Create an unbound, unconnected UDP socket
// The domain of the socket is not determined before calling of Open() or Connect() or SendTo()
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop)
: UdpSocket()
, mLoop(loop)
, mHandler(NULL)
{
    assert(mLoop != NULL);
}


// Create an unbound, unconnected UDP socket
// The domain of the socket is given
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, int domain)
: UdpSocket(domain)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler.SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
    mHandler.SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
}

// Create an UDP socket bound to given address
// The domain of the socket is determined by address family
// See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr, bool reuseaddr, bool reuseport)
: UdpSocket(addr, reuseaddr, reuseport)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler.SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
    mHandler.SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
}

// Destructor, deriviation is allow for extension
AsyncUdpSocket::~AsyncUdpSocket()
{
    if(mHandler != NULL)
    {
        delete mHandler;
    }

    while(!mOutBuffers.empty())
    {
        BufferAddress& ba = mOutBuffer.front();
        delete ba.buf;
        mOutBuffer.pop();
    }
}

bool AsyncUdpSocket::EnableReading()
{
    assert(!UdpSocket::Invalid());
    if(mHandler == NULL)
    {
        mHandler = new EventHandler(mLoop, GetSocket());
        mHandler.SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
        mHandler.SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
    }
    assert(mHandler != NULL);
    mHandler.EnableReading();
    return true;
}

bool AsyncUdpSocket::EnableWriting()
{
    assert(!UdpSocket::Invalid());
    if(mHandler == NULL)
    {
        mHandler = new EventHandler(mLoop, GetSocket());
        mHandler.SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
        mHandler.SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
    }
    assert(mHandler != NULL);
    mHandler.EnableWriting();
    return true;
}

bool AsyncUdpSocket::Open()
{
    return (UdpSocket::Open() && EnableReading())
}

bool AsyncUdpSocket::Open(const SocketAddress& addr)
{
    return (UdpSocket::Open(addr) && EnableReading());
}

ssize_t UdpTransceiver::SendTo(const void* p, size_t n, const SocketAddress& addr, int flags)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(p, n, addr);
    }
    else
    {
        mLoop->Invoke(std::bind(&UdpTransceiver::SendInLoop, this, std::make_shared<StreamBuffer>(p, n) , addr));
    }
    return true;
}

ssize_t UdpTransceiver::SendTo(StreamBuffer* buf, const SocketAddress& addr)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(buf->Read(), buf->Readable(), addr);
        buf->Clear(); /// ???
    }
    else
    {
        mLoop->Invoke(std::bind(&UdpTransceiver::SendInLoop, this, std::make_shared<StreamBuffer>(buf), addr));
        buf->Clear(); /// ???
    }
    return true;
}

void UdpTransceiver::SendInLoop(StreamBufferPtr buf, SocketAddress addr)
{
    DoSend(buf->Read(), buf->Readable(), addr);
}

// Send to given address
void UdpTransceiver::DoSend(const void* p, size_t n, const SocketAddress& addr)
{
    ssize_t sent = 0;
    if(mOutBuffers.empty())
    {
        sent = mSocket.SendTo(p, n, addr.SockAddr(), addr.Length());
    }

    if(sent < 0) // error
    {
        std::cout << "UdpSocket::DoSend return error: " << sent << "\n";
        sent = 0;
    }

    if(sent < n) // Only send part of the data
    {
        std::cout << "UdpSocket::DoSend send partially: " << sent << "\n";
        mOutBuffers.push(BufferAddress(new StreamBuffer(p, n), addr));
        EnableWriting();
    }
}

// EventHandler::EventCallback
// Read is ready
void AsyncUdpSocket::OnRead(SOCKET s)
{
    assert(s == GetSocket());
    ssize_t n = 0;
    SocketAddress addr;
    socklen_t addrlen = addr.Length();
    if(mInBuffer.Writable(2048))
    {
        n = mSocket.ReceiveFrom(mInBuffer.Write(), mInBuffer.Writable(), addr.SockAddr(), &addrlen);
    }
    if(n > 0)
    {
        mInBuffer.Write(n);
        if(mReceivedCallback)
        {
            mReceivedCallback(this, &mInBuffer, &addr);
        }
    }
    else
    {
        std::cout << "UdpSocket::OnRead() return error: " << n << "\n";
    }
}

void AsyncUdpSocket::OnWrite(SOCKET s)
{
    assert(s == GetSocket());
    while(!mOutBuffers.empty())
    {
        BufferAddress& ba = mOutBuffers.front();
        SendTo(ba.buf, ba.addr);
        if(!ba.buf->Empty())
        {
            break;
        }
        delete ba.buf;
        mOutBuffers.pop();
    }
}

NET_BASE_END 

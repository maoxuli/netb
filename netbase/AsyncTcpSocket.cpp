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

#include "AsyncTcpSocket.h"
#include <cassert>

NET_BASE_BEGIN

// Create an unbound, unconnected TCP socket
// The domain of the socket is not determined before calling of Connect()
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop)
: TcpSocket()
, mLoop(loop)
, mHandler(NULL)
{
    assert(mLoop != NULL);
}

// Create an unbound, unconnected TCP socket
// The domain of the socket is given
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, int domain)
: TcpSocket(domain)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler->SetReadCallback(std::bind(&AysncTcpSocket::OnRead, this, _1));
    mHandler->SetWriteCallback(std::bind(&AysncTcpSocket::OnWrite, this, _1));
}


// Create a TCP socket bound to given local address
// The domain of the socket is determined by address family
// See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr, bool reuseaddr, bool reuseport)
: TcpSocket(addr, reuseaddr, reuseport)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler->SetReadCallback(std::bind(&AysncTcpSocket::OnRead, this, _1));
    mHandler->SetWriteCallback(std::bind(&AysncTcpSocket::OnWrite, this, _1));
}

// Create a TCP socket with externally established connection
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* conencted, const SocketAddress* local)
: TcpSocket(s, connected, local)
, mLoop(loop)
, mHandler(new EventHandler(mLoop, GetSocket()))
{
    assert(mLoop != NULL);
    assert(mHandler != NULL);
    mHandler->SetReadCallback(std::bind(&AysncTcpSocket::OnRead, this, _1));
    mHandler->SetWriteCallback(std::bind(&AysncTcpSocket::OnWrite, this, _1));
}

// Destructor, deriviation is allowed for extension
AsyncTcpSocket::~AsyncTcpSocket()
{
    if(mHandler != NULL)
    {
        delete mHandler;
    }
}

bool AsyncTcpSocket::EnableReading()
{
    assert(!TcpSocket::Invalid());
    if(mHanlder == NULL)
    {
        mHandler = new EventHandler(mLoop, GetSocket());
        assert(mHandler != NULL);
        mHandler->SetReadCallback(std::bind(&AysncTcpSocket::OnRead, this, _1));
        mHandler->SetWriteCallback(std::bind(&AysncTcpSocket::OnWrite, this, _1));
    }
    mHanlder.EnableReading();
    return true;
}

bool AsyncTcpSocket::EnableWriting()
{
    assert(!TcpSocket::Invalid());
    if(mHanlder == NULL)
    {
        mHandler = new EventHandler(mLoop, GetSocket());
        assert(mHandler != NULL);
        mHandler->SetReadCallback(std::bind(&AysncTcpSocket::OnRead, this, _1));
        mHandler->SetWriteCallback(std::bind(&AysncTcpSocket::OnWrite, this, _1));
    }
    mHanlder.EnableWriting();
    return true;
}

bool AsyncTcpSocket::Connected() 
{
    return (TcpSocket::Connected() && EnableReading());
}

bool AsyncTcpSocket::Connect(const SocketAddress& addr)
{
    return (TcpSocket::Connect(addr) && EnableReading());
}

bool AsyncTcpSocket::Disconnect()
{
    if(IsConnected())
    {
        TcpSocket::Disconnect();
        mHandler->Detach();
    }
    return true;
}

// Send data over the connection,
// Directly sent or buffered
ssize_t AsyncTcpSocket::Send(const void* p, size_t n, int flags)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(p, n);
    }
    else
    {
        mLoop->Invoke(std::bind(&TcpConnection::SendInLoop, this, std::make_shared<StreamBuffer>(p, n)));
    }
    return true;
}

// Send data
// The actual data sending must be done on the thread loop 
// to ensure the order of data sending
ssize_t AsyncTcpSocket::Send(StreamBuffer* buf, int flags)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(buf->Read(), buf->Readable());
        buf->Clear(); // ??? Is it necessary to clear buffer?
    }
    else
    {
        mLoop->Invoke(std::bind(&TcpConnection::SendInLoop, this, std::make_shared<StreamBuffer>(buf)));
        buf->Clear(); // ??? 
    }
    return true;
}

// Thread loop will invoke this function
void TcpConnection::SendInLoop(StreamBufferPtr buf)
{
    DoSend(buf->Read(), buf->Readable());
}

// This function must be called in the thread
void TcpConnection::DoSend(const void* p, size_t n)
{    
    // Try send directly if out buffer is empty
    ssize_t sent = 0;
    if(mOutBuffer.Empty())
    {
        sent = mSocket.Send(p, n);
    }
    
    if(sent < 0) // error
    {
        std::cout << "TcpConnection::DoSend return error: " << sent << "\n";
        sent = 0;
    }
    
    if(sent < n) // async send
    {
        mOutBuffer.Write((unsigned char*)p + sent, n - sent);
        EnableWriting();
    }
}

// Ready to read
// Read data into in buffer
void TcpConnection::OnRead()
{
    ssize_t n = 0;
    if(mInBuffer.Writable(2048))
    {
        n = mSocket.Receive(mInBuffer.Write(), mInBuffer.Writable());
    }

    if(n > 0)
    {
        mInBuffer.Write(n);
        if(mReceivedCallback)
        {
            mReceivedCallback(this, &mInBuffer);
        }
    }
    else // Error
    {
        std::cout << "TcpConnection::OnRead return " << n << ", code: " << SocketError::Code() << ". Disonnect!\n";
        Close(false);
    }
}

// Ready to write
// Try to send data in out buffer
void AsyncTcpSocket::OnWrite()
{
    if(mOutBuffer.Readable() > 0)
    {
        ssize_t sent = mSocket.Send(mOutBuffer.Read(), mOutBuffer.Readable());
        mOutBuffer.Read(sent);
    }

    if(mOutBuffer.Readable() == 0)
    {
        mHandler.DisableWriting();
    }
}

NET_BASE_END

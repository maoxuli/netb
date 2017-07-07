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

#include "TcpConnection.hpp"
#include "EventLoop.hpp"
#include "SocketError.hpp"
#include <memory>

NET_BASE_BEGIN

// Always initialize with external initialized socket
TcpConnection::TcpConnection(EventLoop* loop, SOCKET s)
: mLoop(loop)
, mSocket(s)
, mLocalAddress()
, mRemoteAddress()
, mHandler(loop, mSocket)
{
    mSocket.Block(false); // working in non-block mode
    mHandler.SetWriteCallback(std::bind(&TcpConnection::OnWrite, this));
    mHandler.SetReadCallback(std::bind(&TcpConnection::OnRead, this));
}

// Always initialize with external initialized socket
TcpConnection::TcpConnection(EventLoop* loop, SOCKET s, const SocketAddress& local, const SocketAddress& remote)
: mLoop(loop)
, mSocket(s)
, mLocalAddress(local)
, mRemoteAddress(remote)
, mHandler(loop, mSocket)
{
    mSocket.Block(false); // working in non-block mode
    mHandler.SetWriteCallback(std::bind(&TcpConnection::OnWrite, this));
    mHandler.SetReadCallback(std::bind(&TcpConnection::OnRead, this));
}

TcpConnection::~TcpConnection()
{

}

// Called only once when the connection is established
// by TcpListener or TcpConnector
void TcpConnection::Connected()
{
    if(mLocalAddress.Empty())
    {
        socklen_t addrlen = mLocalAddress.SockAddrLen();
        mSocket.LocalAddress(mLocalAddress.SockAddr(), &addrlen);
        assert(addrlen == mLocalAddress.SockAddrLen());
    }

    if(mRemoteAddress.Empty())
    {
        socklen_t addrlen = mRemoteAddress.SockAddrLen();
        mSocket.LocalAddress(mRemoteAddress.SockAddr(), &addrlen);
        assert(addrlen == mRemoteAddress.SockAddrLen());
    }

    mLoop->Invoke(std::bind(&TcpConnection::ConnectedInLoop, this));
}

void TcpConnection::ConnectedInLoop()
{
    if(mConnectedCallback)
    {
        mConnectedCallback(this);
    }
    mHandler.EnableReading();
}

// Application may close the connection
// Close to stop writing anything, but may still receiving or not
void TcpConnection::Close(bool keepReceiving)
{
    mLoop->Invoke(std::bind(&TcpConnection::CloseInLoop, this, keepReceiving));
}

// Close the connection
// Todo: how close the connection?
void TcpConnection::CloseInLoop(bool keepReceiving)
{
    if(mClosedCallback)
    {
        mClosedCallback(this, keepReceiving);
    }
    mSocket.Shutdown(keepReceiving ? false : true); // Only shutdown writing or both
    if(keepReceiving)
        mHandler.DisableWriting(); // Keep receiving
    else
        mHandler.Detach();
}

// Send data over the connection,
// Directly sent or buffered
bool TcpConnection::Send(void* p, size_t n)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(p, n);
    }
    else
    {
        mLoop->Invoke(std::bind(&TcpConnection::SendInLoop, this, std::make_shared<ByteBuffer>(p, n)));
    }
    return true;
}

// Send data
// The actual data sending must be done on the thread loop 
// to ensure the order of data sending
bool TcpConnection::Send(ByteStream* buf)
{
    if(mLoop->IsInLoopThread())
    {
        DoSend(buf->Read(), buf->Readable());
        buf->Clear(); // ??? Is it necessary to clear buffer?
    }
    else
    {
        mLoop->Invoke(std::bind(&TcpConnection::SendInLoop, this, std::make_shared<ByteBuffer>(buf)));
        buf->Clear(); // ??? 
    }
    return true;
}

// Thread loop will invoke this function
void TcpConnection::SendInLoop(ByteBufferPtr buf)
{
    DoSend(buf->Read(), buf->Readable());
}

// This function must be called in the thread
void TcpConnection::DoSend(void* p, size_t n)
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
        mHandler.EnableWriting();
    }
}

// Ready to write
// Try to send data in out buffer
void TcpConnection::OnWrite()
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

NET_BASE_END

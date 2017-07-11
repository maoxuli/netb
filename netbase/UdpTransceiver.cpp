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

#include "UdpTransceiver.hpp"
#include "EventLoop.hpp"

NET_BASE_BEGIN

UdpTransceiver::UdpTransceiver(EventLoop* loop, sa_family_t family)
: mLoop(loop)
, mAddress()
, mSocket(family, SOCK_DGRAM, IPPROTO_UDP)
, mHandler(loop, mSocket)
{
    mHandler.SetReadCallback(std::bind(&UdpTransceiver::OnRead, this));
    mHandler.SetWriteCallback(std::bind(&UdpTransceiver::OnWrite, this));   
}

UdpTransceiver::UdpTransceiver(EventLoop* loop, const char* host, unsigned short port, sa_family_t family)
: mLoop(loop)
, mAddress(host, port, family)
, mSocket(family, SOCK_DGRAM, IPPROTO_UDP)
, mHandler(loop, mSocket)
{
    mHandler.SetReadCallback(std::bind(&UdpTransceiver::OnRead, this));
    mHandler.SetWriteCallback(std::bind(&UdpTransceiver::OnWrite, this));   
}

UdpTransceiver::UdpTransceiver(EventLoop* loop, const SocketAddress& addr)
: mLoop(loop)
, mAddress(addr)
, mSocket(addr.Family(), SOCK_DGRAM, IPPROTO_UDP)
, mHandler(loop, mSocket)
{
    mHandler.SetReadCallback(std::bind(&UdpTransceiver::OnRead, this));
    mHandler.SetWriteCallback(std::bind(&UdpTransceiver::OnWrite, this));   
}

UdpTransceiver::~UdpTransceiver()
{

}

bool UdpTransceiver::Open()
{
    mHandler.EnableReading();
    return true;
}

bool UdpTransceiver::Connect(const char* host, unsigned short port)
{
    return false;
}

bool UdpTransceiver::Connect(const SocketAddress& addr)
{
    return false;
}

bool UdpTransceiver::Send(StreamBuffer* buf)
{
    return false;
}

bool UdpTransceiver::Send(const void* p, size_t n)
{
    return false;
}

bool UdpTransceiver::Send(const void* p, size_t n, const SocketAddress& addr)
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

bool UdpTransceiver::Send(StreamBuffer* buf, const SocketAddress& addr)
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
    }
}

// EventHandler::EventCallback
// Read is ready
void UdpTransceiver::OnRead()
{
    // Read into Buffer 
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

void UdpTransceiver::OnWrite()
{
    // Write buffers 
    while(!mOutBuffers.empty())
    {
        BufferAddress& ba = mOutBuffers.front();
        if(Send(ba.buf, ba.addr))
        {
            delete ba.buf;
            mOutBuffers.pop();
        }
    }
}

NET_BASE_END 

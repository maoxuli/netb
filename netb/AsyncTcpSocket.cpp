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

#include "AsyncTcpSocket.hpp"
#include <cassert>

NETB_BEGIN

using namespace std::placeholders;

// With no address info
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop) noexcept
: TcpSocket()
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// With any address of given family
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, sa_family_t family) noexcept
: TcpSocket(family)
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}


// With given address
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr) noexcept
: TcpSocket(addr)
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// With externally established connection
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* connected) noexcept
: TcpSocket(s, connected)
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Destructor, deriviation is allowed for extension
AsyncTcpSocket::~AsyncTcpSocket() noexcept
{
    if(_handler != NULL)
    {
        delete _handler;
    }
}

bool AsyncTcpSocket::EnableReading(Error* e) noexcept
{
    assert(_loop != NULL);
    assert(GetSocket() != INVALID_SOCKET);
    // Init read event handler
    if(_handler == NULL)
    try
    {
        _handler = new EventHandler(_loop, GetSocket());
        _handler->SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncTcpSocket::OnWrite, this, _1));
    }
    catch(std::bad_alloc&)
    {
        SET_ERROR(e, "New EventHandler failed.", 0);
        _handler = NULL;
        return false;
    }
    assert(_handler != NULL);
    // Set socket to non-block
    if(!TcpSocket::Block(0, e))
    {
        return false;
    }
    // Register interested event 
    _handler->EnableReading();
    return true;
}

bool AsyncTcpSocket::EnableWriting(Error* e) noexcept
{
    assert(_loop != NULL);
    assert(GetSocket() != INVALID_SOCKET);
    // Init read event handler
    if(_handler == NULL)
    try
    {
        _handler = new EventHandler(_loop, GetSocket());
        _handler->SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncTcpSocket::OnWrite, this, _1));
    }
    catch(std::bad_alloc&)
    {
        SET_ERROR(e, "New EventHandler failed.", 0);
        _handler = NULL;
        return false;
    }
    assert(_handler != NULL);
    // Set socket to non-block
    if(!TcpSocket::Block(0, e))
    {
        return false;
    }
    // Register interested event 
    _handler->EnableWriting();
    return true;
}

void AsyncTcpSocket::Connected() 
{
    Error e;
    if(!EnableReading(&e))
    {
        THROW_ERROR(e);
    }
}

bool AsyncTcpSocket::Connected(Error* e) noexcept 
{
    return TcpSocket::Connected(e) && EnableReading(e);
}

void AsyncTcpSocket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e))
    {
        THROW_ERROR(e);
    }
}

bool AsyncTcpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    return (TcpSocket::Connect(addr, e) && EnableReading(e));
}

bool AsyncTcpSocket::Close(Error* e) noexcept
{
    if(IsConnected())
    {
        TcpSocket::Close();
        assert(_handler != NULL);
        _handler->Detach();
    }
    return true;
}

// Async mode I/O
// Send will buffer the data in sending buffer if it can not be send out immediately. 
// The return value is number of bytes that was sent or buffered. 
// The actual number of bytes that was sent out is notified with SentCallback.
// return a number less than the data size, indicate the sending buffer is full.
// return value is less than 0, indicate errors occurred. 
ssize_t AsyncTcpSocket::Send(const void* p, size_t n, int flags, Error* e) noexcept
{
    if(_loop->IsInLoopThread())
    {
        ssize_t sent = 0;
        if(_out_buffer.Empty())
        {
            sent = TcpSocket::Send(p, n, flags, e);
        }
        if(sent < n) // async send
        {
            size_t offset = sent > 0 ? sent : 0;
            if(_out_buffer.Write((unsigned char*)p + offset, n - offset))
            {
                sent  = n;
            }
            EnableWriting();
        }
        return sent;
    }
    std::unique_lock<std::mutex> lock(_out_buffer_mutex);
    if(!_out_buffer.Write((unsigned char*)p, n))
    {
        return 0;
    }
    EnableWriting();
    return n;
}

// Send data
// The actual data sending must be done on the thread loop 
// to ensure the order of data sending
ssize_t AsyncTcpSocket::Send(StreamBuffer* buf, int flags, Error* e) noexcept
{
    ssize_t ret = Send(buf->Read(), buf->Readable(), flags, e);
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

// Ready to read
// Read data into in buffer
void AsyncTcpSocket::OnRead(SOCKET s) noexcept
{
    ssize_t n = 0;
    if(_in_buffer.Writable(2048))
    {
        n = TcpSocket::Receive(_in_buffer.Write(), _in_buffer.Writable());
    }
    if(n > 0)
    {
        _in_buffer.Write(n);
        if(_received_callback)
        {
            _received_callback(this, &_in_buffer);
        }
    }
    else // Error
    {
        std::cout << "AsyncTcpSocket::OnRead return " << n << ", code: " << ErrorCode::Current() << ". Close!\n";
        TcpSocket::Close();
    }
}

// Ready to write
// Try to send data in out buffer
void AsyncTcpSocket::OnWrite(SOCKET s) noexcept
{
    if(_out_buffer.Readable() > 0)
    {
        ssize_t sent = TcpSocket::Send(_out_buffer.Read(), _out_buffer.Readable());
        if(sent > 0) _out_buffer.Read(sent);
    }
    if(_out_buffer.Readable() == 0)
    {
        assert(_handler != NULL);
        _handler->DisableWriting();
    }
}

NETB_END

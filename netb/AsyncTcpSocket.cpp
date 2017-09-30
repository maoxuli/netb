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

// std::placeholders::_1, _2, ...
using namespace std::placeholders;

// Any local address, family is given by connected address
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop) noexcept
: TcpSocket()
, _loop(loop)
, _handler(0)
{
    assert(_loop);
}

// Any local address of given family, only working in given family
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, sa_family_t family) noexcept
: TcpSocket(family)
, _loop(loop)
, _handler(0)
{
    assert(_loop);
}


// Fixed local address, only working in the family of given address
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr, bool reuse_addr, bool reuse_port) noexcept
: TcpSocket(addr, reuse_addr, reuse_port)
, _loop(loop)
, _handler(0)
{
    assert(_loop);
}

// Externally established connection with connected address
AsyncTcpSocket::AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* addr) noexcept
: TcpSocket(s, addr)
, _loop(loop)
, _handler(0)
{
    assert(_loop);
}

// Destructor
AsyncTcpSocket::~AsyncTcpSocket() noexcept
{
    // Isolate from event loop
    if(_handler)
    {
        _handler->Detach(); // block until done
        delete _handler;
        _handler = 0;
    }
}

// Init async I/O events handler
// Called after connection is established and receiving is ready 
// or buffered sending is ready 
bool AsyncTcpSocket::InitHandler(Error* e)
{
    if(!_loop)
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::InitHandler : Event loop is not set.", ErrorCode::INVAL);
        return false;
    }
    if(!Socket::Valid())
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::InitHandler : Socket is not opened yet.", ErrorCode::BADF);
        return false;
    }
    if(!Socket::Block(false, e))
    {
        return false;
    }
    if(!_handler)
    {
        _handler = new (std::nothrow) EventHandler(_loop, GetSocket());
        if(!_handler)
        {
            SET_RUNTIME_ERROR(e, "AsyncTcpSocket::InitHandler : New event handler failed.", ErrorCode::NOMEM);
            return false;
        }
        _handler->SetReadCallback(std::bind(&AsyncTcpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncTcpSocket::OnWrite, this, _1));
    }
    return true;
}

// Register I/O events to enable reading
bool AsyncTcpSocket::EnableReading(Error* e)
{
    if(!InitHandler(e))
    {
        return false;
    }
    return _handler->EnableReading();
}

// Reister I/O events to enable writing
bool AsyncTcpSocket::EnableWriting(Error* e)
{
    if(!InitHandler(e))
    {
        return false;
    }
    return _handler->EnableWriting();
}

// Set status for externally established connection
// Enable async facility on success
bool AsyncTcpSocket::Connected(Error* e) noexcept 
{
    return TcpSocket::Connected(e) && EnableReading(e);
}

// Actively connect to remote address, in block mode
// Enable async facility on success
bool AsyncTcpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    if(!TcpSocket::Connect(addr, e) || !EnableReading(e))
    {
        Close(); // clean on failure
        return false;
    }
    return true;
}

// Actively connect to remote address, in non-block mode with timeout
// timeout of -1 for async mode
// Enable async facility on success
// Todo: async mode, enable async waiting for connected events and notify
bool AsyncTcpSocket::Connect(const SocketAddress& addr, int timeout, Error* e) noexcept
{
    if(timeout < 0)
    {
        return Connect(addr, e);
    }
    if(!TcpSocket::Connect(addr, timeout, e) || !EnableReading(e))
    {
        Close(); // Clean on failure
        return false;
    }
    return true;
}

// Close the connection
// Clean async facility
bool AsyncTcpSocket::Close(Error* e) noexcept
{
    // Isolate from event loop first
    if(_handler != nullptr)
    {
        _handler->Detach(); // block until done
        delete _handler;
        _handler = nullptr;
    }
    return TcpSocket::Close(e);
}

// Async mode I/O
// Send will buffer the data in sending buffer if it can not be send out immediately. 
// The return value is number of bytes that was sent or buffered. 
// The actual number of bytes that was sent out is notified with SentCallback.
// return a number less than the data size, indicate the sending buffer is full.
// return value is less than 0, indicate errors occurred. 
ssize_t AsyncTcpSocket::Send(const void* p, size_t n, Error* e) noexcept
{
    assert(_loop != nullptr);
    if(_loop->IsInLoopThread())
    {
        ssize_t sent = 0;
        if(_out_buffer.Empty())
        {
            sent = TcpSocket::Send(p, n, 0, e); // non-block send
        }
        if(sent < n) // buffered left data
        {
            size_t off = sent < 0 ? 0 : sent;
            if(_out_buffer.Write((char*)p + off, n - off))
            {
                sent = n;
            }
        }
        if(!_out_buffer.Empty())
        {
            EnableWriting();
        }
        return sent;
    }
    std::unique_lock<std::mutex> lock(_out_buffer_mutex);
    if(!_out_buffer.Write((char*)p, n))
    {
        return 0;
    }
    EnableWriting();
    return n;
}

// Overloading this function for send out the data from received callback
ssize_t AsyncTcpSocket::Send(StreamBuffer* buf, Error* e) noexcept
{
    return Send(*buf, e);
}

// Ready to read
// Read data into in buffer and notify
void AsyncTcpSocket::OnRead(SOCKET s)
{
    ssize_t n = 0;
    if(_in_buffer.Writable(2048))
    {
        n = Socket::Receive(_in_buffer.Write(), _in_buffer.Writable());
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
        assert(_handler);
        _handler->DisableReading();
        if(_connected_callback)
        {
            _connected_callback(this, false);
        }
    }
}

// Ready to write
// Try to send data in out buffer and notify
void AsyncTcpSocket::OnWrite(SOCKET s)
{
    if(_out_buffer.Readable() > 0)
    {
        ssize_t sent = Socket::Send(_out_buffer.Read(), _out_buffer.Readable());
        if(sent > 0) _out_buffer.Read(sent);
    }
    if(_out_buffer.Readable() == 0)
    {
        assert(_handler != nullptr);
        _handler->DisableWriting();
    }
}

NETB_END

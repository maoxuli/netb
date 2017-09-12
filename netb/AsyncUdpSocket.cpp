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

#include "AsyncUdpSocket.hpp"
#include <cassert>

NETB_BEGIN

// std::placeholders::_1, _2, ...
using namespace std::placeholders;

// Any family and address determined by following operations
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop) noexcept
: UdpSocket()
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Fixed family, only working for given family
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, sa_family_t family) noexcept
: UdpSocket(family)
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Fixed local address
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr) noexcept
: UdpSocket(addr)
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Destructor
AsyncUdpSocket::~AsyncUdpSocket() noexcept
{
    // Isolate from event loop 
    if(_handler)
    {
        _handler->Detach(); // block until done
        delete _handler;
        _handler = nullptr;
    }
    // Clear sending buffers
    while(!_out_buffers.empty())
    {
        BufferAddress& ba = _out_buffers.front();
        delete ba.buf;
        _out_buffers.pop();
    }
}

// Init async facility
bool AsyncUdpSocket::InitHandler(Error* e)
{
    if(!_loop)
    {
        SET_LOGIC_ERROR(e, "Event loop is not set.");
        return false;
    }
    if(!Socket::Valid())
    {
        SET_LOGIC_ERROR(e, "Socket is not opened.");
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
            SET_LOGIC_ERROR(e, "New event handler failed.");
            return false;
        }
        _handler->SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
    }
    return true;
}

// Register async I/O events
bool AsyncUdpSocket::EnableReading(Error* e)
{
    if(!InitHandler(e)) return false;
    assert(_handler);
    _handler->EnableReading();
    return true;
}

// Register async I/O events
bool AsyncUdpSocket::EnableWriting(Error* e)
{
    if(!InitHandler(e)) return false;
    assert(_handler);
    _handler->EnableWriting();
    return true;
}

// Open to receive data
// Enable async I/O 
bool AsyncUdpSocket::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept
{
    if(!UdpSocket::Open(addr, reuse_addr, reuse_port, e) || !EnableReading(e))
    {
        Close(); // clean on failure
        return false;
    }
    return true;
}

// Close 
// clean asycn I/O
bool AsyncUdpSocket::Close(Error* e) noexcept
{
    if(_handler)
    {
        _handler->Detach(); // block until done
        delete _handler;
        _handler = nullptr;
    }
    return UdpSocket::Close(e);
}

// In async mode, data may be buffered to sending
// Todo: buffer limit, error handling
ssize_t AsyncUdpSocket::SendTo(const void* p, size_t n, const SocketAddress* addr, Error* e) noexcept
{
    // Out of thread sending, lock and buffer
    assert(_loop);
    if(!_loop->IsInLoopThread())
    {
        {
            std::unique_lock<std::mutex> lock(_out_buffers_mutex);
            _out_buffers.push(BufferAddress(new StreamBuffer(p, n), addr));
        }
        EnableWriting();
        return n;
    }
    // In thread sending
    ssize_t sent = 0;
    if(_out_buffers.empty())
    {
        sent = UdpSocket::SendTo(p, n, addr, 0, e);
    }
    if(sent < n)
    {
        _out_buffers.push(BufferAddress(new StreamBuffer(p, n), addr));
        EnableWriting();
    }
    return sent;
}

// In async mode, data may be buffered for sending
ssize_t AsyncUdpSocket::Send(const void* p, size_t n, Error* e) noexcept
{
    assert(false);
    return -1;
}

// EventHandler::EventCallback
// Read is ready
void AsyncUdpSocket::OnRead(SOCKET s)
{
    assert(s == GetSocket());
    ssize_t n = 0;
    SocketAddress addr;
    if(_in_buffer.Writable(RECEIVE_BUFFER_SIZE))
    {
        n = UdpSocket::ReceiveFrom(_in_buffer.Write(), _in_buffer.Writable(), &addr, 0);
    }
    if(n > 0)
    {
        _in_buffer.Write(n);
        if(_received_callback)
        {
            _received_callback(this, &_in_buffer, &addr);
        }
    }
    else
    {
        std::cout << "UdpSocket::OnRead() return error: " << n << "\n";
    }
}

// EventHandler::EventCallback
// Write is ready
void AsyncUdpSocket::OnWrite(SOCKET s)
{
    assert(s == GetSocket());
    std::unique_lock<std::mutex> lock(_out_buffers_mutex);
    while(!_out_buffers.empty())
    {
        BufferAddress& ba = _out_buffers.front();
        UdpSocket::SendTo(ba.buf, &ba.addr, 0);
        if(!ba.buf->Empty())
        {
            break;
        }
        delete ba.buf;
        _out_buffers.pop();
    }
    if(_out_buffers.empty())
    {
        assert(_handler);
        _handler->DisableWriting();
    }
}

NETB_END 

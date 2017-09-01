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

NET_BASE_BEGIN

using namespace std::placeholders;

// Domain is not given, so socket is not created 
// until calling of Open(), Connect(), or SendTo()
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop) noexcept
: UdpSocket()
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Socket is created for given domain
// Address is set to "any" host and port for the given domain
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, sa_family_t family) noexcept
: UdpSocket(family)
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Socket is created for the domain of given address
AsyncUdpSocket::AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr) noexcept
: UdpSocket(addr)
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Destructor, deriviation is allow for extension
AsyncUdpSocket::~AsyncUdpSocket() noexcept
{
    // Detach event handler from event loop to avoid further callback
    if(_handler)
    {
        _handler->Detach(); // block until done
        delete _handler;
    }

    // Clear sending buffer
    while(!_out_buffers.empty())
    {
        BufferAddress& ba = _out_buffers.front();
        delete ba.buf;
        _out_buffers.pop();
    }
}

bool AsyncUdpSocket::EnableReading(Error* e) noexcept
{
    assert(GetSocket() != INVALID_SOCKET);
    if(_handler == NULL)
    {
        try
        {
            _handler = new EventHandler(_loop, GetSocket());
        }
        catch(std::bad_alloc&)
        {
            SET_ERROR(e, "New event handler failed.", 0);
            _handler = NULL;
            return false;
        }
        assert(_handler != NULL);
        _handler->SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
    }
    assert(_handler != NULL);
    _handler->EnableReading();
    return true;
}

bool AsyncUdpSocket::EnableWriting(Error* e) noexcept
{
    assert(UdpSocket::GetSocket() != INVALID_SOCKET);
    if(_handler == NULL)
    {
        try
        {
            _handler = new EventHandler(_loop, GetSocket());
        }
        catch(std::bad_alloc&)
        {
            SET_ERROR(e, "New event handler failed.", 0);
            _handler = NULL;
            return false;
        }
        assert(_handler != NULL);
        _handler->SetReadCallback(std::bind(&AsyncUdpSocket::OnRead, this, _1));
        _handler->SetWriteCallback(std::bind(&AsyncUdpSocket::OnWrite, this, _1));
    }
    assert(_handler != NULL);
    _handler->EnableWriting();
    return true;
}

// Open to receive data
void AsyncUdpSocket::Open()
{
    Error e;
    if(!Open(&e))
    {
        THROW_ERROR(e);
    }
}

bool AsyncUdpSocket::Open(Error* e) noexcept
{
    return UdpSocket::Open(e) && EnableReading(e);
}

void AsyncUdpSocket::Open(const SocketAddress& addr)
{
    Error e;
    if(!Open(addr, &e))
    {
        THROW_ERROR(e);
    }
}

bool AsyncUdpSocket::Open(const SocketAddress& addr, Error* e) noexcept
{
    return UdpSocket::Open(addr, e) && EnableReading(e);
}

ssize_t AsyncUdpSocket::SendTo(const void* p, size_t n, const SocketAddress* addr, int flags) noexcept
{
    // Out of thread sending, lock and buffer
    if(!_loop->IsInLoopThread())
    {
        std::unique_lock<std::mutex> lock(_out_buffers_mutex);
        _out_buffers.push(BufferAddress(new StreamBuffer(p, n), addr));
        EnableWriting();
        return n;
    }
    ssize_t sent = 0;
    if(_out_buffers.empty())
    {
        sent = UdpSocket::SendTo(p, n, addr, flags);
    }
    if(sent < n)
    {
        _out_buffers.push(BufferAddress(new StreamBuffer(p, n), addr));
        EnableWriting();
    }
    return sent;
}

ssize_t AsyncUdpSocket::SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags) noexcept
{
    ssize_t sent = SendTo(buf->Read(), buf->Readable(), addr);
    if(sent > 0)
    {
        buf->Read(sent);
    }
    return sent;
}

// EventHandler::EventCallback
// Read is ready
void AsyncUdpSocket::OnRead(SOCKET s) noexcept
{
    assert(s == GetSocket());
    ssize_t n = 0;
    SocketAddress addr;
    if(_in_buffer.Writable(2048))
    {
        n = UdpSocket::ReceiveFrom(_in_buffer.Write(), _in_buffer.Writable(), &addr);
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

void AsyncUdpSocket::OnWrite(SOCKET s) noexcept
{
    assert(s == GetSocket());
    std::unique_lock<std::mutex> lock(_out_buffers_mutex);
    while(!_out_buffers.empty())
    {
        BufferAddress& ba = _out_buffers.front();
        SendTo(ba.buf, &ba.addr);
        if(!ba.buf->Empty())
        {
            break;
        }
        delete ba.buf;
        _out_buffers.pop();
    }
}

NET_BASE_END 

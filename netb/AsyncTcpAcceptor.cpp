/*
 * Copyright (C) 2010-2015, Maoxu Li. http://maoxuli.com/dev
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

#include "AsyncTcpAcceptor.hpp"

NETB_BEGIN

// std::placeholders::_1, _2, ...
using namespace std::placeholders;

// Dynamic address
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop) noexcept
: TcpAcceptor()
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Fixed family with any or dynamic address
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop, sa_family_t family) noexcept
: TcpAcceptor(family)
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Fixed address
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr, bool reuse_addr, bool reuse_port) noexcept
: TcpAcceptor(addr, reuse_addr, reuse_port)
, _loop(loop)
, _handler(nullptr)
{
    assert(_loop);
}

// Destructor
AsyncTcpAcceptor::~AsyncTcpAcceptor() noexcept
{
    // Isolate from event loop before destroyed
    if(_handler)
    {
        _handler->Detach(); // block until done
        delete _handler;
        _handler = nullptr;
    }
}

// Register I/O events to enable async reading
bool AsyncTcpAcceptor::EnableReading(Error* e)
{
    if(!_loop)
    {
        SET_LOGIC_ERROR(e, "AsyncTcpAcceptor::EnableReading : Event loop is not set.", ErrorCode::INVAL);
        return false;
    }
    if(!_accepted_callback) 
    {
        SET_LOGIC_ERROR(e, "AsyncTcpAcceptor::EnableReading : Accepted callback is not set.", ErrorCode::INVAL);
        return false;
    }
    if(!Socket::Valid())
    {
        SET_LOGIC_ERROR(e, "AsyncTcpAcceptor::EnableReading : Socket is not opened yet.", ErrorCode::BADF);
        return false;
    }
    // always set to non-block for async mode
    // It is not necessary to set here, but can catch errors before actual I/O
    if(!Socket::Block(false, e)) 
    {
        return false;
    }
    if(!_handler)
    {
        _handler = new (std::nothrow) EventHandler(_loop, GetSocket());
        if(!_handler)
        {
            SET_RUNTIME_ERROR(e, "AsyncTcpAcceptor::EnableReading : New EventHandler failed.", ErrorCode::NOMEM);
            return false;
        }
        _handler->SetReadCallback(std::bind(&AsyncTcpAcceptor::OnRead, this, _1));
    }
    assert(_handler);
    _handler->EnableReading();
    return true;
}

// Open
// Enable async mode to accept incomming connections
bool AsyncTcpAcceptor::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept
{
    if(!TcpAcceptor::Open(addr, reuse_addr, reuse_port, e) || !EnableReading(e))
    {
        Close(); // clean on failure
        return false;
    }
    return true;
}

// Close
// Clean async facility
bool AsyncTcpAcceptor::Close(Error* e) noexcept
{
    // First isolate from event loop
    if(_handler != nullptr)
    { 
        _handler->Detach(); // block until done
        delete _handler;
        _handler = nullptr;
    }
    return TcpAcceptor::Close(e);
}

// EventHandler::ReadCallbck
// Called when TCP socket is ready to accept a incomming connection
void AsyncTcpAcceptor::OnRead(SOCKET s)
{
    assert(_accepted_callback);
    assert(s == GetSocket());
    SocketAddress in_addr;
    SOCKET in_s = Socket::AcceptFrom(&in_addr); // non-block and ignore errors
    if(in_s != INVALID_SOCKET)
    { 
        if(!_accepted_callback || !_accepted_callback(this, in_s, &in_addr))
        {
            CloseSocket(in_s); // clean on callback failure
        }
    }
}

NETB_END

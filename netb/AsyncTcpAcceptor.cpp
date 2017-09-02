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

#include "AsyncTcpAcceptor.hpp"

NETB_BEGIN

using namespace std::placeholders;

// Constructor, with local address info
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop) noexcept
: TcpAcceptor() // noexcept
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Constructor, with local address info
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop, sa_family_t family) noexcept
: TcpAcceptor(family) // rethrow by default
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Constructor, with local address info
AsyncTcpAcceptor::AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr) noexcept
: TcpAcceptor(addr) // rethrow by default
, _loop(loop)
, _handler(NULL)
{
    assert(_loop != NULL);
}

// Destructor
AsyncTcpAcceptor::~AsyncTcpAcceptor() noexcept
{
    if(_handler != NULL)
    {
        _handler->Detach(); // isolate from loop, block until done
        delete _handler;
        _handler = NULL;
    }
}

// Enalbe aysnc after creating socket
bool AsyncTcpAcceptor::EnableReading(Error* e) noexcept
{
    assert(_loop != NULL);
    assert(GetSocket() != INVALID_SOCKET);
    // Init read event handler
    if(_handler == NULL)
    try
    {
        _handler = new EventHandler(_loop, GetSocket());
        _handler->SetReadCallback(std::bind(&AsyncTcpAcceptor::OnRead, this, _1));
    }
    catch(std::bad_alloc&)
    {
        SET_ERROR(e, "New EventHandler failed.", 0);
        _handler = NULL;
        return false;
    }
    assert(_handler != NULL);
    // Set socket to non-block
    if(!Block(false, e))
    {
        return false;
    }
    // Register interested event 
    _handler->EnableReading();
    return true;
}

// Open for accepting connections
void AsyncTcpAcceptor::Open()
{
    Error e;
    if(!Open(&e))
    {
        THROW_ERROR(e);
    }
}

// Open for accepting connection
bool AsyncTcpAcceptor::Open(Error* e) noexcept
{
    if(!TcpAcceptor::Open(e))
    {
        return false;
    }
    return EnableReading(e);
}

// Open on given address, start to accept connection
void AsyncTcpAcceptor::Open(const SocketAddress& addr)
{
    Error e;
    if(!Open(addr, &e))
    {
        THROW_ERROR(e);
    }
}

// Open on given address, start to accept connection
bool AsyncTcpAcceptor::Open(const SocketAddress& addr, Error* e) noexcept
{
    if(!TcpAcceptor::Open(addr, e))
    {
        return false;
    } 
    return EnableReading(e);
}

bool AsyncTcpAcceptor::Close(Error* e) noexcept
{
    if(_handler)
    {
        _handler->Detach();
    }
    return TcpAcceptor::Close(e);
}

// EventHandler::ReadCallbck
// Called when TCP socket is ready to accept a incomming connection
void AsyncTcpAcceptor::OnRead(SOCKET s) noexcept
{
    assert(_accepted_callback);
    assert(s == GetSocket());
    SocketAddress in_addr;
    SOCKET in_s = TcpAcceptor::AcceptFrom(&in_addr);
    if(in_s != INVALID_SOCKET)
    { 
        if(!_accepted_callback || !_accepted_callback(this, in_s, &in_addr))
        {
            CloseSocket(in_s);
        }
    }
}

NETB_END

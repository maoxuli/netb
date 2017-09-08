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

#include "TcpAcceptor.hpp"
#include "SocketSelector.hpp"
#include <cassert>

NETB_BEGIN

// No given address
TcpAcceptor::TcpAcceptor() noexcept
: _timeout(-1) // by dedault in block mode
, _backlog(SOMAXCONN)
, _opened(false)
{

}

// Any address of given family
TcpAcceptor::TcpAcceptor(sa_family_t family) noexcept
: _timeout(-1) // by dedault in block mode
, _backlog(SOMAXCONN)
, _opened(false)
{
    _address.Reset(family); // noexcept
}

// given address
TcpAcceptor::TcpAcceptor(const SocketAddress& addr) noexcept
: _address(addr) // noexcept
, _timeout(-1) // by dedault in block mode
, _backlog(SOMAXCONN)
, _opened(false)
{

}

TcpAcceptor::~TcpAcceptor() noexcept
{

}

// Open on saved address
void TcpAcceptor::Open()
{
    Error e;
    if(!Open(&e))
    {
        THROW_ERROR(e);
    }
}

// Open on saved address
bool TcpAcceptor::Open(Error* e) noexcept
{
    // Always let caller close 
    if(_opened)
    {
        SET_LOGIC_ERROR(e, "Has been opened already.");
        return false;
    }
    // Open on initial address
    SocketAddress addr(_address); 
    if(addr.Empty()) // If no initial address, any address for default family
    {
        addr.Reset(AF_INET); // noexcept
    }
    return Open(addr, e);
}

// Open on given address
void TcpAcceptor::Open(const SocketAddress& addr)
{
    Error e;
    if(!Open(addr, &e))
    {
        THROW_ERROR(e);
    }
}

// Open on given address
bool TcpAcceptor::Open(const SocketAddress& addr, Error* e) noexcept
{
    // Always let caller close 
    if(_opened)
    {
        SET_LOGIC_ERROR(e, "Has been opened already.");
        return false;
    }

    // if socket is not yet init, do it now
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    assert(!Socket::Valid());

    // bind address and listen
    return Socket::Bind(addr, e) && Socket::Listen(_backlog, e);
}

// Close socket, stop to accept incomming connection
bool TcpAcceptor::Close(Error* e) noexcept
{
    _opened = false;
    return Socket::Close(e); 
}

SocketAddress TcpAcceptor::Address(Error* e) const noexcept
{
    if(_opened)
    {
        return Socket::Address();
    }
    return _address;
}

// Accept a connection
SOCKET TcpAcceptor::Accept()
{
    Error e;
    SOCKET s;
    if((s = Accept(&e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

SOCKET TcpAcceptor::Accept(Error* e) noexcept
{
    if(!_opened)
    {
        SET_LOGIC_ERROR(e, "Not opened yet.");
        return INVALID_SOCKET;
    }
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, e))
    {
        return INVALID_SOCKET;
    }
    return Socket::Accept(e);
}

SOCKET TcpAcceptor::AcceptFrom(SocketAddress* addr)
{
    Error e;
    SOCKET s;
    if((s = AcceptFrom(addr, &e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

// Accept a connection
SOCKET TcpAcceptor::AcceptFrom(SocketAddress* addr, Error* e) noexcept
{
    if(addr == NULL)
    {
        return Accept(e);
    }
    if(!_opened)
    {
        SET_LOGIC_ERROR(e, "Not opened yet.");
        return INVALID_SOCKET;
    }
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, e))
    {
        return INVALID_SOCKET;
    }
    return Socket::AcceptFrom(addr, e);
}

/////////////////////////////////////////////////////////////////////////////

// Backlog for listen
void TcpAcceptor::Backlog(int size)
{
    Error e;
    if(!Backlog(size, &e))
    {
        THROW_ERROR(e);
    }
}

// Backlog for listen
bool TcpAcceptor::Backlog(int size, Error* e) noexcept
{
    if(size < 1)
    {
        SET_LOGIC_ERROR(e, "Invalid backlog for TCP socket.");
        return false;
    } 
    _backlog = size;
    return true;
}

// I/O block
void TcpAcceptor::Block(int timeout) 
{
    Error e;
    if(!Block(timeout, &e))
    {
        THROW_ERROR(e);
    }
}

// -1: block, 0: non-block, >0: block with timeout
bool TcpAcceptor::Block(int timeout, Error* e) noexcept
{
    _timeout = timeout;
    return Socket::Block(timeout < 0 ? true : false, e);
}

// Option
void TcpAcceptor::ReuseAddress(bool reuse) // default is false
{
    Error e;
    if(!ReuseAddress(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

bool TcpAcceptor::ReuseAddress(bool reuse, Error* e) noexcept
{
    return Socket::ReuseAddress(reuse, e);
}

// Option
void TcpAcceptor::ReusePort(bool reuse)  // default is false
{
    Error e;
    if(!ReusePort(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

bool TcpAcceptor::ReusePort(bool reuse, Error* e) noexcept
{
    return Socket::ReusePort(reuse, e);
}

NETB_END

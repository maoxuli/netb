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

// dynamic address
TcpAcceptor::TcpAcceptor() noexcept
: _address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
, _backlog(-1)
{

}

// fixed family, with any address or dynamic address
TcpAcceptor::TcpAcceptor(sa_family_t family) noexcept
: _address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
, _backlog(-1)
{
    // fixed family with any address
    _address.Reset(family);
}

// fixed address
TcpAcceptor::TcpAcceptor(const SocketAddress& addr, bool reuse_addr, bool reuse_port) noexcept
: _address(addr)
, _reuse_addr(reuse_addr)
, _reuse_port(reuse_port)
, _backlog(-1)
{

}

TcpAcceptor::~TcpAcceptor() noexcept
{

}

// Backlog for listen, -1 for default
void TcpAcceptor::SetBacklog(int backlog) noexcept
{
    _backlog = backlog;
}

// Open with fix address or fix family with any address
void TcpAcceptor::Open()
{
    Error e;
    if(!Open(&e)) 
    {
        THROW_ERROR(e);
    }
}

// Open with fix address or fix family with any address
bool TcpAcceptor::Open(Error* e) noexcept
{
    if(_address.Empty()) // no initial address or family
    {
        SET_LOGIC_ERROR(e, "Address or family not assigned before opening");
        return false;
    }
    return Open(_address, _reuse_addr, _reuse_port, e);
}

// Open with dynamic address, or fixed family with dynamic address
void TcpAcceptor::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port)
{
    Error e;
    if(!Open(addr, reuse_addr, reuse_port, &e)) 
    {
        THROW_ERROR(e);
    }
}

// Open with dynamic address, or fixed family with dynamic address
// with default reuse rule
bool TcpAcceptor::Open(const SocketAddress& addr, Error* e) noexcept
{
    return Open(addr, true, true, e);
}

// Open with dynamic address, or fixed family with dynamic address
bool TcpAcceptor::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept
{
    if(!_address.Empty() && addr != _address) // initial address vs given address
    {
        if(!_address.Any() || addr.Family() != _address.Family())
        {
            SET_LOGIC_ERROR(e, "Given address is not qualified for initial.");
            return false;
        }
    }
    // Open socket
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    // set reuse rules beore bind
    if(!Socket::ReuseAddress(reuse_addr, e) || !Socket::ReusePort(reuse_port, e))
    {
        return false;
    }
    // bind address and listen
    return Socket::Bind(addr, e) && Socket::Listen(_backlog, e);
}

// Close and be ready to open again
bool TcpAcceptor::Close(Error* e) noexcept
{
    return Socket::Close(e); 
}

// bound address or given address before opened
SocketAddress TcpAcceptor::Address(Error* e) const noexcept
{
    if(Socket::Valid())
    {
        return Socket::Address(e);
    }
    return _address;
}

// Accept a connection, in block mode
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

// Accept a connection, in block mode
SOCKET TcpAcceptor::Accept(Error* e) noexcept
{
    if(!Socket::Block(true, e)) // set to block mode
    {
        return INVALID_SOCKET;
    }
    return Socket::Accept(e); // block mode
}

// Accept a connection, in non-block mode with timeout
SOCKET TcpAcceptor::Accept(int timeout)
{
    Error e;
    SOCKET s;
    if((s = Accept(timeout, &e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

// Accept a connection, in non-block mode with timeout
SOCKET TcpAcceptor::Accept(int timeout, Error* e) noexcept
{
    if(timeout < 0) // block mode
    {
        return Accept(e);
    }
    if(!Socket::Block(false, e)) // set to non-block mode
    {
        return INVALID_SOCKET;
    }
    // Here we check ready status first and then accept in non-block mode. 
    // A more official flow is accept in non-block mode first, and then 
    // check the status, if it is EWOULDBLOCK or EAGIN, then check ready 
    // in timeout time, and accept again on ready event. 
    if(timeout > 0 && !Socket::WaitForRead(timeout, e)) // timout
    {
        return INVALID_SOCKET;
    }
    return Socket::Accept(e); // non-block mode
}

// Accept a connection, in block mode
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

// Accept a connection, in block mode
SOCKET TcpAcceptor::AcceptFrom(SocketAddress* addr, Error* e) noexcept
{
    if(!Socket::Block(true, e)) // set to block mode
    {
        return INVALID_SOCKET;
    }
    return Socket::AcceptFrom(addr, e); // block mode
}

// Accept a connection, in non-block mode with timeout
SOCKET TcpAcceptor::AcceptFrom(SocketAddress* addr, int timeout)
{
    Error e;
    SOCKET s;
    if((s = AcceptFrom(addr, timeout, &e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

// Accept a connection, in non-block mode with timeout
SOCKET TcpAcceptor::AcceptFrom(SocketAddress* addr, int timeout, Error* e) noexcept
{
    if(timeout < 0) // block mode
    {
        return AcceptFrom(addr, e);
    }
    if(!Socket::Block(false, e)) // set to non-block mode
    {
        return INVALID_SOCKET;
    }
    // Here we check ready status first and then accept in non-block mode. 
    // A more official flow is accept in non-block mode first, and then 
    // check the status, if it is EWOULDBLOCK or EAGIN, then check ready 
    // in timeout time, and accept again on ready event. 
    if(timeout > 0 && !Socket::WaitForRead(timeout, e)) // timout
    {
        return INVALID_SOCKET;
    }
    return Socket::AcceptFrom(addr, e); // non-block mode
}

NETB_END

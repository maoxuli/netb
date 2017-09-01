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

#include "UdpSocket.hpp"
#include "SocketSelector.hpp"
#include <cassert>

NET_BASE_BEGIN

// Constructor, with no address info
UdpSocket::UdpSocket() noexcept
: _timeout(-1)
, _opened(false)
, _connected(false)
{

}

// Constructor, with address family
UdpSocket::UdpSocket(sa_family_t family) noexcept
: _timeout(-1)
, _opened(false)
, _connected(false)
{
    _address.Reset(family);
}

// Constructor, with initial local address
UdpSocket::UdpSocket(const SocketAddress& addr) noexcept
: _address(addr)
, _timeout(-1)
, _opened(false)
, _connected(false)
{

}

// Destructor
UdpSocket::~UdpSocket() noexcept
{

}

// Open on current address
// throw exception on errors
void UdpSocket::Open()
{
    Error e;
    if(!Open(&e))
    {
        THROW_ERROR(e);
    }
}

// Open on initial local address
// return false on errors
bool UdpSocket::Open(Error* e) noexcept
{
    if(_opened)
    {
        return true;
    }
    SocketAddress addr(_address);
    if(addr.Empty())
    {
        addr.Reset(AF_INET); // default family and any address
    }
    return Open(addr, e);
}

// Open on given address
// throw exception on errors
void UdpSocket::Open(const SocketAddress& addr)
{
    Error e; 
    if(!Open(addr, &e))
    {
        THROW_ERROR(e);
    }
}

// Open on given address
// return false on errors
bool UdpSocket::Open(const SocketAddress& addr, Error* e) noexcept
{
    if(_opened)
    {
        if(_address ==  addr || Address() == addr)
        {
            return true;
        }
        Close();
    }
    assert(!addr.Empty());
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    if(!Socket::Bind(addr, e))
    {
        return false;
    }
    _address = addr;
    _opened = true;
    return true;
}

// Close the opened socket
// return false on errors
bool UdpSocket::Close(Error* e) noexcept
{
    _connected = false;
    _connected_address.Reset();
    _opened = false;
    return Socket::Close(e);
}

// Get local address
// given local address or actual bound address after connected
// return empty address on errors
SocketAddress UdpSocket::Address(Error* e) const noexcept
{
    if(IsOpened())
    {
        return Socket::Address();
    }
    return _address;
}

// connect to given address
// Throw exception on errors
void UdpSocket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e))
    {
        THROW_ERROR(e);
    }
}

// Connect to given address
// Only bind to the address, no actual I/O occurred
// return false on errors
bool UdpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_DGRAM, IPPROTO_UDP, e))
    {
        return false;
    }
    if(_connected)
    {
        // In theory _connected_address == ConnectedAddress()
        // but who knows...it is not enforced here
        if(_connected_address == addr || ConnectedAddress() == addr)
        {
            return true;
        }
    }
    if(!Socket::Connect(addr, e))
    {
        return false;
    }
    _connected_address = addr;
    return true;
}

// Get connected address
// return empty address on errors
SocketAddress UdpSocket::ConnectedAddress(Error* e) const noexcept
{
    if(!_connected)
    {
        SET_SOCKET_ERROR(e, "Not connected.", 0);
        return _connected_address;
    }
    return Socket::ConnectedAddress(e);
}

// Send data to given address
// Todo: check connected status and conflict with connected address
ssize_t UdpSocket::SendTo(const void* p, size_t n, const SocketAddress* addr, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::SendTo(p, n, addr, flags);
}

// Send data to given address
// Todo: check connected status and conflict with connected address
ssize_t UdpSocket::SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::SendTo(buf, addr, flags);
}

// Send data to connected address
// Todo: check connected status
ssize_t UdpSocket::Send(const void* p, size_t n, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Send(p, n, flags);
}

// Send data to connected address
// Todo: check connected status
ssize_t UdpSocket::Send(StreamBuffer* buf, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Send(buf, flags);
}

// Receive data and get remote address
// Todo: check connected status
ssize_t UdpSocket::ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::ReceiveFrom(p, n, addr, flags);
}

// Receive data and get remote address
// Todo: check connected status
ssize_t UdpSocket::ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::ReceiveFrom(buf, addr, flags);
}

// Receive data from connected address
// Todo: check connected status
ssize_t UdpSocket::Receive(void* p, size_t n, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Receive(p, n, flags);
}

// Receive data from connected address
// Todo: check connected status
ssize_t UdpSocket::Receive(StreamBuffer* buf, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Receive(buf, flags);
}

/////////////////////////////////////////////////////////////////////////////

// I/O mode
// -1: block, 0: non-block, >0: block with timeout
// Throw exception on errors
void UdpSocket::Block(int timeout)
{
    Error e;
    if(Block(timeout, &e))
    {
        THROW_ERROR(e);
    }
}

// I/O mode
// -1: block, 0: non-block, >0: block with timeout
// default is block
// return false on errors
bool UdpSocket::Block(int timeout, Error* e) noexcept 
{
    _timeout = timeout;
    return Socket::Block(timeout < 0 ? true : false, e);
}

// Option of reuse address
// default is false
// throw exception on errors
void UdpSocket::ReuseAddress(bool reuse)
{
    Error e;
    if(!ReuseAddress(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

// Opention of reuse address
// default is false
// return false on errors
bool UdpSocket::ReuseAddress(bool reuse, Error* e) noexcept
{
    return Socket::ReuseAddress(reuse, e);
}

// Option of reuse port
// default is false
// throw exception on errors
void UdpSocket::ReusePort(bool reuse)
{
    Error e;
    if(!ReusePort(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

// Option of reuse port
// default is false
// return false on errors
bool UdpSocket::ReusePort(bool reuse, Error* e) noexcept
{
    return Socket::ReusePort(reuse, e);
}

NET_BASE_END 

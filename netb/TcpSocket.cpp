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

#include "TcpSocket.hpp"
#include "SocketSelector.hpp"
#include <cassert>

NETB_BEGIN

// Constructor, with no address info
TcpSocket::TcpSocket() noexcept
: _timeout(-1)
, _connected(false)
{

}

// Constructor, with any address of given family 
TcpSocket::TcpSocket(sa_family_t family) noexcept
: _timeout(-1)
, _connected(false)
{
    _address.Reset(family);
}

// Constructor, with given address
TcpSocket::TcpSocket(const SocketAddress& addr) noexcept
: _address(addr)
, _timeout(-1)
, _connected(false)
{

}
 
// Constructor, with externally established connection
TcpSocket::TcpSocket(SOCKET s, const SocketAddress* connected) noexcept
: Socket(s) 
, _connected_address(connected)
, _timeout(-1)
, _connected(false)
{

}

// Destructor
TcpSocket::~TcpSocket() noexcept
{

}

// Connect to given address
// throw exception on errors
void TcpSocket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e))
    {
        THROW_ERROR(e);
    }
}

// Connect to given address
// return false on errors
bool TcpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    if(_connected)
    {
        if(_connected_address == addr || ConnectedAddress() == addr) 
        {
            return true;
        }
        Close(); // Prepare for connect to new address
    }
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    if(!Socket::Connect(addr, e))
    {
        return false;
    }
    assert(false); //Todo: analyze status
    if(_timeout > 0 && !Socket::WaitForReady(_timeout, e))
    {
        // Analyze status
        return false;
    }
    _connected_address = addr;
    _connected = true;
    return true;
}

void TcpSocket::Connected()
{
    _connected = true;
}

bool TcpSocket::Connected(Error* e) noexcept
{
    _connected = true;
    return true;
}

// Local address
SocketAddress TcpSocket::Address(Error* e) const noexcept
{
    if(IsConnected())
    {
        return Socket::Address();
    }
    return _address;
}

// Connected address
SocketAddress TcpSocket::ConnectedAddress(Error* e) const noexcept
{
    if(IsConnected())
    {
        return Socket::ConnectedAddress(e);
    }
    return _connected_address;
}

// Close the socket
bool TcpSocket::Close(Error* e) noexcept
{
    _connected = false;
    _connected_address.Reset();
    return Socket::Close(e); 
}

// Send data over connection
ssize_t TcpSocket::Send(const void* p, size_t n, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Send(p, n, flags);
}

ssize_t TcpSocket::Send(StreamBuffer* buf, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForWrite(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Send(buf, flags);
}

ssize_t TcpSocket::Receive(void* p, size_t n, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Receive(p, n, flags);
}

ssize_t TcpSocket::Receive(StreamBuffer* buf, int flags) noexcept
{
    if(_timeout > 0 && !Socket::WaitForRead(_timeout, NULL))
    {
        return -1;
    }
    return Socket::Receive(buf, flags);
}

//////////////////////////////////////////////////////////////////////////////////

// I/O block
// -1: block, 0: non-block, >0: block with timeout
void TcpSocket::Block(int timeout) 
{
    Error e;
    if(!Block(timeout, &e))
    {
        THROW_ERROR(e);
    }
}

// IO mode
// -1: block, 0: non-block, >0: block with timeout
bool TcpSocket::Block(int timeout, Error* e) noexcept
{
    _timeout = timeout;
    return Socket::Block(timeout < 0 ? true : false, e);
}

// Option of reuse address
void TcpSocket::ReuseAddress(bool reuse)  
{
    Error e;
    if(!ReuseAddress(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

// Option of reuse address 
bool TcpSocket::ReuseAddress(bool reuse, Error* e) noexcept
{
    return Socket::ReuseAddress(reuse, e);
}

// Option of reuse port
void TcpSocket::ReusePort(bool reuse)  // default is false
{
    Error e;
    if(!ReusePort(reuse, &e))
    {
        THROW_ERROR(e);
    }
}

// Option of reuse port
bool TcpSocket::ReusePort(bool reuse, Error* e) noexcept
{
    return Socket::ReusePort(reuse, e);
}

// Option of no delay
void TcpSocket::NoDelay(bool no)  
{
    Error e;
    if(!NoDelay(no, &e))
    {
        THROW_ERROR(e);
    }
}

// option of no delay
bool TcpSocket::NoDelay(bool no, Error* e) noexcept
{
    int flag = no ? 1 : 0;
    return Socket::SetOption(IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int), e);
}

// Option of keep alive
void TcpSocket::KeepAlive(int time) 
{
    Error e;
    if(!KeepAlive(time, &e))
    {
        THROW_ERROR(e);
    }
}

// Option of keep alvie
bool TcpSocket::KeepAlive(int time, Error* e) noexcept
{
    int flag = time > 0 ? 1 : 0;
    if(!Socket::SetOption(SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(int), e))
    {
        return false;
    }
    if(time > 0 && !Socket::SetOption(IPPROTO_TCP, TCP_KEEPALIVE, &time, sizeof(int), e))
    {
        return false;
    }
    return true;
}

void TcpSocket::SendBuffer(int size)
{
    Error e;
    if(!SendBuffer(size, &e))
    {
        THROW_ERROR(e);
    }  
}

bool TcpSocket::SendBuffer(int size, Error* e) noexcept
{
    return Socket::SetOption(SOL_SOCKET, SO_SNDBUF, &size, sizeof(int), e);
}

void TcpSocket::ReceiveBuffer(int size)
{
    Error e;
    if(!ReceiveBuffer(size, &e))
    {
        THROW_ERROR(e);
    }
}

bool TcpSocket::ReceiveBuffer(int size, Error* e) noexcept
{
    return Socket::SetOption(SOL_SOCKET, SO_RCVBUF, &size, sizeof(int), e);
}

NETB_END

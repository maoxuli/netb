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

// any local address
// dynamic family is given by connected address
TcpSocket::TcpSocket() noexcept
: _address() // empty address
, _connected_address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
{

}

// any local address
// fixed family
TcpSocket::TcpSocket(sa_family_t family) noexcept
: _address() // empty address
, _connected_address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
{
    // fixed family with any address
    _address.Reset(family);
}

// fixed local address
TcpSocket::TcpSocket(const SocketAddress& addr, bool reuse_addr, bool reuse_port) noexcept
: _address(addr)
, _connected_address() // empty address
, _reuse_addr(reuse_addr)
, _reuse_port(reuse_port)
{

}
 
// Externally established connection with connected address
TcpSocket::TcpSocket(SOCKET s, const SocketAddress* addr) noexcept
: Socket(s) 
, _address() // empty address
, _connected_address(addr)
, _reuse_addr(false)
, _reuse_port(false)
{

}

// Destructor
TcpSocket::~TcpSocket() noexcept
{

}

// Set connected status for externally established connection
void TcpSocket::Connected()
{
    Error e;
    if(!Connected(&e)) 
    {
        THROW_ERROR(e);
    }
}

// Set connected status for externally established connection
bool TcpSocket::Connected(Error* e) noexcept
{
    // Check given socket type
    if(Socket::Type() != SOCK_STREAM)
    {
        SET_LOGIC_ERROR(e, "Attached is not a TCP socket.");
        return false;
    }
    // Check connectd address, will indicate connected status
    SocketAddress addr = Socket::ConnectedAddress(e);
    if(addr.Empty() && e)
    {
        return false;
    }
    // Check given conneced address
    if(!_connected_address.Empty() && _connected_address != addr)
    {
        SET_LOGIC_ERROR(e, "Attached connected address is wrong.");
        return false;
    }
    return true;
}

// Do connect in block or non-block mode
bool TcpSocket::DoConnect(const SocketAddress& addr, bool block, Error* e)
{
    if(!_address.Empty() && addr.Family() != _address.Family())
    {
        SET_LOGIC_ERROR(e, "Mismatched address family.");
        return false;
    }
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    if(!_address.Empty() && !_address.Any() && !Socket::Bind(_address, e))
    {
        return false;
    }
    if(!Socket::Block(block, e))
    {
        return false;
    }
    return Socket::Connect(addr, e);
}

// Actively connect to remote address, in block mode
void TcpSocket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e)) 
    {
        THROW_ERROR(e);
    }
}


// Actively connect to remote address, in block mode
bool TcpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    return DoConnect(addr, true, e);
}

// Actively connect to remote address, in non-block mode with timeout
// timeout of -1 for block mode
void TcpSocket::Connect(const SocketAddress& addr, int timeout)
{
    Error e;
    if(!Connect(addr, timeout, &e))
    {
        THROW_ERROR(e);
    }
}

// Actively connect to remote address, in non-block mode with timeout
// timeout of -1 for block mode
bool TcpSocket::Connect(const SocketAddress& addr, int timeout, Error* e) noexcept
{
    if(timeout < 0)
    {
        return Connect(addr, e);
    }
    Error err;
    if(!DoConnect(addr, false, &err))
    {
        if(timeout > 0) // Check status in timeout
        {
            // If the status is EINPROGRESS, check ready to write in timeout
            // More details need to check here!
            if(ErrorCode::IsInProgress(err.Code()) && WaitForWrite(timeout, &err))
            {
                return true;
            }
            // Handler status of EAGAIN
        }
        SET_ERROR(e, err.Message(), err.Code());
        SET_ERROR_CLASS(e, err.Class());
        return false;
    }
    return true;
}

// close the socket
// Return false on errors, but socket is closed anyway
// Todo: tell if the connection is externally established
bool TcpSocket::Close(Error* e) noexcept
{
    return Socket::Close(e); 
}

// Bound local address or given address before connected
SocketAddress TcpSocket::Address(Error* e) const noexcept
{
    if(Socket::Valid())
    {
        return Socket::Address(e);
    }
    return _address;
}

// Connected address
SocketAddress TcpSocket::ConnectedAddress(Error* e) const noexcept
{
    if(Socket::Valid())
    {
        return Socket::ConnectedAddress(e);
    }
    return _connected_address;
}

// Send data over connection, in block mode
ssize_t TcpSocket::Send(const void* p, size_t n, Error* e) noexcept
{
    if(!Socket::Block(true, e)) return -1;
    return Socket::Send(p, n, 0, e);
}

// Send data over connection, in block mode
ssize_t TcpSocket::Send(StreamBuffer* buf, Error* e) noexcept
{
    ssize_t ret = Send(buf->Read(), buf->Readable(), e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Send data over connection, in non-block mode with timeout
// timeout of -1 for block mode
ssize_t TcpSocket::Send(const void* p, size_t n, int timeout, Error* e) noexcept
{
    if(timeout < 0) return Send(p, n, e);
    if(!Socket::Block(false, e)) return -1;

    // Here we check ready to write status first and then write.
    // More official flow is send first, and if sending is failed 
    // wait for ready to write event in timeout and write again.
    if(timeout > 0 && !Socket::WaitForWrite(timeout, e))
    {
        return -1;
    }
    return Socket::Send(p, n, 0, e);
}

// Send data over connection, in block mode
ssize_t TcpSocket::Send(StreamBuffer* buf, int timeout, Error* e) noexcept
{
    ssize_t ret = Send(buf->Read(), buf->Readable(), timeout, e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Receive data from the connection, in block mode
ssize_t TcpSocket::Receive(void* p, size_t n, Error* e) noexcept
{
    if(!Socket::Block(true, e)) return -1;
    return Socket::Receive(p, n, 0, e);
}

// Receive data from the connection, in block mode
ssize_t TcpSocket::Receive(StreamBuffer* buf, Error* e) noexcept
{
    if(!buf->Writable(2048))
    {
        SET_LOGIC_ERROR(e, "Buffer is not enough.");
        return -1;
    }
    ssize_t ret = Receive(buf->Write(), buf->Writable(), e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

// Receive data from the connection, in non-block mode with timeout
// timeout of -1 for block mode
ssize_t TcpSocket::Receive(void* p, size_t n, int timeout, Error* e) noexcept
{
    if(timeout < 0) return Receive(p, n, e);
    if(!Socket::Block(false, e)) return -1;

    // Here we check ready to write status first and then write.
    // More official flow is send first, and if sending is failed 
    // wait for ready to write event in timeout and write again.
    if(timeout > 0 && !Socket::WaitForRead(timeout, e))
    {
        return -1;
    }
    return Socket::Receive(p, n, 0, e);
}

ssize_t TcpSocket::Receive(StreamBuffer* buf, int timeout, Error* e) noexcept
{
    if(!buf->Writable(2048))
    {
        SET_LOGIC_ERROR(e, "Buffer is not enough.");
        return -1;
    }
    ssize_t ret = Receive(buf->Write(), buf->Writable(), timeout, e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

NETB_END

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

NETB_BEGIN

// Prepare write buffer for socket receive
#define PREPARE_BUFFER(buf) 											\
		do{ 															\
			assert(buf); 												\
			if(!buf->Readable(DEFAULT_RECEIVE_BUFFER)) 					\
			{															\
				SET_LOGIC_ERROR(e, "No enough buffer for receiving.");	\
				return -1;												\
			}															\
		} while(0) // no trailing ;

// Any address determined by following operations
UdpSocket::UdpSocket() noexcept
: _address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
{

}

// Fixed family, address is determined by following operations
UdpSocket::UdpSocket(sa_family_t family) noexcept
: _address() // empty address
, _reuse_addr(false)
, _reuse_port(false)
{
    // Fixed family with any address
    _address.Reset(family);
}

// Fixed address
UdpSocket::UdpSocket(const SocketAddress& addr, bool reuse_addr, bool reuse_port) noexcept
: _address(addr)
, _reuse_addr(reuse_addr)
, _reuse_port(reuse_port)
{

}

// Destructor
UdpSocket::~UdpSocket() noexcept
{

}

// Open to receive data
// Family or local address is given on initial
void UdpSocket::Open()
{
    Error e;
    if(!Open(&e)) THROW_ERROR(e);
}

// Open to receive data
// Family or local address is given on initial
bool UdpSocket::Open(Error* e) noexcept
{
    if(_address.Empty()) // no initial address or family
    {
        SET_LOGIC_ERROR(e, "Address or family not assigned before opening.");
        return false;
    }
    return Open(_address, _reuse_addr, _reuse_port, e);
}

// Open to receive data
// Dynamic local address or fixed family with dynamic address
void UdpSocket::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port)
{
    Error e; 
    if(!Open(addr,reuse_addr, reuse_port, &e)) 
    {
        THROW_ERROR(e);
    }
}

// Open to receive data
// Dynamic local address or fixed family with dynamic address
// with default reuse rules
bool UdpSocket::Open(const SocketAddress& addr, Error* e) noexcept
{
    return Open(addr, true, true, e);
}

// Open to receive data
// Dynamic local address or fixed family with dynamic address
bool UdpSocket::Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept
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
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_DGRAM, IPPROTO_UDP, e))
    {
        return false;
    }
    // set reuse rules beore bind
    if(!Socket::ReuseAddress(reuse_addr, e) || !Socket::ReusePort(reuse_port, e))
    {
        return false;
    }
    // bind address
    return Socket::Bind(addr, e);
}

// Close and be ready to open again
// return false on errors but socket is closed anyway
bool UdpSocket::Close(Error* e) noexcept
{
    return Socket::Close(e);
}

// Local bound address or given local address
SocketAddress UdpSocket::Address(Error* e) const noexcept
{
    if(Socket::Valid())
    {
        return Socket::Address(e);
    }
    return _address;
}

// Connect to a remote address
// Empty address will remove the assocication
void UdpSocket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e)) 
    {
        THROW_ERROR(e);
    }
}

// Connect to a remote address
// NULL address will remove the assocication
bool UdpSocket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    if(!Socket::Valid() && !Socket::Create(addr.Family(), SOCK_DGRAM, IPPROTO_UDP, e))
    {
        return false;
    }
    if(!_address.Empty() && !addr.Empty() && addr.Family() != _address.Family())
    {
        SET_LOGIC_ERROR(e, "Mismatched address family.");
        return false;
    }
    return Socket::Connect(addr, e);
}

// Get connected address
SocketAddress UdpSocket::ConnectedAddress(Error* e) const noexcept
{
    return Socket::ConnectedAddress(e);
}

// Send data to given address, in block mode
ssize_t UdpSocket::SendTo(const void* p, size_t n, const SocketAddress* addr, Error* e) noexcept
{
    if(!Socket::Block(true, e)) return -1;
    return Socket::SendTo(p, n, addr, 0, e);
}

// Send data to given address, in block mode
ssize_t UdpSocket::SendTo(StreamBuffer* buf, const SocketAddress* addr, Error* e) noexcept
{
    assert(buf != NULL);
    ssize_t ret = SendTo(buf->Read(), buf->Readable(), addr, e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Send data to given address, in non-block mode with timeout
ssize_t UdpSocket::SendTo(const void* p, size_t n, const SocketAddress* addr, int timeout, Error* e) noexcept
{
    if(timeout < 0) return SendTo(p, n, addr, e);
    if(Socket::Block(false, e)) return -1;

    if(timeout > 0 && !Socket::WaitForWrite(timeout, e))
    {
        return -1;
    }
    return Socket::SendTo(p, n, addr, 0, e);
}

// Send data to given address, in non-block mode with timeout
ssize_t UdpSocket::SendTo(StreamBuffer* buf, const SocketAddress* addr, int timeout, Error* e) noexcept
{
    assert(buf != NULL);
    ssize_t ret = SendTo(buf->Read(), buf->Readable(), addr, timeout, e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Send data to connected address, block mode
ssize_t UdpSocket::Send(const void* p, size_t n, Error* e) noexcept
{
    if(!Socket::Block(true, e)) return -1;
    return Socket::Send(p, n, 0, e);
}

// Send data to connected address, block mode
ssize_t UdpSocket::Send(StreamBuffer* buf, Error* e) noexcept
{
    ssize_t ret = Send(buf->Read(), buf->Readable(), e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Send data to connected address, non-block mode with timeout
ssize_t UdpSocket::Send(const void* p, size_t n, int timeout, Error* e) noexcept
{
    if(timeout < 0) return Send(p, n, e);
    if(Socket::Block(false, e)) return -1;

    if(timeout > 0 && !Socket::WaitForWrite(timeout, e))
    {
        return -1;
    }
    return Socket::Send(p, n, 0, e);
}

// Send data to connected address, non-block mode with timeout
ssize_t UdpSocket::Send(StreamBuffer* buf, int timeout, Error* e) noexcept
{
    assert(buf != NULL);
    ssize_t ret = Send(buf->Read(), buf->Readable(), timeout, e);
    if(ret > 0) buf->Read(ret);
    return ret;
}

// Receive data and get remote address, block mode
ssize_t UdpSocket::ReceiveFrom(void* p, size_t n, SocketAddress* addr, Error* e) noexcept
{
    if(!Socket::Block(true, e)) return -1;
    return Socket::ReceiveFrom(p, n, addr, 0, e);
}

// Receive data and get remote address, block mode
ssize_t UdpSocket::ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, Error* e) noexcept
{
    assert(buf != NULL);
    if(!buf->Writable(RECEIVE_BUFFER_SIZE))
    {
        SET_LOGIC_ERROR(e, "No enought buffer to receive.");
        return -1;
    }
    ssize_t ret = ReceiveFrom(buf->Write(), buf->Writable(), addr, e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

// Receive data and get remote address, non-block mode with timeout
ssize_t UdpSocket::ReceiveFrom(void* p, size_t n, SocketAddress* addr, int timeout, Error* e) noexcept
{
    if(timeout < 0) return ReceiveFrom(p, n, addr, e);
    if(!Socket::Block(false, e)) return -1;

    if(timeout > 0 && !Socket::WaitForRead(timeout, e))
    {
        return -1;
    }
    return Socket::ReceiveFrom(p, n, addr, 0, e);
}

// Receive data and get remote address, non-block mode with timeout
ssize_t UdpSocket::ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int timeout, Error* e) noexcept
{
    assert(buf != NULL);
    if(!buf->Writable(RECEIVE_BUFFER_SIZE))
    {
        SET_LOGIC_ERROR(e, "No enought buffer to receive.");
        return -1;
    }
    ssize_t ret = ReceiveFrom(buf->Write(), buf->Writable(), addr, timeout, e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

// Receive data from connected address, block mode
ssize_t UdpSocket::Receive(void* p, size_t n, Error* e) noexcept
{
    assert(p != NULL && n > 0);
    if(!Socket::Block(true, e)) return -1;
    return Socket::Receive(p, n, 0, e);
}

// Receive data from connected address, block mode
ssize_t UdpSocket::Receive(StreamBuffer* buf, Error* e) noexcept
{
    assert(buf != NULL);
    if(!buf->Writable(RECEIVE_BUFFER_SIZE))
    {
        SET_LOGIC_ERROR(e, "No enought buffer to receive.");
        return -1;
    }
    ssize_t ret = Receive(buf->Write(), buf->Writable(), e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

// Receive data from connected address, non-block mode with timeout
ssize_t UdpSocket::Receive(void* p, size_t n, int timeout, Error* e) noexcept
{
    assert(p != NULL && n > 0);
    if(!Socket::Block(false, e)) return -1;
    if(timeout > 0 && !Socket::WaitForRead(timeout, e))
    {
        return -1;
    }
    return Socket::Receive(p, n, 0, e);
}

// Receive data from connected address, non-block mode with timeout
ssize_t UdpSocket::Receive(StreamBuffer* buf, int timeout, Error* e) noexcept
{
    assert(buf != NULL);
    if(!buf->Writable(RECEIVE_BUFFER_SIZE))
    {
        SET_LOGIC_ERROR(e, "No enought buffer to receive.");
        return -1;
    }
    ssize_t ret = Receive(buf->Write(), buf->Writable(), timeout, e);
    if(ret > 0) buf->Write(ret);
    return ret;
}

NETB_END 

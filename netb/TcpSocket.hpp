/*
 * Copyright (C) 2010 Maoxu Li. All rights reserved. maoxu@lebula.com
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

#ifndef NETB_TCP_SOCKET_HPP
#define NETB_TCP_SOCKET_HPP

#include "Socket.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// TcpSocket is a wrapper class of TCP socket. 
//
class TcpSocket : protected Socket
{
public:
    // any local address
    // family is determined by connected address
    TcpSocket() noexcept; 

    // any local address
    // fixed family
    explicit TcpSocket(sa_family_t family) noexcept;

    // fixed local address
    explicit TcpSocket(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true) noexcept;

    // Externally established connection with connected address
    TcpSocket(SOCKET s, const SocketAddress* addr) noexcept;

    // Destructor
    virtual ~TcpSocket() noexcept;

    // Internal SOCKET descriptor is exposed for external use, e.g. for select
    SOCKET GetSocket() const noexcept { return Socket::Descriptor(); }

    // Set connected status for externally established connection
    virtual void Connected(); // throw on errors
    virtual bool Connected(Error* e) noexcept; 

    // Actively connect to remote address, in block mode
    virtual void Connect(const SocketAddress& addr); // throw on errors
    virtual bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Actively connect to remote address, in non-block mode with timeout
    // timeout of -1 for block mode
    virtual void Connect(const SocketAddress& addr, int timeout); // throw on errors
    virtual bool Connect(const SocketAddress& addr, int timeout, Error* e) noexcept;

    // close the socket
    // Return false on errors, but socket is closed anyway
    virtual bool Close(Error* e = nullptr) noexcept;

    // Bound local address or given address before connected
    SocketAddress Address(Error* e = nullptr) const noexcept;

    // Connected address
    SocketAddress ConnectedAddress(Error* e = nullptr) const noexcept;

    // Send data over the connection, in block mode
    virtual ssize_t Send(const void* p, size_t n, Error* e = nullptr) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Send data over the connection, in non-block mode with timeout
    // timeout of -1 for block mode
    virtual ssize_t Send(const void* p, size_t n, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int timeout, Error* e = nullptr) noexcept;

    // Receive data from the connection, in block mode
    virtual ssize_t Receive(void* p, size_t n, Error* e = nullptr) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Receive data from the connection, in non-block mode with timeout
    // timeout of -1 for block mode
    virtual ssize_t Receive(void* p, size_t n, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, int timeout, Error* e = nullptr) noexcept;

protected:
    // Initial local address: empty, fixed family, or fixed address
    SocketAddress _address;

    // Initial connected address, only used for externally established connection
    SocketAddress _connected_address;

    // reuse rules, only used for fixed initial local address
    bool _reuse_addr;
    bool _reuse_port;

    // Actual connect in block or non-block mode
    bool DoConnect(const SocketAddress& addr, bool block, Error* e);
};

NETB_END

#endif

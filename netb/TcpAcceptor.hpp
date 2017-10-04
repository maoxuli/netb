/*
 * Copyright (C) 2010, Maoxu Li. http://maoxuli.com/dev
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

#ifndef NETB_TCP_ACCEPTOR_HPP
#define NETB_TCP_ACCEPTOR_HPP

#include "Socket.hpp"

NETB_BEGIN

//
// TcpAcceptor is a wrapper class of TCP socket that works as a server to accept 
// incomming TCP connections. 
//
class TcpAcceptor : protected Socket
{
public:
    // dynamic address
    // determined by following operations
    TcpAcceptor() noexcept; 

    // fixed family with any address or dynamic address
    // determined by following operations
    explicit TcpAcceptor(sa_family_t family) noexcept;

    // fixed address
    explicit TcpAcceptor(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true) noexcept; 
    
    // Destructor
    virtual ~TcpAcceptor() noexcept;

    // Internal SOCKET descriptor is exposed for external use, e.g. for select()
    SOCKET GetSocket() const { return Socket::Descriptor(); }

    // Setup backlog before open if not using the system default
    // -1 for system default
    void SetBacklog(int backlog = -1) noexcept;

    // Open with fixed address, or fixed family with any address
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open with dynamic address, or fixed family with dynamic address
    virtual void Open(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true); // throw on errors
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept; // with default reuse rules
    virtual bool Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept;

    // Close and be ready to open again
    // return false on errors, but socket is closed anyway
    virtual bool Close(Error* e = nullptr) noexcept;

    // Actual bound address or given address before opened
    SocketAddress Address(Error* e = nullptr) const noexcept;

    // Accept a connection, in block mode
    virtual SOCKET Accept(); // thow on errors
    virtual SOCKET Accept(Error* e) noexcept;

    // Accept a connection, int non-block mode with timeout (miliseconds)
    // timeout of -1 for block mode 
    virtual SOCKET Accept(int timeout); // throw on errors
    virtual SOCKET Accept(int timeout, Error* e) noexcept;

    // Accept a connection, in block mode
    virtual SOCKET AcceptFrom(SocketAddress* addr); // thow on errors
    virtual SOCKET AcceptFrom(SocketAddress* addr, Error* e) noexcept;
    
    // Accept a connection, int non-block mode with timeout (miliseconds)
    // timeout of -1 for block mode 
    virtual SOCKET AcceptFrom(SocketAddress* addr, int timeout); // throw on errors
    virtual SOCKET AcceptFrom(SocketAddress* addr, int timeout, Error* e) noexcept;

protected: 
    // Initial address: empty, fixed family, or fixed address
    SocketAddress _address; 

    // reuse rules, only used for fixed initial address
    bool _reuse_addr;
    bool _reuse_port;
    
    // Backlog for TCP listen, default is SOMAXCONN
    // -1 for default
    int _backlog; 
};

NETB_END

#endif

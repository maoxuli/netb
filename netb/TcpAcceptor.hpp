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

#ifndef NETB_TCP_ACCEPTOR_HPP
#define NETB_TCP_ACCEPTOR_HPP

#include "Socket.hpp"

NETB_BEGIN

//
// TcpAcceptor is a wrapper class of TCP socket that works as a server to accept 
// incomming TCP connections. 
//
class TcpAcceptor : private Socket
{
public:
    // Constructor, with given local address
    TcpAcceptor() noexcept; // no address
    explicit TcpAcceptor(sa_family_t family) noexcept; // any address in family
    explicit TcpAcceptor(const SocketAddress& addr) noexcept; // given address
    virtual ~TcpAcceptor() noexcept;

    // Internal SOCKET descriptor is exposed for external use, e.g. for select
    SOCKET GetSocket() const { return Socket::Descriptor(); }

    // Open to accept incomming connection
    // Create socket, bind to local address, and listen for connection
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open on given address
    virtual void Open(const SocketAddress& addr); // throw on errors
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept;

    // Close, stop to accept incomming connection
    // Close the socket, ready for open again
    virtual bool Close(Error* e = NULL) noexcept;

    // Status
    bool IsOpened() const noexcept { return _opened; }

    // Given local address before opened, and actual bound address after opened
    SocketAddress Address(Error* e = NULL) const noexcept;

    // Accept a connection, in block or non-block mode
    virtual SOCKET Accept(); // thow on errors
    virtual SOCKET Accept(Error* e) noexcept;

    // address is a pointer rather a reference, so could be NULL
    virtual SOCKET AcceptFrom(SocketAddress* addr); // throw on errors
    virtual SOCKET AcceptFrom(SocketAddress* addr, Error* e) noexcept;

public:
    // Backlog for listen
    void Backlog(int size); // default SOMAXCONN
    bool Backlog(int size, Error* e) noexcept;

    // IO mode, block or non-block with timeout
    // 0: non-block, -1: block, >0: block with timeout
    void Block(int timeout); // default is block
    bool Block(int timeout, Error* e) noexcept;

    // Option of reuse address
    void ReuseAddress(bool reuse); // default is false
    bool ReuseAddress(bool reuse, Error* e) noexcept;

    // Option of reuse port
    void ReusePort(bool reuse);  // default is false
    bool ReusePort(bool reuse, Error* e) noexcept;

private: 
    SocketAddress _address; // Given local address
    int _timeout;
    int _backlog;
    bool _opened;
};

NETB_END

#endif

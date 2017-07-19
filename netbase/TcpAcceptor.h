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

#ifndef NET_BASE_TCP_ACCEPTOR_H
#define NET_BASE_TCP_ACCEPTOR_H

#include "Socket.h"
#include "SocketAddress.h"

NET_BASE_BEGIN

//
// TcpAcceptor is a wrapper class of TCP socket that works as a server to accept 
// incomming TCP connections. It works on an internal lower-level socket wrapper 
// class Socket, which holds the SOCKET descriptor and supports all related 
// operations on the descriptor. 
//
class TcpAcceptor : protected Socket
{
public:
    // Create an unbound TCP socket working as TCP server
    // The domain of the socket is not determined before calling Open()
    TcpAcceptor();

    // Create an unbound TCP socket working as TCP server
    // The domain of the sochet is given
    explicit TcpAcceptor(int domain);

    // Create a TCP socket that bound to give local address
    // The family of the socket is determined by address
    // See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
    TcpAcceptor(const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Destructor, deriviation is allowed for extension
    virtual ~TcpAcceptor();

    // Internal SOCKET descriptor is exposed for external use, e.g. for select
    SOCKET GetSocket() const { return *this; }

    // Open the TCP server on prior given local address 
    // or passed in address
    virtual bool Open(int backlog = SOMAXCONN);
    virtual bool Open(const SockAddress& addr, int backlog = SOMAXCONN)
    virtual bool Open(const SockAddress& addr, bool reuseaddr, bool reuseport, int backlog = SOMAXCONN);

    // Local address bound to the socket
    bool IsOpened() const { return mOpened; }
    SocketAddress Address() const { return mAddress; }

    // Accept a connection, in block or non-block mode
    virtual SOCKET Accept();
    virtual SOCKET Accept(SocketAddress* addr);

public: 
    // IO mode, block or non-block 
    bool Block(bool block);
    bool Block() const;

protected: 
    SocketAddress mAddress; // Local address
    bool mOpened; // status
};

NET_BASE_END

#endif

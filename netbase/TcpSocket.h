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

#ifndef NET_BASE_TCP_SOCKET_H
#define NET_BASE_TCP_SOCKET_H

#include "Socket.h"
#include "SocketAddress.h"

NET_BASE_BEGIN

//
// TcpSocket is a wrapper class of TCP socket that works as a connection between 
// client and server. On server side, it can be initialized with a TCP conenction 
// SOCKET descriptor accepted by TCP server, and set the status to connected 
// directly. On cliet side, it can be initilaized with an internal TCP SOCKET 
// descriptor, and connect to server to establish the connection. 
//
class TcpSocket : protected Socket
{
public:
    // Create an unbound, unconnected TCP socket
    // The domain of the socket is not determined before calling Connect()
    TcpSocket();

    // Create a unbound, unconnected TCP socket
    // The domain of the socket is given
    explicit TcpSocket(int domain)

    // Create a TCP socket bound to given local address
    // The domain of the socket is determined by the address family
    // See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
    TcpSocket(const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Create a TCP socket with externally established connection
    TcpSocket(SOCKET s, const SocketAddress* conencted, const SocketAddress* local = NULL);

    // Destructor, deriviation is allowed for extension
    virtual ~TcpSocket();

    // Internal SOCKET descriptor is exposed for external use, e.g. for select
    SOCKET GetSocket() const { return *this; }

    // Local address binded to the socket
    SocketAddress Address() const { return mAddress; }

    // Set connected status directly
    // Used for externally established connection
    virtual bool Connected();

    // Connect to server to establish connection
    virtual bool Connect(const SocketAddress& addr);
    virtual void Disconnect();

    // Check if the connection is established
    bool IsConnected() const { return mConnected; }
    SocketAddress ConnectedAddress() const { return mConnectedAddress; }

    // Send data over the connection
    virtual ssize_t Send(const void* p, size_t n, int flags = 0);
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0);

    // Receive data from the connection
    virtual ssize_t Receive(void* p, size_t n, int flags = 0);
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0);

public:
    // IO mode, block or non-block
    bool Block(bool block);
    bool Block() const;

    // Options that can be set and get
    // TCP_NODELAY
    bool NoDelay(bool);
    bool NoDelay() const;

    // TCP_NOPUSH
    bool NoPush(bool);
    bool NoPush() const;

    // Time in seconds to send keep alive probs, 0 for not keep alive
    // See SO_KEEPALIVE and TCP_KEEPALIVE
    size_t KeepAlive(size_t);
    size_t KeepAlive() const;

    // TCP_MAXSEG
    size_t SegmentSize(size_t);
    size_t SegmentSize() const;

    bool SendBuffer(size_t);
    size_t SendBuffer() const;

    bool ReceiveBuffer(size_t);
    size_t ReceiveBuffer() const;

protected:
    SocketAddress mAddress; // Local address
    SocketAddress mConnectedAddress; // Connected address
    bool mConnected; // Status
};

NET_BASE_END

#endif

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

#ifndef NET_BASE_UDP_SOCKET_H
#define NET_BASE_UDP_SOCKET_H

#include "Socket.h"
#include "SocketAddress.h"

NET_BASE_BEGIN

//
// UdpSocket is a wraper class of UDP socket. It is not designed as a 
// derived class of Socket, but working based on an internal Socket object. 
//
// Todo: Broadcast and Multicast support 
//
class UdpSocket : protected Socket
{
public:
    // Create an unbound, unconnected UDP socket
    // The domain of the socket is not determined before calling of Open() or Connect() or SendTo()
    UdpSocket();

    // Create an unbound, unconnected UDP socket
    // The domain of the socket is given
    explicit UdpSocket(int domain);

    // Create a UDP socket that bound to given local address
    // The domain of the socket is determined by address family
    // see socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport 
    UdpSocket(const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Destructor, derivation is allowed for extension
    virtual ~UdpSocket(); 

    // Internal socket is exposed for external use, e.g. for select
    SOCKET GetSocket() const { return *this; }

    // Open the socket to receive data
    // The socket will be bound to passed-in address on instantiation
    virtual bool Open();

    // Open the socket to receive data
    // The socket will be bound to given local address
    virtual bool Open(const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Local address bound to the socket
    bool IsOpened() const { return mOpened; }
    SocketAddress Address() { return mAddress; }

    // Connect the socket to a remote address, or disconnect from current remote address
    bool Connect(const SocketAddress& addr);
    void Disconnect();

    // Check connection status, and connected address
    bool IsConnected() const { return mConnected; }
    SocketAddress ConnectedAddress() const;

    // Send (non-connection)
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress& addr, int flags = 0);
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress& addr, int flags = 0);

    // Send (connection)
    virtual ssize_t Send(const void* p, size_t n, int flags = 0);
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0);

    // Receive (non-connection)
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0);
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0);

    // Receive (connection)
    virtual ssize_t Receive(void* p, size_t n, int flags = 0);
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0);

public:
    // IO mode, block or non-block
    bool Block(bool block);
    bool Block() const;

protected:
    SocketAddress mAddress;
    bool mOpened;
    bool mConnected;
};

NET_BASE_END

#endif

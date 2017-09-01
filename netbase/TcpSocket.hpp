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

#ifndef NET_BASE_TCP_SOCKET_HPP
#define NET_BASE_TCP_SOCKET_HPP

#include "Socket.hpp"

NET_BASE_BEGIN

//
// TcpSocket is a wrapper class of TCP socket. 
//

class TcpSocket : private Socket
{
public:
    // Constructor, with local address 
    TcpSocket() noexcept; // no address
    explicit TcpSocket(sa_family_t family) noexcept; // any address of given family
    explicit TcpSocket(const SocketAddress& addr) noexcept; // initial address

    // Constructor, with externally established connection
    // address is const pointer rather reference, so could be NULL
    TcpSocket(SOCKET s, const SocketAddress* connected) noexcept;

    // Destructor
    virtual ~TcpSocket() noexcept;

    // Internal SOCKET descriptor is exposed for external use, e.g. for select
    SOCKET GetSocket() const noexcept { return Socket::Descriptor(); }

    // Connect to remote address
    virtual void Connect(const SocketAddress& addr); // throw on errors
    virtual bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Set connected status
    virtual void Connected(); // throw on errors
    virtual bool Connected(Error* e) noexcept; 

    // Connected status
    bool IsConnected() const noexcept { return _connected; }

    // Local address
    // actual bound address after connected
    SocketAddress Address(Error* e = NULL) const noexcept;

    // Connected address
    SocketAddress ConnectedAddress(Error* e = NULL) const noexcept;

    // close the socket
    bool Close(Error* e = NULL) noexcept;

    // Send data over the connection
    virtual ssize_t Send(const void* p, size_t n, int flags = 0) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0) noexcept;

    // Receive data from the connection
    virtual ssize_t Receive(void* p, size_t n, int flags = 0) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0) noexcept;

public:
    // IO mode
    // -1: block, 0: non-block, >0: block with timeout
    void Block(int timeout); // throw on errors
    bool Block(int timeout, Error* e) noexcept;

    // Option of reuse address
    void ReuseAddress(bool reuse); // throw on errors
    bool ReuseAddress(bool reuse, Error* e) noexcept;

    // Option of reuse port
    void ReusePort(bool reuse); // throw errors
    bool ReusePort(bool reuse, Error* e) noexcept;

    // Option of no delay
    // TCP_NODELAY
    void NoDelay(bool no); // throw on errors
    bool NoDelay(bool no, Error* e) noexcept;

    // TCP_NOPUSH
    void NoPush(bool no); // throw on errors
    bool NoPush(bool no, Error* e) noexcept;

    // Time in seconds to send keep alive probs, 0 for not keep alive
    // See SO_KEEPALIVE and TCP_KEEPALIVE
    void KeepAlive(int time); // throw on errors
    bool KeepAlive(int time, Error* e) noexcept;

    // TCP_MAXSEG
    void SegmentSize(int size); // throw on errors
    bool SegmentSize(int size, Error* e) noexcept;

    void SendBuffer(int size); // throw on errors
    bool SendBuffer(int size, Error* e) noexcept;

    void ReceiveBuffer(int size); // throw on errors
    bool ReceiveBuffer(int size, Error* e) noexcept;

private:
    SocketAddress _address; // Given local address to bind
    SocketAddress _connected_address; // Give connected address
    int _timeout; // IO mode, support block with timout
    bool _connected;
};

NET_BASE_END

#endif

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

#ifndef NET_BASE_UDP_SOCKET_HPP
#define NET_BASE_UDP_SOCKET_HPP

#include "Socket.hpp"

NET_BASE_BEGIN

//
// UdpSocket is a wraper class of UDP socket
//

class UdpSocket : private Socket
{
public:
    // Constructor, with local address info
    // The socket will not created and local address is not bound in construction
    // but both will happen implicitly if it is necessary for a function call.
    UdpSocket() noexcept; // no address
    explicit UdpSocket(sa_family_t family) noexcept; // any address of given family
    explicit UdpSocket(const SocketAddress& addr) noexcept; // initial address

    // Destructor
    virtual ~UdpSocket() noexcept; 

    // Internal socket is exposed for external use, e.g. for select
    SOCKET GetSocket() const noexcept { return Socket::Descriptor(); }

    // Open
    // Create socket and bind to initial local address
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open
    // Create socket and bind to given address
    virtual void Open(const SocketAddress& addr); // throw on errors
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept;

    // Close opened socket
    virtual bool Close(Error* e = NULL) noexcept;

    // Status of opened
    bool IsOpened() const noexcept { return _opened; }

    // Local address
    // Actual bound address after opened
    SocketAddress Address(Error* e = NULL) const noexcept;

    // Connect to a remote address
    // Empty address will remove the assocication
    void Connect(const SocketAddress& addr); // throw on errors
    bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Status of connected
    bool IsConnected() const noexcept { return _connected; }

    // Connected address
    // Actual connected address after connected
    SocketAddress ConnectedAddress(Error* e = NULL) const noexcept;

    // Send data to given address
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, int flags = 0) noexcept;
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags = 0) noexcept;

    // Send data to connected address
    virtual ssize_t Send(const void* p, size_t n, int flags = 0) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0) noexcept;

    // Receive data and get remote address
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0) noexcept;
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0) noexcept;

    // Receive data from connected address
    virtual ssize_t Receive(void* p, size_t n, int flags = 0) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0) noexcept;

public:
    // IO mode
    // -1: block, 0: non-block, >0: block with timeout
    void Block(int timeout); // throw on erros
    bool Block(int timeout, Error* e) noexcept;

    // Socket option, reuse address
    void ReuseAddress(bool reuse); // throw on erros
    bool ReuseAddress(bool reuse, Error* e) noexcept;

    // Socket option, reuse port
    void ReusePort(bool reuse); // throw on errors
    bool ReusePort(bool reuse, Error* e) noexcept;

private:
    SocketAddress _address; // Given local address
    SocketAddress _connected_address; // Given connected address
    int _timeout; // IO mode, support block with timeout
    bool _opened;
    bool _connected;
};

NET_BASE_END

#endif

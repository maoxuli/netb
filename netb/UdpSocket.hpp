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

#ifndef NETB_UDP_SOCKET_HPP
#define NETB_UDP_SOCKET_HPP

#include "Socket.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// UdpSocket is a wraper class of UDP socket
//
class UdpSocket : protected Socket
{
public:
    // Any local address, determined by following operations
    UdpSocket() noexcept;

    // Fixed family, only working in given family
    // Local address is determined by following operations
    explicit UdpSocket(sa_family_t family) noexcept;

    // Fixed local address
    explicit UdpSocket(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true) noexcept; // initial address

    // Destructor
    virtual ~UdpSocket() noexcept; 

    // Internal socket is exposed for external use, e.g. for select
    SOCKET GetSocket() const noexcept { return Socket::Descriptor(); }

    // Open to receive data
    // Fixed family or local address is given on initial
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open to receive data
    // Dynamic local address or fixed family with dynamic address
    virtual void Open(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true); // throw on errors
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept; // with default reuse rules
    virtual bool Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept;

    // Close opened socket, and ready for open again
    // return false when errors occurred
    // but the socket is closed anyway, even with errors
    virtual bool Close(Error* e = nullptr) noexcept;

    // Actual bound address of given address
    SocketAddress Address(Error* e = nullptr) const noexcept;

    // Connect to a remote address
    // Empty address will remove the assocication
    void Connect(const SocketAddress& addr); // throw on errors
    bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Connected address
    SocketAddress ConnectedAddress(Error* e = nullptr) const noexcept;

public:
    // Send data to given address, in block mode
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, Error* e = nullptr) noexcept;
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, Error* e = nullptr) noexcept;

    // Send data to given address, in non-block mode with timeout
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, int timeout, Error* e = nullptr) noexcept;

    // Send data to connected address, block mode
    virtual ssize_t Send(const void* p, size_t n, Error* e = nullptr) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Send data to connected address, in non-block mode with timeout
    virtual ssize_t Send(const void* p, size_t n, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int timeout, Error* e = nullptr) noexcept;

    // Receive data and get remote address, block mode
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, Error* e = nullptr) noexcept;
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, Error* e = nullptr) noexcept;

    // Receive data and get remote address, in non-block mode with timeout
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int timeout, Error* e = nullptr) noexcept;

    // Receive data from connected address, block mode
    virtual ssize_t Receive(void* p, size_t n, Error* e = nullptr) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Receive data from connected address, in non-block mode with timeout
    virtual ssize_t Receive(void* p, size_t n, int timeout, Error* e = nullptr) noexcept;
    virtual ssize_t Receive(StreamBuffer* buf, int timeout, Error* e = nullptr) noexcept;

protected:
    // Initial local address
    // may be empty, fixed address, or any address with fixed family
    SocketAddress _address; 

    // reuse rules
    bool _reuse_addr;
    bool _reuse_port;
};

NETB_END

#endif

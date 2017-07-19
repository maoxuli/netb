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

#ifndef NET_BASE_SOCKET_H
#define NET_BASE_SOCKET_H

#include "Config.h"
#include "Uncopyable.h"
#include "SocketDef.h"

NET_BASE_BEGIN

//
// Socket is a general wrapper class of socket API related to a SOCKET descriptor. 
// It holds a SOCKET descriptor and provides functions for all possible operations 
// working on a SOCKET descriptor. Socket object can be initialized with any kind 
// of SOCKET descriptor (i.e. different domain, type, and protocol), thus supports 
// all possible operations and options for all kinds of SOCKET descriptors. 
// 
// Socket is a lower level (ant very thin) wrapper of socket API with the major 
// purpose of cross-platform programming. 
// 
class Socket : private Uncopyable
{
public: 
    // INVALID_SOCKET
    Socket();

    // Create a socket with given information
    Socket(int domain, int type, int protocol);

    // Externally initialized socket
    explicit Socket(SOCKET s);

    // Destructor, will close the socket
    ~Socket();

    // A Socket object is based on a SOCKET descriptor, three ways to disable:
    // Shutdown: see shutdown() function
    // Close: see close()/closesocket() function
    // Detach: separate the SOCKET descriptor from this object, may attach again
    bool Shutdown(int how = SHUT_RDWR); 
    void Close();
    SOCKET Detach();

    // Create a internal socket with given information
    // If current socket is not INVALID_SOCKET, it will be closed
    Socket& Create(int domain, int type, int protocol);

    // Attach a externally initialized SOCKET descriptor to the object
    // If current socket is not INVALID_SOCKET, it will be closed
    Socket& Attach(SOCKET s);

    // Socket object is always corresponding to a SOCKET descriptor
    operator SOCKET() const { return fd; };

    // General features of the SOCKET descriptor
    bool Invalid() const { return fd == INVALID_SOCKET; }
    sa_family_t Family() const { return (sa_family_t)Domain(); }
    int Domain() const; // Typically defined for socket, PF_XXX
    int Type() const; // SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...
    int Protocol() const; // IPPROTO_TCP, IPPROTO_UDP, ...

    // Comparision operators
    bool operator==(const Socket& s) const { return fd == s.fd; }
    bool operator!=(const Socket& s) const { return fd != s.fd; }
    bool operator<(const Socket& s) const { return fd < s.fd; }
    bool operator>(const Socket& s) const { return fd > s.fd; }
    bool operator<=(const Socket& s) const { return fd <= s.fd; }
    bool Operator>=(const Socket& s) const { return fd >= s.fd; }

public: 
    // Explicitly bind to a local address
    bool Bind(const struct sockaddr* addr, socklen_t addrlen);

    // Get local address, if binded already
    bool Address(struct sockaddr* addr, socklen_t* addrlen) const;

    // For a TCP socket, start to wait for incomming connections
    bool Listen(int backlog = SOMAXCONN);

    // For TCP socket, accept an incomming connection
    SOCKET Accept();
    SOCKET Accept(struct sockaddr* addr, socklen_t* addrlen);

    // For a TCP socket, connect to remote address to establish outgoing connection
    // For a UDP socket, bind to remote address only
    bool Connect(const struct sockaddr* addr, socklen_t addrlen);

    // Get connected address, if connected already
    bool ConnectedAddress(struct sockaddr* addr, socklen_t* addrlen) const;

    // Send and receive data via a connected socket
    ssize_t Send(const void* p, size_t n, int flags = 0);
    ssize_t Receive(void* p, size_t n, int flags = 0);

    // Send and receive data via a non-connected UDP socket
    ssize_t SendTo(const void* p, size_t n, const struct sockaddr* addr, socklen_t addrlen, int flags = 0);
    ssize_t ReceiveFrom(void* p, size_t n, struct sockaddr* addr, socklen_t* addrlen, int flags = 0);

public: 
    // Control flag of IO mode: block or non-block
    bool Block(bool block); // default is block
    bool Block() const;

    // Socket option of reuse address and reuse port
    bool ReuseAddress(bool reuse); // default is false
    bool ReuseAddress() const;

    bool ReusePort(bool reuse); // default is false
    bool ReusePort() const;

    // Set and get options of socket
    bool SetOption(int level, int name, const void* val, socklen_t len);
    bool GetOption(int level, int name, void* val, socklen_t* len) const;

private:
    // Internal socket descriptor (file descriptor)
    SOCKET fd;
};

NET_BASE_END

#endif

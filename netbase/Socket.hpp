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

#ifndef NET_BASE_SOCKET_HPP
#define NET_BASE_SOCKET_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>

typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

NET_BASE_BEGIN

//
// Socket is a supper class that wraps a socket and all related operations on it.
// It supports all types of sockets, including TCP, UDP, RAW, and so on.
// The major purpose of this wrapper is cross-platform. 
//
class Socket : Uncopyable
{
public: 
    // INVALID_SOCKET
    Socket();

    // Any type of socket
    Socket(int domain, int type, int protocol);

    // Attach an external socket
    Socket(SOCKET s);

    // Close the socket
    ~Socket();

    // A Socket object works based on a socket descriptor, three ways to stop its working:
    // 1. Shutdown: disable the functions of the socket descriptor, while it is still opened.
    // 2. Close: release the socket descriptor, thus is is closed. 
    // 3. Detach: separate the socket descriptor from the object.
    bool Shutdown(int how = SHUT_RDWR); 
    void Close();
    SOCKET Detach();

    // Attach a external socket to the object
    // If current socket is not INVALID_SOCKET, it will be closed
    void Attach(SOCKET s);

    // Basically Socket is a wrapper of a socket descriptor and operations on it,
    // so a Socket object has a corresponding socket descriptor always, although
    // it may be INVALID_SOCKET. 
    // Typecasting will return the corresponding socket descriptor.
    operator SOCKET() const { return fd; };

    // Basic features of the socket
    bool Valid() const { return fd != INVALID_SOCKET; }
    sa_family_t Family() const; // Typically defined for socket address, AF_XXX
    int Domain() const; // Typically defined for socket, PF_XXX
    int Type() const;
    int Protocol() const;

    // Explicitly bind an address to the socket
    bool Bind(const struct sockaddr* addr, socklen_t addrlen);
    
    // For a TCP socket, start to wait for incomming connections
    bool Listen(int backlog = SOMAXCONN);

    // For TCP socket, accept an incomming connection
    SOCKET Accept();
    SOCKET Accept(struct sockaddr* addr, socklen_t* addrlen);

    // For a TCP socket, connect to remote address to establish outgoing connection
    // For a UDP socket, bind to remote address only
    bool Connect(const struct sockaddr* addr, socklen_t addrlen);;

    // A socket can bind to a local address explicitly, 
    // or implicitly with some operations that need a local address binded firstly. 
    bool LocalAddress(struct sockaddr* addr, socklen_t* addrlen) const;

    // A TCP socket has binded remote address if it is a established connection. 
    // A UDP socket has binded remote address if it is connected explicitly. 
    bool RemoteAddress(struct sockaddr* addr, socklen_t* addrlen) const;

    // Send and receive data via a connected socket
    ssize_t Send(void* p, size_t n);
    ssize_t Receive(void* p, size_t n);

    // Send and receive data via a non-connected socket
    ssize_t SendTo(void* p, size_t n, const struct sockaddr* addr, socklen_t addrlen);
    ssize_t ReceiveFrom(void* p, size_t n, struct sockaddr* addr, socklen_t* addrlen);

public:
    // Options
    bool Block(bool);
    bool Block() const;

    bool ReuseAddress(bool);
    bool ReuseAddress() const;

    // For TCP socket
    bool NoDelay(bool);
    bool NoDelay() const;

    // For TCP socket
    bool KeepAlive(bool);
    bool KeepAlive() const;

    bool SendBuffer(size_t);
    size_t SendBuffer() const;

    bool ReceiveBuffer(size_t);
    size_t ReceiveBuffer() const;

private:
    // Internal socket descriptor (file descriptor)
    SOCKET fd;
};

NET_BASE_END

#endif

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

#ifndef NETB_SOCKET_HPP
#define NETB_SOCKET_HPP

#include "SocketConfig.hpp"
#include "SocketAddress.hpp"
#include "SocketSelector.hpp"

NETB_BEGIN

//
// Wrapper function for closing socket
// return false on errors
// but the socket is closed anyway, even on errors
//
bool CloseSocket(SOCKET s, Error* e = nullptr) noexcept; 

//
// Socket is a wrapper class for socket API related to a socket. It holds 
// a socket descriptor and provides all possible functions working on the 
// socket. A Socket object can be initialized to any kind of socket (with 
// different domain, type, and protocol), so it must implement all possible 
// functions for all kinds of sockets. Thereby, Socket class has all
// necessary functions to open socket, set socket options, constrol socket 
// working status, and perform I/O on a socket. By this mean, Socket is a 
// complete socket wrapper class, which can support any socket programming 
// without derivation. 
// 
// Socket is a lower level (ant very thin) wrapper of socket API with the 
// major purpose of cross-platform programming. It provides an interface 
// that is very similar to the original socket API. But working together 
// with classes of SocketAddress, SockeSelector, StreamBuffer, as well as 
// Error and Exception, socket programming is more easy and more safe. 
// 
// In general, Socket maintains a ownership semantic. If a Socket object 
// has a valid, opened socket descriptor, then it owns the socket and is 
// responsible to close it when the object is destroyed. Socket is defined 
// as a non-copyable object to avoid trouble of implicit ownership movement.
// 
class Socket : private Uncopyable
{
public: 
    // Construct an object with no bound socket 
    // Call Create() or Attach() later if necessary
    Socket() noexcept;

    // Construct an object with an open socket
    // No socket is opened if errors ocurred
    Socket(int domain, int type, int protocol); // throw on errors
    Socket(int domian, int type, int protocol, Error* e) noexcept; 

    // Constuct an object and attch an externally opened socket
    explicit Socket(SOCKET s) noexcept;

    // Destructor, close opened socket
    // failure on closing is ignored
    virtual ~Socket() noexcept;

    // Open socket
    // Close current socket firstly
    // No socket is opened if errors occurred
    void Create(int domain, int type, int protocol); // throw on error
    bool Create(int domain, int type, int protocol, Error* e) noexcept;

    // Attach an externally opened socket
    // Close current socket firstly
    void Attach(SOCKET s) noexcept;

    // Separate the socket form the object
    // The ownership of the socket is taken over by caller
    SOCKET Detach() noexcept;

    // Close current socket
    // Return false if error occurred
    // but the socke is closed anyway, even on errors
    // see close()/closesocket() of socket API for details
    bool Close(Error* e = nullptr) noexcept;

    // Shutdwon the socket
    // return false if error occured
    // see shutdown() of socket API fo details
    enum { SHUT_READ = SHUT_RD, SHUT_WRITE = SHUT_WR, SHUT_BOTH = SHUT_RDWR };
    bool Shutdown(int how = SHUT_BOTH, Error* e = nullptr) noexcept; 

    // Validate the socket
    bool Valid() const noexcept { return _fd != INVALID_SOCKET; }

    // Get descriptor of the socket
    // the ownership of the socket is not affected
    operator SOCKET() const noexcept { return _fd; }
    SOCKET Descriptor() const noexcept { return _fd; }

    // Get some features of the socket
    sa_family_t Family(Error* e = nullptr) const noexcept; // AF_XXX
    int Domain(Error* e = nullptr) const noexcept; // PF_XXX
    int Type(Error* e = nullptr) const noexcept; // SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...
    int Protocol(Error* e = nullptr) const noexcept; // IPPROTO_TCP, IPPROTO_UDP, ...

public: 
    // Explicitly bind the socket to a local address
    void Bind(const SocketAddress& addr); // throw on error
    bool Bind(const SocketAddress& addr, Error* e) noexcept;

    // Get local bound address
    // return empty address on errors
    SocketAddress Address(Error* e = nullptr) const noexcept;

    // Listen to start waiting for incomming connections (for TCP socket only)
    enum { DEFAULT_BACKLOG = SOMAXCONN };
    void Listen(int backlog = DEFAULT_BACKLOG); // Throw on errors
    bool Listen(int backlog, Error* e) noexcept;

    // Accept an incomming connection (for TCP socket only)
    // return INVALID_SOCKET on errors
    SOCKET Accept(SocketAddress* addr = NULL); // throw on error
    SOCKET Accept(SocketAddress* addr, Error* e) noexcept;

    // Connect to remote address to establish outgoing connection (for TCP socket)
    // or bind a remote address for I/O (for UDP socket), remove binding with empty address
    void Connect(const SocketAddress& addr); // throw on error
    bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Get connected address
    // return empty address on errors
    SocketAddress ConnectedAddress(Error* e = nullptr) const noexcept;

    // Timeout control for read and write
    // Return true if I/O is ready, return false if timeout or errors occurred
    // timeout in milliseconds, -1: block, 0: non-block, >0: block with timeout
    bool WaitForRead(int timeout = -1, Error* e = nullptr) noexcept;
    bool WaitForWrite(int timeout = -1, Error* e = nullptr) noexcept;

    // Timeout control for I/O events
    // Return -1 for errors, 0 for timeout, and >0 for I/O events
    // timeout in milliseconds, -1: errors, 0: timeout, >0: events
    int WaitForReady(int timeout = -1, Error* e = nullptr) noexcept; 

    // Send and receive data through connected socket
    ssize_t Send(const void* p, size_t n, int flags = 0, Error* e = nullptr) noexcept;
    ssize_t Receive(void* p, size_t n, int flags = 0, Error* e = nullptr) noexcept;

    // Send and receive data through non-connected socket
    // If the socket is connected and no addr is given, equivalent to Send() and Receive()
    // If the socket is connected and addr is given, addr must be equal to connected address
    ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, int flags = 0, Error* e = nullptr) noexcept;
    ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0, Error* e = nullptr) noexcept;

    // Send and recieve data hold in struct msghdr
    // see sendmsg() and recvmsg() of socket API for details
    ssize_t SendMessage(const struct msghdr* msg, int flags = 0, Error* e = nullptr) noexcept;
    ssize_t ReceiveMessage(struct msghdr* msg, int flags = 0, Error* e = nullptr) noexcept;

public: 
    // Control flag of IO mode: block or non-block
    void Block(bool block); // default is block
    bool Block(bool block, Error* e) noexcept;

    // Socket option of reuse address
    void ReuseAddress(bool reuse); // default is false
    bool ReuseAddress(bool reuse, Error* e) noexcept;

    // Socket option of reuse port
    void ReusePort(bool reuse); // default is false
    bool ReusePort(bool reuse, Error* e) noexcept;

    // Set and get options of socket
    bool SetOption(int level, int name, const void* val, socklen_t len, Error* e) noexcept;
    bool GetOption(int level, int name, void* val, socklen_t* len, Error* e) const noexcept;

private:
    // Internal socket descriptor (file descriptor)
    SOCKET _fd; 

    // Initialize socket
    // return false on errors
    bool InitSocket(int domain, int type, int protocol, Error* e) noexcept;
};

NETB_END

#endif

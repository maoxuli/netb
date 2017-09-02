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
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// Wrapper function for close socket
// the socket is closed anyway, even with errors
// return false on errors occurred
//
bool CloseSocket(SOCKET s, Error* e = NULL) noexcept; 

//
// Socket is a general wrapper class of socket API related to a socket descriptor. 
// It holds a socket descriptor and implements functions for all possible operations 
// working on a socket descriptor. Socket object can be initialized with any kind 
// of socket descriptor (i.e. different domain, type, and protocol), thus supports 
// all possible operations and options for all kinds of socket descriptors. By this 
// mean, Socket is a complete wrapper class of socket API that can complete socket 
// programming by itself without derivation. 
// 
// Socket is a lower level (ant very thin) wrapper of socket API with the major 
// purpose of cross-platform programming. It provides an easy-to-use interface for 
// socket programming with the coordination of classes Socket, SocketSelector, 
// SocketAddress, and StreamBuffer. It also supports a mixed style error handling 
// with classes of Error and Exception. 
//
// Socket maintains strict ownership semantics. If a Socket object has a valid, 
// opened socket descriptor, then it owns the socket and will close it when the 
// object is destroyed. By this mean Socket is a non-copyable object, it can not 
// be passed to functions by value. 
// 
class Socket : private Uncopyable
{
public: 
    // No socket is opened
    // Need to call Create() or Attach()
    Socket() noexcept;

    // Open socket for given domain, type, and protocol
    // No socket is opened if errors ocurred
    Socket(int domain, int type, int protocol); // throw on errors
    Socket(int domian, int type, int protocol, Error* e) noexcept; 

    // Attch an externally opened socket
    explicit Socket(SOCKET s) noexcept;

    // Close opened socket, failure is ignored
    virtual ~Socket() noexcept;

    // Open socket for given domain, type, and protocol
    // Close currently opened socket firstly, failure is ignored
    // No socket is opened if errors occurred
    Socket& Create(int domain, int type, int protocol); // throw on error
    bool Create(int domain, int type, int protocol, Error* e) noexcept;

    // Attach an externally opened socket
    // Close currently opened socket firstly, failure is ignored
    Socket& Attach(SOCKET s) noexcept;

    // Separate the socket form the object
    // The opened socket is taken over by caller
    SOCKET Detach() noexcept;

    // Close opened socket, see close()/closesocket() of socket API
    // Return false if error occurred
    // Note: the socket has been always closed, even error occurred
    // no retry is necessary  
    bool Close(Error* e = NULL) noexcept;

    // Shutdwon the socket, see shutdown() of socket API
    // return false if error occured
    enum { SHUT_READ = SHUT_RD, SHUT_WRITE = SHUT_WR, SHUT_BOTH = SHUT_RDWR };
    bool Shutdown(int how = SHUT_BOTH, Error* e = NULL) noexcept; 

    // Validate the socket
    bool Valid() const { return _fd != INVALID_SOCKET; }

    // Get descriptor of the socket
    operator SOCKET() const noexcept { return _fd; }
    SOCKET Descriptor() const noexcept { return _fd; }

    // Get features of the socket
    sa_family_t Family(Error* e = NULL) const noexcept; // AF_XXX
    int Domain(Error* e = NULL) const noexcept; // PF_XXX
    int Type(Error* e = NULL) const noexcept; // SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, ...
    int Protocol(Error* e = NULL) const noexcept; // IPPROTO_TCP, IPPROTO_UDP, ...

public: 
    // Explicitly bind to a local address
    Socket& Bind(const SocketAddress& addr); // throw on error
    bool Bind(const SocketAddress& addr, Error* e) noexcept;

    // Get local binded address
    SocketAddress Address(Error* e = NULL) const noexcept;

    // TCP socket listen to start waiting for incomming connections
    enum { DEFAULT_BACKLOG = SOMAXCONN };
    Socket& Listen(int backlog = DEFAULT_BACKLOG); // Throw on errors
    bool Listen(int backlog, Error* e) noexcept;

    // TCP socket accepts an incomming connection
    SOCKET Accept(); // throw on error
    SOCKET Accept(Error* e) noexcept;

    // Accept with remote address
    // Equal to Accept() if addr is given by NULL
    SOCKET AcceptFrom(SocketAddress* addr); // throw on error
    SOCKET AcceptFrom(SocketAddress* addr, Error* e) noexcept;

    // TCP socket connects to remote address to establish outgoing connection
    // UDP socket connects to bind a remote address only, empty address to remove 
    Socket& Connect(const SocketAddress& addr); // throw on error
    bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // connected status
    bool IsConnected() const;

    // Get connected address
    SocketAddress ConnectedAddress(Error* e) const noexcept;

    // Select events of the sockets
    // -1: block, 0: non-block, >0: block with timeout
    bool WaitForRead(int timeout = -1, Error* e = NULL) noexcept;
    bool WaitForWrite(int timeout = -1, Error* e = NULL) noexcept;

    // -1: errors, 0: timeout, >0: events
    int WaitForReady(int timeout = -1, Error* e = NULL) noexcept; 

    // Send and receive data through connected socket
    ssize_t Send(const void* p, size_t n, int flags = 0) noexcept;
    ssize_t Send(StreamBuffer* buf, int flags = 0) noexcept;

    ssize_t Receive(void* p, size_t n, int flags = 0) noexcept;
    ssize_t Receive(StreamBuffer* buf, int flags = 0) noexcept;

    // Send and receive data through non-connected socket
    // If the socket is connected and addr is given by NULL, equivalent to Send and Receive
    // If the socket is connected and valid addr is given, addr must be equal to connected address
    ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, int flags = 0) noexcept;
    ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags = 0) noexcept;

    ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0) noexcept;
    ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0) noexcept;

public: 
    // Control flag of IO mode: block or non-block
    Socket& Block(bool block); // default is block
    bool Block(bool block, Error* e) noexcept;

    // Socket option of reuse address and reuse port
    Socket& ReuseAddress(bool reuse); // default is false
    bool ReuseAddress(bool reuse, Error* e) noexcept;

    Socket& ReusePort(bool reuse); // default is false
    bool ReusePort(bool reuse, Error* e) noexcept;

    // Set and get options of socket
    bool SetOption(int level, int name, const void* val, socklen_t len, Error* e) noexcept;
    bool GetOption(int level, int name, void* val, socklen_t* len, Error* e) const noexcept;

private:
    // Internal socket descriptor (file descriptor)
    SOCKET _fd; 

    // Initialize socket
    bool InitSocket(int domain, int type, int protocol, Error* e) noexcept;
};

NETB_END

#endif

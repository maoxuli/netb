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

#include "Socket.hpp"
#include <sstream>
#include <cassert>

NETB_BEGIN

// Wrapper function of close socket
// The socket is closed anyway, but maybe with errors
// return false is with errors
bool CloseSocket(SOCKET s, Error* e) noexcept
{
    if(s == INVALID_SOCKET) return true;
#ifdef _WIN32
    int ret = ::closesocket(s);
#else
    int ret = ::close(s);
#endif 
    s = INVALID_SOCKET;
    if( ret < 0)
    {
        SET_SOCKET_ERROR(e, ErrorCode::Description(), ErrorCode::Current());
        return false;
    }  
    return true;
}

// No socket is opened
// Need to call Create() or Attach()
Socket::Socket() noexcept
: _fd(INVALID_SOCKET)
{

}

// Open socket for given domain, type, and protocol
// No socket is opened if errors ocurred
Socket::Socket(int domain, int type, int protocol)
{
    Error e;
    if(!InitSocket(domain, type, protocol, &e))
    {
        THROW_ERROR(e);
    }
}

// Open socket for given domain, type, and protocol
// No socket is opened if errors ocurred
Socket::Socket(int domain, int type, int protocol, Error* e) noexcept
{
    InitSocket(domain, type, protocol, e);
}

// Attch an externally opened socket
Socket::Socket(SOCKET s) noexcept
: _fd(s)
{

}

// Close opened socket, failure is ignored
Socket::~Socket() noexcept
{
    if(_fd != INVALID_SOCKET)
    {
        Close();
    }
}

// Open socket for given domain, type, protocol
bool Socket::InitSocket(int domain, int type, int protocol, Error* e) noexcept
{
    assert(_fd == INVALID_SOCKET);
    _fd = ::socket(domain, type, protocol);
    if(_fd == INVALID_SOCKET)
    {
        SET_SOCKET_ERROR(e, "Open socket failed.", ErrorCode::Current());
        return false;
    }
    return true;
}

// Open socket for given domain, type, and protocol
// Close currently opened socket firstly, failure is ignored
// No socket is opened if errors occurred
Socket& Socket::Create(int domain, int type, int protocol)
{
    Error e;
    if(!Create(domain, type, protocol, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

// Open socket for given domain, type, and protocol
// Close currently opened socket firstly, failure is ignored
// No socket is opened if errors occurred
bool Socket::Create(int domain, int type, int protocol, Error* e) noexcept
{
    if(_fd != INVALID_SOCKET)
    {
        Close();
    }
    return InitSocket(domain, type, protocol, e);
}

// Attach an externally opened socket
// Close currently opened socket firstly, failure is ignored
Socket& Socket::Attach(SOCKET s) noexcept
{
    if(_fd == s)
    {
        return *this;
    }
    if(_fd != INVALID_SOCKET)
    {
        Close();
    }
    _fd = s;
    return *this;
}

// Separate the socket form the object
// The opened socket is taken over by caller
SOCKET Socket::Detach() noexcept
{
    SOCKET s = _fd;
    _fd = INVALID_SOCKET;
    return s;
}

// Close opened socket, see close()/closesocket() of socket API
// Event errors returned, the socket is usually closed, the return 
// value are used for diagnose only. For EINTR error, the socket is 
// closed on most platform, but with exceptions, e.g. HP-UX.
// Error is return for diagnose only
bool Socket::Close(Error* e) noexcept
{
    if(!CloseSocket(_fd, e))
    {
        return false;
    }
    return true;
}

// Shutdwon the socket, see shutdown() of socket API
bool Socket::Shutdown(int how, Error* e) noexcept
{
    if(_fd == INVALID_SOCKET) return true;
    int ret = ::shutdown(_fd, how);
    if(ret < 0)
    {
        SET_SOCKET_ERROR(e, "Shutdown socket failed.", ErrorCode::Current());
        return false;
    }
    return true;
}

// Get features of the socket
sa_family_t Socket::Family(Error* e) const noexcept
{
    return (sa_family_t)Domain(e);
}

int Socket::Domain(Error* e) const noexcept
{
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(struct sockaddr_storage));
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if(::getsockname(_fd, (sockaddr*)&addr, &addrlen) < 0)
    {
        SET_SOCKET_ERROR(e, "Get socket domain failed.", ErrorCode::Current());
        return PF_UNSPEC;
    }
    return addr.ss_family;
}

int Socket::Type(Error* e) const noexcept
{
    int type;
    socklen_t len = sizeof(int);
    if(::getsockopt(_fd, SOL_SOCKET, SO_TYPE, &type, &len) < 0)
    {
        SET_SOCKET_ERROR(e, "Get socket type failed.", ErrorCode::Current());
        return 0;
    }
    return type;
}

int Socket::Protocol(Error* e) const noexcept
{
    assert(false);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

// Explicitly bind to a local address
// throw on errors
Socket& Socket::Bind(const SocketAddress& addr)
{
    Error e;
    if(!Bind(addr, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

// return errors in out paramter
bool Socket::Bind(const SocketAddress& addr, Error* e) noexcept
{
    if(_fd == INVALID_SOCKET
       && !InitSocket(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    if(::bind(_fd, addr.Addr(), addr.Length()) < 0)
    {
        std::ostringstream info;
        info << "Bind address to socket failed [" << _fd << "][" << addr.ToString() << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

// Get local binded address
SocketAddress Socket::Address(Error* e) const noexcept
{
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(struct sockaddr_storage));
    if(_fd == INVALID_SOCKET) return addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if(::getsockname(_fd, (sockaddr*)&addr, &addrlen) < 0)
    {
        std::ostringstream info;
        info << "Get socket local address failed [" << _fd << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
    }
    return addr;
}

// TCP socket listen to start waiting for incomming connections
// Throw on errors
Socket& Socket::Listen(int backlog)
{
    Error e;
    if(!Listen(backlog, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

bool Socket::Listen(int backlog, Error* e) noexcept
{
    if(_fd == INVALID_SOCKET)
    {
        SocketAddress addr(0, AF_INET); // default any address
        if(!Bind(addr, e))
        {
            return false;
        }
    }
    assert(_fd != INVALID_SOCKET);
    if(::listen(_fd, backlog) < 0)
    {
        std::ostringstream info;
        info << "Socket lsiten failed [" << _fd << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

// TCP socket accepts an incomming connection
// Note: ::accept is not a I/O function
SOCKET Socket::Accept()
{
    SOCKET s;
    Error e;
    if((s = Accept(&e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

SOCKET Socket::Accept(Error* e) noexcept
{
    assert(_fd != INVALID_SOCKET);
    SOCKET s;
    while((s = ::accept(_fd, NULL, NULL)) == INVALID_SOCKET)
    {
        if(!ErrorCode::IsInterrupted())
        {
            std::ostringstream info;
            info << "Socket accept failed [" << _fd << "].";
            SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
            break;
        }
    }
    return s;
}

// Accept with remote address
// Equal to Accept() if addr is given by NULL
SOCKET Socket::AcceptFrom(SocketAddress* addr)
{
    Error e;
    SOCKET s;
    if((s = AcceptFrom(addr, &e)) == INVALID_SOCKET)
    {
        THROW_ERROR(e);
    }
    return s;
}

SOCKET Socket::AcceptFrom(SocketAddress* addr, Error* e) noexcept
{
    assert(_fd != INVALID_SOCKET);
    if(!addr) return Accept(e);
    SOCKET s;
    addr->Reset();
    socklen_t addrlen = addr->Length();
    while((s = ::accept(_fd, (sockaddr*)addr, &addrlen)) == INVALID_SOCKET)
    {
        if(!ErrorCode::IsInterrupted())
        {
            std::ostringstream info;
            info << "Socket accept failed [" << _fd << "].";
            SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
            break;
        }
    }
    return s;
}

// TCP socket connects to remote address to establish outgoing connection
// UDP socket connects to bind a remote address only
Socket& Socket::Connect(const SocketAddress& addr)
{
    Error e;
    if(!Connect(addr, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

// ::connect is a I/O function and has different behavior in block and non-block mode
// In block mode: return 0 denotes success in remote end
// In non-block mode: always return -1, InProgress/WouldBlock denotes no error occurred.
// status need to be checked with select() and getsockopt()
bool Socket::Connect(const SocketAddress& addr, Error* e) noexcept
{
    if(_fd == INVALID_SOCKET
       && !InitSocket(addr.Family(), SOCK_STREAM, IPPROTO_TCP, e))
    {
        return false;
    }
    while(::connect(_fd, addr.Addr(), addr.Length()) < 0)
    {
        if(ErrorCode::IsInProgress()) 
        {
            // non-block mode, status need to be checked later
            break;
        }
        else if(!ErrorCode::IsInterrupted())
        {
            // failed with errors, info is the address
            std::ostringstream info;
            info << "Socket connect failed [" << _fd << "][" << addr.ToString() << "].";
            SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
            return false;
        }
        // Only try again when returned for interruption in block mode.
    }
    return true;
}

// Get connected address
SocketAddress Socket::ConnectedAddress(Error* e) const noexcept
{
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(struct sockaddr_storage));
    if(_fd == INVALID_SOCKET) return addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if(::getpeername(_fd, (struct sockaddr*)&addr, &addrlen) < 0)
    {
        std::ostringstream info;
        info << "Get socket remote address failed [" << _fd << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
    }
    return addr;
}

// Select events of the sockets
// -1: block, 0: non-block, >0: block with timeout
bool Socket::WaitForRead(int timeout, Error* e) noexcept
{
    SocketSelector selector(_fd, SOCKET_EVENT_READ);
    std::vector<SocketSelector::SocketEvents> sockets;
    if(!selector.Select(sockets, timeout, e))
    {
        return false;
    }
    assert(!sockets.empty());
    assert(sockets[0].fd == _fd);
    assert((sockets[0].events | SOCKET_EVENT_READ) > 0);
    return true;
}

bool Socket::WaitForWrite(int timeout, Error* e) noexcept
{
    SocketSelector selector(_fd, SOCKET_EVENT_WRITE);
    std::vector<SocketSelector::SocketEvents> sockets;
    if(!selector.Select(sockets, timeout, e))
    {
        return false;
    }
    assert(!sockets.empty());
    assert(sockets[0].fd == _fd);
    assert((sockets[0].events | SOCKET_EVENT_WRITE) > 0);
    return true;
}

// -1: errors, 0: timeout, >0: events
int Socket::WaitForReady(int timeout, Error* e) noexcept
{
    SocketSelector selector(_fd, SOCKET_EVENT_READ | SOCKET_EVENT_WRITE);
    std::vector<SocketSelector::SocketEvents> sockets;
    if(!selector.Select(sockets, timeout, e))
    {
        return -1;
    }
    assert(!sockets.empty());
    assert(sockets[0].fd == _fd);
    assert(sockets[0].events > 0);
    return sockets[0].events;
} 

// Return > 0 denotes the number of bytes has been sent
// Return = 0 denotes not ready, try later
// Return < 0 denotes errors ocurred
ssize_t Socket::Send(const void* p, size_t n, int flags) noexcept
{ 
    if(_fd == INVALID_SOCKET) return -1;
    assert(p != NULL);
    ssize_t ret;
    while((ret = ::send(_fd, p, n, flags)) < 0)
    {
        if(ErrorCode::IsWouldBlock())
        {
            // Non-block mode returned, need to wait for ready to send
            ret = 0;
            break;
        }
        else if(!ErrorCode::IsInterrupted())
        {
            // return on errors
            break;
        }
        // Only try again when returned for interruption in block mode
    }
    return ret;
}

ssize_t Socket::Send(StreamBuffer* buf, int flags) noexcept
{
    assert(buf != NULL);
    ssize_t ret = Send(buf->Read(), buf->Readable(), flags);
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

// Return > 0 denotes the number of bytes has been sent
// Return = 0 denotes not ready, try later
// Return < 0 denotes errors ocurred
ssize_t Socket::Receive(void* p, size_t n, int flags) noexcept
{
    if(_fd == INVALID_SOCKET) return -1;
    assert(p != NULL);
    ssize_t ret;
    while((ret = ::recv(_fd, p, n, flags)) < 0)
    {
        if(ErrorCode::IsWouldBlock())
        {
            // Non-block mode returned, need to wait for ready to receive 
            ret = 0;
            break;
        }
        if(!ErrorCode::IsInterrupted())
        {
            // return on errors
            break;
        }
        // Only try again when returned for interruption in block mode.
    }
    return ret;
}

ssize_t Socket::Receive(StreamBuffer* buf, int flags) noexcept
{
    assert(buf != NULL);
    ssize_t ret = Receive(buf->Write(), buf->Writable(), flags);
    if(ret > 0)
    {
        buf->Write(ret);
    }
    return ret;
}

// Send and receive data through non-connected socket
// If the socket is connected and addr is given by NULL, equivalent to Send and Receive
// If the socket is connected and valid addr is given, addr must be equal to connected address
// Return > 0 denotes the number of bytes has been sent
// Return = 0 denotes not ready, try later
// Return < 0 denotes errors ocurred
// Todo: check the address is equal to remote address when connected
ssize_t Socket::SendTo(const void* p, size_t n, const SocketAddress* addr, int flags) noexcept
{
    if(_fd == INVALID_SOCKET) return -1;
    if(!addr) return Send(p, n, flags);
    assert(p != NULL);
    ssize_t ret;
    while((ret = ::sendto(_fd, p, n, flags, addr->Addr(), addr->Length())) < 0)
    {
        if(ErrorCode::IsWouldBlock())
        {
            // Non-block mode returned, need to wait for ready to send
            ret = 0;
            break;
        }
        else if(!ErrorCode::IsInterrupted())
        {
            // return on errors
            break;
        }
        // Only try again when returned for interruption in block mode
    }
    return ret;
}

ssize_t Socket::SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags) noexcept
{
    assert(buf != NULL);
    ssize_t ret = SendTo(buf->Read(), buf->Readable(), addr, flags);
    if(ret > 0)
    {
        buf->Read(ret);
    }
    return ret;
}

// Return > 0 denotes the number of bytes has been sent
// Return = 0 denotes not ready, try later
// Return < 0 denotes errors ocurred
// Todo: check the address is equal to remote address when connected
ssize_t Socket::ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags) noexcept
{
    
    if(_fd == INVALID_SOCKET) return -1;
    if(!addr) return Receive(p, n, flags);
    assert(p != NULL);
    ssize_t ret;
    socklen_t addrlen = addr->Length();
    while((ret = ::recvfrom(_fd, p, n, flags, addr->Addr(), &addrlen)) < 0)
    {
        if(ErrorCode::IsWouldBlock())
        {
            // Non-block mode returned, need to wait for ready to receive 
            ret = 0;
            break;
        }
        if(!ErrorCode::IsInterrupted())
        {
            // return on errors
            break;
        }
        // Only try again when returned for interruption in block mode.
    }
    return ret;
}

ssize_t Socket::ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags) noexcept
{
    assert(buf != NULL);
    ssize_t ret = ReceiveFrom(buf->Write(), buf->Writable(), addr, flags);
    if(ret > 0)
    {
        buf->Write(ret);
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////

// socket IO control, block mode or non-block mode
Socket& Socket::Block(bool block)
{
    Error e;
    if(!Block(block, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

bool Socket::Block(bool block, Error* e) noexcept
{
#ifdef _WIN32
    unsigned long arg = block ? 0 : 1;
    int ret = ::ioctlsocket(_fd, FIONBIO, &arg);
#else
    int flags = ::fcntl(_fd, F_GETFL);
    if(block) flags &= ~O_NONBLOCK;
    else flags |= O_NONBLOCK;
    int ret = ::fcntl(_fd, F_SETFL, flags);
#endif
    if(ret < 0)
    {
        std::ostringstream info;
        info << "Set socket block option failed [" << _fd << "][" << block << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

// socket option of reuse address
Socket& Socket::ReuseAddress(bool reuse)
{
    Error e;
    if(!ReuseAddress(reuse, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

bool Socket::ReuseAddress(bool reuse, Error* e) noexcept
{
    int flag = reuse ? 1 : 0;
    if(::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, int(sizeof(int))) != 0)
    {
        std::ostringstream info;
        info << "Set socket reuse address option failed [" << _fd << "][" << reuse << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

// socket option of reuse port
Socket& Socket::ReusePort(bool reuse)
{
    Error e;
    if(!ReusePort(reuse, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

bool Socket::ReusePort(bool reuse, Error* e) noexcept
{
    assert(false);
    return false;
}

// Set socket options
bool Socket::SetOption(int level, int name, const void* val, socklen_t len, Error* e) noexcept
{
    if(::setsockopt(_fd, level, name, val, len) < 0)
    {
        std::ostringstream info;
        info << "Set socket option failed [" << _fd << "][" << level << "," << name << "," << val << "," << len << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

// Get socket options
bool Socket::GetOption(int level, int name, void* val, socklen_t* len, Error* e) const noexcept
{
    if(::getsockopt(_fd, level, name, val, len) < 0)
    {
        std::ostringstream info;
        info << "Get socket option failed [" << _fd << "][" << level << "," << name << "," << val << "," << len << "].";
        SET_SOCKET_ERROR(e, info.str(), ErrorCode::Current());
        return false;
    }
    return true;
}

NETB_END

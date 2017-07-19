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

#include "Socket.h"
#include "SocketError.h"
#include <cassert>

NET_BASE_BEGIN

Socket::Socket()
: fd(INVALID_SOCKET)
{

}

Socket::Socket(int domain, int type, int protocol)
: fd(::socket(domain, type, protocol))
{
    assert(fd != INVALID_SOCKET);
}

Socket::Socket(SOCKET s)
: fd(s)
{

}

Socket::~Socket()
{
    if(fd != INVALID_SOCKET)
    {
        Close();
    }
}

void Socket::Close()
{
#ifdef _WIN32
    ::closesocket(fd);
#else
    ::close(fd);
#endif  
}

bool Socket::Shutdown(int how)
{
    return (::shutdown(fd, how) != SOCKET_ERROR);
}

SOCKET Socket::Detach()
{
    SOCKET s = fd;
    fd = INVALID_SOCKET;
    return s;
}

Socket& Socket::Create(int domain, int type, int protocol)
{
    if(fd != INVALID_SOCKET)
    {
        Close();
    }
    fd = ::socket(domain, type, protocol);
    assert(fd != INVALID_SOCKET);
    return *this;
}

Socket& Socket::Attach(SOCKET s)
{
    if(fd == s)
    {
        return *this;
    }
    if(fd != INVALID_SOCKET)
    {
        Close();
    }
    fd = s;
    return *this;
}

int Socket::Domain() const
{
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(struct sockaddr_storage));
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if(::getsockname(fd, (sockaddr*)&addr, &addrlen) == 0)
    {
        return addr.ss_family;
    }
    return PF_UNSPEC;
}

int Socket::Type() const
{
    int type;
    socklen_t len = sizeof(type);
    if(::getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &len) ==  0)
    {
        return type;
    }
    return 0;
}

int Socket::Protocol() const 
{
    return 0;
}

bool Socket::Bind(const struct sockaddr* addr, socklen_t addrlen)
{
    return (::bind(fd, addr, addrlen) != SOCKET_ERROR);
}

bool Socket::Listen(int backlog)
{
    while(::listen(fd, backlog) == SOCKET_ERROR)
    {
        if(!SocketError::Interrupted())
        {
            return false;
        }
    }
    return true;
}

SOCKET Socket::Accept()
{
    SOCKET ret;
    while((ret = ::accept(fd, NULL, NULL)) == INVALID_SOCKET)
    {
        if(!SocketError::Interrupted())
        {
            return false;
        }
    }
    return ret;
}

SOCKET Socket::Accept(struct sockaddr* addr, socklen_t* addrlen)
{
    assert(addr != NULL);
    SOCKET ret;
    while((ret = ::accept(fd, addr, addrlen)) == SOCKET_ERROR)
    {
        if(!SocketError::Interrupted())
        {
            return false;
        }
    }
    return ret;
}

bool Socket::Connect(const struct sockaddr* addr, socklen_t addrlen)
{
repeat:
    if(::connect(fd, addr, addrlen) == SOCKET_ERROR)
    {
        if(SocketError::Interrupted())
        {
            goto repeat;
        }
        if(SocketError::InProgress())
        {
            return true;
        }
        return false; // cases?
    }
    return true;
}

bool Socket::LocalAddress(struct sockaddr* addr, socklen_t* addrlen) const
{
    if(::getsockname(fd, addr, addrlen) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}

bool Socket::RemoteAddress(struct sockaddr* addr, socklen_t* addrlen) const
{
    if(::getpeername(fd, addr, addrlen) == SOCKET_ERROR)
    {
        if(SocketError::NotConnected())
        {
            return false;
        }
        return false; // cases?
    }
    return true;
}

ssize_t Socket::Send(const void* p, size_t n)
{
    ssize_t ret = 0;
    while(true)
    {
        if((ret = ::write(fd, p, n)) == SOCKET_ERROR)
        {
            if(SocketError::Interrupted())
            {
                continue;
            }
        }
        break;
    }
    return ret;
}

ssize_t Socket::Receive(void* p, size_t n)
{
    ssize_t ret = 0;
    while(true)
    {
        if((ret = ::read(fd, p, n)) == SOCKET_ERROR)
        {
            if(SocketError::Interrupted())
            {
                continue;
            }
        }
        break;
    }
    return ret;
}

ssize_t Socket::SendTo(const void* p, size_t n, const struct sockaddr* addr, socklen_t addrlen)
{
    ssize_t ret = 0;
    while(true)
    {
        if((ret = ::sendto(fd, p, n, 0, addr, addrlen)) == SOCKET_ERROR)
        {
            if(SocketError::Interrupted())
            {
                continue;
            }
        }
        break;
    }
    return ret;
}

ssize_t Socket::ReceiveFrom(void* p, size_t n, struct sockaddr* addr, socklen_t* addrlen)
{
    ssize_t ret = 0;
    while(true)
    {
        if((ret = ::recvfrom(fd, p, n, 0, addr, addrlen)) == SOCKET_ERROR)
        {
            if(SocketError::Interrupted())
            {
                continue;
            }
        }
        break;
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////

// socket IO control, block mode or non-block mode
bool Socket::Block(bool block)
{
    if(block)
    {
#ifdef _WIN32
        unsigned long arg = 0;
        if(::ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
        {
            return false;
        }
#else
        int flags = ::fcntl(fd, F_GETFL);
        flags &= ~O_NONBLOCK;
        if(::fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
        {
            return false;
        }
#endif
    }
    else
    {
#ifdef _WIN32
        unsigned long arg = 1;
        if(::ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
        {
            return false;
        }
#else
        int flags = ::fcntl(fd, F_GETFL);
        flags |= O_NONBLOCK;
        if(::fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
        {
            return false;
        }
#endif
    }
    return true;
}

// socket option of reuse address
bool Socket::ReuseAddress(bool reuse)
{
    int flag = reuse ? 1 : 0;
    if(::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}

// socket option of reuse port
bool Socket::ReusePort(bool reuse)
{
    return false;
}

// Set socket options
bool Socket::SetOption(int level, int name, const void* val, socklen_t len)
{
    return false;
}

// Get socket options
bool Socket::GetOption(int level, int name, void* val, socklen_t* len) const
{
    return false;
}

NET_BASE_END

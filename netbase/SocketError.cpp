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

#include "SocketError.hpp"
#include <errno.h>

NET_BASE_BEGIN

int SocketError::Code()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool SocketError::Interrupted()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEINTR;
#else
#   ifdef EPROTO
    return errno == EINTR || errno == EPROTO;
#   else
    return errno == EINTR;
#   endif
#endif
}

bool SocketError::AcceptInterrupted()
{
    if(Interrupted())
    {
        return true;
    }
#ifdef _WIN32
    int errnum = WSAGetLastError();
    return errnum == WSAECONNABORTED ||
           errnum == WSAECONNRESET ||
           errnum == WSAETIMEDOUT;
#else
    return errno == ECONNABORTED ||
           errno == ECONNRESET ||
           errno == ETIMEDOUT;
#endif
}

bool SocketError::InProgress()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EINPROGRESS;
#endif
}

bool SocketError::WouldBlock()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

bool SocketError::Timeout()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAETIMEDOUT;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

bool SocketError::NoMoreFds()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEMFILE;
#else
    return errno == EMFILE || errno== ENFILE;
#endif
}

bool SocketError::NoBuffers()
{
#ifdef _WIN32
    int errnum = WSAGetLastError();
    return errnum == WSAENOBUFS ||
           errnum == WSAEFAULT;
#else
    return errno == ENOBUFS;
#endif
}

bool SocketError::NotConnected()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAENOTCONN;
#elif defined(__APPLE__) || defined(__FreeBSD__)
    return errno == ENOTCONN || errno == EINVAL;
#else
    return errno == ENOTCONN;
#endif
}

bool SocketError::ConnectFailed()
{
#ifdef _WIN32
    int errnum = WSAGetLastError();
    return errnum == WSAECONNREFUSED ||
           errnum == WSAETIMEDOUT ||
           errnum == WSAENETUNREACH ||
           errnum == WSAEHOSTUNREACH ||
           errnum == WSAECONNRESET ||
           errnum == WSAESHUTDOWN ||
           errnum == WSAECONNABORTED;
#else
    return errno == ECONNREFUSED ||
           errno == ETIMEDOUT ||
           errno == ENETUNREACH ||
           errno == EHOSTUNREACH ||
           errno == ECONNRESET ||
           errno == ESHUTDOWN ||
           errno == ECONNABORTED;
#endif
}

bool SocketError::ConnectionRefused()
{
#ifdef _WIN32
    int errnum = WSAGetLastError();
    return errnum == WSAECONNREFUSED;
#else
    return errno == ECONNREFUSED;
#endif
}

bool SocketError::ConnectionLost()
{
#ifdef _WIN32
    int errnum = WSAGetLastError();
    return errnum == WSAECONNRESET ||
           errnum == WSAESHUTDOWN ||
           errnum == WSAENOTCONN ||
           errnum == WSAECONNABORTED;
#else
    return errno == ECONNRESET ||
           errno == ENOTCONN ||
           errno == ESHUTDOWN ||
           errno == ECONNABORTED ||
           errno == EPIPE;
#endif
}

NET_BASE_END

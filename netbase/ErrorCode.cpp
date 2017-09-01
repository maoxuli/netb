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

#include "ErrorCode.hpp"
#include <errno.h>

NET_BASE_BEGIN

// Last error code for just occurred error
int ErrorCode::Current()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

void ErrorCode::SetCurrent(int code)
{
#ifdef _WIN32
    WSASetLastError(code);
#else
    errno = code;
#endif    
}

// Text description of error code
std::string ErrorCode::Description()
{
    return Description(Current());
}

std::string ErrorCode::Description(int code)
{
    return "";
}

int ErrorCode::Timeout()
{
    return 0;
}

// Invalid socket error code
int ErrorCode::InvalidSocket()
{
#ifdef _WIN32
    return WSAENOTSOCK;
#else
    return ENOTSOCK;
#endif 
}

bool ErrorCode::IsInvalidSocket()
{
    return IsInvalidSocket(Current());
}

bool ErrorCode::IsInvalidSocket(int code)
{
#ifdef _WIN32
    return code == WSAENOTSOCK;
#else
    return code == ENOTSOCK;
#endif 
}

int ErrorCode::UnsupportedFamily()
{
    return 0;
}

bool ErrorCode::IsUnsupportedFamily()
{
    return IsUnsupportedFamily(Current());
}

bool ErrorCode::IsUnsupportedFamily(int code)
{
    return code == UnsupportedFamily();
}

// System interruption in block mode
int ErrorCode::Interrupted()
{
#ifdef _WIN32
    return WSAEINTR;
#else
    return EINTR;
#endif
}

bool ErrorCode::IsInterrupted()
{
    return IsInterrupted(Current());
}

bool ErrorCode::IsInterrupted(int code)
{
#ifdef _WIN32
    return code == WSAEINTR;
#else
    return errno == EINTR;
#endif
}

// Connect is in progress in non-block mode
// EINPROGRESS/WSAWOULDBLOCK
int ErrorCode::InProgress()
{
#ifdef _WIN32
    return WSAEWOULDBLOCK;
#else
    return EINPROGRESS;
#endif
}

bool ErrorCode::IsInProgress()
{
    return IsInProgress(Current());
}

bool ErrorCode::IsInProgress(int code)
{
#ifdef _WIN32
    return code == WSAEWOULDBLOCK;
#else
    return code == EINPROGRESS;
#endif
}

// I/O is not ready in non-block mode
// EWOULDBLOCK/EAGAIN/ASAWOULDBLOCK
int ErrorCode::WouldBlock()
{
#ifdef _WIN32
    return WSAEWOULDBLOCK;
#else
    return EWOULDBLOCK;
#endif    
}

bool ErrorCode::IsWouldBlock()
{
    return IsWouldBlock(Current());
}

bool ErrorCode::IsWouldBlock(int code)
{
#ifdef _WIN32
    return code == WSAEWOULDBLOCK;
#else
    return code == EAGAIN || code == EWOULDBLOCK;
#endif
}

NET_BASE_END

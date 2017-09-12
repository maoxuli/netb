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

NETB_BEGIN

// Get system last-error
int ErrorCode::Current()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

// Set system last error
void ErrorCode::SetCurrent(int code)
{
#ifdef _WIN32
    WSASetLastError(code);
#else
    errno = code;
#endif    
}

// Text description of last-error
std::string ErrorCode::Description()
{
    return Description(Current());
}

// Text description of given error code
std::string ErrorCode::Description(int code)
{
    return "";
}

// Interrupted error in block mode
int ErrorCode::Interrupted()
{
#ifdef _WIN32
    return WSAEINTR;
#else
    return EINTR;
#endif
}

// Is last-error an interrupted error?
bool ErrorCode::IsInterrupted()
{
    return IsInterrupted(Current());
}

// Is given code an interrupted error?
bool ErrorCode::IsInterrupted(int code)
{
#ifdef _WIN32
    return code == WSAEINTR;
#else
    return errno == EINTR;
#endif
}

// In progress error in non-block mode
int ErrorCode::InProgress()
{
#ifdef _WIN32
    return WSAEWOULDBLOCK;
#else
    return EINPROGRESS;
#endif
}

// Is last-error an in progress error?
bool ErrorCode::IsInProgress()
{
    return IsInProgress(Current());
}

// I given error code an in progress error?
bool ErrorCode::IsInProgress(int code)
{
#ifdef _WIN32
    return code == WSAEWOULDBLOCK;
#else
    return code == EINPROGRESS;
#endif
}

// Would block error in non-block mode
// I/O is not ready yet. 
int ErrorCode::WouldBlock()
{
#ifdef _WIN32
    return WSAEWOULDBLOCK;
#else
    return EWOULDBLOCK;
#endif    
}

// Is last-error a would block error?
bool ErrorCode::IsWouldBlock()
{
    return IsWouldBlock(Current());
}

// Is given error code a would block error?
bool ErrorCode::IsWouldBlock(int code)
{
#ifdef _WIN32
    return code == WSAEWOULDBLOCK;
#else
    return code == EAGAIN || code == EWOULDBLOCK;
#endif
}

NETB_END

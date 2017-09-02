/*
 * Copyright (C) 2010-2016, Maoxu Li. Email: maoxu@lebula.com
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

#include "SocketSelector.hpp"
#include <cassert>

NETB_BEGIN

SocketSelector::SocketSelector() noexcept
{
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);
}

SocketSelector::SocketSelector(SOCKET s, int events) noexcept
{
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);
    SetupEvents(s, events);
}
SocketSelector::SocketSelector(const std::vector<SocketEvents>& sockets) noexcept
{
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);
    assert(false);
}

SocketSelector::SocketSelector(const fd_set* read, const fd_set* write, const fd_set* except) noexcept
{
    assert(!read);
    FD_COPY(read, &_read_set);
    assert(!write);
    FD_COPY(write, &_write_set);
    assert(!except);
    FD_COPY(except, &_except_set);
}

SocketSelector::~SocketSelector() noexcept
{
    
}

// Setup interested socket and events
// May result in adding or removing socket and events
void SocketSelector::SetupEvents(SOCKET s, int events) noexcept
{    
    if(events == SOCKET_EVENT_NONE)
    {
        Remove(s);
    }
    else
    {
        std::vector<SOCKET>::iterator it = _sockets.begin(); 
        while(it != _sockets.end() && *it != s)
        {
            ++it;
        }
        if(it == _sockets.end())
        {
            _sockets.push_back(s);
            std::make_heap(_sockets.begin(), _sockets.end());
        }

        if(events & SOCKET_EVENT_READ)
        {
            FD_SET(s, &_read_set);
        }
        else
        {
            FD_CLR(s, &_read_set);
        }

        if(events & SOCKET_EVENT_WRITE)
        {
            FD_SET(s, &_write_set);
        }
        else
        {
            FD_CLR(s, &_write_set);
        }

        if(events & SOCKET_EVENT_EXCEPT)
        {
            FD_SET(s, &_except_set);
        }
        else
        {
            FD_CLR(s, &_except_set);
        }
    }
}

// Remove a socket and its associated events
void SocketSelector::Remove(SOCKET s) noexcept
{
    for(std::vector<SOCKET>::iterator it = _sockets.begin(), 
        end = _sockets.end(); it != end; ++it)
    {
        if(*it == s)
        {
            _sockets.erase(it);
            break;
        }
    }
    std::make_heap(_sockets.begin(), _sockets.end());
    FD_CLR(s, &_read_set);
    FD_CLR(s, &_write_set);
    FD_CLR(s, &_except_set);
}

// Select sockets with active events
// timeout seconds, -1 for block
void SocketSelector::Select(std::vector<SocketEvents>& sockets, int timeout)
{
   Error e;
   if(!Select(sockets, timeout, &e))
   {
       THROW_ERROR(e);
   }
}

// Select sockets with active events
// timeout seconds, -1 for block
bool SocketSelector::Select(std::vector<SocketEvents>& sockets, int timeout, Error* e) noexcept
{
    ssize_t ret = 0;
    while(true)
    {
        FD_COPY(&_read_set, &_active_read_set);
        FD_COPY(&_write_set, &_active_write_set);
        FD_COPY(&_except_set, &_active_except_set);

        if(timeout < 0)
        {
            ret = ::select(_sockets.front() + 1, &_active_read_set, &_active_write_set, &_active_except_set, NULL); 
        }
        else
        {
            struct timeval tv;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            ret = ::select(_sockets.front() + 1, &_active_read_set, &_active_write_set, &_active_except_set, &tv); 
        }

        if(ret == 0)
        {
            // timeout, return with error
            SET_SOCKET_ERROR(e, "Socket select timeout.", ErrorCode::Timeout());
            return false;
        }
        else if(ret < 0)
        {
            // only try again on system interruption, otherwise return with errors
            if(!ErrorCode::IsInterrupted())
            {
                SET_SOCKET_ERROR(e, "Socket select failed.", ErrorCode::Current());
                return false;
            }
        }
        else  // succeed, breakl to process
        {
            break;
        }
    }
    // Check active sockets if not timeout
    sockets.clear();
    for(std::vector<SOCKET>::iterator it = _sockets.begin(), 
        end = _sockets.end(); it != end; ++it)
    {
        SOCKET fd = *it;
        int events = SOCKET_EVENT_NONE;
        if(FD_ISSET(fd, &_active_read_set))
        {
            events |= SOCKET_EVENT_READ;
        }
        if(FD_ISSET(fd, &_active_write_set))
        {
            events |= SOCKET_EVENT_WRITE;
        }
        if(FD_ISSET(fd, &_active_except_set))
        {
            events |= SOCKET_EVENT_EXCEPT;
        }

        if(events != SOCKET_EVENT_NONE)
        {
            sockets.push_back(SocketEvents(fd, events));
        }
    }
    return true;
}

NETB_END

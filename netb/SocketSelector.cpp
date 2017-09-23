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
    Set(s, events);
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
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);

    if(read) FD_COPY(read, &_read_set);
    if(write) FD_COPY(write, &_write_set);
    if(except) FD_COPY(except, &_except_set);
}

SocketSelector::~SocketSelector() noexcept
{
    
}

// Setup interested socket and events
// May result in adding or removing socket and events
bool SocketSelector::Set(SOCKET s, int events, Error* e) noexcept 
{    
    if(events == 0)
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
    return true;
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
            std::make_heap(_sockets.begin(), _sockets.end());
            break;
        }
    }
    FD_CLR(s, &_read_set);
    FD_CLR(s, &_write_set);
    FD_CLR(s, &_except_set);
}

// Select sockets with active events
// timeout in miliseconds, -1 for block
int SocketSelector::Select(std::vector<SocketEvents>& sockets, int timeout)
{
   Error e;
   int ret;
   if((ret = Select(sockets, timeout, &e)) < 0)
   {
       THROW_ERROR(e);
   }
   return ret;
}

// Select sockets with active events
// timeout in miliseconds, -1 for block
// Return -1 on errors, 0 on timeout, number of active sockets on success
int SocketSelector::Select(std::vector<SocketEvents>& sockets, int timeout, Error* e) noexcept
{
    ssize_t ret = 0;
    while(true)
    {
        FD_COPY(&_read_set, &_active_read_set);
        FD_COPY(&_write_set, &_active_write_set);
        FD_COPY(&_except_set, &_active_except_set);
        int nfds = _sockets.empty() ? 1 : _sockets.front() + 1;
        if(timeout < 0)
        {
            ret = ::select(nfds, &_active_read_set, &_active_write_set, &_active_except_set, 0); 
        }
        else
        {
            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;
            ret = ::select(nfds, &_active_read_set, &_active_write_set, &_active_except_set, &tv); 
        }
        if(ret < 0) // errors
        {
            // only try again on system interruption
            if(ErrorCode::IsInterrupted())
            {
                continue;
            }
            // otherwise return with errors
            SET_SYSTEM_ERROR(e, "Socket select failed [" << ret << "]");
            return ret;;
        }
        // success or timeout, proceed
        break;
    }
    // Check active sockets if not timeout
    sockets.clear();
    if(ret > 0)
    {
        for(auto it = _sockets.begin(), end = _sockets.end(); it != end; ++it)
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
    }
    return ret;
}

NETB_END

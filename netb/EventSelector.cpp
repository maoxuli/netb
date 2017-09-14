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

#include "EventSelector.hpp"
#include "EventHandler.hpp"

NETB_BEGIN

int EventSelector::WaitForEvents(std::vector<EventHandler*>& handlers, int timeout)
{
    handlers.clear();
    std::vector<struct SocketSelector::SocketEvents> sockets;
    int ret = _selector.Select(sockets, timeout, nullptr); // ignore errors
    if(ret > 0)
    {
        for(auto it = sockets.begin(), end = sockets.end(); it != end; ++it)
        {
            EventHandler* handler = _handlers[it->fd];
            assert(handler);
            handler->SetActiveEvents(it->events);
            handlers.push_back(handler);
        }
    }
    return ret;
}

// Update the interested I/O events of a EventHandler.
// This calling indicates that the handler is active and 
// can be access for further information
void EventSelector::SetupHandler(EventHandler* handler)
{
    SOCKET fd = handler->GetSocket();
    unsigned int events = handler->GetEvents();

    if(events == SOCKET_EVENT_NONE)
    {
        _handlers.erase(fd);
        _selector.Remove(fd);
    }
    else
    {
        // Always cover prior settings for safe
        _handlers[fd] = handler;
        _selector.SetupEvents(fd, events);
    }
}

// Remove a EventHandler, this calling indicate that a handler
// may be dead and further information may not accessed
void EventSelector::RemoveHandler(EventHandler* handler)
{
    std::map<SOCKET, EventHandler*>::iterator it = _handlers.begin();
    while(it != _handlers.end() && it->second != handler)
    {
        ++it;
    }
    if(it != _handlers.end()) // found
    {
        SOCKET fd = it->first;
        _handlers.erase(it);
        _selector.Remove(fd);
    }
}

NETB_END

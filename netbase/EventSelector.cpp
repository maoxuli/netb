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

#include "EventSelector.h"
#include "EventHandler.h"
#include <cassert>

NET_BASE_BEGIN

EventSelector::EventSelector()
{

}

EventSelector::~EventSelector()
{
    
}

// @param timout 0 for immediate return, -1 for wait unlimited
void EventSelector::WaitEvents(std::vector<EventHandler*>& handlers, int timeout)
{
    // Select events
    std::vector<struct SocketSelector::SocketEvents> sockets;
    mSelector.Select(sockets, timeout);
    handlers.clear();
    for(std::vector<struct SocketSelector::SocketEvents>::iterator it = sockets.begin(), 
        end = sockets.end(); it != end; ++it)
    {
        EventHandler* handler = mHandlers[it->fd];
        assert(handler != NULL);
        handler->SetActiveEvents(it->events);
        handlers.push_back(handler);
    }
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
        mHandlers.erase(fd);
        mSelector.Remove(fd);
    }
    else
    {
        //if(mHandlers.find(fd) == mHandlers.end())
        //{
        //    mHandlers[fd] = handler;
        //} 
        // Update the handler anyway
        // adapt to the cases of SOCKET ownership switch
        // the prior handler may not be disabled in time
        mHandlers[fd] = handler;
        mSelector.SetupEvents(fd, events);
    }
}

// Remove a EventHandler, this calling indicate that a handler
// may be dead and further information may not accessed
void EventSelector::RemoveHandler(EventHandler* handler)
{
    // locate the handler and associated socket
    std::map<SOCKET, EventHandler*>::iterator it = mHandlers.begin();
    while(it != mHandlers.end() && it->second != handler)
    {
        ++it;
    }
    if(it != mHandlers.end()) // exist
    {
        SOCKET fd = it->first;
        mHandlers.erase(it);
        mSelector.Remove(fd);
    }
}

NET_BASE_END

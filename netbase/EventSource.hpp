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

#ifndef NET_BASE_EVENT_SOURCE_HPP
#define NET_BASE_EVENT_SOURCE_HPP

#include "Config.hpp"
#include <vector>

NET_BASE_BEGIN

class EventHandler;
class EventSource
{
public:    
    virtual ~EventSource() { }
    
    // Wait for active events, 
    // Return as a list of EventHandlers that hadle those active events
    // timeout seconds, -1 for block 
    virtual void WaitEvents(std::vector<EventHandler*>& handlers, int timeout = -1) = 0;
    
    // Update the interested I/O events of a EventHandler.
    // This calling indicates that the handler is active and 
    // can be access for further information
    virtual void SetupHandler(EventHandler* handler) = 0;
    
    // Remove a EventHandler, this calling indicate that a handler
    // may be dead and further information may not accessed
    virtual void RemoveHandler(EventHandler* handler) = 0;

};

NET_BASE_END

#endif

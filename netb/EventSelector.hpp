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

#ifndef NETB_EVENT_SELECTOR_HPP
#define NETB_EVENT_SELECTOR_HPP

#include "SocketSelector.hpp"
#include <vector>
#include <map>

NETB_BEGIN

//
// EventSelector is a I/O event waiter based on socket selector 
// It is a internal object for event loop and all functions will 
// be called in the same loop. So EventSelector is not thread safe.
//
class EventHandler;
class EventSelector : private Uncopyable
{
public:
    // Wait for active events, 
    // Return as a list of EventHandlers that hadle those active events
    // timeout seconds, -1 for block 
    void WaitForEvents(std::vector<EventHandler*>& handlers, int timeout = -1); // throw on errors
    bool WaitForEvents(std::vector<EventHandler*>& handlers, int timeout, Error* e) noexcept;
    
    // Update the interested I/O events of an EventHandler.
    // This establish a connection between event selector and event handler.
    // Selector will get and set information from/to handler
    void SetupHandler(EventHandler* handler) noexcept;
    
    // Remove an EventHandler
    // This cut the connection between event selector and event handler.
    void RemoveHandler(EventHandler* handler) noexcept;
    
protected:
    // SocketSelector as events pump
    SocketSelector _selector;
    
    // List of EventHandlers
    std::map<SOCKET, EventHandler*> _handlers;
};

NETB_END

#endif

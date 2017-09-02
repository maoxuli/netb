/*
 * Copyright (C) 2010-2017, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NETB_EVENT_HANDLER_HPP
#define NETB_EVENT_HANDLER_HPP

#include "SocketConfig.hpp"
#include <functional>

NETB_BEGIN

//
// Event handler is a bridge between a socket and a event loop.
// It register interested socket and associated events to event loop, 
// and call back to handle read or write on the socket when event 
// loop detect active events.
//
class EventLoop;
class EventHandler
{
public:
    // An event handler is bound to a socket
    EventHandler(EventLoop* loop, SOCKET s);
    ~EventHandler();

    // Callback for events
    typedef std::function<void(SOCKET)> EventCallback;

    void SetReadCallback(const EventCallback& cb)
    {
        _read_callback = cb; 
    }
    
    void SetWriteCallback(const EventCallback& cb)
    { 
        _write_callback = cb; 
    }
    
    // Register interested events 
    void EnableReading();
    void DisableReading();
    
    void EnableWriting();
    void DisableWriting();
    
    // Isolate this handler from event loop
    // Block until done
    // This make sure cut the callback from event loop before deleting
    void Detach();

private:
    // EventLoop
    // Works as a event dispatcher
    EventLoop* _loop;
    
    // SOCKET
    SOCKET _socket;

    SOCKET GetSocket() const { return _socket; }
    
    // Events interested
    int _events;
    mutable std::mutex _events_mutex;

    void Update();
    void UpdateInLoop();
    int GetEvents() const;

    // Active events to handle
    int _active_events;
    void SetActiveEvents(int events) { _active_events |= events; };
    void HandleEvents();

    bool _detached;
    std::mutex _detach_mutex;
    std::condition_variable _detach_cond;
    
    void DetachInLoop();

    // Open access to EventHandler and EventLoop 
    friend class EventSelector;
    friend class EventLoop;

    // Callback of events
    EventCallback _read_callback;
    EventCallback _write_callback;
};

NETB_END

#endif

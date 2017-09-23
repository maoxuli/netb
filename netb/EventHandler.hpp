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
// Event handler is a bridge between a socket and event dispacher.
// It register interested socket and associated events to event loop, 
// and call back to handle read or write on the socket when event 
// loop detect ready events.
// 
// The event handler is designed to interact with event loop.
// Event loop suppose event handler alway working in the own thread, 
// but event handler may be accessed by other threads. so the 
// interface of event handler should be thread safe. 
//
class EventLoop;
class EventHandler
{
public:
    // An event handler is bound to a socket
    EventHandler(EventLoop* loop, SOCKET s);
    ~EventHandler();

    // Callback for ready events
    typedef std::function<void(SOCKET)> EventCallback;

    // Set callback for read ready event
    void SetReadCallback(const EventCallback& cb)
    {
        _read_callback = cb; 
    }
    
    // Set callback for wirte ready event
    void SetWriteCallback(const EventCallback& cb)
    { 
        _write_callback = cb; 
    }
    
    // Reading event
    bool EnableReading(); 
    void DisableReading();
    
     // Writing event
    bool EnableWriting();
    void DisableWriting();
    
    // Isolate this handler from event loop
    // Block until done
    // This make sure cut the callback from event loop
    // Only called once before deleting
    bool Detach(); 

private:
    // Event dispatcher
    EventLoop* _loop;
    
    // Bound SOCKET
    SOCKET _socket;

    // For event dispatcher
    SOCKET GetSocket() const { return _socket; }

    // Callback from event loop
    void AttachInLoop();
    
    // Events interested
    int _events;
    mutable std::mutex _events_mutex;

    // Update to event dispatcher
    bool Update();
    void UpdateInLoop();

    // call from event dispatcher
    int GetEvents() const;

    // Callback from event dispatcher 
    void HandleEvents(int events);

    // Detached from event dispatcher
    // before deleting
    bool _detached;
    std::mutex _detach_mutex;
    std::condition_variable _detach_cond;
    
    // Callback from event loop
    void DetachInLoop();

    // Open access to EventLoop 
    friend class EventLoop;

    // Callback of events
    EventCallback _read_callback;
    EventCallback _write_callback;
};

NETB_END

#endif

/*
 * Copyright (C) 2010-2015, Maoxu Li. http://maoxuli.com/dev
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
#include <mutex>
#include <condition_variable>

NETB_BEGIN

//
// Event handler is a bridge between a socket and event loop.
// It register interested socket and associated events to event loop, 
// and wait for calling back form the loop to handle read or write on 
// the socket when event is ready.
// 
// The event handler is designed to interact with event loop.
// Event loop suppose event handler alway working in the own thread, 
// but event handler is always bound to a socket object thus used by 
// the socket. Socket I/O is usually bound to a eventloop thus 
// performed in a single thread, but opening and closing, and status 
// controling of the socket is not bound to the event loop. The final 
// result is event handler may be called by external thread. So the 
// interface of event handler should be consider thread issues.  
//
// The genral working flow of an event handler is: 
// 1. Create a event handler object and bound to the socket that 
//    expects event-driven notifications. 
// 2. Event handler register itself to event loop on initiation. 
// 3. Enable reading or writing or both to wait for ready events. 
// 4. Calling back from event loop and perform I/O on socket. 
// 5. Detach from event loop when socket is ready to be closed. 
// 6. Detach operation will be blocked until done, now event handler 
//    is isolated from event loop and is safe to be deleted. 
// 
// If above operations are called in the same thread of event loop, 
// they are performed immediately in order. 
// 
// If they are called in external thread, they are dispatched to the 
// event loop and will be called back by event loop in order. 
// 
// If they are called mixed in thread internal or external, the 
// thread internal calling will be always performed immediately, but 
// the order with external calling is not determined. Need to be 
// analyzed further. 
// 
class EventLoop;
class EventHandler
{
public:
    // An event handler is always bound to 
    // an event loop and a socket
    EventHandler(EventLoop* loop, SOCKET s);
    ~EventHandler();

    // Callback form event loop for ready events
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

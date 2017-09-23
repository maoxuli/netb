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

#ifndef NETB_EVENT_LOOP_HPP
#define NETB_EVENT_LOOP_HPP

#include "Uncopyable.hpp"
#include "EventHandler.hpp"
#include "SocketSelector.hpp"
#include "SocketPipe.hpp"
#include <thread>
#include <mutex>
#include <functional>
#include <cassert>

NETB_BEGIN

//
// Event loop is the event dispacher of reactor design pattern. 
// It provides interface to register event handlers and interested 
// events, and dispatch the ready events captured by internal I/O
// events demultiplexer to associated event handlers. 
// 
// This is a simplified event loop only supoorts socket I/O ready 
// notification by registering event handlers and dispaching ready 
// events to the handlers. It also supports function running 
// notification by setting a general function object as callback.  
// Timer notification is not supported in this implementation. 
// 
class EventHandler;
class EventLoop : private Uncopyable
{
public:    
    EventLoop();
    ~EventLoop();
    
    // Run the loop
    // Only called in the loop thread
    void Run();
    
    // Stop running loop
    // Sould be thread safe
    void Stop();
    
    // Register a handler with interested events
    // Must called in loop thread
    bool RegisterHandler(EventHandler* handler, int events = 0);

    // Update a handler with interested events
    // Return false if the handler has not been registered
    // Must called in loop thread
    bool UpdateHandler(EventHandler* handler, int events = 0);

    // Remote a registered handler
    // Must called in loop thread
    bool RemoveHandler(EventHandler* handler);

    // Function that can be invoked by the loop
    typedef std::function<void()> Functor;

    // Set a function that will be invoked in the loop
    // It will be invoked immediately if called in the loop thread
    // Otherwise appended to the waiting list
    void Invoke(const Functor& f);

    // Set a function that will be invoked in the loop later
    // Append to the waiting list
    void InvokeLater(const Functor& f);
    
    // Check in owner thread
    // thread safe ?
    bool IsInLoopThread() const
    {
        return _thread_id == std::this_thread::get_id();
    }
    
    // Check and assert in owner thread
    void AssertInLoopThread() const
    {
        assert(IsInLoopThread());
    }

private:
    // Owner thread id
    const std::thread::id _thread_id;
    
    // Stop signal
    // Todo: using atomic type for thread safe
    bool _stop;
    
    // Events demultiplexing and dispaching
    // Todo: using a cross-list to manage the handlers
    SocketSelector _selector;
    std::vector<EventHandler*> _handlers;
    EventHandler* _current_handler;
    bool _event_handling; // only used in loop

    // FIFO queue of functions waiting for running
    // Always run all queued founctions per loop
    // so vector is just fine
    std::vector<const Functor> _queue;
    std::mutex _queue_mutex;
    bool _queue_invoking; // only used in loop

private:
    // Wake up from sleeping
    void Wakeup();
    void OnWakeupRead();

    // Socket pipe for wakeup
    SocketPipe _wakeup_pipe;
    EventHandler _wakeup_handler;
};

NETB_END

#endif

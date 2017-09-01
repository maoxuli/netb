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

#ifndef NET_BASE_EVENT_LOOP_HPP
#define NET_BASE_EVENT_LOOP_HPP

#include "Uncopyable.hpp"
#include "EventSelector.hpp"
#include "EventHandler.hpp"
#include "SocketPipe.hpp"
#include <thread>
#include <mutex>
#include <functional>
#include <cassert>

NET_BASE_BEGIN

//
// Event driven thread loop, one loop per thread
// The purpose of an event loop is to keep your thread busy when there is work to do
// and put your thread to sleep when there is none.
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
    
    // Setup and remove a handler that handle some events
    // First will update the interested socket list and associcated events
    // Later will set active events and hanle events with this handler, 
    // so it is a pointer or reference, not const
    // Must be called in loop thread
    void SetupHandler(EventHandler* handler);
    void RemoveHandler(EventHandler* handler);

    // Function that can be invoked by the loop
    typedef std::function<void()> Functor;

    // Set a function that will be invoked in the loop
    // It will be invoked immediately if called in the loop thread
    // Otherwise appended to the waiting list
    void Invoke(const Functor& f);

    // Set a function that will be invoked in the loop later
    // Append to the waiting list
    void InvokeLater(const Functor& f);

    // Set a function that will be invoked after a given delay (seconds)
    void InvokeAfter(const Functor& f, int delay);

    // Set a function that will be invoked every given interval (seconds)
    void InvokeEvery(const Functor& f, int interval);
    
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
    
    // EventSelector is a internal object to manage interested sockets 
    // and associated events, as well generate the active list.
    // Event setting and handling are always done in the loop 
    EventSelector _selector;
    std::vector<EventHandler*> _active_handlers;
    EventHandler* _current_handler;
    bool _event_handling; // only used in loop

    // FIFO queue of functions waiting for running
    // Always run all queued founctions per loop
    // so vector is just fine
    std::vector<const Functor> _queue;
    std::mutex _queue_mutex;
    bool _queue_invoking; // only used in loop

    // Todo: timer and timingly running functions

private:
    // Wake up from sleeping
    void Wakeup();
    void OnWakeupRead();

    // Socket pipe for wakeup
    SocketPipe _wakeup_pipe;
    EventHandler _wakeup_handler;
};

NET_BASE_END

#endif

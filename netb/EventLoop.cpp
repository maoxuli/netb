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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EventLoop.hpp"

NETB_BEGIN

EventLoop::EventLoop()
: _thread_id(std::this_thread::get_id())
, _stop(false)
, _current_handler(nullptr)
, _event_handling(false)
, _queue_invoking(false)
, _wakeup_handler(this, _wakeup_pipe.ReadSocket())
{
    // Handle reading event of wake up
    _wakeup_handler.SetReadCallback(std::bind(&EventLoop::OnWakeupRead, this));
}

EventLoop::~EventLoop()
{

}

// Run the loop
// Usually it is always called in the owener thread, either current 
// thread or newly created thread
void EventLoop::Run()
{
    AssertInLoopThread();
    _stop = false;
    _wakeup_handler.EnableReading();
    
    while(!_stop)
    {
        // Block to wait for active events
        std::vector<struct SocketSelector::SocketEvents> sockets;
        if(_selector.Select(sockets, -1, nullptr) > 0) // ignore errors
        {
            _event_handling = true;
            for(auto it = sockets.begin(), end = sockets.end(); it != end; ++it)
            {
                _current_handler = _handlers[it->fd];
                assert(_current_handler);
                _current_handler->HandleEvents(it->events);
            }
            _current_handler = nullptr;
            _event_handling = false;
        }
        // Invoking Queued functions
        std::vector<const Functor> functions;
        _queue_invoking = true;
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            functions.swap(_queue);
        }
        for(size_t i = 0; i < functions.size(); ++i)
        {
            functions[i]();
        }
        _queue_invoking = false;
    }
}

// Stop running loop
// Todo: using atomic type for thread safe
void EventLoop::Stop()
{
    _stop = true;
    if(!IsInLoopThread())
    {
        Wakeup();
    }
}

// Register a handler that interested in some events
// update the handlers list
bool EventLoop::RegisterHandler(EventHandler* handler, int events)
{
    AssertInLoopThread();
    auto it = _handlers.begin();
    for(; it != _handlers.end(); ++it)
    {
        if(*it == handler) break;
    }
    if(it == _handlers.end()) // not found
    {
        _handlers.push_back(handler);
    }
    return _selector.Set(handler->GetSocket(), events);
}

// Update a handler that interested in some events
// update socket's events
bool EventLoop::UpdateHandler(EventHandler* handler, int events)
{
    AssertInLoopThread();
    auto it = _handlers.begin();
    for(; it != _handlers.end(); ++it)
    {
        if(*it == handler) break;
    }
    if(it == _handlers.end()) // not found
    {
        return false;
    }
    return _selector.Set(handler->GetSocket(), events);
}

// Remove a handler
bool EventLoop::RemoveHandler(EventHandler* handler)
{
    assert(!_event_handling);
    auto it = _handlers.begin();
    while(it != _handlers.end())
    {
        if(*it == handler) // found
        {
            it = _handlers.erase(it);
            _selector.Remove(handler->GetSocket());
        }
        else
        {
            ++it;
        }
    }
    return true;
}

// Set a function that will be invoked in the loop
// Invoked immediately if called in the loop thread
// Otherwise append to the queue
void EventLoop::Invoke(const Functor& f)
{
    if(IsInLoopThread())
    {
        f();
    }
    else
    {
        InvokeLater(f);
    }
}

// Set a function that will be invoked later
// append to the queue
void EventLoop::InvokeLater(const Functor& f)
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _queue.push_back(f);
    }
    if(!IsInLoopThread() || _queue_invoking)
    {
        Wakeup();
    }
}

void EventLoop::Wakeup()
{
    unsigned char c = 0;
    if(_wakeup_pipe.Write(&c, 1) < 0)
    {
        // If wakeup pipe dead, the loop should stop working
        assert(false);
        throw Exception("Pipe in event loop stop working.");
    }
}

void EventLoop::OnWakeupRead()
{
    unsigned char c = 0;
    if(_wakeup_pipe.Read(&c, 1) < 0)
    {
        // If wakeup pipe dead, the loop should stop working
        assert(false);
        throw Exception("Pipe in event loop stop working.");
    }
}

NETB_END

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
, _current_handler(NULL)
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
        // Seleting on events
        _active_handlers.clear();
        _selector.WaitForEvents(_active_handlers, -1); // Block to wait for active events
        // Events handling
        _event_handling = true;
        for(std::vector<EventHandler*>::const_iterator it = _active_handlers.begin();
             it != _active_handlers.end(); ++it)
        {
            _current_handler = *it;
            _current_handler->HandleEvents();
        }
        _current_handler = NULL;
        _event_handling = false;
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

// Setup and remove a handler that handle some events
// Actually update the socket list and associcated events
void EventLoop::SetupHandler(EventHandler* handler)
{
    AssertInLoopThread();
    _selector.SetupHandler(handler);
}

// Setup and remove a handler that handle some events
// Actually update the socket list and associcated events
void EventLoop::RemoveHandler(EventHandler* handler)
{
    AssertInLoopThread();
    if(_event_handling)
    {
        assert(_current_handler == handler // self
               || std::find(_active_handlers.begin(), _active_handlers.end(),
                handler) == _active_handlers.end()); // Not active handler
    }
    _selector.RemoveHandler(handler);
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

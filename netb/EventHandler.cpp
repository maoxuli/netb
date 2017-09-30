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

#include "EventHandler.hpp"
#include "EventLoop.hpp"

NETB_BEGIN

/*
Implementation Note: basically all functions may be called from inside of the 
thread or outside. Outside thread calling will be dipatched to the event loop 
and running later. The calling will be done in order, but can not supposed 
the prior calling has been done.   
*/

// always bound to an event loop and a socket
// if in loop thread, complete register immediately 
// otherwise, the register will be done by the loop async. 
EventHandler::EventHandler(EventLoop* loop, SOCKET s)
: _loop(loop)
, _socket(s)
, _events(0)
, _detached(true)
{
    assert(_loop);
    _loop->Invoke(std::bind(&EventHandler::AttachInLoop, this));
}

// Always detach the handler from loop before deleting
// Todo: timeout control for external thread deleting
EventHandler::~EventHandler()
{
    // block until done
    Detach(); // ignore errors
}

// Isolate from event loop
// if in the loop thread, done immediately
// otherwise dispatched to the loop and block until done
bool EventHandler::Detach()
{
    assert(_loop);
    if(_detached) return true;
    if(!_loop->IsInLoopThread())
    {
        _loop->Invoke(std::bind(&EventHandler::DetachInLoop, this));
        std::unique_lock<std::mutex> lock(_detach_mutex);
        while(!_detached)
        {
            _detach_cond.wait(lock);
        }  
        return true; 
    }
    _detached = _loop->RemoveHandler(this);
    return _detached;
}

// Isolate from event loop
// callback from event loop
void EventHandler::DetachInLoop()
{
    assert(_loop);
    _loop->AssertInLoopThread();
    _loop->RemoveHandler(this);
    std::unique_lock<std::mutex> lock(_detach_mutex);
    _detached = true;
    _detach_cond.notify_one();
}

// Attach to event loop
void EventHandler::AttachInLoop()
{
    assert(_loop);
    _loop->AssertInLoopThread();
    _detached = !(_loop->RegisterHandler(this));
}

// Set intresting event, reading
bool EventHandler::EnableReading()
{
    {
        std::unique_lock<std::mutex> lock(_events_mutex);
        _events |= SOCKET_EVENT_READ;
    }
    return Update();
}

void EventHandler::DisableReading()
{
    {
        std::unique_lock<std::mutex> lock(_events_mutex);
        _events &= ~SOCKET_EVENT_READ;
    }
    Update();
}

// Set interesting event, writing
bool EventHandler::EnableWriting()
{
    {
        std::unique_lock<std::mutex> lock(_events_mutex);
        _events |= SOCKET_EVENT_WRITE;
    }
    return Update();
}

void EventHandler::DisableWriting()
{
    {
        std::unique_lock<std::mutex> lock(_events_mutex);
        _events &= ~SOCKET_EVENT_WRITE;
    }
    Update();
}

// Notify event loop to update
// if inside the loop thread, done immediately, 
// otherwise done by loop later, suppose register is done or is still pending.
bool EventHandler::Update()
{
    assert(_loop);
    if(!_loop->IsInLoopThread())
    {
        _loop->Invoke(std::bind(&EventHandler::UpdateInLoop, this));
        return true;
    }
    assert(!_detached);
    return _loop->UpdateHandler(this);
}

// Notify event loop to update interested events
void EventHandler::UpdateInLoop()
{
    assert(_loop);
    assert(_loop->IsInLoopThread());
    assert(!_detached);
    _loop->UpdateHandler(this);
}

// call from event dispatcher
int EventHandler::GetEvents() const 
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    return _events;
}

// Handle current active events
// Call back by event loop
void EventHandler::HandleEvents(int events)
{
    if(events & SOCKET_EVENT_READ)
    {
        if(_read_callback) _read_callback(_socket);
    }
    
    if(events & SOCKET_EVENT_WRITE)
    {
        if(_write_callback) _write_callback(_socket);
    }
}

NETB_END

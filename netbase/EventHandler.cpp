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

NET_BASE_BEGIN

// bound to a socket
EventHandler::EventHandler(EventLoop* loop, SOCKET s)
: _loop(loop)
, _socket(s)
, _events(SOCKET_EVENT_NONE)
, _active_events(SOCKET_EVENT_NONE) 
, _detached(true)
{
    assert(_loop != NULL);
}

EventHandler::~EventHandler()
{
    Detach();
}

// Isolate from event loop
// Block until done
void EventHandler::Detach()
{
    if(_detached) return;
    assert(_loop != NULL);
    if(_loop->IsInLoopThread())
    {
        DetachInLoop();
    }
    else
    {
        _loop->Invoke(std::bind(&EventHandler::DetachInLoop, this));
        {
            std::unique_lock<std::mutex> lock(_detach_mutex);
            while(!_detached)
            {
                _detach_cond.wait(lock);
            }  
        }      
    }
}

// Isolate from event loop
void EventHandler::DetachInLoop()
{
    assert(_loop != NULL);
    _loop->AssertInLoopThread();
    _loop->RemoveHandler(this);
    std::unique_lock<std::mutex> lock(_detach_mutex);
    _detached = true;
    _detach_cond.notify_one();
}

// Set intresting events
void EventHandler::EnableReading()
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    _events |= SOCKET_EVENT_READ;
    Update();
}

void EventHandler::DisableReading()
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    _events &= ~SOCKET_EVENT_READ;
    Update();
}

void EventHandler::EnableWriting()
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    _events |= SOCKET_EVENT_WRITE;
    Update();
}

void EventHandler::DisableWriting()
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    _events &= ~SOCKET_EVENT_WRITE;
    Update();
}

// Notify event loop to update
void EventHandler::Update()
{
    assert(_loop != NULL);
    if(_loop->IsInLoopThread())
    {
        UpdateInLoop();
    }
    else
    {
        _loop->Invoke(std::bind(&EventHandler::UpdateInLoop, this));
    }
}

// Notify event loop to update interested events
void EventHandler::UpdateInLoop()
{
    assert(_loop != NULL);
    assert(_loop->IsInLoopThread());
    _loop->SetupHandler(this);
    _detached = false;
}

// EventLoop/EventHandler get interested events
int EventHandler::GetEvents() const
{
    std::unique_lock<std::mutex> lock(_events_mutex);
    return _events;
}

// Handle current active events
// Call back by event loop
void EventHandler::HandleEvents()
{
    if(_active_events & SOCKET_EVENT_READ)
    {
        if(_read_callback) _read_callback(_socket);
    }
    
    if(_active_events & SOCKET_EVENT_WRITE)
    {
        if(_write_callback) _write_callback(_socket);
    }
}

NET_BASE_END

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

#ifndef NET_BASE_EVENT_HANDLER_H
#define NET_BASE_EVENT_HANDLER_H

#include "Config.h"
#include "SocketDef.h"
#include <functional>

NET_BASE_BEGIN

//
// EventHandler is interface object with EventLoop
// So thread safe is a concern 
//
class EventLoop;
class EventHandler
{
public:
    EventHandler(EventLoop* loop, SOCKET s);
    ~EventHandler();

    // Callbakcs for events
    typedef std::function<void(SOCKET)> EventCallback;

    // Defined by owner of SOCKET
    void SetReadCallback(const EventCallback& cb)
    {
        mReadCallback = cb; 
    }
    
    void SetWriteCallback(const EventCallback& cb)
    { 
        mWriteCallback = cb; 
    }
    
    // Set interested events
    // Call by owner of SOCKET
    void EnableReading();
    void DisableReading();
    
    void EnableWriting();
    void DisableWriting();
    
    // Isolate this handler from event loop
    void Detach();

private:
    // Update events
    void Update();
    void UpdateInLoop();
    void DetachInLoop();

private:
    // SOCKET of this EventHandler
    SOCKET GetSocket() const { return mSocket; }

    // Interested events to handle
    unsigned int GetEvents() const { return mEvents; }
    
    // Set currently active events
    // Call by event selector
    void SetActiveEvents(unsigned int events) { mActiveEvents |= events; };

    // Handle current events
    // Call by run loop
    void HandleEvents();

    friend class EventSelector;
    friend class EventLoop;

private:
    // EventLoop
    // Works as a event dispatcher
    EventLoop* mLoop;
    
    // SOCKET
    SOCKET mSocket;
    
    // Interesting events, and current active events
    unsigned int mEvents;
    unsigned int mActiveEvents;
    
    // Callback of events
    EventCallback mReadCallback;
    EventCallback mWriteCallback;
};

NET_BASE_END

#endif

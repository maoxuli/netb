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

EventHandler::EventHandler(EventLoop* loop, SOCKET s)
: mLoop(loop)
, mSocket(s)
, mEvents(SOCKET_EVENT_NONE)
, mActiveEvents(SOCKET_EVENT_NONE)
{
    
}

EventHandler::~EventHandler()
{

}

// Set intresting events
void EventHandler::EnableReading()
{
    mEvents |= SOCKET_EVENT_READ;
    Update();
}

void EventHandler::DisableReading()
{
    mEvents &= ~SOCKET_EVENT_READ;
    Update();
}

void EventHandler::EnableWriting()
{
    mEvents |= SOCKET_EVENT_WRITE;
    Update();
}

void EventHandler::DisableWriting()
{
    mEvents &= ~SOCKET_EVENT_WRITE;
    Update();
}

void EventHandler::Detach()
{
    mLoop->Invoke(std::bind(&EventHandler::DetachInLoop, this));
}

void EventHandler::Update()
{
    mLoop->Invoke(std::bind(&EventHandler::UpdateInLoop, this));
}

void EventHandler::UpdateInLoop()
{
    mLoop->SetupHandler(this);
}

void EventHandler::DetachInLoop()
{
    mLoop->RemoveHandler(this);
}

// Handle current active events
// Call by EventLoop
void EventHandler::HandleEvents()
{
    if(mActiveEvents & SOCKET_EVENT_READ)
    {
        if(mReadCallback) mReadCallback(mSocket);
    }
    
    if(mActiveEvents & SOCKET_EVENT_WRITE)
    {
        if(mWriteCallback) mWriteCallback(mSocket);
    }
}

NET_BASE_END

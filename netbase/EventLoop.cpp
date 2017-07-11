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
#include "EventSelector.hpp"
#include <functional>

NET_BASE_BEGIN

EventLoop::EventLoop(EventSource* source)
: mThreadID(std::this_thread::get_id())
, mStop(false)
, mSource(source ? source : new EventSelector())
, mCurrentHandler(NULL)
, mEventHandling(false)
, mFunctionInvoking(false)
, mWakeupPipe()
, mWakeupReadHandler(this, mWakeupPipe.ReadSocket())
{
    // Handle reading event of wake up
    mWakeupReadHandler.SetReadCallback(std::bind(&EventLoop::OnWakeupRead, this));
}

EventLoop::~EventLoop()
{

}

// Run the loop
void EventLoop::Run()
{
    AssertInLoopThread();
    mStop = false;
    mWakeupReadHandler.EnableReading();
    
    while(!mStop)
    {
        // Seleting on events
        mActiveHandlers.clear();
        mSource->WaitEvents(mActiveHandlers, -1); // Block to wait for active events
        // Events handling
        mEventHandling = true;
        for(std::vector<EventHandler*>::iterator it = mActiveHandlers.begin();
             it != mActiveHandlers.end(); ++it)
        {
            mCurrentHandler = *it;
            mCurrentHandler->HandleEvents();
        }
        mCurrentHandler = NULL;
        mEventHandling = false;
        // Invoking Queued functions
        std::vector<Functor> functions;
        mFunctionInvoking = true;
        {
            std::unique_lock<std::mutex> lock(mFunctionMutex);
            functions.swap(mFunctionQueue);
        }
        for(size_t i = 0; i < functions.size(); ++i)
        {
            functions[i]();
        }
        mFunctionInvoking = false;
    }
}

// Stop running loop
void EventLoop::Stop()
{
    mStop = true;
    if(!IsInLoopThread())
    {
        Wakeup();
    }
}

// Setup a event handler to selector, update all tied events
void EventLoop::SetupHandler(EventHandler* handler)
{
    AssertInLoopThread();
    mSource->SetupHandler(handler);
}

// Remove a event handler from selector, remove all events
void EventLoop::RemoveHandler(EventHandler* handler)
{
    AssertInLoopThread();
    if(mEventHandling)
    {
        assert(mCurrentHandler == handler // self
               || std::find(mActiveHandlers.begin(), mActiveHandlers.end(),
                handler) == mActiveHandlers.end()); // Not active handler
    }
    mSource->RemoveHandler(handler);
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
        std::unique_lock<std::mutex> lock(mFunctionMutex);
        mFunctionQueue.push_back(f);
    }

    if(!IsInLoopThread() || mFunctionInvoking)
    {
        Wakeup();
    }
}

void EventLoop::Wakeup()
{
    unsigned char c = 0;
    if(mWakeupPipe.Write(&c, 1) < 0)
    {
        // If wakeup pipe dead, the loop should stop working
        assert(false);
    }
}

void EventLoop::OnWakeupRead()
{
    unsigned char c = 0;
    if(mWakeupPipe.Read(&c, 1) < 0)
    {
        // If wakeup pipe dead, the loop should stop working
        assert(false);
    }
}

NET_BASE_END

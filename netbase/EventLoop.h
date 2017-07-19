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

#ifndef NET_BASE_EVENT_LOOP_H
#define NET_BASE_EVENT_LOOP_H

#include "Config.h"
#include "EventSelector.h"
#include "EventHandler.h"
#include "SocketPipe.h"
#include <thread>
#include <cassert>

NET_BASE_BEGIN

//
// Event driven thread loop, one loop per thread
// The purpose of an event loop is to keep your thread busy when there is work to do
// and put your thread to sleep when there is none.
//
class EventLoop
{
public:    
    EventLoop();
    ~EventLoop();
    
    // Run the loop
    // Only called in owner thread
    void Run();
    
    // Stop running of the loop
    void Stop();
    
    // Setup and remove a handler that handle some events
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
    bool IsInLoopThread() const
    {
        return mThreadID == std::this_thread::get_id();
    }
    
    // Check and assert in owner thread
    void AssertInLoopThread()
    {
        assert(IsInLoopThread());
    }
    
private:
    // Owner thread id
    const std::thread::id mThreadID;
    
    // Stop signal
    bool mStop;
    
    // Generate and handle active events
    // EventSource coupled with EventHandler to generate active events
    EventSelector mSelector;
    std::vector<EventHandler*> mActiveHandlers;
    EventHandler* mCurrentHandler;
    bool mEventHandling;

    // Queue of functions
    // Finish all queued founctions per loop
    // So vector is just fine
    std::vector<Functor> mFunctionQueue;
    std::mutex mFunctionMutex;
    bool mFunctionInvoking;

    // Timer functions

private:
    // Wake up from sleeping
    void Wakeup();
    void OnWakeupRead();

    // Socket pipe for wakeup
    SocketPipe mWakeupPipe;
    EventHandler mWakeupReadHandler;
};

NET_BASE_END

#endif

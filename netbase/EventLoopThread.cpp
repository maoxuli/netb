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

#include "EventLoopThread.h"
#include "EventLoop.h"

NET_BASE_BEGIN

EventLoopThread::EventLoopThread()
: mLoop(NULL)
{
    
}

EventLoopThread::~EventLoopThread()
{
    if(mLoop != NULL)
    {
        mLoop->Stop();
        mThread.join();
    }
}

EventLoop* EventLoopThread::Start()
{
    mThread = std::thread(&EventLoopThread::ThreadFunc, this);
    {
        std::unique_lock<std::mutex> lock(mMutex);
        while(mLoop == NULL)
        {
            mCondition.wait(lock);
        }
    }
    return mLoop;
}

void EventLoopThread::ThreadFunc()
{
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mLoop = &loop;
        mCondition.notify_one();
    }
    loop.Run();
}

NET_BASE_END

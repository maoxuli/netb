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

#include "EventLoopThread.hpp"

NETB_BEGIN

EventLoopThread::EventLoopThread()
: _loop(NULL)
{
    
}

// Todo: thread safe
EventLoopThread::~EventLoopThread()
{
    if(_loop != NULL)
    {
        _loop->Stop();
        _thread.join();
    }
}

EventLoop* EventLoopThread::Start()
{
    _thread = std::thread(&EventLoopThread::ThreadFunc, this);
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while(_loop == NULL)
        {
            _condition.wait(lock);
        }
    }
    return _loop;
}

void EventLoopThread::ThreadFunc()
{
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = &loop;
        _condition.notify_one();
    }
    loop.Run();
}

NETB_END

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

#ifndef NET_BASE_EVENT_LOOP_THREAD_HPP
#define NET_BASE_EVENT_LOOP_THREAD_HPP

#include "EventLoop.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

NET_BASE_BEGIN

class EventLoopThread
{
public:    
    EventLoopThread();
    ~EventLoopThread();
    
    // Start the thread and run the internal EventLoop
    // The loop has been running when returned
    EventLoop* Start();
    
private:
    // EventLoop on thread
    EventLoop* _loop;
    
    // Thread control
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _condition;

    // Thread function
    void ThreadFunc();
};

NET_BASE_END

#endif

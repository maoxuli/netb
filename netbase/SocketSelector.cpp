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

#include "SocketSelector.hpp"
#include "SocketError.hpp"

NET_BASE_BEGIN

SocketSelector::SocketSelector()
{
    mMaxSocket = 0;
    FD_ZERO(&mReadFdSet);
    FD_ZERO(&mWriteFdSet);
    FD_ZERO(&mExceptFdSet);
}

SocketSelector::~SocketSelector()
{
    
}

// @param timout 0 for immediate return, -1 for wait unlimited
bool SocketSelector::Select(std::vector<SocketEvents>& sockets, int timeout)
{
    // Select events
    ssize_t ret = 0;
    while(true)
    {
        FD_COPY(&mReadFdSet, &mActiveReadFdSet);
        FD_COPY(&mWriteFdSet, &mActiveWriteFdSet);
        FD_COPY(&mExceptFdSet, &mActiveExceptFdSet);

        if(timeout < 0)
        {
            ret = ::select(mMaxSocket + 1, &mActiveReadFdSet, &mActiveWriteFdSet, &mActiveExceptFdSet, NULL); 
        }
        else
        {
            struct timeval tv;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            ret = ::select(mMaxSocket + 1, &mActiveReadFdSet, &mActiveWriteFdSet, &mActiveExceptFdSet, &tv); 
        }

        if(ret == SOCKET_ERROR)
        {
            if(SocketError::Interrupted())
            {
                continue;
            }
            // Print error information, and write logger
            // Sleep 5s to avoid looping
        }
        break;
    }

    if(ret < 0)  // error
    {
        std::cout << "SocketSelector::Select return error: " << SocketError::Code() << "\n";
        return false;
    }

    if(ret == 0) // timeout
    {
        std::cout << "SocketSelector::Select return timeout.\n";
        return false;
    }

    // Check active sockets if not timeout
    sockets.clear();
    for(std::vector<SOCKET>::iterator it = mSockets.begin(), 
        end = mSockets.end(); it != end; ++it)
    {
        SOCKET fd = *it;
        unsigned int events = SOCKET_EVENT_NONE;
        if(FD_ISSET(fd, &mActiveReadFdSet))
        {
            events |= SOCKET_EVENT_READ;
        }
        if(FD_ISSET(fd, &mActiveWriteFdSet))
        {
            events |= SOCKET_EVENT_WRITE;
        }
        if(FD_ISSET(fd, &mActiveExceptFdSet))
        {
            events |= SOCKET_EVENT_EXCEPT;
        }

        if(events != SOCKET_EVENT_NONE)
        {
            sockets.push_back(SocketEvents(fd, events));
        }
    }
    return true;
}

// Add or remove events with EventHandler 
void SocketSelector::SetupEvents(SOCKET s, unsigned int events)
{    
    if(events == SOCKET_EVENT_NONE)
    {
        Remove(s);
    }
    else
    {
        std::vector<SOCKET>::iterator it = mSockets.begin(); 
        while(it != mSockets.end() && *it != s)
        {
            ++it;
        }
        if(it == mSockets.end())
        {
            mSockets.push_back(s);
        }

        // ??? Only track the max socket, but not adjust when a 
        // socket is removed. So...
        if(s > mMaxSocket) 
        {
            mMaxSocket = s;
        }

        if(events & SOCKET_EVENT_READ)
        {
            FD_SET(s, &mReadFdSet);
        }
        else
        {
            FD_CLR(s, &mReadFdSet);
        }

        if(events & SOCKET_EVENT_WRITE)
        {
            FD_SET(s, &mWriteFdSet);
        }
        else
        {
            FD_CLR(s, &mWriteFdSet);
        }

        if(events & SOCKET_EVENT_EXCEPT)
        {
            FD_SET(s, &mExceptFdSet);
        }
        else
        {
            FD_CLR(s, &mExceptFdSet);
        }
    }
}

// Remove a socket and all interested event
void SocketSelector::Remove(SOCKET s)
{
    for(std::vector<SOCKET>::iterator it = mSockets.begin(), 
        end = mSockets.end(); it != end; ++it)
    {
        if(*it == s)
        {
            mSockets.erase(it);
            break;
        }
    }
    FD_CLR(s, &mReadFdSet);
    FD_CLR(s, &mWriteFdSet);
    FD_CLR(s, &mExceptFdSet);
}

NET_BASE_END

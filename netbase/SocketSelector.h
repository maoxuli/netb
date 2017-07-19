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

#ifndef NET_BASE_SOCKET_SELECTOR_H
#define NET_BASE_SOCKET_SELECTOR_H

#include "Config.h"
#include "Uncopyable.h"
#include "SocketDef.h"
#include <sys/select.h>
#include <vector>

NET_BASE_BEGIN

class SocketSelector : private Uncopyable
{
public:    
    SocketSelector();
    ~SocketSelector();

    struct SocketEvents
    {
        SocketEvents(SOCKET s, unsigned int e) 
        : fd(s), events(e) { }

        SOCKET fd;
        unsigned int events;
    };

    // Select socket with active events
    // timeout seconds, -1 for block 
    bool Select(std::vector<SocketEvents>& sockets, int timeout = -1);
    
    // Add the interested I/O events of a SOCKET.
    // Only modify the change event
    void SetupEvents(SOCKET s, unsigned int events);
    
    // Remove the events of a SOCKET
    void Remove(SOCKET s);
    
protected:
    // List of sockets, that has interested events
    std::vector<SOCKET> mSockets;

    // Track the max socket
    // But will not adjust when a socket is removed
    // So ...
    int mMaxSocket;

    // List of sockets, by interested events    
    fd_set mReadFdSet;
    fd_set mWriteFdSet;
    fd_set mExceptFdSet;

    // Active sockets, by active events after select
    fd_set mActiveReadFdSet;
    fd_set mActiveWriteFdSet;
    fd_set mActiveExceptFdSet;
};

NET_BASE_END

#endif

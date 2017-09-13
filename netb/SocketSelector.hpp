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

#ifndef NETB_SOCKET_SELECTOR_HPP
#define NETB_SOCKET_SELECTOR_HPP

#include "SocketConfig.hpp"
#include <sys/select.h>
#include <vector>

NETB_BEGIN

//
// SocketSelector is a wrapper class of socket API select(). 
//
// On Windows, select() only supports 64 FDs. This may be fixed by:
// #define FD_SETSIZE 512 or some larger number
//
class SocketSelector : private Uncopyable
{
public: 
    // Socket and associated events
    struct SocketEvents
    {
        SocketEvents(SOCKET s, unsigned int ev) 
        : fd(s), events(ev) { }
        const SOCKET fd;
        int events;
    };

    // Constructor and Destructor
    SocketSelector() noexcept;
    SocketSelector(SOCKET s, int events) noexcept;
    SocketSelector(const std::vector<SocketEvents>& sockets) noexcept;
    SocketSelector(const fd_set* read, const fd_set* write, const fd_set* except) noexcept;
    virtual ~SocketSelector() noexcept;

    // Setup interested socket and events
    // May result in adding or removing socket and event
    void SetupEvents(SOCKET s, int events) noexcept;
    
    // Remove a socket and its associcated events
    void Remove(SOCKET s) noexcept;

    // Select sockets with active events
    // timeout in miliseconds, -1 for block
    // return -1 on errors
    // return number of active sockets, 0 indicates timeout
    void Select(std::vector<SocketEvents>& sockets, int timeout = -1); // throw on errors
    int Select(std::vector<SocketEvents>& sockets, int timeout, Error* e) noexcept;
    
private:
    // List of sockets that have interested events
    // This is also a max-heap, the front of which is always the max socket
    std::vector<SOCKET> _sockets;

    // List of sockets, by interested events    
    fd_set _read_set;
    fd_set _write_set;
    fd_set _except_set;

    // Active sockets, by active events
    fd_set _active_read_set;
    fd_set _active_write_set;
    fd_set _active_except_set;
};

NETB_END

#endif
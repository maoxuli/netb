/*
 * Copyright (C) 2010 Maoxu Li. All rights reserved. maoxu@lebula.com
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

#ifndef NET_BASE_RAW_SOCKET_HPP
#define NET_BASE_RAW_SOCKET_HPP

#include "Config.hpp"
#include "Socket.hpp"

NET_BASE_BEGIN

//
// RawSocket is a wrpper of RAW socket, working based on internal Socket.
// It only provides another easy and neat interface to use RAW socket.
//
class RawSocket
{
public: 
    RawSocket(int domain, int type, int protocol);
    RawSocket(SOCKET s);
    virtual ~RawSocket();

    // Internal socket
    SOCKET GetSocket() const { return mSocket; }

protected:
    Socket mSocket;
};

NET_BASE_END

#endif

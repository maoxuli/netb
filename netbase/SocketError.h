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

#ifndef NET_BASE_SOCKET_ERROR_H
#define NET_BASE_SOCKET_ERROR_H

#include "Config.h"

NET_BASE_BEGIN

//
// SocketError
//
class SocketError
{
public: 
    // Error code
    static int Code();

public: 
    // Specific error status
    static bool Interrupted();
    static bool AcceptInterrupted();
    static bool InProgress();
    static bool WouldBlock();
    static bool Timeout();

    static bool NoMoreFds();
    static bool NoBuffers();

    static bool NotConnected();
    static bool ConnectFailed();
    static bool ConnectionRefused();
    static bool ConnectionProgress();
    static bool ConnectionLost();
};

NET_BASE_END

#endif

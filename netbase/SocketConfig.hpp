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

#ifndef NET_BASE_SOCKET_CONFIG_HPP
#define NET_BASE_SOCKET_CONFIG_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"
#include "ErrorCode.hpp"
#include "Error.hpp"
#include "Exception.hpp"

// Headers for socket
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

NET_BASE_BEGIN

// Socket type and const values
typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

// Socket events
enum {
    SOCKET_EVENT_NONE = 0,
    SOCKET_EVENT_READ = 1,
    SOCKET_EVENT_WRITE = 2,
    SOCKET_EVENT_EXCEPT = 4
};

// Socket initializer for Winsock
class SocketInitializer
{
public:
	SocketInitializer() 
    {
 	#if defined(WIN32)
		WSADATA wsadata;
		::WSAStartup(MAKEWORD(2, 0), &wsadata);
	#endif
    }

	~SocketInitializer() 
    { 
	#if defined(WIN32)
		::WSACleanup();
	#endif
    }
};

NET_BASE_END

#endif

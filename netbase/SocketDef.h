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

#ifndef NET_BASE_SOCKET_DEF_H
#define NET_BASE_SOCKET_DEF_H

#include "Config.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)

// SOCKET events 
#define SOCKET_EVENT_NONE       0
#define SOCKET_EVENT_READ       1
#define SOCKET_EVENT_WRITE      2
#define SOCKET_EVENT_EXCEPT     4

NET_BASE_BEGIN

typedef int SOCKET;

NET_BASE_END

#endif

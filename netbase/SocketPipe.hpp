/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NET_BASE_SOCKET_PIPE_HPP
#define NET_BASE_SOCKET_PIPE_HPP

#include "Config.hpp"
#include "Socket.hpp"

NET_BASE_BEGIN

//
// A pipe simulated with TCP connection
// 
class SocketPipe 
{
public: 
    SocketPipe();
    ~SocketPipe();

    // Access internal socket pair
    // Not yield ownership, so external can not close the socket
    SOCKET ReadSocket() const { return mReader; }
    SOCKET WriteSocket() const { return mWriter; }

    // Blocked reading and writing
    ssize_t Read(void* p, size_t n);
    ssize_t Write(const void* p, size_t n);
   
private:
    Socket mReader;
    Socket mWriter;

    bool MakePair(Socket& reader, Socket& writer);
};

NET_BASE_END

#endif

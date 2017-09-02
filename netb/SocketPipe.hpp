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

#ifndef NETB_SOCKET_PIPE_HPP
#define NETB_SOCKET_PIPE_HPP

#include "Socket.hpp"

NETB_BEGIN

//
// A pipe simulated with TCP connection
// 
class SocketPipe 
{
public: 
    SocketPipe(); // throw on errors
    SocketPipe(Error* e) noexcept;
    virtual ~SocketPipe() noexcept;

    // Access internal socket pair
    // Not yield ownership, so external can not close the socket
    SOCKET ReadSocket() const { return _reader.Descriptor(); }
    SOCKET WriteSocket() const { return _writer.Descriptor(); }

    // Blocked reading and writing
    ssize_t Read(void* p, size_t n, int flags = 0) noexcept;
    ssize_t Write(const void* p, size_t n, int flags = 0) noexcept;
   
private:
    Socket _reader;
    Socket _writer;

    void MakePair(Socket& reader, Socket& writer); // throw on errors
};

NETB_END

#endif

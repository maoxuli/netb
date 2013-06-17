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

#include "SocketPipe.hpp"
#include "SocketAddress.hpp"

NET_BASE_BEGIN

SocketPipe::SocketPipe()
: mReader(PF_INET, SOCK_STREAM, IPPROTO_TCP)
, mWriter(PF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    MakePair(mReader, mWriter);
}

SocketPipe::~SocketPipe()
{
    mReader.Shutdown();
    mWriter.Shutdown();
}

bool SocketPipe::MakePair(Socket& reader, Socket& writer)
{
    SocketAddress addr(NULL, 0); // Local loopback address
    reader.Block(true); 
    reader.Bind(addr.SockAddr(), addr.SockAddrLen());
    reader.Listen(1); // reader works as server socket

    addr.Reset();
    socklen_t addrlen = addr.SockAddrLen();
    reader.LocalAddress(addr.SockAddr(), &addrlen); // Get actual listening address

    writer.Block(true);
    writer.Connect(addr.SockAddr(), addr.SockAddrLen()); // Writer works as client socket

    SOCKET s = reader.Accept(); // Established connection
    reader.Attach(s); // reader shift to connection, and close server socket 

    return true;
}

ssize_t SocketPipe::Read(void* p, size_t n)
{
    return mReader.Receive(p, n);
}

ssize_t SocketPipe::Write(void* p, size_t n)
{
    return mWriter.Send(p, n);
}

NET_BASE_END

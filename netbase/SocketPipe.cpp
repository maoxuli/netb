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

#include "SocketPipe.h"

NET_BASE_BEGIN

SocketPipe::SocketPipe()
: mReader(PF_INET, SOCK_STREAM, IPPROTO_TCP)
, mWriter(PF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    MakePair(mReader, mWriter);
}

SocketPipe::~SocketPipe()
{

}

bool SocketPipe::MakePair(Socket& reader, Socket& writer)
{
    // Reader socket listen on any port of local loopback address 
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    reader.Bind((sockaddr*)&addr, sizeof(struct sockaddr_in));
    reader.Listen(1);

    // Get actual listening address
    memset(&addr, 0, sizeof(struct sockaddr_in));
    socklen_t addrlen = sizeof(struct sockaddr_in);
    reader.Address((sockaddr*)&addr, &addrlen); 

    // Establish connection from writer socket to reader socket
    writer.Connect((sockaddr*)&addr, addrlen);
    SOCKET s = reader.Accept();

    // Close reader socket and keep connection socket
    reader.Attach(s);
    return true;
}

ssize_t SocketPipe::Read(void* p, size_t n)
{
    return mReader.Receive(p, n);
}

ssize_t SocketPipe::Write(const void* p, size_t n)
{
    return mWriter.Send(p, n);
}

NET_BASE_END

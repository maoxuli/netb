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

NETB_BEGIN

SocketPipe::SocketPipe()
{
    try
    {
        _reader.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        _writer.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        MakePair(_reader, _writer);
    }
    catch(...)
    {
        throw;
    }
}

SocketPipe::SocketPipe(Error* e) noexcept
{
    try
    {
        _reader.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        _writer.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        MakePair(_reader, _writer);
    }
    catch(const Exception& ex)
    {
        SET_ERROR_CLASS(e, ex.Class());
        SET_ERROR_INFO(e, ex.Info());
        SET_ERROR_CODE(e, ex.Code());
    }
}

SocketPipe::~SocketPipe() noexcept
{

}

// throw on errors
void SocketPipe::MakePair(Socket& reader, Socket& writer)
{
    try
    {
        // Reader socket listen on any port of local loopback address 
        SocketAddress addr("loopback", 0);
        reader.Bind(addr).Listen(1);
        addr = reader.Address(); // actual address

        // Establish connection from writer socket to reader socket
        writer.Connect(addr);
        reader.Attach(reader.Accept());
    }
    catch(...)
    {
        throw;
    }
}

ssize_t SocketPipe::Read(void* p, size_t n, int flags) noexcept
{
    return _reader.Receive(p, n, flags);
}

ssize_t SocketPipe::Write(const void* p, size_t n, int flags) noexcept
{
    return _writer.Send(p, n, flags);
}

NETB_END

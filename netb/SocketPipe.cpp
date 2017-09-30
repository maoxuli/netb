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
    _reader.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    _writer.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    Error e;
    if(!MakePair(_reader, _writer, &e))
    {
        THROW_ERROR(e);
    }
}

SocketPipe::SocketPipe(Error* e) noexcept
{
    if(_reader.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP, e)
       && _writer.Create(PF_INET, SOCK_STREAM, IPPROTO_TCP, e))
    {
        MakePair(_reader, _writer, e);
    }
}

SocketPipe::~SocketPipe() noexcept
{

}

bool SocketPipe::MakePair(Socket& reader, Socket& writer, Error* e)
{
    try
    {
        // Reader socket listen on any port of local loopback address 
        SocketAddress addr("loopback", 0);
        reader.Bind(addr);
        reader.Listen(1);
        addr = reader.Address(); // actual address
        // Establish connection from writer socket to reader socket
        writer.Connect(addr);
        reader.Attach(reader.Accept());
    }
    catch(const RuntimeException& ex)
    {
        SET_ERROR(e, ex.Message(), ex.Code());
        SET_ERROR_CLASS(e, RuntimeError());
    }
    catch(const LogicException& ex)
    {
        SET_ERROR(e, ex.Message(), ex.Code());
        SET_ERROR_CLASS(e, LogicError());
    }
    catch(const Exception& ex)
    {
        SET_ERROR(e, ex.Message(), ex.Code());
        SET_ERROR_CLASS(e, GeneralError());
        return false;
    }
    catch(const std::exception& ex)
    {
        SET_ERROR(e, ex.what(), 0);
        SET_ERROR_CLASS(e, GeneralError());
        return false;
    }
    return true;
}

ssize_t SocketPipe::Read(void* p, size_t n, Error* e) noexcept
{
    return _reader.Receive(p, n, 0, e);
}

ssize_t SocketPipe::Write(const void* p, size_t n, Error* e) noexcept
{
    return _writer.Send(p, n, 0, e);
}

NETB_END

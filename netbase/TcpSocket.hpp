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

#ifndef NET_BASE_TCP_SOCKET_HPP
#define NET_BASE_TCP_SOCKET_HPP

#include "Config.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"
#include "StreamBuffer.hpp"

NET_BASE_BEGIN

//
// TcpSocket is a wrapper of TCP socket, working based on internal Socket object.
// It only provides another easy and neat interface to use TCP socket.
//
class TcpSocket
{
public: 
    TcpSocket(int domain = PF_INET);
    virtual ~TcpSocket();

    // Internal socket
    SOCKET GetSocket() const { return mSocket; }

    // Bind to local address
    bool Bind(unsigned short port);
    bool Bind(const char* host, unsigned short port);
    bool Bind(const SocketAddress& addr);

    // Working as TCP server, wait for incomming connection
    bool Listen(int backlog = SOMAXCONN);

    // Working as TCP server, accept a incomming connection
    SOCKET Accept();
    SOCKET Accept(SocketAddress* addr);

    // Working as TCP client, establish a outgoing connection
    bool Connect(const char* host, unsigned short port);
    bool Connect(const SocketAddress& addr);

    // Features
    bool LocalAddress(SocketAddress* addr) const;
    bool RemoteAddress(SocketAddress* addr) const;

    // Send data
    ssize_t Send(void* p, size_t n);
    ssize_t Send(StreamBuffer* buf);

    // Receive data
    ssize_t Receive(void* p, size_t n);
    ssize_t Receive(StreamBuffer* buf);

public:
    // Options
    bool Block(bool);
    bool Block() const;

    bool ReuseAddress(bool);
    bool ReuseAddress() const;

    bool NoDelay(bool);
    bool NoDelay() const;

    bool KeepAlive(bool);
    bool KeepAlive() const;

    bool SendBuffer(size_t);
    size_t SendBuffer() const;

    bool ReceiveBuffer(size_t);
    size_t ReceiveBuffer() const;

protected:
    Socket mSocket;
};

NET_BASE_END

#endif

/*
 * Copyright (C) 2010-2016, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NET_BASE_TCP_CONNECTOR_HPP
#define NET_BASE_TCP_CONNECTOR_HPP

#include "SocketAddress.hpp"
#include "TcpSocket.hpp"
#include "EventHandler.hpp"
#include "TcpConnection.hpp"
#include <map>

NET_BASE_BEGIN

//
// TcpConnector is a wrapper of TCP socket to connect to TCP server that 
// Works in asynchronous mode.
//
// TcpConnector work in multiple connections mode. Every calling of Connect() 
// with different address will open a new SOCKET and assigned to a new 
// connection object. 
//
// Multiple local address to same server need multiple TcpConnector.
//
// TcpConnector will generate, own, and manage the established connection. 
// The users of TcpConnector will get an asynchronous notification when a 
// connection is failed to establish, or a connection has been established.
// And they can hold and use the connection but CAN NOT DESTROY the object.
//
class EventLoop;
class TcpConnector
{
public:
    TcpConnector(EventLoop* loop, sa_family_t family = AF_INET); // Not bind to local address
    TcpConnector(EventLoop* loop, unsigned short port, sa_family_t family = AF_INET); // Bind to local port
    TcpConnector(EventLoop* loop, const char* host, unsigned short port, sa_family_t family = AF_INET); // Bind to local host and port
    TcpConnector(EventLoop* loop, const SocketAddress& addr); // Bind to local address
    ~TcpConnector();

    // connect to remote address
    bool Connect(unsigned short port);
    bool Connect(const char* host, unsigned short port);
    bool Connect(const SocketAddress& addr);

    // Notify when connection established
    void SetConnectedCallback(const TcpConnection::ConnectedCallback& cb) { mConnectedCallback = cb; }

private: 
    // Event loop on thread
    EventLoop* mLoop;

    // Given local address
    SocketAddress mAddress;

    // Established connection
    std::map<SocketAddress, TcpConnection*> mConnections;

    // Callback
    TcpConnection::ConnectedCallback mConnectedCallback;
};

NET_BASE_END

#endif

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

#ifndef NET_BASE_TCP_LISTENER_HPP
#define NET_BASE_TCP_LISTENER_HPP

#include "SocketAddress.hpp"
#include "TcpSocket.hpp"
#include "EventHandler.hpp"
#include "TcpConnection.hpp"
#include <functional>
#include <string>
#include <map>

NET_BASE_BEGIN

//
// TcpLitener is a wrapper of TCP socket in listening that works in asynchronous mode. 
// The object may be instantiated with given address and listen on that address then. 
// It may listen on an address that different from that passed in on instantiation. 
// This lets TcpListener can try different address and finally successful in listening. 
// 
// TcpListener will generate, own, and manage established TCP connections.
// The users of TcpListener will get an asynchronous notification when a connection 
// is established (connected) and they can hold and use them but they 
// CAN NOT DESTROY the object.
// The users will also get asynchronous notifications when a connection is closed, 
// or data is received via a connection. 
//
class EventLoop;
class TcpListener
{
public:
    // No address binded
    TcpListener(EventLoop* loop, sa_family_t family = AF_INET);

    // Loopback address for 0 port, and wildcard for others
    TcpListener(EventLoop* loop, unsigned short port, sa_family_t family = AF_INET);

    // Loopback address for NULL host
    TcpListener(EventLoop* loop, const char* host, unsigned short port, sa_family_t family = AF_INET);

    // Given address
    TcpListener(EventLoop* loop, const SocketAddress& addr);

    // Deconstructor
    ~TcpListener();
    
    // Current status
    bool IsListening() const { return mListening; }

    // Listen on address passed in on instantiation
    bool Listen(int backlog = SOMAXCONN);

    // Listen on given address
    // Loopback address for 0 port, and wildcard for others
    bool Listen(unsigned short port, int backlog = SOMAXCONN);

    // Listen on given address
    // Loopback address for NULL host
    bool Listen(const char* host, unsigned short port, int backlog = SOMAXCONN);

    // Listen on given address
    bool Listen(const SocketAddress& addr, int backlog = SOMAXCONN);

    // Actual listening address
    // Note: Not thread safe
    SocketAddress Address() const;

    // Notify of connection established
    // must set before listening is started
    void SetConnectedCallback(const TcpConnection::ConnectedCallback& cb) { mConnectedCallback = cb; }

private:
    // EventHandler::EventCallback
    void OnRead(SOCKET s);

private: 
    // EventLoop
    EventLoop* mLoop;

    // TCP Socket
    SocketAddress mAddress;
    Socket mSocket;
    EventHandler mHandler;
    bool mListening;

    // Established TCP connections
    std::vector<TcpConnection*> mConnections;

    // Callback
    TcpConnection::ConnectedCallback mConnectedCallback;
};

NET_BASE_END

#endif

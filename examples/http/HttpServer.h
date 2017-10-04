/*
 * Copyright (C) 2015, Maoxu Li. http://maoxuli.com/dev
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

#ifndef NETB_HTTP_SERVER_H
#define NETB_HTTP_SERVER_H

#include "AsyncTcpAcceptor.hpp"
#include "AsyncTcpSocket.hpp"
#include "HttpMessage.hpp"
#include <map>

NETB_BEGIN

// HTTP connection receive request and return response
class HttpConnection : public AsyncTcpSocket 
{
public:
    HttpConnection(EventLoop* loop, SOCKET s, const SocketAddress* connected);

private:
    // Request message from this connection
    HttpRequest _request;

    // TcpConnection::ReceivedCallback
    void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf);

    // Handle incoming request message
    void HandleRequest(AsyncTcpSocket* conn);

    // Send response message
    void SendResponse(AsyncTcpSocket* conn, const HttpResponse& response);
};

// HTTP server is a TCP acceptor
// It manages the incomming connections
class HttpServer : public AsyncTcpAcceptor
{
public:
    // Constructor, with local address
    HttpServer(EventLoop* loop, const SocketAddress& addr);

    // Destructor, close all connections
    virtual ~HttpServer();

private:
    // Connections
    std::map<SOCKET, HttpConnection*> _connections;

    // TcpAcceptor::AcceptedCallback
    bool OnAccepted(AsyncTcpAcceptor* acceptor, SOCKET s, const SocketAddress* addr);
    void OnConnected(AsyncTcpSocket* conn, bool connected);
};

NETB_END

#endif

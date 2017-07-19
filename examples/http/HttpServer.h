/*
 * Copyright (C) 2013, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NET_BASE_HTTP_SERVER_H
#define NET_BASE_HTTP_SERVER_H

#include "HttpMessage.h"
#include "TcpAcceptor.h"
#include "TcpSocket.h"
#include <vector>

NET_BASE_BEGIN

class EventLoop;
class HttpServer
{
public:
    HttpServer(EventLoop* loop);
    virtual ~HttpServer();

    bool Start();

protected:
    // TcpConnection::ConnectedCallback
    void OnConnected(TcpConnection* conn);

    // TcpConnection::ReceivedCallback
    void OnReceived(TcpConnection* conn, StreamBuffer* buf);

    // TcpConnection::ClosedCallback
    void OnClosed(TcpConnection* conn);
    
protected:
  // IO event loop
  EventLoop* mLoop;

  // TcpListener
  TcpListener mListener;

  // List of http connections
  std::map<TcpConnection*, HttpRequest*> mRequests;

  // Handle incoming request message
  virtual void HandleRequest(TcpConnection* conn, HttpRequest* request);

  // Send response message
  void SendResponse(TcpConnection* conn, HttpResponse* response);
};

NET_BASE_END

#endif

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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "HttpServer.hpp"
#include "EventLoop.hpp"

NET_BASE_BEGIN

using namespace std::placeholders;

HttpServer::HttpServer(EventLoop* loop)
: mLoop(loop)
, mListener(loop, NULL, 80)
{
    mListener.SetConnectedCallback(std::bind(&HttpServer::OnConnected, this, _1));
}

HttpServer::~HttpServer()
{
    // close all connections
}

bool HttpServer::Start()
{
    // start tcp listener
    return mListener.Listen();
}

// Tcp listener accepted a incoming connection
void HttpServer::OnConnected(TcpConnection* conn)
{
    assert(conn != NULL);
    if(mRequests.find(conn) == mRequests.end())
    {
        mRequests[conn] = new HttpRequest();
    }
    conn->SetReceivedCallback(std::bind(&HttpServer::OnReceived, this, _1, _2));
    conn->SetClosedCallback(std::bind(&HttpServer::OnClosed, this, _1));
}

void HttpServer::OnReceived(TcpConnection* conn, StreamBuffer* buf)
{
    assert(conn != NULL);
    HttpRequest* request = mRequests[conn];
    assert(request != NULL);

    if(request->FromBuffer(buf))
    {
        HandleRequest(conn, request);
        request->Reset();
    }
}

// Http connection is closed
void HttpServer::OnClosed(TcpConnection* conn)
{
    assert(conn != NULL);
    delete mRequests[conn];
    mRequests.erase(conn);
}

void HttpServer::SendResponse(TcpConnection* conn, HttpResponse* response)
{
    StreamBuffer buf;
    response->ToBuffer(&buf);
    conn->Send(&buf);
}

void HttpServer::HandleRequest(TcpConnection* conn, HttpRequest* request)
{

}

NET_BASE_END

// httpserver 
int main(const int argc, char* argv[])
{
    netbase::EventLoop loop;
    netbase::HttpServer server(&loop);
    server.Start();
    loop.Run();
    return 0;
}

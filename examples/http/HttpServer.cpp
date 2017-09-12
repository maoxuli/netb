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

#include "HttpServer.h"

NETB_BEGIN

using namespace std::placeholders;

HttpConnection::HttpConnection(EventLoop* loop, SOCKET s, const SocketAddress* connected)
: AsyncTcpSocket(loop, s, connected)
{
    SetReceivedCallback(std::bind(&HttpConnection::OnReceived, this, _1, _2));
}

void HttpConnection::OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
{
    assert(conn == this);
    assert(buf != nullptr);
    if(_request.FromBuffer(buf))
    {
        HandleRequest(conn);
        _request.Reset();
    }
}

void HttpConnection::HandleRequest(AsyncTcpSocket* conn)
{
    std::cout << "Received a HTTP request: \n";
    std::cout << _request.ToString();

    HttpResponse res;
    SendResponse(conn, res);
}

void HttpConnection::SendResponse(AsyncTcpSocket* conn, const HttpResponse& response)
{
    assert(conn != nullptr);
    StreamBuffer buf;
    response.ToBuffer(&buf);
    conn->Send(&buf);
}

/////////////////////////////////////////////////////////////////////////////////////////

// Constructor, with local address
HttpServer::HttpServer(EventLoop* loop, const SocketAddress& addr)
: AsyncTcpAcceptor(loop, addr)
{
    SetAcceptedCallback(std::bind(&HttpServer::OnAccepted, this, _1, _2, _3));
}

// Destructor, close all connections
HttpServer::~HttpServer()
{
    for(auto it = _connections.begin(); it != _connections.end(); ++it)
    {
        HttpConnection* conn = it->second;
        delete conn;
    }
}

// Tcp acceptor accepted a incoming connection
bool HttpServer::OnAccepted(AsyncTcpAcceptor* acceptor, SOCKET s, const SocketAddress* addr)
{
    assert(acceptor == this);
    assert(s != INVALID_SOCKET);
    assert(_connections.find(s) == _connections.end());
    HttpConnection* conn = new HttpConnection(GetLoop(), s, addr);
    conn->SetConnectedCallback(std::bind(&HttpServer::OnConnected, this, _1, _2));
    conn->Connected();
    _connections[s] = conn;
    return true;
}

// Connected status changed
// Delete the connection if it is disconnected
void HttpServer::OnConnected(AsyncTcpSocket* conn, bool connected)
{
    assert(conn != nullptr);
    if(!connected)
    {
        auto it = _connections.find(conn->GetSocket());
        if(it != _connections.end())
        {
            assert(conn == it->second);
            delete conn;
            _connections.erase(it);
        }
    }
}

NETB_END

//////////////////////////////////////////////////////////////////////////////////////

// HTTP server 
int main(const int argc, char* argv[])
{
    // Service port, by default 8080
    unsigned short port = 8080;
    if(argc == 2) // https 8090
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }
    netb::EventLoop loop; // running on current thread
    netb::HttpServer server(&loop, netb::SocketAddress(port));
    server.Open();
    loop.Run();
    return 0;
}

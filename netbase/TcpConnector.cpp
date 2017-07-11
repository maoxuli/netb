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

#include "TcpConnector.hpp"
#include "EventLoop.hpp"

NET_BASE_BEGIN

using namespace std::placeholders;

TcpConnector::TcpConnector(EventLoop* loop, sa_family_t family)
: mLoop(loop)
, mAddress(NULL, 0, family) 
{

}

// Bind to local host and port
TcpConnector::TcpConnector(EventLoop* loop, const char* host, unsigned short port, sa_family_t family)
: mLoop(loop)
, mAddress(host, port, family) 
{

}

TcpConnector::TcpConnector(EventLoop* loop, const SocketAddress& addr)
: mLoop(loop)
, mAddress(addr)
{

}

TcpConnector::~TcpConnector()
{
 
}

bool TcpConnector::Connect(const char* host, unsigned short port)
{
    SocketAddress addr(host, port, mAddress.Family());
    return Connect(addr);
}

// Connect works in block mode 
// So that the result can be get immedicately
bool TcpConnector::Connect(const SocketAddress& addr)
{
    TcpConnection* conn = mConnections[addr];
    if(conn != NULL)
    {
        conn->Connected();
        return true;
    }

    Socket sock(mAddress.Family(), SOCK_STREAM, IPPROTO_TCP);
    sock.Block(true);
    sock.Bind(mAddress.SockAddr(), mAddress.Length());
    if(!sock.Connect(addr.SockAddr(), addr.Length()))
    {
        return false;
    }
    
    conn = new TcpConnection(mLoop, sock.Detach());
    mConnections[addr] = conn;
    conn->SetConnectedCallback(mConnectedCallback);
    conn->Connected();
    return true;
}

NET_BASE_END

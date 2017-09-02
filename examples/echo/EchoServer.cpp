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

#include "AsyncUdpSocket.hpp"
#include "AsyncTcpAcceptor.hpp"
#include "AsyncTcpSocket.hpp"
#include "StreamBuffer.hpp"
#include "StreamPeeker.hpp"
#include "StreamReader.hpp"
#include "StreamWriter.hpp"
#include <string>
#include <map>

NETB_BEGIN

// RFC 862
// Echo server

using namespace std::placeholders;

// UDP
class UdpEchoServer : public AsyncUdpSocket
{
public:
    UdpEchoServer(EventLoop* loop, unsigned short port = 9007)
    : AsyncUdpSocket(loop, SocketAddress(port))
    {
        SetReceivedCallback(std::bind(&UdpEchoServer::OnReceived, this, _1, _2, _3));
        ReuseAddress(true);
        ReusePort(true);
    }

private:
    void OnReceived(AsyncUdpSocket* sock, StreamBuffer* buf, const SocketAddress* addr)
    {
        assert(buf != NULL);
        std::string s;
        if(StreamPeeker(buf).String(s, buf->Readable()))
        {
            std::cout << "Received: " << s << "\n";
        }
        assert(sock != NULL);
        assert(addr != NULL);
        sock->SendTo(buf, addr);
    }
};

// TCP
class TcpEchoServer : public AsyncTcpAcceptor
{
public:
    TcpEchoServer(EventLoop* loop, unsigned short port = 9007)
    : AsyncTcpAcceptor(loop, SocketAddress(port))
    {
        SetAcceptedCallback(std::bind(&TcpEchoServer::OnAccepted, this, _1, _2, _3));
        ReuseAddress(true);
        ReusePort(true);
    }

private:
    std::map<SOCKET, AsyncTcpSocket*> _connections;

    bool OnAccepted(AsyncTcpAcceptor* acceptor, SOCKET s, const SocketAddress* addr)
    {
        assert(s != INVALID_SOCKET);
        assert(_connections.find(s) == _connections.end());
        std::cout << "Connected: " << s << "\n";
        AsyncTcpSocket* conn = new AsyncTcpSocket(GetLoop(), s, addr);
        assert(conn != NULL);
        conn->SetConnectedCallback(std::bind(&TcpEchoServer::OnConnected, this, _1, _2));
        conn->SetReceivedCallback(std::bind(&TcpEchoServer::OnReceived, this, _1, _2));
        conn->Connected();
        _connections[s] = conn;
        return true;
    }

    void OnConnected(AsyncTcpSocket* conn, bool connected)
    {
        assert(conn != NULL);
        if(!connected)
        {
            std::cout << "Disconnected: " << conn->GetSocket() << "\n";
            auto it = _connections.find(conn->GetSocket());
            assert(it != _connections.end());
            delete it->second;
            _connections.erase(it);
        }
    }

    void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
    {
        assert(conn != NULL);
        std::string s;
        if(StreamPeeker(buf).String(s, buf->Readable()))
        {
            std::cout << "Received: " << s << "\n";
        }
        conn->Send(buf);
    }
};

NETB_END

/////////////////////////////////////////////////////////////////////////////

// Open echo server on given port, by default 9007
int main(const int argc, char* argv[])
{
    // Service port, by default 9007
    unsigned short port = 9007;
    if(argc == 2) // echoserver 9017
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }

    // Thread loop
    netb::EventLoop loop;
    netb::Error e;
    netb::UdpEchoServer udps(&loop, port);
    netb::TcpEchoServer tcps(&loop, port);
    if(!udps.Open(&e))
    {
        std::cout << "UDP server open failed. " << e.Info() << "\n";
    }
    std::cout << "UDP server opened on: " << udps.Address().ToString() << "\n";
    if(!tcps.Open(&e))
    {
        std::cout << "TCP server open failed. " << e.Info() << "\n";
    }
    std::cout << "TCP server opened on: " << tcps.Address().ToString() << "\n";
    loop.Run();
    return 0;
}

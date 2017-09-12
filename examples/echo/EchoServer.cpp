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

using namespace std::placeholders;

// UDP Echo Server, RFC 862
class UdpEchoServer : public AsyncUdpSocket
{
public:
    UdpEchoServer(EventLoop* loop, unsigned short port = 9007)
    : AsyncUdpSocket(loop, SocketAddress(port))
    {
        SetReceivedCallback(std::bind(&UdpEchoServer::OnReceived, this, _1, _2, _3));
    }

private:
    void OnReceived(AsyncUdpSocket* sock, StreamBuffer* buf, const SocketAddress* addr)
    {
        assert(buf != nullptr);
        std::string s;
        if(StreamPeeker(buf).String(s))
        {
            std::cout << "Received: " << s << "\n";
        }
        assert(sock != nullptr);
        assert(addr != nullptr);
        sock->SendTo(buf, addr);
    }
};

// TCP Echo Server, RFC 862
class TcpEchoServer : public AsyncTcpAcceptor
{
public:
    TcpEchoServer(EventLoop* loop, unsigned short port = 9007)
    : AsyncTcpAcceptor(loop, SocketAddress(port, AF_INET))
    {
        SetAcceptedCallback(std::bind(&TcpEchoServer::OnAccepted, this, _1, _2, _3));
    }

private:
    std::map<SOCKET, AsyncTcpSocket*> _connections;

    bool OnAccepted(AsyncTcpAcceptor* acceptor, SOCKET s, const SocketAddress* addr)
    {
        assert(s != INVALID_SOCKET);
        assert(_connections.find(s) == _connections.end());
        std::cout << "Connected [" << s << "]\n";
        if(addr != nullptr) std::cout << "From [" << addr->ToString() << "]\n";
        AsyncTcpSocket* conn = new AsyncTcpSocket(GetLoop(), s, addr);
        assert(conn != nullptr);
        conn->SetConnectedCallback(std::bind(&TcpEchoServer::OnConnected, this, _1, _2)); // for disconnected
        conn->SetReceivedCallback(std::bind(&TcpEchoServer::OnReceived, this, _1, _2)); // for received
        conn->Connected();
        _connections[s] = conn;
        return true; // accept the connection
    }

    void OnConnected(AsyncTcpSocket* conn, bool connected)
    {
        assert(conn != nullptr);
        if(!connected)
        {
            std::cout << "Disconnected [" << conn->GetSocket() << "]\n";
            auto it = _connections.find(conn->GetSocket());
            assert(it != _connections.end());
            delete it->second;
            _connections.erase(it);
        }
    }

    void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
    {
        assert(conn != nullptr);
        std::string s;
        if(StreamPeeker(buf).String(s))
        {
            std::cout << "Received: " << s << "\n";
        }
        conn->Send(buf);
    }
};

NETB_END

/////////////////////////////////////////////////////////////////////////////

// Server
// Todo: exit singals
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
    // Open both TCP server and UDP server
    // running on single thread (current thread)
    netb::EventLoop loop;
    //netb::UdpEchoServer udps(&loop, port);
    netb::TcpEchoServer tcps(&loop, port);
    netb::Error e;
    /*if(!udps.Open(&e))
    {
        std::cout << "UDP server open failed. [" << e.Info() << "][" << e.Code() << "]\n";
    }
    std::cout << "UDP server opened. [" << udps.Address().ToString() << "]\n";*/
    if(!tcps.Open(&e))
    {
        std::cout << "TCP server open failed. [" << e.Info() << "][" << e.Code() << "]\n";
        return -1;
    }
    std::cout << "TCP server opened. [" << tcps.Address().ToString() << "]\n";
    loop.Run();
    return 0;
}

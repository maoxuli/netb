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

#include "AsyncTcpAcceptor.hpp"
#include "AsyncTcpSocket.hpp"
#include "StreamBuffer.hpp"
#include "StreamPeeker.hpp"

NETB_BEGIN

// std::placeholders::_1, _2, ...
using namespace std::placeholders;

// TCP Echo Server
class TcpEchoServer : public AsyncTcpAcceptor
{
public:
    // Given service port
    TcpEchoServer(EventLoop* loop, unsigned short port = 9007)
    : AsyncTcpAcceptor(loop, SocketAddress(port, AF_INET))
    {
        SetAcceptedCallback(std::bind(&TcpEchoServer::OnAccepted, this, _1, _2, _3));
    }

    ~TcpEchoServer()
    {
        for(auto it = _connections.begin(); it != _connections.end(); ++it)
        {
            delete *it;
        }
    }

private:
    // Established connection
    std::vector<AsyncTcpSocket*> _connections;

    // Accepted callback
    bool OnAccepted(AsyncTcpAcceptor* acceptor, SOCKET s, const SocketAddress* addr)
    {
        try
        {
            assert(s != INVALID_SOCKET);
            AsyncTcpSocket* conn = new AsyncTcpSocket(GetLoop(), s, addr);
            std::cout << "Connected [" << s << "][" << conn << "]";
            if(addr) std::cout << "[" << addr->String() << "]";
            std::cout << "\n";
            conn->SetConnectedCallback(std::bind(&TcpEchoServer::OnConnected, this, _1, _2)); // for disconnected
            conn->SetReceivedCallback(std::bind(&TcpEchoServer::OnReceived, this, _1, _2)); // for received
            _connections.push_back(conn);
            conn->Connected();
        }
        catch(Exception& ex)
        {
            std::cout << "Exception: " << ex.Report() << std::endl;
        }
        return true;
    }

    // Connected callback
    void OnConnected(AsyncTcpSocket* conn, bool connected)
    {
        assert(conn);
        if(!connected)
        {
            // There is no guarantee that the socket of this connection is still 
            // valid, so the best way is finding the object pointer, rather than 
            // the socket.
            std::cout << "Disconnected [" << conn << "]" << "\n";
            for(auto it = _connections.begin(); it != _connections.end(); ++it)
            {
                if(*it == conn)
                {
                    _connections.erase(it);
                    delete conn;
                    break;
                }
            }
        }
    }

    // Received callback
    void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
    {
        assert(conn);
        assert(buf);
        std::string s;
        if(StreamPeeker(buf).String(s))
        {
            std::cout << "Received [" << s << "]" << "\n";
        }
        conn->Send(buf); // ignore errors in sending
    }
};

NETB_END

/////////////////////////////////////////////////////////////////////////////

// Echo server
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
    
    // Error handling with exceptions
    try
    {
        netb::EventLoop loop; // running on single thread (current thread)
        netb::TcpEchoServer echos(&loop, port);
        echos.Open();
        std::cout << "Opened [" << echos.Address().String() << "]" << "\n";
        loop.Run();
    }
    catch(netb::Exception& ex)
    {
        std::cout << "Exception: " << ex.Report() << std::endl;
    }
    return 0;
}

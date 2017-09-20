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
#include "RandomReader.hpp"

NETB_BEGIN

using namespace std::placeholders;

// UDP Echo Server
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
        assert(sock);
        assert(buf);
        assert(addr);
        std::cout << "Received [" << addr->String() << "]["<< buf->Readable() << "]";
        std::string s;
        if(RandomReader(buf).String(0, s))
        {
            std::cout << "[" << s << "]";
        }
        std::cout << std::endl;
        sock->SendTo(buf, addr);
        buf->Flush();
    }
};

NETB_END

/////////////////////////////////////////////////////////////////////////////

// UDP Echo Server
// Todo: exit singals
int main(const int argc, char* argv[])
{
    // Service port, by default 9007
    unsigned short port = 9007;
    if(argc == 2) // echos 9017
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port = (unsigned short)n;
        }
    }
    
    // Erro handling with return values and error object
    netb::Error e;
    netb::EventLoop loop;
    netb::UdpEchoServer echos(&loop, port);
    if(!echos.Open(&e))
    {
        std::cout << e.Report() << std::endl;
        return 0;
    }
    std::cout << "Opened [" << echos.Address().String() << "]" << std::endl;
    loop.Run();
    return 0;
}

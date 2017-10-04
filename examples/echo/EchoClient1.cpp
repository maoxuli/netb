/*
 * Copyright (C) 2013, Maoxu Li. http://maoxuli.com/dev
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

#include "EventLoopThread.hpp"
#include "AsyncTcpSocket.hpp"
#include "StreamReader.hpp"

NETB_BEGIN

// std::placeholders::_1, _2, ...
using namespace std::placeholders;

// TCP Echo Client
class EchoClient : public AsyncTcpSocket
{
public:
    EchoClient(EventLoop* loop) 
    : AsyncTcpSocket(loop) 
    { 
        SetReceivedCallback(std::bind(&EchoClient::OnReceived, this, _1, _2));
    }

    void SendMessage(const std::string& s)
    {
        Error e;
        if(Send(s.data(), s.length(), &e) < 0) 
        {
            std::cout << e.Report() << std::endl;
        }
    }

private:
    // Received callback
    void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
    {
        assert(buf != NULL);
        std::cout << "Received [" << buf->Readable() << "]";
        std::string s;
        if(StreamReader(buf).String(s))
        {
            std::cout << "[" << s << "]";
        }
        std::cout << std::endl;
        buf->Flush();
    }
};

NETB_END

///////////////////////////////////////////////////////////////////////////

// TCP echo client
int main(const int argc, char* argv[])
{
    std::string host;
    unsigned short port = 9007; // By default, port is 9007
    if(argc == 2) // echoc 9007
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    else if(argc == 3) // echoc 192.168.1.1 9007
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }

    // Error handling with exceptions
    try
    {
        // Echo client running on a separate thread 
        // Current thread is used for user input 
        netb::EventLoopThread io_thread;
        netb::EventLoop* loop = io_thread.Start();
        netb::EchoClient echoc(loop);
        echoc.Connect(netb::SocketAddress(host, port, AF_INET));

        // User input 
        std::cout << "Connected, please input message, q to exit.\n";
        std::string line;
        while(true)
        {
            std::cin >> line;
            if(line == "q" || line == "Q")
            {
                break;
            }
            echoc.SendMessage(line);
        }
    }
    catch(const netb::Exception& ex)
    {
        std::cout << ex.Report() << std::endl;
    }
    return 0;
}

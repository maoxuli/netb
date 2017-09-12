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

#include "TcpSocket.hpp"
#include "StreamBuffer.hpp"
#include "StreamWriter.hpp"
#include "StreamReader.hpp"
#include "StreamPeeker.hpp"
#include <string>
#include <cstdlib>

NETB_BEGIN

// TCP Echo Client, RFC 862
class EchoClient : public TcpSocket
{
public:
    EchoClient() : TcpSocket() { }
    ~EchoClient() { }

    bool SendText(const std::string& s)
    {
        StreamBuffer buf;
        StreamWriter(buf).String(s);
        return Send(&buf) > 0;
    }

    std::string ReceiveText()
    {
        std::string s;
        StreamBuffer buf;
        Block(1000);
        Receive(&buf);
        if(buf.Readable() > 0)
        {
            StreamReader(buf).String(s);
        }
        return s;
    }
};

NETB_END

///////////////////////////////////////////////////////////////////////////

int main(const int argc, char* argv[])
{
    std::string host;
    unsigned short port = 9007; // By default, port is 7
    if(argc == 2) // echoclient 9007
    {
        int n = atoi(argv[1]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    else if(argc == 3) // echoclient 192.168.1.1 9007
    {
        host = argv[1];
        int n = atoi(argv[2]);
        if(n > 0 && n <= 65535)
        {
            port  = n;
        }
    }
    netb::EchoClient client;
    if(!client.Connect(netb::SocketAddress(host, port), nullptr))
    {
        std::cout << "Echo client failed to connect: " << host << ":" << port << ".\n";
        return -1;
    }
    int n = 0;
    while(++n < 1000)
    {
        std::ostringstream oss;
        oss << "This is a test text for echo, " << n << "\n";
        std::string s = oss.str();
        client.Block(3000);
        client.SendText(s);
        s = client.ReceiveText();
        std::cout << s << "\n";
    }
    return 0;
}

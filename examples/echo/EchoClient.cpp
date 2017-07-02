/*
 * Copyright (C) 20113, Maoxu Li. Email: maoxu@lebula.com
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

#include "EventLoop.hpp"
#include "EventLoopThread.hpp"
#include "TcpConnector.hpp"
#include "StreamBuffer.hpp"
#include "StreamWriter.hpp"
#include "StreamReader.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace netbase;
using namespace std::placeholders;

class EchoClient
{
public: 
    EchoClient(EventLoop* loop) 
    : mLoop(loop)
    , mConnector(mLoop)
    , mConnection(NULL)
    {
        mConnector.SetConnectedCallback(std::bind(&EchoClient::OnConnected, this, _1));
    }

    ~EchoClient() 
    {

    }

    bool Connect(const char* host, unsigned short port)
    {
        assert(mConnection == NULL);
        return mConnector.Connect(host, port);
    }

    bool SendMessage(const std::string& msg)
    {
        StreamWriter(mBuffer).SerializeString(msg, "\r\n");
        if(mConnection == NULL)
        {
            return false;
        }
        mConnection->Send(&mBuffer);
        return true;
    }

private: 
    void OnConnected(TcpConnection* conn)
    {
        assert(mConnection == NULL);
        mConnection = conn;
        mConnection->SetReceivedCallback(std::bind(&EchoClient::OnReceived, this, _1, _2));
        mConnection->SetClosedCallback(std::bind(&EchoClient::OnClosed, this, _1, _2));
        std::cout << "Outgoing connection: " << mConnection->LocalAddress().ToString() << "-"
                                             << mConnection->RemoteAddress().ToString() << ".\n";
    }

    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        assert(conn == mConnection);
        std::cout << "Received " << buf->Readable() << " bytes.\n";
        std::string msg;
        StreamReader reader(buf);
        while(reader.SerializeString(msg, "\r\n"))
        {
            std::cout << msg << ".\n";
        }
    }

    void OnClosed(TcpConnection* conn, bool keepReceiving)
    {
        if(conn == mConnection)
        {
            mConnection = NULL;
        }
    }

private: 
    EventLoop* mLoop;
    TcpConnector mConnector;
    TcpConnection* mConnection;
    StreamBuffer mBuffer;
};

int main(const int argc, char* argv[])
{
    const char* host = NULL;
    if(argc > 1) // echoclient 192.168.1.1 
    {
        host = argv[1];
    }

    EventLoopThread thread; // Separating IO thread
    EventLoop* loop = thread.Start();
    EchoClient client(loop);
    if(!client.Connect(host, 9002))
    {
        std::cout << "Connect to " << host << ":9002" << "failed.\n";
        return 0;
    }

    // UI thread get user input and send to server, repeatedly, untile 
    // user input exit
    std::cout << "Connected to " << host << ":9002.\n";
    std::cout << "Please input a message, input exit to quit.\n";
    std::string msg;
    while(true)
    {
        std::cin >> msg;
        if(msg == "exit")
        {
            break;
        }
        client.SendMessage(msg);
    }
    return 0;
}
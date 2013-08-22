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

#include "EventLoop.hpp"
#include "EventLoopThread.hpp"
#include "UdpSocket.hpp"
#include "StreamBuffer.hpp"
#include "StreamReader.hpp"
#include "StreamWriter.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>


using namespace netbase;
using namespace std::placeholders;

class ChatPeer
{
public: 
    ChatPeer(EventLoop* loop, short port)  // Listening port
    : mLoop(loop)
    , mSocket(loop, port)
    , mInChat(false)
    {
        (void)mLoop;
        mSocket.SetReceivedCallback(std::bind(&ChatPeer::OnReceived, this, _1, _2, _3));
    }

    ~ChatPeer() 
    {

    }

    bool Open()
    {
        return mSocket.Open();
    }

    bool Contact(const std::string& ip, short port)
    {
        if(mInChat)
        {
            std::cout << "Chat is in progress.\n";
            return false;
        }
        mSocket.Connect(ip, port);
        StreamWriter(mBuffer).SerializeString("Hello", "\r\n");
        mSocket.Send(mBuffer);
        assert(mBuffer.Empty());
        return true;
    }

    bool SendMessage(const std::string& msg)
    {
        if(!mInChat)
        {
            std::cout << "Hello to start chat.\n";
            return false;
        }
        StreamWriter(mBuffer).SerializeString(msg, "\r\n");
        mSocket.Send(mBuffer);
        assert(mBuffer.Empty());
        return true;
    }

private: 
    void OnReceived(UdpSocket* socket, StreamBuffer* buf, SocketAddress* addr)
    {
        std::string msg;
        StreamReader(buf).SerializeString(msg, "\r\n");
        assert(buf->Empty());
        if(msg.substr(0, 7) == "Hello")
        {
            if(!mInChat)
            {
                socket->Connect(addr);
                mInChat = true;
                StreamWriter(buf).SerializeString(msg, "\r\n");
                socket->Send(buf);
            }
            std::cout << "Chat is in progress.\n";
        }
        else
        {
            std::cout << msg << "\r\n";
        }
    }

private: 
    EventLoop* mLoop;
    UdpSocket mSocket;
    bool mInChat;
    StreamBuffer mBuffer;
};



// updchat 23.34.22.1
int main(const int argc, char* argv[])
{
    assert(argc >= 2);
    EventLoopThread thread; // IO on separate thread
    EventLoop* loop = thread.Start();

    // Waiting on given port
    ChatPeer peer(thread.Start(), 9003);
    if(!peer.Open())
    {
        std::cout << "ChatPeer failed to open.\n";
    }

    char* host = NULL;
    if(argc > 1)
    {
        host = argv[1];
    }
    if(!peer.Contact(host, 9003))
    {
        
    }

    // UI thread 
    std::cout << "Hello [IP] [PORT] to start chat.\n";
    std::cout << "Bye to stop chat.\n";
    std::cout << "Exit to quite application.\n";
    std::string msg;
    while(true)
    {
        std::cin >> msg;
        if(msg == "Exit")
        {
            break;
        }
        
        if(msg.substr(0,7) == "Hello") 
        {
            std::string ip;
            short port;
            std::istringstream iss(msg);
            iss >> ip >> port; 
            peer.Contact(ip, port);
        }
        else
        {
            peer.SendMessage(msg);
        }  
    }

    return 0;
}

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
#include "TcpListener.hpp"
#include "TcpConnector.hpp"
#include "TcpConnection.hpp"
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
    , mListener(mLoop, port)
    , mConnector(mLoop)
    , mConnection(NULL)
    {
        mListener.SetConnectedCallback(std::bind(&ChatPeer::OnConnected, this, _1));
    }

    ~ChatPeer() 
    {

    }

    bool Open()
    {
        return mListener.Listen();
    }

    bool Contact(const std::string& ip, short port)
    {
        if(mConnection != NULL)
        {
            std::cout << "Chat is in progress.\n";
            return false;
        }
        return mConnector.Connect(ip, port);
    }

    bool SendMessage(const std::string& msg)
    {
        if(mConnection == NULL)
        {
            std::cout << "Hello to start chat.\n";
            return false;
        }
        StreamWriter(mBuffer).SerializeString(msg, "\r\n");
        mConnection->Send(&mBuffer);
        assert(mBuffer.Empty());
        return true;
    }

private: 
    void OnConnected(TcpConnection* conn)
    {
        if(mConnection != NULL)
        {
            conn->Close();
            return;
        }
        mConnection = conn;
        mConnection->SetReceivedCallback(std::bind(&ChatPeer::OnReceived, this, _1, _2));
        mConnection->SetClosedCallback(std::bind(&ChatPeer::OnClosed, this, _1));
        std::cout << "Chat is in progress.\n";
    }
    
    void OnReceived(TcpConnection* conn, StreamBuffer* buf)
    {
        assert(conn == mConnection);
        std::string msg;
        StreamReader(buf).SerializeString(msg, "\r\n");
        assert(buf->Empty());
        std::cout << msg << "\r\n";
    }

    void OnClosed(TcpConnection* conn)
    {
        if(conn == mConnection)
        {
            mConnection = NULL;
        }
    }

private: 
    EventLoop* mLoop;
    TcpListener mListener;
    TcpConnector mConnector;
    TcpConnection* mConnection;
    StreamBuffer mBuffer;
};

// tcpchat 9001 22.33.21.6 9004
int main(const int argc, char* argv[])
{
    short port = 9090; // Using default listening port
    if(argc > 1) // Or giving a port when launch: tcpchat 9090
    {
        int n = atoi(argv[1]);
        if(n > 0 && n < 65536)
        {
            port = (short)n;
        }
    }

    // IO on separate thread
    EventLoopThread thread;
    EventLoop* loop = thread.Start();

    // Waiting on given port
    ChatPeer peer(loop, port);
    peer.Open();

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
        
        if(msg.substr(0,7) == "Connect") 
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
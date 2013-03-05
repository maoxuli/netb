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

#ifndef NET_BASE_TCP_CONNECTION_HPP
#define NET_BASE_TCP_CONNECTION_HPP

#include "Socket.hpp"
#include "SocketAddress.hpp"
#include "EventHandler.hpp"
#include "StreamBuffer.hpp"

NET_BASE_BEGIN

//
// TcpConnection is a wrapper of a connected TCP socket that works in 
// asynchronous mode. 
// The socket is sucessfully accepted by TcpListener or connected by 
// TcpConnector 
// 
class EventLoop;
class TcpConnection
{
public: 
    TcpConnection(EventLoop* loop, SOCKET s);
    ~TcpConnection();

    // Notify status of connected 
    // Set this callback before calling Connected for thread safety
    typedef std::function<void (TcpConnection*)> ConnectedCallback;
    void SetConnectedCallback(const ConnectedCallback& cb) { mConnectedCallback = cb; }

    // Called only once when the connection is established
    // by TcpListener or TcpConnector
    void Connected();

    // Local address, or remote address if connected
    SocketAddress LocalAddress();
    SocketAddress RemoteAddress();

    // Send data 
    bool Send(void* p, size_t n);
    bool Send(StreamBuffer* buf);

    // Async receive
    typedef std::function<void (TcpConnection*, StreamBuffer*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { mReceivedCallback = cb; };

    // Close the transceiver
    void Close(bool keepReceiving = false);

    // Notification of closed
    typedef std::function<void (TcpConnection*, bool keepReceiving)> ClosedCallback;
    void SetClosedCallback(const ClosedCallback& cb) { mClosedCallback = cb; };

private:
    // Functions invoked by the loop for thread safety
    void ConnectedInLoop();
    void CloseInLoop(bool keepReceiving);

    // Buffer is copied and transfered with a shared_ptr
    // it will delete the buffer once it is processed
    typedef std::shared_ptr<StreamBuffer> StreamBufferPtr;
    void SendInLoop(StreamBufferPtr buf);

    void DoSend(void* p, size_t n);

private:
    // EventHandler::EventCallback
    // Socket events handling
    void OnRead();
    void OnWrite();
        
private:
    // EventLoop on the thread
    EventLoop* mLoop;

    // Connected socket and events handler
    Socket mSocket;
    EventHandler mHandler;
    
    // Callbacks to application
    ConnectedCallback mConnectedCallback;
    ReceivedCallback mReceivedCallback;
    ClosedCallback mClosedCallback;
    
    // Stream buffer for reading and sending
    StreamBuffer mInBuffer;
    StreamBuffer mOutBuffer;
};

NET_BASE_END

#endif 

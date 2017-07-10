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
// TcpConnection is a wrapper of a externally connected TCP socket.
// Send and receive data in asynchronous mode.
// 
class EventLoop;
class TcpConnection
{
public: 
    // Always initialize with external initialized socket
    TcpConnection(EventLoop* loop, SOCKET s);
    TcpConnection(EventLoop* loop, SOCKET s, const SocketAddress& local, const SocketAddress& remote);
    ~TcpConnection();

    // Notification of connected status
    // Calling back by the thread loop
    typedef std::function<void (TcpConnection*)> ConnectedCallback;
    void SetConnectedCallback(const ConnectedCallback& cb) { mConnectedCallback = cb; }

    // Once connected status is confirmed
    // Calling to trigger the notification of status
    void Connected();

    // Notification of received data
    // Calling back by the thread loop
    typedef std::function<void (TcpConnection*, StreamBuffer*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { mReceivedCallback = cb; };

    // Send data over the connection
    // Actual sending is done on the thread loop
    // All data will be sent, buffered for sending if necessary
    bool Send(void* p, size_t n);
    bool Send(StreamBuffer* buf);

    // Notification of closed status
    // Calling back from the thread loop
    typedef std::function<void (TcpConnection*, bool keepReceiving)> ClosedCallback;
    void SetClosedCallback(const ClosedCallback& cb) { mClosedCallback = cb; };

    // Close the connection
    // Calling to shutdown and trigger closed status
    void Close(bool keepReceiving = false);

    // Local address, remote address
    const SocketAddress& LocalAddress() const { return mLocalAddress; }
    const SocketAddress& RemoteAddress() const { return mRemoteAddress; }

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
    SocketAddress mLocalAddress;
    SocketAddress mRemoteAddress;
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

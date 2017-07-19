/*
 * Copyright (C) 2010 Maoxu Li. All rights reserved. maoxu@lebula.com
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

#ifndef NET_BASE_ASYNC_TCP_SOCKET_H
#define NET_BASE_ASYNC_TCP_SOCKET_H

#include "TcpSocket.h"
#include "EventLoop.h"
#include "EventHandler.h"
#include "StreamBuffer.h"
#include <functional>

NET_BASE_BEGIN

//
// AsynTcpSocket is a wrapper class of TCP socket that works in async mode. 
// It is based on TcpSocket, with extension to receive data in async mode.  
//
class AsyncTcpSocket : public TcpSocket
{
public:
    // Create an unbound, unconnected TCP socket
    // The domain of the socket is not determined before calling of Connect()
    explicit AsyncTcpSocket(EventLoop* loop);

    // Create an unbound, unconnected TCP socket
    // The domain of the socket is given
    AsyncTcpSocket(EventLoop* loop, int domain);

    // Create a TCP socket bound to given local address
    // The domain of the socket is determined by address family
    // See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
    AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Create a TCP socket with externally established connection
    AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* conencted, const SocketAddress* local = NULL);

    // Destructor, deriviation is allowed for extension
    virtual ~AsyncTcpSocket();

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return mLoop; }

    // Set status of connected directly once connection is established externally
    virtual bool Connected() override;

    // Connect to given address to establish connection
    virtual bool Connect(const SocketAddress& addr) override;

    // Disconnect the connection
    virtual void Disconnect() override;

    // Send data over the connection
    // data is sent out or put into sending buffer
    virtual ssize_t Send(const void* p, size_t n, int flags = 0) override;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0) override;

    // Notification of data is received
    typedef std::function<void (AsyncTcpSocket*, StreamBuffer*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { mReceivedCallback = cb; };

    // Notification of socket is closed internally
    typedef std::function<void (AsyncTcpSocket*)> ClosedCallback;
    void SetClosedCallback(const ClosedCallback& cb) { mClosedCallback = cb; };

private:
    // Receive is restricted to access internally in async mode
    virtual ssize_t Receive(void* p, size_t n, int flags = 0) override;
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0) override;

protected:
    // Functions invoked by the loop for thread safety
    void ConnectedInLoop();
    void CloseInLoop(bool keepReceiving);

    // Buffer is copied and transfered with a shared_ptr
    // it will delete the buffer once it is processed
    typedef std::shared_ptr<StreamBuffer> StreamBufferPtr;
    void SendInLoop(StreamBufferPtr buf);

    void DoSend(const void* p, size_t n);
        
protected:
    // Async facility
    EventLoop* mLoop;
    EventHandler* mHandler;
    ReceivedCallback mReceivedCallback;
    ClosedCallback mClosedCallback;

    // Buffer for receiving and sending
    StreamBuffer mInBuffer;
    StreamBuffer mOutBuffer;

    // Enable reading and writing
    bool EnableReading();
    bool EnableWriting();
    
    // EventHandler::EventCallback
    void OnRead(SOCKET s);
    void OnWrite(SOCKET s);
};

NET_BASE_END

#endif

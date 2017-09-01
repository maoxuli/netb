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

#ifndef NET_BASE_ASYNC_TCP_SOCKET_HPP
#define NET_BASE_ASYNC_TCP_SOCKET_HPP

#include "TcpSocket.hpp"
#include "EventLoop.hpp"
#include "EventHandler.hpp"
#include "StreamBuffer.hpp"
#include <functional>

NET_BASE_BEGIN

//
// AsynTcpSocket is a wrapper class of TCP socket that works in async mode. 
//
class AsyncTcpSocket : public TcpSocket
{
public:
    // Constructor, with local address info
    explicit AsyncTcpSocket(EventLoop* loop) noexcept;
    AsyncTcpSocket(EventLoop* loop, sa_family_t family) noexcept; 
    AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr) noexcept;

    // Constructor, with externally established connection
    AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* connected) noexcept;

    // Destructor
    virtual ~AsyncTcpSocket() noexcept;

    // Event loop is exposed for external use
    EventLoop* GetLoop() const noexcept { return _loop; }

    // Connect to given address to establish connection
    virtual void Connect(const SocketAddress& addr); // throw on erros
    virtual bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Set status of connected directly once connection is established externally
    virtual void Connected(); // throw on errors
    virtual bool Connected(Error* e) noexcept;

    // Close the connection
    virtual bool Close(Error* e = NULL) noexcept;

    // Send data over the connection
    virtual ssize_t Send(const void* p, size_t n, int flags = 0) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0) noexcept;

    // Receive data from the connection
    virtual ssize_t Receive(void* p, size_t n, int flags = 0) noexcept
    {
        return -1;
    }
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0) noexcept
    {
        return -1;
    }

    // Notification of connected
    typedef std::function<void (AsyncTcpSocket*, bool)> ConnectedCallback;
    void SetConnectedCallback(const ConnectedCallback& cb) noexcept { _connected_callback = cb; }

    // Notification of data is sent
    typedef std::function<void (AsyncTcpSocket*, size_t)> SentCallback;
    void SetSentCallback(const SentCallback& cb) noexcept { _sent_callback = cb; };

    // Notification of data is received
    typedef std::function<void (AsyncTcpSocket*, StreamBuffer*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) noexcept { _received_callback = cb; };
        
private:
    // Async facility
    EventLoop* _loop;
    EventHandler* _handler;
    ConnectedCallback _connected_callback;
    SentCallback _sent_callback;
    ReceivedCallback _received_callback;

    // Buffer for receiving and sending
    StreamBuffer _in_buffer;
    StreamBuffer _out_buffer;
    std::mutex _out_buffer_mutex;

    // Enable reading and writing
    bool EnableReading(Error* e = NULL) noexcept;
    bool EnableWriting(Error* e = NULL) noexcept;
    
    // EventHandler::EventCallback
    void OnRead(SOCKET s) noexcept;
    void OnWrite(SOCKET s) noexcept;
};

NET_BASE_END

#endif

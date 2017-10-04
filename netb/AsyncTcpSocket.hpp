/*
 * Copyright (C) 2010-2015, Maoxu Li. http://maoxuli.com/dev
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

#ifndef NETB_ASYNC_TCP_SOCKET_HPP
#define NETB_ASYNC_TCP_SOCKET_HPP

#include "TcpSocket.hpp"
#include "EventLoop.hpp"
#include "EventHandler.hpp"
#include "StreamBuffer.hpp"
#include <functional>

NETB_BEGIN

//
// AsynTcpSocket is a wrapper class of TCP socket that works in async mode. 
//
class AsyncTcpSocket : public TcpSocket
{
public:
    // Any local address, family is given by connected address
    explicit AsyncTcpSocket(EventLoop* loop) noexcept;

    // Any local address of given family, only working in given family
    AsyncTcpSocket(EventLoop* loop, sa_family_t family) noexcept; 

    // Fixed local address, only working in the family of given address
    AsyncTcpSocket(EventLoop* loop, const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true) noexcept;

    // Externally established connection with connected address
    AsyncTcpSocket(EventLoop* loop, SOCKET s, const SocketAddress* addr) noexcept;

    // Destructor
    virtual ~AsyncTcpSocket() noexcept;

    // Event loop is exposed for external use
    EventLoop* GetLoop() const noexcept { return _loop; }

    // Set status for externally established connection
    // Enable async facility
    using TcpSocket::Connected;
    virtual bool Connected(Error* e) noexcept;

    // Actively connect to remote address, in block mode
    // Enable async facility on success
    using TcpSocket::Connect;
    virtual bool Connect(const SocketAddress& addr, Error* e) noexcept;

    // Actively connect to remote address, in non-block mode with timeout
    // timeout of -1 for block mode
    // Enable async facility on success
    virtual bool Connect(const SocketAddress& addr, int timeout, Error* e) noexcept;

    // Close the connection
    // Clean asycn facility
    virtual bool Close(Error* e = nullptr) noexcept;

    // Send data over the connection
    // in async mode, send always return immediately and data is buffered
    using TcpSocket::Send;
    virtual ssize_t Send(const void* p, size_t n, Error* e = nullptr) noexcept;

    // Overloading for send data from received callback
    virtual ssize_t Send(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Notification of connected status
    typedef std::function<void (AsyncTcpSocket*, bool)> ConnectedCallback;
    void SetConnectedCallback(const ConnectedCallback& cb) noexcept { _connected_callback = cb; }

    // Notification of data is sent
    typedef std::function<void (AsyncTcpSocket*, size_t)> SentCallback;
    void SetSentCallback(const SentCallback& cb) noexcept { _sent_callback = cb; };

    // Notification of data is received
    typedef std::function<void (AsyncTcpSocket*, StreamBuffer*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) noexcept { _received_callback = cb; };

public:
    // In asynchronous mode, send always working in async mode
    // Send data over the connection, in non-block mode with timeout
    virtual ssize_t Send(const void* p, size_t n, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Send : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    virtual ssize_t Send(StreamBuffer& buf, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Send : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

   // In asynchronous mode, data is received internally
   // and notify application by ReceivedCallback
    virtual ssize_t Receive(void* p, size_t n, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    virtual ssize_t Receive(StreamBuffer* buf, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    virtual ssize_t Receive(void* p, size_t n, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    virtual ssize_t Receive(StreamBuffer* buf, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AsyncTcpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

private:
    // Async facility
    EventLoop* _loop;
    EventHandler* _handler;
    ConnectedCallback _connected_callback;
    SentCallback _sent_callback;
    ReceivedCallback _received_callback;

    // Receiving buffer
    StreamBuffer _in_buffer;

    // Sending buffer
    StreamBuffer _out_buffer;
    std::mutex _out_buffer_mutex;

    // Register I/O events to enable reading and writing
    bool InitHandler(Error* = nullptr);
    bool EnableReading(Error* e = nullptr);
    bool EnableWriting(Error* e = nullptr);
    
    // EventHandler::EventCallback
    // I/O event is ready
    void OnRead(SOCKET s);
    void OnWrite(SOCKET s);
};

NETB_END

#endif

/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NETB_ASYNC_UDP_SOCKET_HPP
#define NETB_ASYNC_UDP_SOCKET_HPP

#include "UdpSocket.hpp"
#include "EventLoop.hpp"
#include "EventHandler.hpp"
#include "StreamBuffer.hpp"
#include <functional>
#include <queue>

NETB_BEGIN

//
// AysncUdpSocket is a wraper class of UDP socket that supports async I/O. 
// 
// Send() and SendTo() work in non-block mode, and calling them will return 
// immediately. The return values indicate the number of bytes that written 
// into sedning buffer, rather than sent out. The data in sending buffer 
// will be sent out internally and the count is notified by a callback. 
//
// There is no functions used for receiving data. Data is received internally 
// into receiving buffer and the buffer address is notified by a callback. 
//
class AsyncUdpSocket : public UdpSocket
{
public:
    // Constructor, with local address info
    explicit AsyncUdpSocket(EventLoop* loop) noexcept;
    AsyncUdpSocket(EventLoop* loop, sa_family_t family) noexcept;
    AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr) noexcept;

    // Destructor
    virtual ~AsyncUdpSocket() noexcept;

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return _loop; }
    
    // Open to receive data
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open to receive data
    // Bind to given address
    virtual void Open(const SocketAddress& addr); // throw on errors
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept;

    // Close
    virtual bool Close(Error* e = NULL) noexcept;

    // Send data to given address
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress* addr, int flags = 0, Error* e = NULL) noexcept;
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, int flags = 0, Error* e = NULL) noexcept;

    // Send data to connected address
    virtual ssize_t Send(const void* p, size_t n, int flags = 0, Error* e = NULL) noexcept;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0, Error* e = NULL) noexcept;

    // Receive data and get remote address
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0, Error* e = NULL) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return -1;
    }
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0, Error* e = NULL) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return -1;
    }

    // Receive data from connected address
    virtual ssize_t Receive(void* p, size_t n, int flags = 0, Error* e = NULL) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return -1;
    }
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0, Error* e = NULL) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return -1;
    }

    // Notification of data is sent
    typedef std::function<void (AsyncUdpSocket*, size_t, const SocketAddress*)> SentCallback;
    void SetSentCallback(const SentCallback& cb) { _sent_callback = cb; }

    // Notification of data is received
    typedef std::function<void (AsyncUdpSocket*, StreamBuffer*, const SocketAddress*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { _received_callback = cb; };

private:
    // Aysnc facility
    EventLoop* _loop;
    EventHandler* _handler;
    SentCallback _sent_callback;
    ReceivedCallback _received_callback;

    // Receiving buffer
    // I/O in single thread
    // a complete datagram is received and notified each time
    // buffer will be clear after each callback
    StreamBuffer _in_buffer;

    // Sending buffer
    // buffer and send out 
    struct BufferAddress
    {
        BufferAddress(StreamBuffer* b, SocketAddress sa)
        : buf(b), addr(sa) { }
        
        StreamBuffer* buf;
        SocketAddress addr;

    };
    std::queue<BufferAddress> _out_buffers;
    std::mutex _out_buffers_mutex;

    // Enable reading and writing
    bool EnableReading(Error* e = NULL) noexcept;
    bool EnableWriting(Error* e = NULL) noexcept;

    // EventHandler::EventCallback;
    void OnRead(const SOCKET s) noexcept;
    void OnWrite(const SOCKET s) noexcept;
};

NETB_END

#endif

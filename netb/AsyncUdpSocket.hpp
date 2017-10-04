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

#ifndef NETB_ASYNC_UDP_SOCKET_HPP
#define NETB_ASYNC_UDP_SOCKET_HPP

#include "UdpSocket.hpp"
#include "EventLoop.hpp"
#include "EventHandler.hpp"
#include <queue>

NETB_BEGIN

//
// AysncUdpSocket is a wraper class of UDP socket with async I/O. 
//
class AsyncUdpSocket : public UdpSocket
{
public:
    // Any family and local address determined by following operations 
    explicit AsyncUdpSocket(EventLoop* loop) noexcept;

    // Fixed family, only working in given family
    AsyncUdpSocket(EventLoop* loop, sa_family_t family) noexcept;

    // Fixed local address
    AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr) noexcept;

    // Destructor
    virtual ~AsyncUdpSocket() noexcept;

    // Get event loop
    EventLoop* GetLoop() const { return _loop; }
    
    // Open to receive data
    // Override to enable async I/O facility
    using UdpSocket::Open;
    virtual bool Open(const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true, Error* e = nullptr) noexcept;

    // Close
    // Override to clean async I/O facility
    virtual bool Close(Error* e = nullptr) noexcept;

    // Send data to given address
    // Async I/O, return immediately and data may be buffered for sending
    using UdpSocket::SendTo;
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress& addr, Error* e = nullptr) noexcept;
    //virtual ssize_t SendTo(StreamBuffer& buf, const SocketAddress& addr, Error* e = nullptr) noexcept;

    // Overloading this function for send data from received callback
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress* addr, Error* e = nullptr) noexcept; 

    // Send data to connected address
    // Async I/O, return immediately and data may be buffered for sending
    using UdpSocket::Send;
    virtual ssize_t Send(const void* p, size_t n, Error* e = nullptr) noexcept;
    //virtual ssize_t Send(StreamBuffer& buf, Error* e = nullptr) noexcept;

    // Overloading this function for send data from received callback
    virtual ssize_t Send(StreamBuffer* buf, Error* e = nullptr) noexcept;

    // Async notification of data is sent
    typedef std::function<void (AsyncUdpSocket*, size_t, const SocketAddress*)> SentCallback;
    void SetSentCallback(const SentCallback& cb) noexcept { _sent_callback = cb; }

    // Async notification of data is received
    typedef std::function<void (AsyncUdpSocket*, StreamBuffer*, const SocketAddress*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) noexcept { _received_callback = cb; };

public:
    // In async mode, send data with timeout is not necessary, data may be buffered for sending
    // to given address
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress& addr, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::SendTo : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // to given address
    virtual ssize_t SendTo(StreamBuffer& buf, const SocketAddress& addr, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::SendTo : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // to connected address
    virtual ssize_t Send(const void* p, size_t n, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::Send : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // to connected address
    virtual ssize_t Send(StreamBuffer& buffer, int timeout, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::SendTo : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // In async mode, data is received internally and notified by callback.
    // Receive data and get remote address
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::ReceiveFrom : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // Receive data and get remote address
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::ReceiveFrom : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // Receive data from connected address
    virtual ssize_t Receive(void* p, size_t n, int flags = 0, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

    // Receive data from connected address
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0, Error* e = nullptr) noexcept
    {
        SET_LOGIC_ERROR(e, "AysncUdpSocket::Receive : Function not work in this mode.", ErrorCode::ACCES);
        return -1;
    }

private:
    // Aysnc facility
    EventLoop* _loop;
    EventHandler* _handler;
    SentCallback _sent_callback;
    ReceivedCallback _received_callback;

    // Receiving buffer
    // for a single single message
    StreamBuffer _in_buffer;

    // Sending buffer
    // message and peer address
    struct BufferAddress
    {
        BufferAddress(const StreamBuffer* b, const SocketAddress& sa)
        : buf(b), addr(sa) { }
        BufferAddress(const StreamBuffer* b)
        : buf(b) { }
        const StreamBuffer* buf;
        const SocketAddress addr;
    };
    std::queue<BufferAddress> _out_buffers;
    std::mutex _out_buffers_mutex;

    // Enable reading and writing
    bool InitHandler(Error* e = nullptr);
    bool EnableReading(Error* e = nullptr);
    bool EnableWriting(Error* e = nullptr);

    // EventHandler::EventCallback;
    // On I/O ready events
    void OnRead(const SOCKET s);
    void OnWrite(const SOCKET s);
};

NETB_END

#endif

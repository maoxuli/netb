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

#ifndef NET_BASE_ASYNC_UDP_SOCKET_H
#define NET_BASE_ASYNC_UDP_SOCKET_H

#include "UdpSocket.h"
#include "EventLoop.h"
#include "EventHandler.h"
#include "StreamBuffer.h"
#include <functional>

NET_BASE_BEGIN

//
// AysncUdpSocket is a wraper class of UDP socket that works in async mode. 
// It is based on UpdSocket with extension of receiving data in async mode. 
//
class AsyncUdpSocket : public UdpSocket
{
public:
    // Create an unbound, unconnected UDP socket
    // The domain of the socket is not determined before calling of Open() or Connect() or SendTo()
    explicit AsyncUdpSocket(EventLoop* loop);

    // Create an unbound, unconnected UDP socket
    // The domain of the socket is given
    AsyncUdpSocket(EventLoop* loop, int domain);

    // Create an UDP socket bound to given address
    // The domain of the socket is determined by address family
    // See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
    AsyncUdpSocket(EventLoop* loop, const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Destructor, deriviation is allow for extension
    virtual ~AsyncUdpSocket();

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return mLoop; }
    
    // Open on given address and start to receive data
    virtual bool Open() override;
    virtual bool Open(const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false) override;

    // Send (non-connection)
    virtual ssize_t SendTo(const void* p, size_t n, const SocketAddress& addr, int flags = 0) override;
    virtual ssize_t SendTo(StreamBuffer* buf, const SocketAddress& addr, int flags = 0) override;

    // Send (connection)
    virtual ssize_t Send(const void* p, size_t n, int flags = 0) override;
    virtual ssize_t Send(StreamBuffer* buf, int flags = 0) override;

    // Notification of data is received
    typedef std::function<void (AsyncUdpSocket*, StreamBuffer*, const SocketAddress& addr)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { mReceivedCallback = cb; };

    // Notification of socket is closed internally
    typedef std::function<void (AsyncTcpSocket*)> ClosedCallback;
    void SetClosedCallback(const ClosedCallback& cb) { mClosedCallback = cb; };

private:
    // Receive is restricted to access internally
    virtual ssize_t ReceiveFrom(void* p, size_t n, SocketAddress* addr, int flags = 0) override;
    virtual ssize_t ReceiveFrom(StreamBuffer* buf, SocketAddress* addr, int flags = 0) override;

    virtual ssize_t Receive(void* p, size_t n, int flags = 0) override;
    virtual ssize_t Receive(StreamBuffer* buf, int flags = 0) override;

protected:
    // Aysnc facility
    EventLoop* loop;
    EventHandler* mHandler;
    ReceivedCallback mReceivedCallback;
    ClosedCallback mClosedCallback;

    // Receive buffer, receive and callback one by one
    StreamBuffer mInBuffer;

    // Send buffer, make sure to send message one by one 
    struct BufferAddress
    {
        BufferAddress(StreamBuffer* b, SocketAddress sa)
        : buf(b), addr(sa) { }
        
        StreamBuffer* buf;
        SocketAddress addr;
    };
    std::queue<BufferAddress> mOutBuffers;

    // Enable reading and writing
    void EnableReading();
    void EnableWriting();

    // EventHandler::EventCallback;
    void OnRead(SOCKET s);
    void OnWrite(SOCKET s);
};

NET_BASE_END

#endif

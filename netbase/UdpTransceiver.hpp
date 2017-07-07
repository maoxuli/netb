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

#ifndef NET_BASE_UDP_TRANSCEIVER_HPP
#define NET_BASE_UDP_TRANSCEIVER_HPP

#include "Socket.hpp"
#include "SocketAddress.hpp"
#include "EventHandler.hpp"
#include "ByteBuffer.hpp"
#include <queue>
#include <memory>

NET_BASE_BEGIN

//
// UdpTransceiver is a wrapper of UDP socket that works in 
// asynchronous mode
//

class EventLoop;
class UdpTransceiver
{
public: 
    // Not bind to local address
    UdpTransceiver(EventLoop* loop, sa_family_t family = AF_INET);

    // Bind to local address
    // If host is NULL and port is 0, assign loop back address
    UdpTransceiver(EventLoop* loop, const char* host, unsigned short port, sa_family_t family = AF_INET);

    // Given address
    UdpTransceiver(EventLoop* loop, const SocketAddress& addr);

    // Destructor
    ~UdpTransceiver();

    // Open on address passed in on instantiation
    bool Open();

    // Open on a given host and port
    bool Open(const char* host, unsigned short port);

    // Open on a given address
    bool Open(const SocketAddress& addr);

    // Async receive
    typedef std::function<void (UdpTransceiver*, ByteStream*, const SocketAddress*)> ReceivedCallback;
    void SetReceivedCallback(const ReceivedCallback& cb) { mReceivedCallback = cb; };

    // Send data with not connected
    bool Send(void* p, size_t n, const SocketAddress& addr);
    bool Send(ByteStream* buf, const SocketAddress& addr);

    // Connect to remote address
    bool Connect(const char* host, unsigned short port);
    bool Connect(const SocketAddress& addr);

    // Send data after connected
    bool Send(void* p, size_t n);
    bool Send(ByteStream* buf);

    // Notification of closed
    typedef std::function<void (UdpTransceiver*, bool keepReceiving)> ClosedCallback;
    void SetClosedCallback(const ClosedCallback& cb) { mClosedCallback = cb; };

    // Close the transceiver
    void Close(bool keepReceiving = false);

    // Local binded address
    const SocketAddress& Address() const { return mAddress; }

    // Remote address if connected
    const SocketAddress& RemoteAddress() const { return mRemoteAddress; }

private:
    // Buffer is copied and transfered with a shared_ptr
    // it will delete the buffer once it is processed
    typedef std::shared_ptr<ByteBuffer> ByteBufferPtr;
    void SendInLoop(ByteBufferPtr buf, SocketAddress addr);

    void DoSend(void* p, size_t n, const SocketAddress& addr);

private:
    // EventHandler::EventCallback
    void OnRead();
    void OnWrite();

private: 
    // Event loop on thread
    EventLoop* mLoop;

    // Socket and local address
    SocketAddress mAddress;
    Socket mSocket;
    EventHandler mHandler;

    // Connect to remote address
    SocketAddress mRemoteAddress;
    bool mConnected;

    // Received and closed Callback
    ReceivedCallback mReceivedCallback;
    ClosedCallback mClosedCallback;

    // Callback per received packet
    ByteBuffer mInBuffer;

    // UDP send packets one by one always 
    struct BufferAddress
    {
        BufferAddress(ByteBuffer* b, SocketAddress sa)
        : buf(b), addr(sa) { }
        
        ByteBuffer* buf;
        SocketAddress addr;
    };

    std::queue<BufferAddress> mOutBuffers;
};

NET_BASE_END

#endif

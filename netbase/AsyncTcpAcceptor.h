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

#ifndef NET_BASE_ASYNC_TCP_ACCEPTOR_H
#define NET_BASE_ASYNC_TCP_ACCEPTOR_H

#include "TcpAcceptor.h"
#include "EventLoop.h"
#include "EventHandler.h"
#include <functional>

NET_BASE_BEGIN

//
// AsynTcpAcceptor is a wrapper class of TCP server socket that works 
// in async mode. It is based on TcpAcceptor, with extension to accept 
// incomming connections in async mode.  
//
class AsyncTcpAcceptor : public TcpAcceptor
{
public:
    // Create an unbound TCP socket as TCP server
    // The domain of the socket is not determined before calling of Open()
    explicit AsyncTcpAcceptor(EventLoop* loop);

    // Create an unbound TCP socket as TCP server
    // The domain of the socket is given
    AsyncTcpAcceptor(EventLoop* loop, int domain);

    // Create an TCP socket bound to given local address
    // The domain of the socket is determined by address family
    // See socket options of SO_REUSEADDR and SO_REUSEPORT for reuseaddr and reuseport
    AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr, bool reuseaddr = false, bool reuseport = false);

    // Destructor, deriviation is allowed for extension
    virtual ~AsyncTcpAcceptor();

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return mLoop; } 

    // Open is overriden to initialize async process
    virtual bool Open(int backlog = SOMAXCONN) override;
    virtual bool Open(const SocketAddress& addr, int backlog = SOMAXCONN) override;

    // Notificaiton of connection is accepted
    typedef std::function<AsyncTcpAcceptor*, void (SOCKET)> AcceptedCallback;
    void SetAcceptedCallback(const AcceptedCallback& cb) { mAcceptedCallback = cb; }

private:
    // Accept is restricted to access internally in async mode
    virtual bool Accept() override;
    virtual bool Accept(SocketAddress* addr) override;

protected: 
    // Async facility
    EventLoop* mLoop;
    EventHandler* mHandler;
    AcceptedCallback mAcceptedCallback;

    // Enable reading
    bool EnableReading();

    // EventHandler::ReadCallbck
    void OnRead(SOCKET s);
};

NET_BASE_END

#endif

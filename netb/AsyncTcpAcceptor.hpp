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

#ifndef NETB_ASYNC_TCP_ACCEPTOR_HPP
#define NETB_ASYNC_TCP_ACCEPTOR_HPP

#include "TcpAcceptor.hpp"
#include "EventLoop.hpp"

NETB_BEGIN

//
// AsynTcpAcceptor is a wrapper class of TCP server socket that works 
// in async mode. 
//
class AsyncTcpAcceptor : public TcpAcceptor
{
public:
    // Constructor, with local address info
    explicit AsyncTcpAcceptor(EventLoop* loop) noexcept; // no address info
    AsyncTcpAcceptor(EventLoop* loop, sa_family_t family) noexcept; // any address of given family
    AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr) noexcept; // given address

    // Destructor
    virtual ~AsyncTcpAcceptor() noexcept;

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return _loop; } 

    // Open is overriden to enable async I/O
    // Init event handler and register interested events
    virtual void Open(); // throw on errors
    virtual bool Open(Error* e) noexcept;

    // Open on given address
    // Init event handler and register interested events
    virtual void Open(const SocketAddress& addr); // throw on errers
    virtual bool Open(const SocketAddress& addr, Error* e) noexcept;

    // Close, and ready for open again
    virtual bool Close(Error* e = NULL) noexcept;

    // Accept a connection, in block or non-block mode
    virtual SOCKET Accept() // thow on errors
    {
        return INVALID_SOCKET;
    }
    virtual SOCKET Accept(Error* e) noexcept
    {
        return INVALID_SOCKET;
    }

    // address is a pointer rather a reference, so could be NULL
    virtual SOCKET AcceptFrom(SocketAddress* addr) // throw on errors
    {
        return INVALID_SOCKET;
    }
    virtual SOCKET AcceptFrom(SocketAddress* addr, Error* e) noexcept
    {
        return INVALID_SOCKET;
    }

    // Notificaiton of connection is accepted
    // Address is a const pointer, rather than a reference, so it could be NULL
    // If the callback return false, indicate application did not take over the ownership of socket, 
    // the socket should be closed internally
    typedef std::function<bool (AsyncTcpAcceptor*, SOCKET, const SocketAddress*)> AcceptedCallback;
    void SetAcceptedCallback(const AcceptedCallback& cb) noexcept { _accepted_callback = cb; }

private: 
    // Async facility
    EventLoop* _loop;
    EventHandler* _handler;
    AcceptedCallback _accepted_callback;

    // Enable async reading
    bool EnableReading(Error* e) noexcept;

    // EventHandler::ReadCallbck
    void OnRead(SOCKET s) noexcept;
};

NETB_END

#endif

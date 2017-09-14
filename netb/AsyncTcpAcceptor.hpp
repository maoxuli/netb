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
    // Dynamic address
    explicit AsyncTcpAcceptor(EventLoop* loop) noexcept;

    // Fixed family with any or dynamic address
    AsyncTcpAcceptor(EventLoop* loop, sa_family_t family) noexcept; 

    // Fixed address
    AsyncTcpAcceptor(EventLoop* loop, const SocketAddress& addr, bool reuse_addr = true, bool reuse_port = true) noexcept;

    // Destructor
    virtual ~AsyncTcpAcceptor() noexcept;

    // Event loop is exposed for external use
    EventLoop* GetLoop() const { return _loop; } 

    // The actual open process
    // Enable async facility on success to accept incomming connections
    using TcpAcceptor::Open;
    virtual bool Open(const SocketAddress& addr, bool reuse_addr, bool reuse_port, Error* e) noexcept;

    // Close
    // Clean async facility 
    virtual bool Close(Error* e = nullptr) noexcept;

    // Notificaiton of connection is accepted
    // If the callback return false, indicate application did not take over the ownership of socket, 
    // the socket should be closed internally
    typedef std::function<bool (AsyncTcpAcceptor*, SOCKET, const SocketAddress*)> AcceptedCallback;
    void SetAcceptedCallback(const AcceptedCallback& cb) noexcept { _accepted_callback = cb; }

public:
    // In async mode, incomming connections are accetpted internally 
    // and notify the application by AcceptedCallback
    virtual SOCKET Accept() // thow on errors
    {
        throw LogicException("Function not work in this mode.");
    }

    virtual SOCKET Accept(Error* e) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return INVALID_SOCKET;
    }

    virtual SOCKET Accept(int timeout) // throw on errors
    {
       throw LogicException("Function not work in this mode.");
    }
    
    virtual SOCKET Accept(int timeout, Error* e) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return INVALID_SOCKET;
    }
    
    virtual SOCKET AcceptFrom(SocketAddress* addr) // thow on errors
    {
        throw LogicException("Function not work in this mode.");
    }

    virtual SOCKET AcceptFrom(SocketAddress* addr, Error* e) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return INVALID_SOCKET;
    }

    virtual SOCKET AcceptFrom(SocketAddress* addr, int timeout) // throw on errors
    {
       throw LogicException("Function not work in this mode.");
    }
    
    virtual SOCKET AcceptFrom(SocketAddress* addr, int timeout, Error* e) noexcept
    {
        SET_LOGIC_ERROR(e, "Function not work in this mode.");
        return INVALID_SOCKET;
    }

private: 
    // Async facility
    EventLoop* _loop;
    EventHandler* _handler;
    AcceptedCallback _accepted_callback;

    // Register I/O events to enable async reading
    bool EnableReading(Error* e);

    // EventHandler::ReadCallbck
    // I/O event is ready
    // Accept incomming connections and notifiy by callback
    void OnRead(SOCKET s);
};

NETB_END

#endif

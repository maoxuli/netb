/*
 * Copyright (C) 2010 Maoxu Li. Email: maoxu@lebula.com
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

#include "SocketAddress.hpp"
#include <sstream>
#include <cassert>

NETB_BEGIN

// Empty, for any protocols
SocketAddress::SocketAddress()
{
    Reset();
}

// Given port, 0 for any port
// Given family
// By default, local wildcard host (INADDR_ANY, or all 0)
SocketAddress::SocketAddress(unsigned short port, sa_family_t family)
{
    Reset(family);
    if(port > 0) 
    {
        Port(port);
    }
}

// Given host, may be:
// "any": INADDR_ANY
// "wildcard": INADDR_ANY
// "none": INADDR_NONE
// "loopback": INADDR_LOOPBACK
// "localhost" or "": INADDR_LOOPBACK
// "broadcast": INADDR_NONE
// "x.x.x.x" or "x:x:x:...": IPv4 or IPv6 address
// Given port, 0 for any port
// Given family, by default AF_INET
SocketAddress::SocketAddress(const std::string& host, unsigned short port, sa_family_t family)
{
    Reset(family);
    if(host != "any" || host != "wildcard")
    {
        Host(host);
    }
    if(port > 0)
    {
        Port(port);
    }
}

SocketAddress::~SocketAddress()
{

}

// ss_len in sockaddr_storage is ignore in this implementation for compatibility
// The length of the address is determined by address family
socklen_t SocketAddress::Length() const
{
    if(this->ss_family == AF_INET)
    {
        return sizeof(struct sockaddr_in);
    }
    if(this->ss_family == AF_INET6)
    {
        return sizeof(struct sockaddr_in6);
    }
    if(this->ss_family != AF_UNSPEC)
    {
        assert(false);
        return 0;
    }
    return sizeof(struct sockaddr_storage);
}

// Set host
bool SocketAddress::Host(const std::string& host)
{
    if(this->ss_family == AF_INET)
    {
        struct sockaddr_in* addrin = reinterpret_cast<struct sockaddr_in*>(this);

        if(host.empty() || host == "loopback"|| host == "localhost")
            addrin->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        else if(host == "any" || host == "wildcard")
            addrin->sin_addr.s_addr = htonl(INADDR_ANY);
        else if(host == "none" || host == "broadcast")
            addrin->sin_addr.s_addr = htonl(INADDR_NONE);
        else
        {
            if(::inet_pton(AF_INET, host.c_str(), &addrin->sin_addr.s_addr) != 1)
            {
                assert(false);
                return false;
            }
        }
    }
    else if(this->ss_family == AF_INET6) 
    {
        struct sockaddr_in6* addrin6 = reinterpret_cast<struct sockaddr_in6*>(this);
        if(host.empty() || host == "loopback" || host == "localhost")
            addrin6->sin6_addr = in6addr_loopback;
        else if(host.empty() || host == "any" || host == "wildcard")
            addrin6->sin6_addr = in6addr_any;
        else if(host == "none" || host == "broadcast")
            assert(false);
	    //addrin6->sin6_addr = in6addr_nodelocal_allnodes;
        else
        {
            if(::inet_pton(AF_INET6, host.c_str(), &addrin6->sin6_addr) != 1)
            {
                assert(false);
                return false;
            }
        }
    }
    else // unsupported family
    {
        assert(false);
        return false;
    }
    return true;
}

// Set port, 0 for any port
bool SocketAddress::Port(unsigned short port)
{
    if(this->ss_family == AF_INET)
    {
        reinterpret_cast<struct sockaddr_in*>(this)->sin_port = htons(port);
    }
    else if(this->ss_family == AF_INET6) 
    {
        reinterpret_cast<struct sockaddr_in6*>(this)->sin6_port = htons(port);
    }
    else // unsupported family
    {
        assert(false);
        return false;
    }
    return true;
}

// Get host of current address
std::string SocketAddress::Host() const
{
    if(this->ss_family == AF_INET)
    {
        const struct sockaddr_in* addr = reinterpret_cast<const struct sockaddr_in*>(this);
        char host[INET_ADDRSTRLEN];
        host[0] = '\0';
        if(inet_ntop(AF_INET, &addr->sin_addr, host, INET_ADDRSTRLEN) != nullptr)
        {
            return host;
        }
        assert(false);
    }
    else if(this->ss_family == AF_INET6)
    {
        const struct sockaddr_in6* addr = reinterpret_cast<const struct sockaddr_in6*>(this);
        char host[INET6_ADDRSTRLEN];
        if(inet_ntop(AF_INET6, &addr->sin6_addr, host, INET6_ADDRSTRLEN) != nullptr)
        {
            return host;
        }
        assert(false);
    }
    else
    {
        assert(false);
    }
    return "";
}

// Get port
// If address family is not supported, throw UnsupportedFamilyException
unsigned short SocketAddress::Port() const
{
    if(this->ss_family == AF_INET)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in*>(this)->sin_port);
    }
    else if(this->ss_family == AF_INET6)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_port);
    }
    else
    {
        assert(false);
    }
    return 0;
}

// Check special address
bool SocketAddress::Wildcard() const  // INADDR_ANY
{
    assert(false);
}

bool SocketAddress::Any() const  // INADDR_ANY:0
{
    assert(false);
}

bool SocketAddress::AnyPort() const  // 0
{
    assert(false);
}

bool SocketAddress::AnyHost() const  // INADDR_ANY
{
    assert(false);
}

bool SocketAddress::Localhost() const  // "localhost"
{
    assert(false);
}

bool SocketAddress::Loopback() const  // INADDR_LOOPBACK
{
    assert(false);
}

bool SocketAddress::Broadcast() const  // INADDR_NONE
{
    assert(false);
}

bool SocketAddress::Multicast() const // 
{
    assert(false);
}

// String format address
std::string SocketAddress::String() const
{    
    char namebuf[1024];
    namebuf[0] = '\0';
    if(::getnameinfo(Addr(), Length(), namebuf, sizeof(namebuf), 0, 0, NI_NUMERICHOST) != 0)
    {
        assert(false);
        return "";
    }
    std::ostringstream oss;
    oss << namebuf << ":" << Port();
    return oss.str();
}

bool SocketAddress::operator==(const SocketAddress& a) const noexcept
{
	if(this->ss_family == a.ss_family) 
    {
		switch(a.ss_family) 
        {
			case AF_INET:
				return (
					(reinterpret_cast<const struct sockaddr_in*>(this)->sin_port == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port)&&
					(reinterpret_cast<const struct sockaddr_in*>(this)->sin_addr.s_addr == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_addr.s_addr));
				break;
			case AF_INET6:
				return (
					(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_port == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port)&&
					(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_flowinfo == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo)&&
					(memcmp(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) == 0)&&
					(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_scope_id == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_scope_id));
				break;
			default:
				return (memcmp(this, &a, sizeof(SocketAddress)) == 0);
		}
	}
	return false;
}

bool SocketAddress::operator<(const SocketAddress& a) const noexcept
{
	if(this->ss_family < a.ss_family)
    {
		return true;
    }
    if(this->ss_family == a.ss_family) 
    {
		switch(a.ss_family) 
        {
			case AF_INET:
				if(reinterpret_cast<const struct sockaddr_in*>(this)->sin_port < reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port)
					return true;
				else if(reinterpret_cast<const struct sockaddr_in*>(this)->sin_port == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port) 
                {
					if(reinterpret_cast<const struct sockaddr_in*>(this)->sin_addr.s_addr < reinterpret_cast<const struct sockaddr_in*>(&a)->sin_addr.s_addr)
						return true;
				}
				break;
			case AF_INET6:
				if(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_port < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port)
					return true;
				else if(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_port == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port) 
                {
					if(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_flowinfo < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo)
						return true;
					else if(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_flowinfo == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo) 
                    {
						if(memcmp(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) < 0)
							return true;
						else if(memcmp(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) == 0) 
                        {
							if(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_scope_id < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_scope_id)
								return true;
						}
					}
				}
				break;
			default:
				return (memcmp(this,&a,sizeof(SocketAddress)) < 0);
		}
	}
	return false;
}

NETB_END

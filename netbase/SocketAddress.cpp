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
#include <netdb.h>
#include <sstream>
#include <cassert>

NET_BASE_BEGIN

// if port number is not 0, assign wildcard address
// if port number is 0, assign loop back address
SocketAddress::SocketAddress(unsigned short port, sa_family_t family) throw()
{
    memset(&mAddress, 0, sizeof(sockaddr_storage));
    if(family == AF_INET)
    {
        sockaddr_in* addrin = reinterpret_cast<sockaddr_in*>(&mAddress);
        addrin->sin_len = sizeof(sockaddr_in);
        addrin->sin_family = AF_INET;
        addrin->sin_port = htons(port);
        addrin->sin_addr.s_addr = port == 0 ? htonl(INADDR_LOOPBACK) : htonl(INADDR_ANY);
    }
    else if(family == AF_INET6) 
    {
        sockaddr_in6* addrin6 = reinterpret_cast<sockaddr_in6*>(&mAddress);
        addrin6->sin6_len = sizeof(sockaddr_in6);
        addrin6->sin6_family = AF_INET6;
        addrin6->sin6_port = htons(port);
        addrin6->sin6_addr = port == 0 ? in6addr_loopback : in6addr_any;
    }
    else // unsupported family
    {
        assert(false);
    }
}

// By default assign loop back address if host is NULL
SocketAddress::SocketAddress(const char* host, unsigned short port, sa_family_t family) throw()
{
    memset(&mAddress, 0, sizeof(sockaddr_storage));

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family; 
    
    char service[8];
    sprintf(service,"%d", port);
    struct addrinfo* res = NULL;
    short retry = 5;
    int rs = 0;
    do
    {
        rs = getaddrinfo(host, service, &hints, &res);
    }
    while(rs == EAI_AGAIN && --retry >= 0);    
    if(rs != 0)
    {
        assert(false);
    }

    if(res != NULL)
    {
        memcpy(&mAddress, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }
}

sa_family_t SocketAddress::Family() const 
{
    return mAddress.ss_family;
}

std::string SocketAddress::Host() const 
{
    return "";
}

void SocketAddress::Port(unsigned short port) 
{
    assert(false);
}

unsigned short SocketAddress::Port() const
{
    if(mAddress.ss_family == AF_INET)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_port);
    }
    if(mAddress.ss_family == AF_INET6)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_port);
    }
    return 0;
}

std::string SocketAddress::ToString() const
{    
    char namebuf[1024];
    namebuf[0] = '\0';
    getnameinfo(SockAddr(), SockAddrLen(), namebuf, sizeof(namebuf), 0, 0, NI_NUMERICHOST);
    
    std::ostringstream oss;
    oss << namebuf << ":" << Port();
    return oss.str();
}

struct sockaddr* SocketAddress::SockAddr() 
{
    return (sockaddr*)&mAddress;
}

const struct sockaddr* SocketAddress::SockAddr() const
{
    return (sockaddr*)&mAddress;
}

// sockaddr_storage::ss_len is ignored in this implementation
// Address length is determined by ss_family
// full lenth of sockaddr_sotrage is returned if the ss_family is AF_UNSPEC 
socklen_t SocketAddress::SockAddrLen() const 
{
    if(mAddress.ss_family == AF_INET)
    {
        return sizeof(sockaddr_in);
    }
    if(mAddress.ss_family == AF_INET6)
    {
        return sizeof(sockaddr_in6);
    }
    return sizeof(sockaddr_storage);
}

bool SocketAddress::operator==(const SocketAddress& a) const throw()
{
	if(mAddress.ss_family == a.Family()) 
    {
		switch(mAddress.ss_family) 
        {
			case AF_INET:
				return (
					(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_port == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port)&&
					(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_addr.s_addr == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_addr.s_addr));
				break;
			case AF_INET6:
				return (
					(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_port == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port)&&
					(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_flowinfo == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo)&&
					(memcmp(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) == 0)&&
					(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_scope_id == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_scope_id));
				break;
			default:
				return (memcmp(&mAddress, &a, sizeof(SocketAddress)) == 0);
		}
	}
	return false;
}

bool SocketAddress::operator<(const SocketAddress& a) const throw()
{
	if(mAddress.ss_family < a.Family())
    {
		return true;
    }
    if(mAddress.ss_family == a.Family()) 
    {
		switch(mAddress.ss_family) 
        {
			case AF_INET:
				if(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_port < reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port)
					return true;
				else if(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_port == reinterpret_cast<const struct sockaddr_in*>(&a)->sin_port) 
                {
					if(reinterpret_cast<const struct sockaddr_in*>(&mAddress)->sin_addr.s_addr < reinterpret_cast<const struct sockaddr_in*>(&a)->sin_addr.s_addr)
						return true;
				}
				break;
			case AF_INET6:
				if(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_port < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port)
					return true;
				else if(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_port == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_port) 
                {
					if(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_flowinfo < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo)
						return true;
					else if(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_flowinfo == reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_flowinfo) 
                    {
						if(memcmp(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) < 0)
							return true;
						else if(memcmp(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_addr.s6_addr,reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_addr.s6_addr,16) == 0) 
                        {
							if(reinterpret_cast<const struct sockaddr_in6*>(&mAddress)->sin6_scope_id < reinterpret_cast<const struct sockaddr_in6*>(&a)->sin6_scope_id)
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

NET_BASE_END

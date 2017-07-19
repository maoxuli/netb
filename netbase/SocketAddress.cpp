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

#include "SocketAddress.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <sstream>
#include <cassert>

NET_BASE_BEGIN

// if host is null and port is 0, loopback address, used for local host only
// if host is null and port is non 0, wildcard address, used for server 
SocketAddress::SocketAddress(const char* host, unsigned short port, sa_family_t family) throw()
{
    memset(this, 0, sizeof(struct sockaddr_storage));
    if(host == NULL)
    {
        if(family == AF_INET)
        {
            struct sockaddr_in* addrin = reinterpret_cast<struct sockaddr_in*>(this);
            addrin->sin_family = AF_INET;
            addrin->sin_port = htons(port);
            addrin->sin_addr.s_addr = (port == 0 ? htonl(INADDR_LOOPBACK) : htonl(INADDR_ANY));
        }
        else if(family == AF_INET6) 
        {
            struct sockaddr_in6* addrin6 = reinterpret_cast<struct sockaddr_in6*>(this);
            addrin6->sin6_family = AF_INET6;
            addrin6->sin6_port = htons(port);
            addrin6->sin6_addr = (port == 0 ? in6addr_loopback : in6addr_any);
        }
        else // unsupported family
        {
            assert(false);
        }
    }
    else
    {
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
            memcpy(this, res->ai_addr, res->ai_addrlen);
            freeaddrinfo(res);
        }
    }
}

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
    return sizeof(struct sockaddr_storage);
}

std::string SocketAddress::Host() const
{
    if(this->ss_family == AF_INET)
    {
        const struct sockaddr_in* addr = reinterpret_cast<const struct sockaddr_in*>(this);
        char host[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET, &addr->sin_addr, host, INET_ADDRSTRLEN) != NULL)
        {
            return host;
        }
    }
    else if(this->ss_family == AF_INET6)
    {
        const struct sockaddr_in6* addr = reinterpret_cast<const struct sockaddr_in6*>(this);
        char host[INET6_ADDRSTRLEN];
        if(inet_ntop(AF_INET6, &addr->sin6_addr, host, INET6_ADDRSTRLEN) != NULL)
        {
            return host;
        }
    }
    return "";
}

void SocketAddress::Port(unsigned short port)
{
    if(this->ss_family == AF_INET)
    {
        struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(this);
        addr->sin_port = htons(port);
    }
    else if(this->ss_family == AF_INET6)
    {
        struct sockaddr_in6* addr = reinterpret_cast<struct sockaddr_in6*>(this);
        addr->sin6_port = htons(port);
    }
}

unsigned short SocketAddress::Port() const
{
    if(this->ss_family == AF_INET)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in*>(this)->sin_port);
    }
    if(this->ss_family == AF_INET6)
    {
        return ntohs(reinterpret_cast<const struct sockaddr_in6*>(this)->sin6_port);
    }
    return 0;
}

std::string SocketAddress::ToString() const
{    
    char namebuf[1024];
    namebuf[0] = '\0';
    getnameinfo(SockAddr(), Length(), namebuf, sizeof(namebuf), 0, 0, NI_NUMERICHOST);
    
    std::ostringstream oss;
    oss << namebuf << ":" << Port();
    return oss.str();
}

bool SocketAddress::operator==(const SocketAddress& a) const throw()
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

bool SocketAddress::operator<(const SocketAddress& a) const throw()
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

NET_BASE_END

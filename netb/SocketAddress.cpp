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
// no throw
SocketAddress::SocketAddress() noexcept
{
    Reset();
}

// Given port, 0 for any port
// Given family, by default AF_INET
// By default, local wildcard host (INADDR_ANY, or all 0)
// throw UnsupportedFamilyException, only when unsupported family is given
SocketAddress::SocketAddress(unsigned short port, sa_family_t family)
try
{
    Reset(family); // no except
    if(port > 0)
    {
        Port(port); // may throw exception
    }
}
catch(...)
{
    // rethrow by default
}


// Given port, 0 for any port
// Given family
// By default, local wildcard host (INADDR_ANY, or all 0)
// return error in Error, only when unsupported family is given
// no throw
SocketAddress::SocketAddress(unsigned short port, sa_family_t family, Error* e) noexcept
{
    Reset(family); // noexcept
    if(port > 0)
    {
        Port(port, e); // noexcept
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
// throw UnsupportedFamilyException, when unsupported family is given
// throw InvalidAddressException, when host is not a valid address in the family
// throw FamilyMismatchException, when host is in wrong format for the family
SocketAddress::SocketAddress(const std::string& host, unsigned short port, sa_family_t family)
try
{
    Reset(family); // noexcept
    if(host != "any" || host != "wildcard")
    {
        Host(host); // may throw
    }
    if(port > 0)
    {
        Port(port); // may throw
    }
}
catch(...)
{
    // rethrow by default
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
// return errors in Error, on all errors
// no throw
SocketAddress::SocketAddress(const std::string& host, unsigned short port, sa_family_t family, Error* e) noexcept
{
    Reset(family); // noexcept
    if(host != "any" || host != "wildcard")
    {
        Host(host, e); // may throw
    }
    if(!e && port > 0)
    {
        Port(port, e); // may throw
    }
}

SocketAddress::~SocketAddress() noexcept
{

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

// Set host
// throw UnsupportedFamilyException if unsupported family is set currently (including AF_UNSPEC)
// throw InvalidAddressException if host string is wrong
// throw FamilyMismatchException if host string is in wrong format
SocketAddress& SocketAddress::Host(const std::string& host)
{
    Error e;
    if(!Host(host, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

// no throw
bool SocketAddress::Host(const std::string& host, Error* e) noexcept
{
    int ret = 1; // for inet_pton
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
            ret = ::inet_pton(AF_INET, host.c_str(), &addrin->sin_addr.s_addr);
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
            addrin6->sin6_addr = in6addr_nodelocal_allnodes;
        else
        {
            ret = ::inet_pton(AF_INET6, host.c_str(), &addrin6->sin6_addr);
        }
    }
    else // unsupported family
    {
        std::ostringstream info;
        info << "Set host address but family is not supported [" << ToString() << "].";
        SET_ADDRESS_ERROR(e, info.str(), ErrorCode::UnsupportedFamily());
        return false;
    }
    // error on inet_pton
    if(ret != 1) 
    {
        if(ret == 0) // src does not contain a character string representing a valid network address in the specified address family.
        {
            // Todo: analyze the string to check if FamilyMismatchException
            std::ostringstream info;
            info << "Convert string to IP address but string not contain a valid IP address in the family [" << host << "] [" << ToString() << "]."; 
            SET_ADDRESS_ERROR(e, info.str(), 0);
            return false;
        }
        else if(ret == -1)
        {
            if(ErrorCode::IsUnsupportedFamily())
            {
                std::ostringstream info;
                info << "Convert string to IP address but family is supported [" << host << "] [" << ToString() << "].";
                SET_ADDRESS_ERROR(e, info.str(), ErrorCode::UnsupportedFamily());
                return false;
            }
        }
        else // unknown error
        {
            SET_ADDRESS_ERROR(e, "Convert string to IP address failed with unknown reason.", 0);
            return false;
        }  
    }
    return true;
}

// Set port, 0 for any port
// throw UnsupportedFamilyException if unsupported family is set currently (including AF_UNSPEC)
SocketAddress& SocketAddress::Port(unsigned short port)
{
    Error e;
    if(!Port(port, &e))
    {
        THROW_ERROR(e);
    }
    return *this;
}

// no throw
bool SocketAddress::Port(unsigned short port, Error* e) noexcept
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
        std::ostringstream info;
        info << "Set port but family is not supported [" << ToString() << "].";
        SET_ADDRESS_ERROR(e, info.str(), ErrorCode::UnsupportedFamily());
        return false;
    }
    return true;
}

// Get host
std::string SocketAddress::Host() const
{
    if(this->ss_family == AF_INET)
    {
        const struct sockaddr_in* addr = reinterpret_cast<const struct sockaddr_in*>(this);
        char host[INET_ADDRSTRLEN];
        host[0] = '\0';
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

// Get port
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

// String for the address
std::string SocketAddress::ToString() const
{    
    char namebuf[1024];
    namebuf[0] = '\0';
    getnameinfo(Addr(), Length(), namebuf, sizeof(namebuf), 0, 0, NI_NUMERICHOST);
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

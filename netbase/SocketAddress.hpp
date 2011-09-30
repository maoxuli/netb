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

#ifndef NET_BASE_SOCKET_ADDRESS_HPP
#define NET_BASE_SOCKET_ADDRESS_HPP

#include "Config.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <string>
#include <cstring>

NET_BASE_BEGIN

class SocketAddress
{
private:
	// Address is keped in sockaddr_storage
	// It is a 128 bytes long structure that is enough for any kind of socket address
	// ss_len is ignored in this implementation
	// The actual lenth of the adress is determined by ss_family
	sockaddr_storage mAddress;

public:
	// Reset the address to empty, thus ss_family is AF_UNSPEC
	void Reset() { memset(&mAddress, 0, sizeof(sockaddr_storage)); }

	// Address information
	bool Empty() const { return Family() == AF_UNSPEC; }
	sa_family_t Family() const;
	std::string Host() const;
	unsigned short Port() const;
	std::string ToString() const;

	// Socket APIs use struct sockaddr as socket address interface
	struct sockaddr* SockAddr();
	const struct sockaddr* SockAddr() const;

	// The actual lenth of current socket address, determined by ss_family
	// return the size of sockaddr_storage only if ss_family is AF_UNSPEC
	socklen_t SockAddrLen() const;

public: 
	// Empty address, all set to 0, AF_UNSPEC
	SocketAddress() throw() { memset(&mAddress, 0, sizeof(sockaddr_storage)); }

	// port is 0 for loopback address, otherwise wildcard address
	// Suitable for server address
	SocketAddress(unsigned short port, sa_family_t family = AF_INET) throw();

	// if host is null, loopback address
	// Suitable for client address
	SocketAddress(const char* host, unsigned short port, sa_family_t family = AF_INET) throw();

public: 
	// Copy constructor, and assigment operator, from other data structure
	SocketAddress(const SocketAddress& sa) throw() { memcpy(&mAddress, &sa.mAddress,sizeof(sockaddr_storage)); }
	SocketAddress(const SocketAddress* sa) throw() { memcpy(&mAddress, &sa->mAddress,sizeof(sockaddr_storage)); }

	SocketAddress(const struct sockaddr_storage& ss) throw() { *this = ss; }
	SocketAddress(const struct sockaddr_storage* ss) throw() { *this = ss; }
	SocketAddress(const struct sockaddr& sa) throw() { *this = sa; }
	SocketAddress(const struct sockaddr* sa) throw() { *this = sa; }
	SocketAddress(const struct sockaddr_in& sa) throw() { *this = sa; }
	SocketAddress(const struct sockaddr_in* sa) throw() { *this = sa; }
	SocketAddress(const struct sockaddr_in6& sa) throw() { *this = sa; }
	SocketAddress(const struct sockaddr_in6* sa) throw() { *this = sa; }

	inline SocketAddress& operator=(const SocketAddress& sa) throw()
	{
		if(&sa != this) 
			memcpy(&mAddress, &sa.mAddress, sizeof(struct sockaddr_storage));
		return *this;
	}

	inline SocketAddress& operator=(const SocketAddress* sa) throw()
	{
		if(sa != this) 
			memcpy(&mAddress, &sa->mAddress, sizeof(struct sockaddr_storage));
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_storage& ss) throw()
	{
		if(&ss != &mAddress)
			memcpy(&mAddress, &ss,sizeof(struct sockaddr_storage));
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_storage* ss) throw()
	{
		if(ss != &mAddress)
			memcpy(&mAddress, ss, sizeof(struct sockaddr_storage));
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			switch(sa.sa_family) 
			{
				case AF_INET:
					memcpy(&mAddress, &sa, sizeof(struct sockaddr_in));
					break;
				case AF_INET6:
					memcpy(&mAddress, &sa, sizeof(struct sockaddr_in6));
					break;
			}
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr* sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			switch(sa->sa_family) 
			{
				case AF_INET:
					memcpy(&mAddress, sa, sizeof(struct sockaddr_in));
					break;
				case AF_INET6:
					memcpy(&mAddress, sa, sizeof(struct sockaddr_in6));
					break;
			}
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, &sa, sizeof(struct sockaddr_in));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in* sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, sa, sizeof(struct sockaddr_in));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in6& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, &sa, sizeof(struct sockaddr_in6));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in6* sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, sa, sizeof(struct sockaddr_in6));
		}
		return *this;
	}

public:
	// Comparing operators
	bool operator==(const SocketAddress& a) const throw();
	bool operator<(const SocketAddress& a) const throw();
	inline bool operator!=(const SocketAddress& a) const throw() { return !(*this == a); }
	inline bool operator>(const SocketAddress& a) const throw() { return (a < *this); }
	inline bool operator<=(const SocketAddress& a) const throw() { return !(a < *this); }
	inline bool operator>=(const SocketAddress& a) const throw() { return !(*this < a); }
};

NET_BASE_END

#endif

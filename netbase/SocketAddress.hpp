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

#include "SocketConfig.hpp"

NET_BASE_BEGIN

//
// A socket address consists of protocol family, host address, and port 
// number, which can be represented with a general address container 
// struct sockaddr_storage. It supports all protocol families including 
// IPv4 and IPv6, and is convertable to and from particular address 
// containers like sockaddr_in, sockaddr_in6, and sockaddr.
//    
// SocketAddress is an wrapper of struct sockaddr_storage, so it is 
// compatible with socket API that using address reprensentations of 
// sockaddr, sockaddr_storage, sockaddr_in, sockaddr_in6, and so on. 
// 
// Relational operators (==, !=, <, <=, >, >=) are supported. However, 
// the results can not be interpreted into any special meaning, other 
// than that they are consistent. 
// 
class SocketAddress : public sockaddr_storage
{
public: 
	// Empty, for any protocols
	// no throw
	SocketAddress() noexcept;

	// Given port, 0 for any port
	// Given family, by default AF_INET
	// By default, local wildcard host (INADDR_ANY, or all 0)
	// throw UnsupportedFamilyException, only when unsupported family is given
	SocketAddress(unsigned short port, sa_family_t family = AF_INET);

	// Given port, 0 for any port
	// Given family
	// By default, local wildcard host (INADDR_ANY, or all 0)
	// return error in Error, only when unsupported family is given
	// no throw
	SocketAddress(unsigned short port, sa_family_t family, Error* e) noexcept;

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
	SocketAddress(const std::string& host, unsigned short port, sa_family_t family = AF_INET);
	
	// Given host, may be:
	// "any" or "": INADDR_ANY
	// "wildcard": INADDR_ANY
	// "none": INADDR_NONE
	// "loopback": INADDR_LOOPBACK
	// "localhost": INADDR_LOOPBACK
	// "broadcast": INADDR_NONE
	// "x.x.x.x" or "x:x:x:...": IPv4 or IPv6 address
	// Given port, 0 for any port
	// Given family, by default AF_INET
	// return errors in SocketError, on all errors
	// no throw
	SocketAddress(const std::string& host, unsigned short port, sa_family_t family, Error* e) noexcept;

	// Copy constructor
	// no throw
	SocketAddress(const SocketAddress& sa) noexcept { memcpy(this, &sa,sizeof(struct sockaddr_storage)); }
	SocketAddress(const SocketAddress* sa) noexcept { memcpy(this, sa,sizeof(struct sockaddr_storage)); }

	// Destructor
	~SocketAddress() noexcept;

	// Assignment operator
	inline SocketAddress& operator=(const SocketAddress& sa) noexcept
	{
		if(this != &sa) 
			memcpy(this, &sa, sizeof(struct sockaddr_storage));
		return *this;
	}

public:
	// ss_len in sockaddr_storage is ignore in this implementation for compatibility
	// The length of the address is determined by address family
	sa_family_t Family() const { return this->ss_family; }
	socklen_t Length() const;

	// SocketAddress is sockaddr_storage so below casting is allowed: 
	// SocketAddress addr;
	// sockaddr* sa = (sockaddr*)&addr;
	// sockaddr_in* sin = (sockaddr_in*)&addr;
	// sockaddr_in6* sin6 = (sockaddr_in6*)&addr;
	// Below functions are used for convenience only
	struct sockaddr* Addr() noexcept
	{
		return reinterpret_cast<struct sockaddr*>(this);
	}

	const struct sockaddr* Addr() const
	{
		return reinterpret_cast<const struct sockaddr*>(this);
	}

	// Reset the object with given family, by default AF_UNSPEC, i.e., empty
	// Support user defined family
	// no throw
	SocketAddress& Reset(sa_family_t family = AF_UNSPEC) noexcept
	{ 
		memset(this, 0, sizeof(struct sockaddr_storage)); 
		ss_family = family; 
		return *this;
	}

	bool Empty() const { return ss_family == AF_UNSPEC; }

	// Set host
	// throw UnsupportedFamilyException if unsupported family is set currently (including AF_UNSPEC)
	// throw InvalidAddressException if host string is wrong
	// throw FamilyMismatchException if host string is in wrong format
	SocketAddress& Host(const std::string& host);
	bool Host(const std::string& host, Error* e) noexcept;

	// Set port, 0 for any port
	// throw UnsupportedFamilyException if unsupported family is set currently (including AF_UNSPEC)
	SocketAddress& Port(unsigned short port);
	bool Port(unsigned short port, Error* e) noexcept;

	// Get host and port
	std::string Host() const;
	unsigned short Port() const;

	// Check special address
	bool Wildcard() const; // INADDR_ANY:0
	bool Any() const; // INADDR_ANY:0
	bool AnyPort() const; // 0
	bool AnyHost() const; // INADDR_ANY
	bool Localhost() const; // "localhost"
	bool Loopback() const; // INADDR_LOOPBACK
	bool Broadcast() const; // INADDR_NONE
	bool Multicast() const; // 224.0.0.0 to 239.255.255.255

	// Output the address to string
	std::string ToString() const;

public: 
	// Copy counstructor, from data structures used in socket address
	// Throw UnsupportedFamilyException if the family is not supported
	SocketAddress(const struct sockaddr& sa) { *this = sa; }
	explicit SocketAddress(const struct sockaddr* sa) { *this = *sa; }

	SocketAddress(const struct sockaddr_in& sa) noexcept { *this = sa; }
	explicit SocketAddress(const struct sockaddr_in* sa) noexcept { *this = *sa; }

	SocketAddress(const struct sockaddr_in6& sa) noexcept { *this = sa; }
	explicit SocketAddress(const struct sockaddr_in6* sa) noexcept { *this = *sa; }
	
	SocketAddress(const struct sockaddr_storage& sa) noexcept { *this = sa; }
	explicit SocketAddress(const struct sockaddr_storage* sa) noexcept { *this = *sa; }

	// Assignment operator, from data structures used in socket address
	// Throw UnsupportedFamilyException if the family is not supported
	inline SocketAddress& operator=(const struct sockaddr& sa)
	{
		if(reinterpret_cast<const struct sockaddr*>(this) != &sa) 
		{
			memset(this, 0, sizeof(struct sockaddr_storage));
			switch(sa.sa_family) 
			{
				case AF_INET:
					memcpy(this, &sa, sizeof(struct sockaddr_in));
					break;
				case AF_INET6:
					memcpy(this, &sa, sizeof(struct sockaddr_in6));
					break;
				default:
					throw AddressException("Unsupported family.", ErrorCode::UnsupportedFamily());
			}
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in& sa) noexcept
	{
		if(reinterpret_cast<const struct sockaddr_in*>(this) != &sa) 
		{
			memset(this, 0, sizeof(struct sockaddr_storage));
			memcpy(this, &sa, sizeof(struct sockaddr_in));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in6& sa) noexcept
	{
		if(reinterpret_cast<const struct sockaddr_in6*>(this) != &sa) 
		{
			memset(this, 0, sizeof(struct sockaddr_storage));
			memcpy(this, &sa, sizeof(struct sockaddr_in6));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_storage& sa) noexcept
	{
		if(this != &sa)
			memcpy(this, &sa,sizeof(struct sockaddr_storage));
		return *this;
	}

public:
	// Comparing operators, necessary for values in collection objects
	bool operator==(const SocketAddress& a) const noexcept;
	bool operator<(const SocketAddress& a) const noexcept;
	inline bool operator!=(const SocketAddress& a) const noexcept { return !(*this == a); }
	inline bool operator>(const SocketAddress& a) const noexcept { return (a < *this); }
	inline bool operator<=(const SocketAddress& a) const noexcept { return !(a < *this); }
	inline bool operator>=(const SocketAddress& a) const noexcept { return !(*this < a); }
};

NET_BASE_END

#endif

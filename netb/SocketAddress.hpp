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

#ifndef NETB_SOCKET_ADDRESS_HPP
#define NETB_SOCKET_ADDRESS_HPP

#include "SocketConfig.hpp"

NETB_BEGIN

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
	// Empty, to be initialized later
	// no throw
	SocketAddress() noexcept;

	// Given port and protocol family
	// Usually used for service address
	// Host is not given, set to local wildcard host (INADDR_ANY) by default
	// If given port is 0, indicate any port
	// If given family is not supported, throw UnsupportedFamilyException
	SocketAddress(unsigned short port, sa_family_t family = AF_INET); // throw on errors
	SocketAddress(unsigned short port, sa_family_t family, Error* e) noexcept;

	// Given host, port, and protocol family
	// Usually used for client to set service address
	// If host is not given (be empty), set to localhost (INADDR_LOOPBACK) by default
	// Geven host:
	// "" or "localhost": INADDR_LOOPBACK
	// "loopback": INADDR_LOOPBACK
	// "wildcard" or "any": INADDR_ANY
	// "broadcast" or "none": INADDR_NONE
	// "x.x.x.x" or "x:x:x:...": IPv4 or IPv6 address
	// Given port: 0 for any port
	// Given family: by default AF_INET
	// If given family is not supported, throw UnsupportedFamilyException
	// If given host is not a valid format, throw InvalidAddressException
	// If given host is not in the family, throw FamilyMismatchException
	SocketAddress(const std::string& host, unsigned short port, sa_family_t family = AF_INET);
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
	// Family of the address
	sa_family_t Family() const noexcept { return this->ss_family; }

	// ss_len in sockaddr_storage is ignore in this implementation for compatibility
	// The length of the address is determined by address family
	// If current family is not supported, throw UnsupportedFamilyException
	socklen_t Length() const;
	socklen_t Length(Error* e) const noexcept;

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

	const struct sockaddr* Addr() const noexcept
	{
		return reinterpret_cast<const struct sockaddr*>(this);
	}

	// Reset the object with given family, by default AF_UNSPEC, i.e., empty
	// no throw
	SocketAddress& Reset(sa_family_t family = AF_UNSPEC) noexcept
	{ 
		memset(this, 0, sizeof(struct sockaddr_storage)); 
		ss_family = family; 
		return *this;
	}

	// return true if the address is empty now
	bool Empty() const noexcept { return ss_family == AF_UNSPEC; }

	// Set host
	// If given host is not a valid format, throw InvalidAddressException
	// If given host is not in the family, throw FamilyMismatchException
	// If given host is in family unsupported, throw UnsupportedFamilyException
	SocketAddress& Host(const std::string& host);
	bool Host(const std::string& host, Error* e) noexcept;

	// Set port, 0 for any port
	// If current address family is not supported, throw UnsupportedFamilyException
	SocketAddress& Port(unsigned short port);
	bool Port(unsigned short port, Error* e) noexcept;

	// Get host of current address
	// If address family is not supported, throw UnsupportedFamilyException
	std::string Host() const;
	std::string Host(Error* e) const noexcept;

	// Get port of current address
	// If address family is not supported, throw UnsupportedFamilyException
	unsigned short Port() const;
	unsigned short Port(Error* e) const noexcept;

	// Check special address
	// If address family is not supported, throw UnsupportedFamilyException
	bool Wildcard(Error* e = NULL) const noexcept; // INADDR_ANY:0
	bool Any(Error* e = NULL) const noexcept; // INADDR_ANY:0
	bool AnyPort(Error* e = NULL) const noexcept; // 0
	bool AnyHost(Error* e = NULL) const noexcept; // INADDR_ANY
	bool Localhost(Error* e = NULL) const noexcept; // "localhost"
	bool Loopback(Error* e = NULL) const noexcept; // INADDR_LOOPBACK
	bool Broadcast(Error* e = NULL) const noexcept; // INADDR_NONE
	bool Multicast(Error* e = NULL) const noexcept; // 224.0.0.0 to 239.255.255.255

	// Output the address to string
	std::string ToString(Error* e = NULL) const noexcept;

public: 
	// Copy counstructor, from data structures used in socket address
	// Throw UnsupportedFamilyException if the family is not supported
	SocketAddress(const struct sockaddr& sa) noexcept { *this = sa; }
	explicit SocketAddress(const struct sockaddr* sa) noexcept { *this = *sa; }

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
			memcpy(this, &sa, sizeof(struct sockaddr_storage));
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

NETB_END

#endif

/*
 * Copyright (C) 2016, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NETB_DNS_RESOLVER_HPP
#define NETB_DNS_RESOLVER_HPP

#include "DnsMessage.hpp" 
#include "UdpSocket.hpp"

#define DEFAULT_DNS_HOST    	"8.8.8.8"
#define DEFAULT_DNS_PORT    	53
#define DEFAULT_RETRY_TIMES 	10
#define DEFAULT_SOCKET_TIMEOUT	200 // ms
#define MAX_DNS_PACKET_SIZE 	512

NETB_BEGIN

namespace dns {
	  
// Synchronous DNS resolver
class Resolver
{        
public:
    // Using default settings for DNS service
    Resolver(); 

    // Given DNS server address
    Resolver(const std::string& host, unsigned short port = DEFAULT_DNS_PORT, sa_family_t family = AF_INET);

    // Destructor 
    virtual ~Resolver();

    // Synchronous resolving       
    bool Resolve(const std::string& name, Question::QTYPE qtype, Response* response, Error* e = 0);
    bool Resolve(Query& query, Response* response, Error* e = 0);
    
private:
    // Socket 
	netb::UdpSocket _socket;
    netb::SocketAddress _address;
};

} // namespace dns

NETB_END

#endif

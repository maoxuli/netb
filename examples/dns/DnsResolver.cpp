/*
 * Copyright (C) 2017, Maoxu Li. http://maoxuli.com/dev
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

#include "DnsResolver.h"
#include "RandomReader.hpp"

NETB_BEGIN

namespace dns {

Resolver::Resolver()
: _address(DEFAULT_DNS_HOST, DEFAULT_DNS_PORT)
{

}

Resolver::Resolver(const std::string& host, unsigned short port, sa_family_t family)
: _address(host, port, family)
{

}

Resolver::~Resolver()
{

}

bool Resolver::Resolve(const std::string& name, Question::QTYPE qtype, Response* response, Error* e)
{
    Query query(name, (unsigned short)qtype);
    return Resolve(query, response, e);
}

bool Resolver::Resolve(Query& query, Response* response, Error* e)
{
    assert(response);
    // Display the query for debug
    std::cout << query.String() << std::endl;
    // Packing the query message into buffer
    StreamBuffer buf;
    if(!query.ToBuffer(&buf))
    {
        SET_ERROR(e, "Packing query into buffer failed", 0);
        return false;
    } 
    // Send query and receive response
    if(_socket.SendTo(buf, _address, e) <= 0)
    {
        return false;
    }
    assert(buf.Readable() == 0);
    buf.Clear();
    if(_socket.ReceiveFrom(&buf, 0, e) <= 0)
    {
        return false;
    }
    if(!response->FromBuffer(&buf))
    {
        SET_ERROR(e, "Unpacking reponse from buffer failed", 0);
        return false;
    }
    return true;
}

} // namespace dns 

NETB_END 

//
// DNS Resovlver Test
//
int main (int argc, const char * argv[])
{
    std::cout <<
    "//\n"
    "// DNS Resolver, v0.2\n"
    "// Copyright 2016, Maoxu Li.\n"
    "//\n"
    "\n";
    
    if(argc < 2)
    {
        std::cout <<
        "// Please input a domain name as agument.\n"
        "// Such as: dns google.com\n";
        return 0;
    }
    
    // Target domain name
    std::string domain = argv[1];

    // Synchronous DNS resovler
    netb::Error e;
    netb::dns::Resolver resolver;

    // A record
    netb::dns::Response respA;
    if(!resolver.Resolve(domain, netb::dns::Question::QTYPE::A, &respA, &e)) 
    {
        std::cout << e.Report() << std::endl;
    }
    std::cout << respA.String() << std::endl;
    
    // CNAME record
    netb::dns::Response respCNAME;
    if(!resolver.Resolve(domain, netb::dns::Question::QTYPE::CNAME, &respCNAME, &e)) 
    {
        std::cout << e.Report() << std::endl;
    }
    std::cout << respCNAME.String() << std::endl;

    // MX record
    netb::dns::Response respMX;
    if(!resolver.Resolve(domain, netb::dns::Question::QTYPE::MX, &respMX, &e)) 
    {
        std::cout << e.Report() << std::endl;
    }
    std::cout << respMX.String() << std::endl;

    // TXT record
    netb::dns::Response respTXT;
    if(!resolver.Resolve(domain, netb::dns::Question::QTYPE::TXT, &respTXT, &e)) 
    {
        std::cout << e.Report() << std::endl;
    }
    std::cout << respTXT.String() << std::endl;

    return 0;
}
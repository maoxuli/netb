/*
 * Copyright (C) 2017 Maoxu Li. All rights reserved. maoxu@lebula.com
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

#ifndef NETB_DNS_MESSAGE_HPP
#define NETB_DNS_MESSAGE_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"
#include "DnsRecord.hpp"

NETB_BEGIN

namespace dns {

/*
RFC 1035    Domain Implementation and Specification    November 1987
4.1.1. Header section format

The header contains the following fields:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   OPCODE  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

ID              A 16 bit identifier assigned by the program that
                generates any kind of query.  This identifier is copied
                the corresponding reply and can be used by the requester
                to match up replies to outstanding queries.
	
QR              A one bit field that specifies whether this message is a
                query (0), or a response (1).
	
OPCODE          A four bit field that specifies kind of query in this
                message.  This value is set by the originator of a query
                and copied into the response.  The values are:

                0               a standard query (QUERY)
                1               an inverse query (IQUERY)
                2               a server status request (STATUS)
                3-15            reserved for future use

AA              Authoritative Answer - this bit is valid in responses,
                and specifies that the responding name server is an
                authority for the domain name in question section.

                Note that the contents of the answer section may have
                multiple owner names because of aliases.  The AA bit
                corresponds to the name which matches the query name, or
                the first owner name in the answer section.

TC              TrunCation - specifies that this message was truncated
                due to length greater than that permitted on the
                transmission channel.

RD              Recursion Desired - this bit may be set in a query and
                is copied into the response.  If RD is set, it directs
                the name server to pursue the query recursively.
                Recursive query support is optional.

RA              Recursion Available - this be is set or cleared in a
                response, and denotes whether recursive query support is
                available in the name server.

Z               Reserved for future use.  Must be zero in all queries
                and responses.

RCODE           Response code - this 4 bit field is set as part of
                responses.  The values have the following
                interpretation:

                0               No error condition
                1               Format error - The name server was
                                unable to interpret the query.
                2               Server failure - The name server was
                                unable to process this query due to a
                                problem with the name server.
                3               Name Error - Meaningful only for
                                responses from an authoritative name
                                server, this code signifies that the
                                domain name referenced in the query does
                                not exist.
                4               Not Implemented - The name server does
                                not support the requested kind of query.
                5               Refused - The name server refuses to
                                perform the specified operation for
                                policy reasons.  For example, a name
                                server may not wish to provide the
                                information to the particular requester,
                                or a name server may not wish to perform
                                a particular operation (e.g., zone
                                transfer) for particular data.
                6-15            Reserved for future use.

QDCOUNT         an unsigned 16 bit integer specifying the number of
                entries in the question section.

ANCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the answer section.

NSCOUNT         an unsigned 16 bit integer specifying the number of name
                server resource records in the authority records
                section.

ARCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the additional records section.
*/
	
class Header 
{
public:
    Header();
    ~Header();
	
    // Is the message is response
    bool Question() const { return _flags.qr == 0; }
    void Question(bool q);

    // Response Code
    enum class RCODE 
    {
        NO_ERROR			= 0, 
        FORMAT_ERROR 		= 1,
        SERVER_FAILURE		= 2,
        NAME_ERROR			= 3,
        NOT_IMPLEMENTED	    = 4,
        REFUSED			    = 5
    };

    // Response Code 
    unsigned char ResponseCode() const { return _flags.rcode; }
    
    // Get number of entries in following sections
    unsigned short QuestionCount() const { return _qdcount; }
    unsigned short AnswerCount() const { return _ancount; }
    unsigned short AuthorityCount() const { return _nscount; }
    unsigned short AdditionalCount() const { return _arcount; }

    // Set number of entries in following sections
    void QuestionCount(unsigned short n) { _qdcount = n; }
    void AnswerCount(unsigned short n) { _ancount = n; }
    void AuthorityCount(unsigned short n) { _nscount = n; }
    void AdditionalCount(unsigned short n) { _arcount = n; }

    // To string
    std::string String() const;

    // Serialization
    template <class T>
    bool Serialize(const T& stream)
    {
        return stream.Integer(_id) && 
               _flags.Serialize(stream) &&
               stream.Integer(_qdcount) && 
               stream.Integer(_ancount) && 
               stream.Integer(_nscount) && 
               stream.Integer(_arcount);
    }
                     
private:
    /*
    Flag fields in header
                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    */

    struct FLAGS {
        uint8_t qr;
        uint8_t opcode;
        uint8_t aa;
        uint8_t tc;
        uint8_t rd;
        uint8_t ra;
        uint8_t z;
        uint8_t rcode;

        FLAGS() : qr(0), opcode(0), aa(0), tc(0)
                , rd(0), ra(0), z(0), rcode(0)
        {

        }
        
        void Decode(uint8_t flags[])
        {
            qr = (flags[0] & 0x80) >> 7;
            opcode = (flags[0] & 0x78) >> 3;
            aa = (flags[0] & 0x04) >> 2;
            tc = (flags[0] & 0x02) >> 1;
            rd = flags[0] & 0x01;
            ra = (flags[1] & 0x80) >> 7;
            z = (flags[1] & 0x70) >> 4;
            rcode = flags[1] & 0x0f;
        }

        void Encode(uint8_t flags[])
        {
            flags[0] = rd | 
                       (tc << 1) | 
                       (aa << 2) |
                       (opcode << 3) |
                       (qr << 7);
            flags[1] = rcode | 
                       (z << 4) | 
                       (ra << 7);
        }

        bool Serialize(const StreamWriter& stream)
        {
            uint8_t flags[2];
            Encode(flags);
            if(!stream.Integer(flags[0])) return false;
            if(!stream.Integer(flags[1])) return false;
            return true;
        }

        bool Serialize(const StreamReader& stream)
        {
            uint8_t flags[2];
            if(!stream.Integer(flags[0])) return false;
            if(!stream.Integer(flags[1])) return false;
            Decode(flags);
            return true;
        }
    };

    // Header Fields
    uint16_t _id;       // ID
    FLAGS    _flags;    // QR, OPCODE, AA, TC, RD, RA, Z, RCODE
    uint16_t _qdcount;  // QDCOUND
    uint16_t _ancount;  // ANCOUNT
    uint16_t _nscount;  // NSCOUNT
    uint16_t _arcount;  // ARCOUND
};

/*
RFC 1035	Domain Implementation and Specification    November 1987
4.1.2. Question section format

The question section is used to carry the "question" in most queries,
i.e., the parameters that define what is being asked. The section
contains QDCOUNT (usually 1) entries, each of the following format:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

QNAME           a domain name represented as a sequence of labels, where
                each label consists of a length octet followed by that
                number of octets.  The domain name terminates with the
                zero length octet for the null label of the root.  Note
                that this field may be an odd number of octets; no
                padding is used.

QTYPE           a two octet code which specifies the type of the query.
                The values for this field include all codes valid for a
                TYPE field, together with some more general codes which
                can match more than one type of RR.

QCLASS          a two octet code that specifies the class of the query.
                For example, the QCLASS field is IN for the Internet.	
*/
	
class Question 
{
public:
    // Question Type
    // Superset of RECORD_TYPE 
    enum class QTYPE
    {
        A           =1, //a host address
        NS          =2, //an authoritative name server
        MD          =3, //a mail destination (Obsolete - use MX)
        MF          =4, //a mail forwarder (Obsolete - use MX)
        CNAME       =5, //the canonical name for an alias
        SOA         =6, //marks the start of a zone of authority
        MB          =7, //a mailbox domain name (EXPERIMENTAL)
        MG          =8, //a mail group member (EXPERIMENTAL)
        MR          =9, //a mail rename domain name (EXPERIMENTAL)
        DUMMY       =10, //a null RR (EXPERIMENTAL)
        WKS         =11, //a well known service description
        PTR         =12, //a domain name pointer
        HINFO       =13, //host information
        MINFO       =14, //mailbox or mail list information
        MX          =15, //mail exchange
        TXT         =16, //text strings
        AXFR        =252, //A request for a transfer of an entire zone
        MAILB       =253, //A request for mailbox-related records (MB, MG or MR)
        MAILA       =254, //A request for mail agent RRs (Obsolete - see MX)
        WILDCARD    =255 // * A request for all records
    };

    // Question Class 
    // Superset of RECORD_CLASS
    enum class QCLASS 
    {
        IN          =1, //the Internet
        CS          =2, //the CSNET class (Obsolete)
        CH          =3, //the CHAOS class
        HS          =4, //Hesiod [Dyer 87]
        WILDCARD    =255 //* any class
    };

public:
    Question();
    Question(const std::string& name, unsigned short qtype, unsigned short qclass = (unsigned short)QCLASS::IN);
    Question(const DomainName& name, unsigned short qtype, unsigned short qclass = (unsigned short)QCLASS::IN);
    ~Question();
    
    // To string
    std::string String() const;
    
    // Serialization
    template <class T>
    bool Serialize(const T& stream)
    {
        return _name.Serialize(stream) && 
               stream.Integer(_type) &&
               stream.Integer(_class);
    }

    
private:
    DomainName _name;
    uint16_t _type;
    uint16_t _class;
}; 

	 
/*
RFC 1035    Domain Implementation and Specification    November 1987
4.1. Format

All communications inside of the domain protocol are carried in a single
format called a message.  The top level format of message is divided
into 5 sections (some of which are empty in certain cases) shown below:

    +---------------------+
    |        Header       |
    +---------------------+
    |       Question      | the question for the name server
    +---------------------+
    |        Answer       | RRs answering the question
    +---------------------+
    |      Authority      | RRs pointing toward an authority
    +---------------------+
    |      Additional     | RRs holding additional information
    +---------------------+	
*/

class Message : private Uncopyable
{
public:
    virtual ~Message();
	
    // String format of the message 
    std::string String() const;

    // Pack to buffer 
    bool ToBuffer(StreamBuffer* buf);

    // Unpack from buffer
    bool FromBuffer(StreamBuffer* buf);
    
protected:
    // Header Section
    Header _header;
    
    // Question Section
    std::vector<Question*> _questions;
    
    // Answers Section
    std::vector<ResourceRecord*> _answers;

    // Authority Section
    std::vector<ResourceRecord*> _authorities;
    
    // Additional Section
    std::vector<ResourceRecord*> _additionals;
};

// DNS Query Message 
// Extend setting interface for DNS query 
class Query : public Message 
{
public:
    Query();
    Query(const std::string& name, unsigned short qtype); 
    virtual ~Query();

};

// DNS Response Message 
// Extend setting interface for DNS response
class Response : public Message 
{
public: 
    Response();
    virtual ~Response();

};

} // namespace dns

NETB_END

#endif

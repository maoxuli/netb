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

#ifndef NETB_DNS_RECORD_HPP
#define NETB_DNS_RECORD_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"
#include "StreamWriter.hpp"
#include "StreamReader.hpp"

NETB_BEGIN 
	
namespace dns {

/*
RFC 1035	Domain Implementation and Specification    November 1987

3.1. Name space definitions

Domain names in messages are expressed in terms of a sequence of labels.
Each label is represented as a one octet length field followed by that
number of octets.  Since every domain name ends with the null label of
the root, a domain name is terminated by a length byte of zero.  The
high order two bits of every length octet must be zero, and the
remaining six bits of the length field limit the label to 63 octets or
less.

To simplify implementations, the total length of a domain name (i.e.,
label octets and label length octets) is restricted to 255 octets or
less.

Although labels can contain any 8 bit values in octets that make up a
label, it is strongly recommended that labels follow the preferred
syntax described elsewhere in this memo, which is compatible with
existing host naming conventions.  Name servers and resolvers must
compare labels in a case-insensitive manner (i.e., A=a), assuming ASCII
with zero parity.  Non-alphabetic codes must match exactly.

4.1.4. Message compression

In order to reduce the size of messages, the domain system utilizes a
compression scheme which eliminates the repetition of domain names in a
message.  In this scheme, an entire domain name or a list of labels at
the end of a domain name is replaced with a pointer to a prior occurance
of the same name.

The pointer takes the form of a two octet sequence:

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    | 1  1|                OFFSET                   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

The first two bits are ones.  This allows a pointer to be distinguished
from a label, since the label must begin with two zero bits because
labels are restricted to 63 octets or less.  (The 10 and 01 combinations
are reserved for future use.)  The OFFSET field specifies an offset from
the start of the message (i.e., the first octet of the ID field in the
domain header).  A zero offset specifies the first byte of the ID field,
etc.

The compression scheme allows a domain name in a message to be
represented as either:

   - a sequence of labels ending in a zero octet
   - a pointer
   - a sequence of labels ending with a pointer
*/
    
class DomainName 
{
public:    
    DomainName();
    DomainName(const std::string& s);

    // from string
    bool FromString(const std::string& s);

    // to string
    std::string String() const;

    // packing
    // return the coded data length
    bool Serialize(const StreamWriter& writer, unsigned short* wlen = 0);

    // unpacking
    // return the coded data length
    bool Serialize(const StreamReader& reader, unsigned short* dlen = 0);
    
private:
    // name parts
    std::vector<std::string> _parts;  

    // valid chars
    static const char* s_valid_chars;
};

/*
3.2.2. TYPE values

TYPE fields are used in resource records.  Note that these types are a
subset of QTYPEs.

A               1 a host address
NS              2 an authoritative name server
MD              3 a mail destination (Obsolete - use MX)
MF              4 a mail forwarder (Obsolete - use MX)
CNAME           5 the canonical name for an alias
SOA             6 marks the start of a zone of authority
MB              7 a mailbox domain name (EXPERIMENTAL)
MG              8 a mail group member (EXPERIMENTAL)
MR              9 a mail rename domain name (EXPERIMENTAL)
NULL            10 a null RR (EXPERIMENTAL)
WKS             11 a well known service description
PTR             12 a domain name pointer
HINFO           13 host information
MINFO           14 mailbox or mail list information
MX              15 mail exchange
TXT             16 text strings
*/
	
enum class RECORD_TYPE 
{
    A       =1, //a host address
    NS      =2, //an authoritative name server
    MD      =3, //a mail destination (Obsolete - use MX)
    MF      =4, //a mail forwarder (Obsolete - use MX)
    CNAME   =5, //the canonical name for an alias
    SOA     =6, //marks the start of a zone of authority
    MB      =7, //a mailbox domain name (EXPERIMENTAL)
    MG      =8, //a mail group member (EXPERIMENTAL)
    MR      =9, //a mail rename domain name (EXPERIMENTAL)
    DUMMY   =10, //a null RR (EXPERIMENTAL)
    WKS     =11, //a well known service description
    PTR     =12, //a domain name pointer
    HINFO   =13, //host information
    MINFO   =14, //mailbox or mail list information
    MX      =15, //mail exchange
    TXT     =16  //text strings
};

/*
3.2.4. CLASS values

CLASS fields appear in resource records.  The following CLASS mnemonics
and values are defined:

IN              1 the Internet
CS              2 the CSNET class (Obsolete - used only for examples in
                some obsolete RFCs)
CH              3 the CHAOS class
HS              4 Hesiod [Dyer 87]
*/

enum class RECORD_CLASS 
{
    IN      =1, //the Internet
    CS      =2, //the CSNET class (Obsolete)
    CH      =3, //the CHAOS class
    HS      =4  //Hesiod [Dyer 87]
};

/*
RFC 1035	Domain Implementation and Specification    November 1987
4.1.3. Resource record format

The answer, authority, and additional sections all share the same
format: a variable number of resource records (RRs), where the number of
records is specified in the corresponding count field in the header.
Each resource record has the following format:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                                               /
    /                      NAME                     /
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     CLASS                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TTL                      |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                   RDLENGTH                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
    /                     RDATA                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

NAME            a domain name to which this resource record pertains.

TYPE            two octets containing one of the RR type codes.  This
                field specifies the meaning of the data in the RDATA
                field.

CLASS           two octets which specify the class of the data in the
                RDATA field.

TTL             a 32 bit unsigned integer that specifies the time
                interval (in seconds) that the resource record may be
                cached before it should be discarded.  Zero values are
                interpreted to mean that the RR can only be used for the
                transaction in progress, and should not be cached.

RDLENGTH        an unsigned 16 bit integer that specifies the length in
                octets of the RDATA field.

RDATA           a variable length string of octets that describes the
                resource.  The format of this information varies
                according to the TYPE and CLASS of the resource record.
                For example, the if the TYPE is A and the CLASS is IN,
                the RDATA field is a 4 octet ARPA Internet address.
	
*/

// Base class for various types of Record Data 
// the rdlength is managed by this object
class RecordData
{
public:
    virtual ~RecordData();

    // Get Type
    virtual unsigned short Type() const = 0;

    // To string
    virtual std::string String() const = 0;

    // Serialization to and from stream
    // rdlen is necessary for some types
    virtual bool Serialize(const StreamReader& reader, unsigned short rdlen) = 0;
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen) = 0;

    // Create object by TYPE
    static RecordData* Create(unsigned short type);
};

// Resource Record 
class ResourceRecord : private Uncopyable
{
public:
    ResourceRecord();
    ~ResourceRecord();
    
    // Type of record
    unsigned short Type() const;

    // To string
    std::string String() const;
    
    // Serialization
    bool Serialize(const StreamReader& reader);
    bool Serialize(const StreamWriter& writer);
                    
protected:
    DomainName  _name;  // NAME
    uint16_t    _type;  // TYPE
    uint16_t    _class; // CLASS
    uint32_t    _ttl;   // TTL
    uint16_t    _rdlen; // RDLENGTH
    RecordData* _rdata;  // RDATA
};

/*
RFC 1035	Domain Implementation and Specification    November 1987	
3.4.1. A RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ADDRESS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

ADDRESS         A 32 bit Internet address.

Hosts that have multiple Internet addresses will have multiple A
records.
*/

class ARecordData : public RecordData
{
public:
    virtual ~ARecordData();

    // Type
    virtual unsigned short Type() const { return (unsigned short)RECORD_TYPE::A; }

    // To String
    virtual std::string String() const;

    // Serialization to and from stream 
    virtual bool Serialize(const StreamReader& reader, unsigned short rdlen);
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen);
    
private:
    // RDATA of A record
    // IPv4 address, 4 bytes/32 bits number
    uint32_t _ip;
};

/*
RFC 1035	Domain Implementation and Specification    November 1987	
3.3.1. CNAME RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                     CNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

CNAME           A <domain-name> which specifies the canonical or primary
                name for the owner.  The owner name is an alias.	
*/

class CNAMERecordData : public RecordData 
{
public:
    virtual ~CNAMERecordData();

    // Get Type
    virtual unsigned short Type() const { return (unsigned short)RECORD_TYPE::CNAME; }
    
    // To String
    virtual std::string String() const;
    
    // Serialization to and from buffer
    virtual bool Serialize(const StreamReader& reader, unsigned short rdlen);
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen);

private:
    // RDATA is the A name refered by the alias
    DomainName _name; 
};

/*
RFC 1035	Domain Implementation and Specification    November 1987
3.3.13. SOA RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                     MNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                     RNAME                     /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    SERIAL                     |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    REFRESH                    |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     RETRY                     |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    EXPIRE                     |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    MINIMUM                    |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

MNAME           The <domain-name> of the name server that was the
                original or primary source of data for this zone.

RNAME           A <domain-name> which specifies the mailbox of the
                person responsible for this zone.

SERIAL          The unsigned 32 bit version number of the original copy
                of the zone.  Zone transfers preserve this value.  This
                value wraps and should be compared using sequence space
                arithmetic.

REFRESH         A 32 bit time interval before the zone should be
                refreshed.

RETRY           A 32 bit time interval that should elapse before a
                failed refresh should be retried.

EXPIRE          A 32 bit time value that specifies the upper limit on
                the time interval that can elapse before the zone is no
                longer authoritative.

MINIMUM         The unsigned 32 bit minimum TTL field that should be
                exported with any RR from this zone.

SOA records cause no additional section processing.
All times are in units of seconds.
*/

class SOARecordData : public RecordData
{
public:
    virtual ~SOARecordData();

    // Type
    virtual unsigned short Type() const { return (unsigned short)RECORD_TYPE::SOA; }

    // To String
    virtual std::string String() const;

    // Serialization to and from stream 
    virtual bool Serialize(const StreamReader& reader, unsigned short rdlen);
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen);
    
private:
    // RDATA of SOA record

};

/*
RFC 1035	Domain Implementation and Specification    November 1987
3.3.9. MX RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                  PREFERENCE                   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                   EXCHANGE                    /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

PREFERENCE      A 16 bit integer which specifies the preference given to
                this RR among others at the same owner.  Lower values
                are preferred.

EXCHANGE        A <domain-name> which specifies a host willing to act as
                a mail exchange for the owner name.
*/
	
class MXRecordData : public RecordData
{
public:
    virtual ~MXRecordData();

    // Get Type
    virtual unsigned short Type() const { return (unsigned short)RECORD_TYPE::MX; }

    // To String 
    virtual std::string String() const;

    // Serialization to and from buffer
    virtual bool Serialize(const StreamReader& stream, unsigned short rdlen);
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen);

private:
    // RDATA
    uint16_t _preference; // PREFERENCE, 2 bytes/16 bits integer
    DomainName _exchange; // EXCHANGE, domain name
};

/*
RFC 1035	Domain Implementation and Specification    November 1987
3.3.14. TXT RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                   TXT-DATA                    /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

TXT-DATA        One or more <character-string>s.

TXT RRs are used to hold descriptive text.  The semantics of the text
depends on the domain where it is found.	

3.3. Standard RRs

........................................<character-string> is a single
length octet followed by that number of characters.  <character-string>
is treated as binary information, and can be up to 256 characters in
length (including the length octet).
*/

class TXTRecordData : public RecordData 
{
public:
    virtual ~TXTRecordData();

    // Get Type
    virtual unsigned short Type() const { return (unsigned short)RECORD_TYPE::TXT; }

    // To string
    virtual std::string String() const;

    // Serialization to and from buffer
    virtual bool Serialize(const StreamReader& reader, unsigned short rdlen);
    virtual bool Serialize(const StreamWriter& writer, unsigned short& rdlen);

private:
    // TXTs 
    std::string _text;
};

} // namespace dns

NETB_END

#endif

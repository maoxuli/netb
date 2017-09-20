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

#include "DnsRecord.hpp"
#include "RandomReader.hpp"
#include "RandomWriter.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

NETB_BEGIN

namespace dns {

ResourceRecord::ResourceRecord()
: _type(0)
, _class(0)
, _ttl(0)
, _rdlen(0)
, _rdata(0)
{
    
}

ResourceRecord::~ResourceRecord()
{
    if(_rdata)
    {
        delete _rdata;
        _rdata = 0;
    }
}

unsigned short ResourceRecord::Type() const 
{
    return _type;
}

std::string ResourceRecord::String() const
{
    if(!_rdata) return "";
    return _rdata->String();  
}

bool ResourceRecord::Serialize(const StreamReader& reader)
{
    bool ret =  _name.Serialize(reader) && 
                reader.Integer(_type) &&
                reader.Integer(_class) &&
                reader.Integer(_ttl) &&
                reader.Integer(_rdlen);
    if(!ret) return false;

    if(!_rdata) _rdata = RecordData::Create(_type);
    if(!_rdata) return false;
    return _rdata->Serialize(reader, _rdlen);        
}

bool ResourceRecord::Serialize(const StreamWriter& writer)
{
    if(!_rdata) return false;
    bool ret = _name.Serialize(writer) && 
               writer.Integer(_type) &&
               writer.Integer(_class) &&
               writer.Integer(_ttl);
    if(!ret) return false;

    size_t offset = 0;
    if(!writer.Integer(_rdlen, &offset)) return false; 
    if(!_rdata->Serialize(writer, _rdlen)) return false;
    if(!RandomWriter(writer.Buffer()).Integer(offset, _rdlen)) return false;
    return true;    
}

////////////////////////////////////////////////////////////////////////////////////

const char* DomainName::s_valid_chars = "0123456789abcdefghijklmnopqrstuvwxyz-_/.";

DomainName::DomainName()
{

}

DomainName::DomainName(const std::string& s)
{
    FromString(s);
}

// from string
bool DomainName::FromString(const std::string& s)
{
    std::string name(s);
    // up to 256 character length
    if(name.length() > 256) return false;
    // to lower case 
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if(name.find_first_not_of(s_valid_chars, 0) != std::string::npos) return false;
    // Add terminate dot
    if(name[name.length() - 1] != '.') 
    {
        name += '.';
    }
    // Divided by dot        
    size_t opos = 0;
    size_t pos;
    while((pos = name.find_first_of(".", opos)) != std::string::npos)
    {
        std::string part;
        part.append(name, opos, pos - opos);
        _parts.push_back(part);
        opos = pos + 1;
    }
    return true;
}

// String format
std::string DomainName::String() const
{
    std::ostringstream oss;
    for(auto it = _parts.begin(); it != _parts.end(); ++it)
    {
        oss << *it << ".";
    }
    return oss.str();
}

// packing
bool DomainName::Serialize(const StreamWriter& writer, unsigned short* wlen)
{
    for(auto it = _parts.begin(); it != _parts.end(); ++it)
    {
        std::string& s = *it;
        if(!s.empty() && s != ".")
        {
            if(!writer.Integer((uint8_t)s.length()) || 
               !writer.String(s))
            {
                return false;
            }
            if(wlen) *wlen += 1 + s.length();
        }
    }
    if(!writer.Integer((uint8_t)0)) return false;
    if(wlen) *wlen += 1;
    return true;
}

// unpacking
bool DomainName::Serialize(const StreamReader& reader, unsigned short* rlen)
{
    uint8_t len;
    std::string s;
    while(true)
    {
        if(!reader.Integer(len)) return false;
        if(rlen) *rlen += 1;
        if(len == 0) return true; // termintate with 0 length
        if(len > 63) break; // pointer
        if(!reader.String(s, (size_t)len)) return false;
        if(rlen) *rlen += len;
        _parts.push_back(s);
    }
    assert(len > 63);
    if(len > 63) // pointer, only one is possible
    {
        uint8_t off;
        if(!reader.Integer(off)) return false;
        if(rlen) *rlen += 1;
        size_t offset = (len & 63 << 8) + off;
        RandomReader rr(reader.Buffer());
        while(true)
        {
            if(!rr.Integer(offset, len)) return false;
            offset += 1;
            if(len == 0) break;
            assert(len <= 63);
            if(len > 63) return false;
            if(!rr.String(offset, s, (size_t)len)) return false;
            offset += len;
            _parts.push_back(s);
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////

RecordData::~RecordData()
{

}

/*
enum class RECORD_TYPE 
{
    A               =1, //a host address
    NS              =2, //an authoritative name server
    MD              =3, //a mail destination (Obsolete - use MX)
    MF              =4, //a mail forwarder (Obsolete - use MX)
    CNAME           =5, //the canonical name for an alias
    SOA             =6, //marks the start of a zone of authority
    MB              =7, //a mailbox domain name (EXPERIMENTAL)
    MG              =8, //a mail group member (EXPERIMENTAL)
    MR              =9, //a mail rename domain name (EXPERIMENTAL)
    NULL            =10, //a null RR (EXPERIMENTAL)
    WKS             =11, //a well known service description
    PTR             =12, //a domain name pointer
    HINFO           =13, //host information
    MINFO           =14, //mailbox or mail list information
    MX              =15, //mail exchange
    TXT             =16 //text strings
};
*/

// Create RecordData object by RECORD_TYPE 
// Return NULL on failure 
RecordData* RecordData::Create(unsigned short type)
{
    RecordData* p = 0;
    switch(type)
    {
        case (unsigned short)RECORD_TYPE::A: p = new (std::nothrow) ARecordData(); break;
        case (unsigned short)RECORD_TYPE::CNAME: p = new (std::nothrow) CNAMERecordData(); break;
        case (unsigned short)RECORD_TYPE::SOA: p = new (std::nothrow) SOARecordData(); break;
        case (unsigned short)RECORD_TYPE::MX: p = new (std::nothrow) MXRecordData(); break;
        case (unsigned short)RECORD_TYPE::TXT: p = new (std::nothrow) TXTRecordData(); break;
        default: break;
    }
    return p;
}

//////////////////////////////////////////////////////////////////////////////

ARecordData::~ARecordData()
{
    
}

// To string format
std::string ARecordData::String() const 
{
    struct in_addr ia;
    ia.s_addr = _ip;
    std::string s("A:");
    s += inet_ntoa(ia);
    s += "\r\n";
    return s;
}

// Serialization to and from stream 
bool ARecordData::Serialize(const StreamReader& reader, unsigned short rdlen)
{
    if(rdlen != sizeof(uint32_t)) return false;
    return reader.Integer(_ip);
}

bool ARecordData::Serialize(const StreamWriter& writer, unsigned short& rdlen)
{
    rdlen = sizeof(uint32_t);
    return writer.Integer(_ip);
}


////////////////////////////////////////////////////////////////////////

CNAMERecordData::~CNAMERecordData()
{
    
}

std::string CNAMERecordData::String() const
{
    std::string s("CNAME:");
    s += _name.String();
    s += "\r\n";
    return s;
}

// Serialization to and from buffer
bool CNAMERecordData::Serialize(const StreamReader& reader, unsigned short rdlen)
{
    unsigned short len;
    if(!_name.Serialize(reader, &len)) return false;
    if(len != rdlen) return false;
    return true;
}

bool CNAMERecordData::Serialize(const StreamWriter& writer, unsigned short& rdlen)
{
    if(!_name.Serialize(writer, &rdlen)) return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////

SOARecordData::~SOARecordData()
{

}

std::string SOARecordData::String() const 
{
    return "SOA, not implemented.\n";
}

// Serialization to and from buffer
bool SOARecordData::Serialize(const StreamReader& reader, unsigned short rdlen)
{
    return true;
}

bool SOARecordData::Serialize(const StreamWriter& writer, unsigned short& rdlen)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////

MXRecordData::~MXRecordData()
{

}

std::string MXRecordData::String() const
{
    std::ostringstream oss;
    oss << "MX:" << _preference << ";" << _exchange.String() << "\r\n";
    return oss.str();
}

// Serialization to and from buffer
bool MXRecordData::Serialize(const StreamReader& reader, unsigned short rdlen)
{
    if(!reader.Integer(_preference)) return false;
    unsigned short len = 0;
    if(!_exchange.Serialize(reader, &len)) return false;
    if(rdlen != sizeof(uint16_t) + len) return false;
    return true;

}

bool MXRecordData::Serialize(const StreamWriter& writer, unsigned short& rdlen)
{
    if(!writer.Integer(_preference)) return false;
    rdlen += sizeof(uint16_t);
    unsigned short len = 0;
    if(!_exchange.Serialize(writer, &len)) return false;
    rdlen += len;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////

TXTRecordData::~TXTRecordData()
{
    
}

std::string TXTRecordData::String() const 
{
    std::string s("TXT:");
    s += _text;
    s += "\r\n";
    return s;
}

// Serialization to and from buffer
bool TXTRecordData::Serialize(const StreamReader& reader, unsigned short rdlen)
{
    uint8_t len;
    if(!reader.Integer(len)) return false;
    if(len != rdlen - 1) return false;
    if(!reader.String(_text, (size_t)len)) return false;
    return true;
}

bool TXTRecordData::Serialize(const StreamWriter& writer, unsigned short& rdlen)
{
    assert(_text.length() <= 256);
    if(!writer.Integer((uint8_t)_text.length())) return false;
    rdlen += sizeof(uint8_t);
    if(!writer.String(_text)) return false;
    rdlen += _text.length();
    return true;
}

} // namespace dns

NETB_END

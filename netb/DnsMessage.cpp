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

#include "DnsMessage.hpp"

NETB_BEGIN

namespace dns {

Message::~Message()
{
    for(Question* q : _questions)
    {
        delete q;
    }
    for(ResourceRecord* rr : _answers)
    {
        delete rr;
    }
    for(ResourceRecord* rr : _authorities)
    {
        delete rr;
    }
    for(ResourceRecord* rr : _additionals)
    {
        delete rr;
    }
}

// To string
std::string Message::String() const
{
    // Header section
    std::ostringstream oss;
    oss << "Header:\r\n" << _header.String();
    // Question section
    if(_questions.size() > 0) oss << "Question:\r\n";
    for(auto it = _questions.begin(); it != _questions.end(); ++it)
    {
        Question* p = *it;
        assert(p);
        oss << p->String();
    }
    // Answer section
    if(_answers.size() > 0) oss << "Answer:\r\n";
    for(auto it = _answers.begin(); it != _answers.end(); ++it)
    {
        ResourceRecord* p = *it;
        assert(p);
        oss << p->String();
    }
    // Authority section
    if(_authorities.size() > 0) oss << "Authority:\r\n";
    for(auto it = _authorities.begin(); it != _authorities.end(); ++it)
    {
        ResourceRecord* p = *it;
        assert(p);
        oss << p->String();
    }
    // Additional section
    if(_additionals.size() > 0) oss << "Additional:\r\n";
    for(auto it = _additionals.begin(); it != _additionals.end(); ++it)
    {
        ResourceRecord* p = *it;
        assert(p);
        oss << p->String() << "\n";
    }
    return oss.str();
}

// Pack to buffer 
// Note: this implementation did not compress the domain name. 
bool Message::ToBuffer(StreamBuffer* buf) 
{
    StreamWriter writer(buf);
    if(!_header.Serialize(writer)) return false;
    for(auto it = _questions.begin(); it != _questions.end(); ++it)
    {
        Question* p = *it;
        if(!p->Serialize(writer)) return false;
    }
    for(auto it = _answers.begin(); it != _answers.end(); ++it)
    {
        ResourceRecord* p = *it;
        if(!p->Serialize(writer)) return false;
    }
    for(auto it = _authorities.begin(); it != _authorities.end(); ++it)
    {
        ResourceRecord* p = *it;
        if(!p->Serialize(writer)) return false;
    }
    for(auto it = _additionals.begin(); it != _additionals.end(); ++it)
    {
        ResourceRecord* p = *it;
        if(!p->Serialize(writer)) return false;
    }
    return true;
}

// Unpack from buffer
// Note: the offset used in domain name compression is relative to the  
// begining of the message so unpacking must be done on all message data. 
bool Message::FromBuffer(StreamBuffer* buf)
{
    StreamReader reader(buf);
    if(!_header.Serialize(reader)) return false;
    for(int i = 0; i < _header.QuestionCount(); i++)
    {
        Question* p = new (std::nothrow) Question();
        if(!p || !p->Serialize(reader)) return false;
        _questions.push_back(p);
    }
    for(int i = 0; i < _header.AnswerCount(); i++)
    {
        ResourceRecord* p = new (std::nothrow) ResourceRecord();
        if(!p || !p->Serialize(reader)) return false;
        _answers.push_back(p);
    }
    for(int i = 0; i < _header.AuthorityCount(); i++)
    {
        ResourceRecord* p = new (std::nothrow) ResourceRecord();
        if(!p || !p->Serialize(reader)) return false;
        _authorities.push_back(p);
    }
    for(int i = 0; i < _header.AdditionalCount(); i++)
    {
        ResourceRecord* p = new (std::nothrow) ResourceRecord();
        if(!p || !p->Serialize(reader)) return false;
        _additionals.push_back(p);
    }
    buf->Flush();
    return true;
}

////////////////////////////////////////////////////////////////////////////

Query::Query()
{
    _header.Question(true);
}

Query::Query(const std::string& name, unsigned short qtype)
{
    _header.Question(true);
    Question* q = new (std::nothrow) Question(name, qtype);
    assert(q != NULL);
    if(q) _questions.push_back(q);
    _header.QuestionCount(_questions.size());
}

Query::~Query()
{

}

/////////////////////////////////////////////////////////////////////////////

Response::Response()
{
    
}

Response::~Response()
{

}

////////////////////////////////////////////////////////////////////////////

Header::Header()
: _id(0)
, _flags()
, _qdcount(0)
, _ancount(0)
, _nscount(0)
, _arcount(0)
{
    
}

Header::~Header()
{
    
}

void Header::Question(bool q)
{
    _flags.qr = q ? 0 : 1;
    if(q)
    {
        _id = rand() % (65535 + 1);
        _flags.rd = 1;
    }
}

std::string Header::String() const 
{
    std::ostringstream oss;
    oss << "QR:" << (int)_flags.qr << " " << "AA:" << (int)_flags.aa << " " 
        << "RD:" << (int)_flags.rd << " " << "RCODE:" << (int)_flags.rcode << "\r\n" 
        << "QDCOUNT:" << _qdcount << " " << "ANCOUNT:" << _ancount  << " "
        << "NSCOUNT:" << _nscount << " " << "ARCOUNT:" << _arcount << "\r\n";
    return oss.str();
}

////////////////////////////////////////////////////////////////////////////////

Question::Question()
: _type(0)
, _class(0)
{

}

Question::Question(const std::string& name, unsigned short qtype, unsigned short qclass)
: _name(name)
, _type(qtype)
, _class(qclass)
{

}

Question::Question(const DomainName& name, unsigned short qtype, unsigned short qclass)
: _name(name)
, _type(qtype)
, _class(qclass)
{
    
}

Question::~Question()
{
    
}

std::string Question::String() const
{
    std::ostringstream oss;
    oss << "NAME:" << _name.String() << " " <<  "QTYPE:" << _type << " " 
        << "CLASS:" << _class << "\r\n";
    return oss.str();
}

} // namespace dns

NETB_END

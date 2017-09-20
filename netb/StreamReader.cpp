/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#include "StreamReader.hpp"

NETB_BEGIN

StreamReader::StreamReader()
: _stream(0)
, _flush(false)
{

}

StreamReader::StreamReader(StreamBuffer* buf, bool flush)
: _stream(buf)
, _flush(flush)
{
    assert(buf);
}

StreamReader::StreamReader(StreamBuffer& buf, bool flush)
: _stream(&buf)
, _flush(flush)
{

}

StreamReader::~StreamReader()
{

}

StreamReader& StreamReader::Attach(StreamBuffer* buf, bool flush)
{
    assert(buf);
    _stream = buf;
    _flush = flush;
    return *this;
}

StreamReader& StreamReader::Attach(StreamBuffer& buf, bool flush)
{
    _stream = &buf;
    _flush = flush;
    return *this;
}

// Read n bytes
bool StreamReader::Bytes(void* p, size_t n) const
{
    if(!_stream) return false;
    if(_stream->Readable() < n) return false;
    if(!_stream->Read(p, n)) return false;
    if(_flush) _stream->Flush();
    return true;
}

// Read integer 
// integer with given type (with indicated length) 
// Todo: endianess concerns for multi-bytes types 
bool StreamReader::Integer(int8_t& v) const
{
    if(!_stream) return false;
    if(!_stream->Read(&v, sizeof(int8_t))) return false;
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(uint8_t& v) const
{
    if(!_stream) return false;
    if(!_stream->Read(&v, sizeof(uint8_t))) return false;
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(int16_t& v) const
{
    if(!_stream) return false;
    uint16_t nv;
    if(!_stream->Read(&nv, sizeof(uint16_t))) return false;
    v = ntohs(nv);
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(uint16_t& v) const
{
    if(!_stream) return false;
    uint16_t nv;
    if(!_stream->Read(&nv, sizeof(uint16_t))) return false;
    v = ntohs(nv);
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(int32_t& v) const
{
    if(!_stream) return false;
    uint32_t nv;
    if(!_stream->Read(&nv, sizeof(uint32_t))) return false;
    v = ntohl(nv);
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(uint32_t& v) const
{
    if(!_stream) return false;
    uint32_t nv;
    if(!_stream->Read(&nv, sizeof(uint32_t))) return false;
    v = ntohl(nv);
    if(_flush) _stream->Flush();
    return true;
}

bool StreamReader::Integer(int64_t& v) const
{
    assert(false);
    return false;
}

bool StreamReader::Integer(uint64_t& v) const
{
    assert(false);
    return false;
}

// Read string
// includes all readable data
bool StreamReader::String(std::string& s) const
{
    if(!_stream) return false;
    size_t n = _stream->Readable();
    if(n == 0) return false;
    return String(s, n);
}

// Read string 
// that is n bytes length
bool StreamReader::String(std::string& s, size_t n) const
{
    if(!_stream) return false;
    if(n > _stream->Readable()) return false;
    const char* p = (const char*)_stream->Read();
    std::string(p, p + n).swap(s);
    if(!_stream->Read(n)) return false;
    if(_flush) _stream->Flush();
    return true;
}

// Read string
// to delimit char ('\0')
bool StreamReader::String(std::string& s, const char delim) const
{
    if(!_stream) return false;
    ssize_t n = _stream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(s, (size_t)n);
}

// Read string 
// to delimit string ('\r\n')
bool StreamReader::String(std::string& s, const char* delim) const
{
    if(!_stream) return false;
    if(strlen(delim) > _stream->Readable()) return false;
    ssize_t n = _stream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(s, (size_t)n);
}

NETB_END

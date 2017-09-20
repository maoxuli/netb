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

#include "RandomReader.hpp"

NETB_BEGIN

RandomReader::RandomReader()
: _stream(0)
, _offset(0)
{

}

RandomReader::RandomReader(StreamBuffer* buf, size_t offset)
: _stream(buf)
, _offset(offset)
{
    assert(buf);
    assert(offset <= buf->Size());
}

RandomReader::RandomReader(StreamBuffer& buf, size_t offset)
: _stream(&buf)
, _offset(offset)
{
    assert(offset <= buf.Size());
}

RandomReader::~RandomReader()
{

}

RandomReader& RandomReader::Attach(StreamBuffer* buf, size_t offset)
{
    assert(buf);
    assert(offset <= buf->Size());
    _stream = buf;
    _offset = offset;
    return *this;
}

RandomReader& RandomReader::Attach(StreamBuffer& buf, size_t offset)
{
    assert(offset <= buf.Size());
    _stream = &buf;
    _offset = offset;
    return *this;
}

// Peek n bytes at offset position
bool RandomReader::Bytes(size_t offset, void* p, size_t n) const
{
    if(!_stream) return false;
    if(_offset + offset + n > _stream->Peekable()) return false;
    return _stream->Peek(_offset + offset, p, n);
}

// Peek integer at offset position
// with given integer type (with indicated length)
bool RandomReader::Integer(size_t offset, int8_t& v) const
{
    if(!_stream) return false;
    return _stream->Peek(offset, &v, sizeof(int8_t));
}

bool RandomReader::Integer(size_t offset, uint8_t& v) const
{
    if(!_stream) return false;

    char b[10];
    uint8_t n;
    _stream->Peek(offset, b, sizeof(uint8_t));
    _stream->Peek(offset, &n, sizeof(uint8_t));
    return _stream->Peek(offset, &v, sizeof(uint8_t));
}

bool RandomReader::Integer(size_t offset, int16_t& v) const
{
    if(!_stream) return false;
    uint16_t nv;
    if(!_stream->Peek(offset, &nv, sizeof(uint16_t))) return false;
    v = ntohs(nv);
    return true;
}

bool RandomReader::Integer(size_t offset, uint16_t& v) const
{
    if(!_stream) return false;
    uint16_t nv;
    if(!_stream->Peek(offset, &nv, sizeof(uint16_t))) return false;
    v = ntohs(nv);
    return true;
}

bool RandomReader::Integer(size_t offset, int32_t& v) const
{
    if(!_stream) return false;
    uint32_t nv;
    if(!_stream->Peek(offset, &nv, sizeof(uint32_t))) return false;
    v = ntohl(nv);
    return true;
}

bool RandomReader::Integer(size_t offset, uint32_t& v) const
{
    if(!_stream) return false;
    uint32_t nv;
    if(!_stream->Peek(offset, &nv, sizeof(uint32_t))) return false;
    v = ntohl(nv);
    return true;
}

bool RandomReader::Integer(size_t offset, int64_t& v) const
{
    assert(false);
    return false;
}

bool RandomReader::Integer(size_t offset, uint64_t& v) const
{
    assert(false);
    return false;
}

// Peek string
// to ending null or include all readable data
bool RandomReader::String(size_t offset, std::string& s) const
{
    if(!_stream) return false;
    if(_offset + offset > _stream->Peekable()) return false;
    ssize_t n = _stream->Peekable(_offset + offset);
    if(n == 0) return false;
    return String(offset, s, (size_t)n);
}

// Peek string 
// that is n bytes length
bool RandomReader::String(size_t offset, std::string& s, size_t n) const
{
    if(!_stream) return false;
    if(_offset + offset + n > _stream->Peekable()) return false;
    const char* p = (const char*)_stream->Peek(_offset + offset);
    std::string(p, p + n).swap(s);
    return true;
}

// Peek string
// to delimit characer (e.g. '\0')
bool RandomReader::String(size_t offset, std::string& s, const char delim) const
{
    if(!_stream) return false;
    if(_offset + offset + sizeof(delim) > _stream->Peekable()) return false;
    ssize_t n = _stream->Peekable(_offset + offset, delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(offset, s, (size_t)n);
}

// Peek string
// to delimit string (e.g. '\r\n')
bool RandomReader::String(size_t offset, std::string& s, const char* delim) const
{
    if(!_stream) return false;
    if(_offset + offset + strlen(delim) > _stream->Peekable()) return false;
    ssize_t n = _stream->Peekable(_offset + offset, delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(offset, s, (size_t)n);
}

NETB_END

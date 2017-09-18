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
bool RandomReader::Bytes(size_t offset, void* p, size_t n)
{
    if(!_stream) return false;
    if(_offset + offset + n > _stream->Peekable()) return false;
    return _stream->Peek(_offset + offset, p, n);
}

// Peek string
// to ending null or include all readable data
bool RandomReader::String(size_t offset, std::string& s)
{
    if(!_stream) return false;
    if(_offset + offset > _stream->Peekable()) return false;
    ssize_t n = _stream->Peekable(_offset + offset);
    if(n == 0) return false;
    return String(offset, s, (size_t)n);
}

// Peek string 
// that is n bytes length
bool RandomReader::String(size_t offset, std::string& s, size_t n)
{
    if(!_stream) return false;
    if(_offset + offset + n > _stream->Peekable()) return false;
    const char* p = (const char*)_stream->Peek(_offset + offset);
    std::string(p, p + n).swap(s);
    return true;
}

// Peek string
// to delimit characer (e.g. '\0')
bool RandomReader::String(size_t offset, std::string& s, const char delim)
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
bool RandomReader::String(size_t offset, std::string& s, const char* delim)
{
    if(!_stream) return false;
    if(_offset + offset + strlen(delim) > _stream->Peekable()) return false;
    ssize_t n = _stream->Peekable(_offset + offset, delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(offset, s, (size_t)n);
}

NETB_END

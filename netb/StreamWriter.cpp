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

#include "StreamWriter.hpp"

NETB_BEGIN

StreamWriter::StreamWriter()
: _stream(0)
{

}

StreamWriter::StreamWriter(StreamBuffer* buf, bool clear)
: _stream(buf)
{
    assert(buf);
    if(clear) _stream->Clear();
}

StreamWriter::StreamWriter(StreamBuffer& buf, bool clear)
: _stream(&buf)
{
    if(clear) _stream->Clear();
}

StreamWriter::~StreamWriter()
{

}

StreamWriter& StreamWriter::Attach(StreamBuffer* buf, bool clear)
{
    assert(buf);
    _stream = buf;
    if(clear) _stream->Clear();
    return *this;
}

StreamWriter& StreamWriter::Attach(StreamBuffer& buf, bool clear)
{
    _stream = &buf;
    if(clear) _stream->Clear();
    return *this;
}

bool StreamWriter::Bytes(const void* p, size_t n, size_t* offset) const
{
    if(!_stream) return false;
    return _stream->Write(p, n, offset);
}

// write integer 
// integer with given type (with indicated length) 
// Todo: endianess concerns for multi-bytes types 
bool StreamWriter::Integer(int8_t v, size_t* offset) const
{
    if(!_stream) return false;
    return _stream->Write(&v, sizeof(int8_t), offset);
}

bool StreamWriter::Integer(uint8_t v, size_t* offset) const
{
    if(!_stream) return false;
    return _stream->Write(&v, sizeof(uint8_t), offset);
}

bool StreamWriter::Integer(int16_t v, size_t* offset) const
{
    if(!_stream) return false;
    uint16_t nv = htons(*reinterpret_cast<uint16_t*>(&v));
    return _stream->Write(&nv, sizeof(uint16_t), offset);
}

bool StreamWriter::Integer(uint16_t v, size_t* offset) const
{
    if(!_stream) return false;
    uint16_t nv = htons(v);
    return _stream->Write(&nv, sizeof(uint16_t), offset);
}

bool StreamWriter::Integer(int32_t v, size_t* offset) const
{
    if(!_stream) return false;
    uint32_t nv = htonl(*reinterpret_cast<uint32_t*>(&v));
    return _stream->Write(&nv, sizeof(uint32_t), offset);
}

bool StreamWriter::Integer(uint32_t v, size_t* offset) const
{
    if(!_stream) return false;
    uint16_t nv = htonl(v);
    return _stream->Write(&nv, sizeof(uint16_t), offset);
}

bool StreamWriter::Integer(int64_t v, size_t* offset) const
{
    assert(false);
    return false;
}

bool StreamWriter::Integer(uint64_t v, size_t* offset) const
{
    assert(false);
    return false;
}

// write string
// all data in the string
bool StreamWriter::String(const std::string& s, size_t* offset) const
{
    if(!_stream) return false;
    return _stream->Write(s.data(), s.length(), offset);
}

// append ending null or not
// append delimit character (e.g. '\0')
bool StreamWriter::String(const std::string& s, const char delim, size_t* offset) const
{
    if(!_stream) return false;
    assert(s[s.length() - 1] != delim);
    std::string ss = s + delim;
    return _stream->Write(ss.data(), ss.length(), offset);
}

// write string
// append delimit string (e.g. "\r\n")
bool StreamWriter::String(const std::string& s, const char* delim, size_t* offset) const
{
    if(!_stream) return false;
    std::string ss = s + delim;
    return _stream->Write(ss.data(), ss.length(), offset);
}

NETB_END

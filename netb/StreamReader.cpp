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
{

}

StreamReader::StreamReader(StreamBuffer* buf, bool reset)
: _stream(buf)
{
    assert(buf);
    if(reset) buf->Reset();
}

StreamReader::StreamReader(StreamBuffer& buf, bool reset)
: _stream(&buf)
{
    if(reset) _stream->Reset();
}

StreamReader::~StreamReader()
{

}

StreamReader& StreamReader::Attach(StreamBuffer* buf, bool reset)
{
    assert(buf);
    _stream = buf;
    if(reset) _stream->Reset();
    return *this;
}

StreamReader& StreamReader::Attach(StreamBuffer& buf, bool reset)
{
    _stream = &buf;
    if(reset) _stream->Reset();
    return *this;
}

// Read n bytes
bool StreamReader::Bytes(void* p, size_t n)
{
    if(!_stream) return false;
    if(_stream->Readable() < n) return false;
    return _stream->Read(p, n);
}

// Read string
// includes all readable data
bool StreamReader::String(std::string& s)
{
    if(!_stream) return false;
    size_t n = _stream->Readable();
    if(n == 0) return false;
    return String(s, n);
}

// Read string 
// that is n bytes length
bool StreamReader::String(std::string& s, size_t n)
{
    if(!_stream) return false;
    if(n > _stream->Readable()) return false;
    const char* p = (const char*)_stream->Read();
    std::string(p, p + n).swap(s);
    _stream->Read(n);
    return true;
}

// Read string
// to delimit char ('\0')
bool StreamReader::String(std::string& s, const char delim)
{
    if(!_stream) return false;
    ssize_t n = _stream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(s, (size_t)n);
}

// Read string 
// to delimit string ('\r\n')
bool StreamReader::String(std::string& s, const char* delim)
{
    if(!_stream) return false;
    if(strlen(delim) > _stream->Readable()) return false;
    ssize_t n = _stream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    return String(s, (size_t)n);
}

NETB_END

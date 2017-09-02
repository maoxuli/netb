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

#include "StreamPeeker.hpp"
#include <cassert>

NETB_BEGIN

StreamPeeker::StreamPeeker()
: _stream(NULL)
, _offset(0)
{

}

StreamPeeker::StreamPeeker(StreamBuffer* buf)
: _stream(buf)
, _offset(0)
{

}

StreamPeeker::StreamPeeker(StreamBuffer& buf)
: _stream(&buf)
, _offset(0)
{

}

StreamPeeker::~StreamPeeker()
{

}

StreamPeeker& StreamPeeker::Attach(StreamBuffer* buf)
{
    _stream = buf;
    _offset = 0;
    return *this;
}

StreamPeeker& StreamPeeker::Attach(StreamBuffer& buf)
{
    _stream = &buf;
    _offset = 0;
    return *this;
}

bool StreamPeeker::Bytes(void* p, size_t n)
{
    if(_stream == NULL) return false;

    if(_stream->Peek(p, n, _offset))
    {
        _offset += n;
        return true;
    }
    return false;
}

bool StreamPeeker::String(std::string& s)
{
    if(_stream == NULL) return false;
    size_t n = _stream->Addressable(_offset);
    if(n == 0) return false;
    return String(s, n);
}

bool StreamPeeker::String(std::string& s, size_t n)
{
    if(_stream == NULL) return false;
    if(_stream->Addressable(_offset) < n) return false;

    const unsigned char* p = (const unsigned char*)_stream->Address(_offset);
    std::string(p, p + n).swap(s);
    _offset += n;
    return true;
}

bool StreamPeeker::String(std::string& s, const char* delim)
{
    if(_stream == NULL) return false;
    if(_stream->Addressable(_offset) < strlen(delim)) return false;

    ssize_t n = _stream->Addressable(delim, _offset);
    if(n < 0) return false;
    if(n == 0) 
    {
        _offset += strlen(delim);
        return true;
    }
    
    const unsigned char* p = (const unsigned char*)_stream->Address(_offset);
    std::string(p, p + n).swap(s);
    _offset += n + strlen(delim);
    return true;
}

NETB_END

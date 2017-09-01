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

NET_BASE_BEGIN

StreamPeeker::StreamPeeker()
: _stream(NULL)
, mOffset(0)
{

}

StreamPeeker::StreamPeeker(StreamBuffer* buf)
: _stream(buf)
, mOffset(0)
{

}

StreamPeeker::StreamPeeker(StreamBuffer& buf)
: _stream(&buf)
, mOffset(0)
{

}

StreamPeeker::~StreamPeeker()
{

}

StreamPeeker& StreamPeeker::Attach(StreamBuffer* buf)
{
    _stream = buf;
    mOffset = 0;
    return *this;
}

StreamPeeker& StreamPeeker::Attach(StreamBuffer& buf)
{
    _stream = &buf;
    mOffset = 0;
    return *this;
}

bool StreamPeeker::Bytes(void* p, size_t n)
{
    if(_stream == NULL) return false;

    if(_stream->Peek(p, n, mOffset))
    {
        mOffset += n;
        return true;
    }
    return false;
}

bool StreamPeeker::String(std::string& s, size_t n)
{
    if(_stream == NULL) return false;
    if(_stream->Addressable(mOffset) < n) return false;

    const unsigned char* p = (const unsigned char*)_stream->Address(mOffset);
    std::string(p, p + n).swap(s);
    mOffset += n;
    return true;
}

bool StreamPeeker::String(std::string& s, const char* delim)
{
    if(_stream == NULL) return false;
    if(_stream->Addressable(mOffset) < strlen(delim)) return false;

    ssize_t n = _stream->Addressable(delim, mOffset);
    if(n < 0) return false;
    if(n == 0) 
    {
        mOffset += strlen(delim);
        return true;
    }
    
    const unsigned char* p = (const unsigned char*)_stream->Address(mOffset);
    std::string(p, p + n).swap(s);
    mOffset += n + strlen(delim);
    return true;
}

NET_BASE_END

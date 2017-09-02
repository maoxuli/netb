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
#include <cassert>

NETB_BEGIN

StreamReader::StreamReader()
: _stream(NULL)
{

}

StreamReader::StreamReader(StreamBuffer* buf)
: _stream(buf)
{

}

StreamReader::StreamReader(StreamBuffer& buf)
: _stream(&buf)
{

}

StreamReader::~StreamReader()
{

}

StreamReader& StreamReader::Attach(StreamBuffer* buf)
{
    _stream = buf;
    return *this;
}

StreamReader& StreamReader::Attach(StreamBuffer& buf)
{
    _stream = &buf;
    return *this;
}

bool StreamReader::Bytes(void* p, size_t n)
{
    if(_stream == NULL) return false;
    if(_stream->Readable() < n) return false;

    return _stream->Read(p, n);
}

bool StreamReader::String(std::string& s)
{
    if(_stream == NULL) return false;
    size_t n = _stream->Readable();
    if(n == 0) return false;
    return String(s, n);
}

bool StreamReader::String(std::string& s, size_t n)
{
    if(_stream == NULL) return false;
    if(_stream->Readable() < n) return false;

    const unsigned char* p = (const unsigned char*)_stream->Read();
    std::string(p, p + n).swap(s);
    _stream->Read(n);
    return true;
}

bool StreamReader::String(std::string& s, const char* delim)
{
    if(_stream == NULL) return false;
    if(_stream->Readable() < strlen(delim)) return false;

    ssize_t n = _stream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    
    const unsigned char* p = (const unsigned char*)_stream->Read();
    std::string(p, p + n).swap(s);
    _stream->Read(n + strlen(delim));
    return true;
}

NETB_END

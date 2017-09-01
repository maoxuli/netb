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
#include <cassert>

NET_BASE_BEGIN

StreamWriter::StreamWriter()
: _stream(NULL)
{

}

StreamWriter::StreamWriter(StreamBuffer* buf)
: _stream(buf)
{

}

StreamWriter::StreamWriter(StreamBuffer& buf)
: _stream(&buf)
{

}

StreamWriter::~StreamWriter()
{

}

StreamWriter& StreamWriter::Attach(StreamBuffer* buf)
{
    _stream = buf;
    return *this;
}

StreamWriter& StreamWriter::Attach(StreamBuffer& buf)
{
    _stream = &buf;
    return *this;
}

bool StreamWriter::Bytes(const void* p, size_t n)
{
    if(_stream == NULL) return false;
    return _stream->Write(p, n);
}

bool StreamWriter::String(const std::string& s, size_t n)
{
    if(_stream == NULL) return false;
    return _stream->Write(s.data(), n);
}

bool StreamWriter::String(const std::string& s, const char* delim)
{
    if(_stream == NULL) return false;
    return _stream->Write(s.data(), s.length()) && 
           _stream->Write(delim, strlen(delim));
}

NET_BASE_END

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

bool StreamWriter::Bytes(const void* p, size_t n) const
{
    if(!_stream) return false;
    return _stream->Write(p, n);
}

// write string
// all data in the string
bool StreamWriter::String(const std::string& s) const
{
    if(!_stream) return false;
    return _stream->Write(s.data(), s.length());
}

// append ending null or not
// append delimit character (e.g. '\0')
bool StreamWriter::String(const std::string& s, const char delim) const
{
    if(!_stream) return false;
    assert(s[s.length()] != delim);
    std::string ss = s + delim;
    return _stream->Write(ss.data(), ss.length());
}

// write string
// append delimit string (e.g. "\r\n")
bool StreamWriter::String(const std::string& s, const char* delim) const
{
    if(!_stream) return false;
    std::string ss = s + delim;
    return _stream->Write(ss.data(), ss.length());
}

NETB_END

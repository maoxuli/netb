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

#include "RandomWriter.hpp"

NETB_BEGIN

RandomWriter::RandomWriter()
: _stream(0)
, _offset(0)
{

}

RandomWriter::RandomWriter(StreamBuffer* buf, size_t offset)
: _stream(buf)
, _offset(offset)
{
    assert(buf);
    assert(offset <= buf->Size());
}

RandomWriter::RandomWriter(StreamBuffer& buf, size_t offset)
: _stream(&buf)
, _offset(offset)
{
    assert(offset <= buf.Size());
}

RandomWriter::~RandomWriter()
{

}

RandomWriter& RandomWriter::Attach(StreamBuffer* buf, size_t offset)
{
    assert(buf);
    assert(offset <= buf->Size());
    _stream = buf;
    _offset = offset;
    return *this;
}

RandomWriter& RandomWriter::Attach(StreamBuffer& buf, size_t offset)
{
    assert(offset <= buf.Size());
    _stream = &buf;
    _offset = offset;
    return *this;
}

// Update n bytes at offset position
bool RandomWriter::Bytes(size_t offset, const void* p, size_t n) const
{
    if(!_stream) return false;
    if(_offset + offset + n > _stream->Peekable()) return false;
    return _stream->Update(_offset + offset, p, n);
}

// Write string at offset position
// include all data in string
bool RandomWriter::String(size_t offset, const std::string& s) const
{
    if(!_stream) return false;
    if(_offset + offset + s.length() > _stream->Peekable()) return false;
    return _stream->Update(_offset + offset, s.data(), s.length());
}

// Udpate string at offset position
// append delimit character (e.g. '\0')
bool RandomWriter::String(size_t offset, const std::string& s, const char delim) const
{
    if(!_stream) return false;
    assert(s[s.length()] != delim);
    std::string ss = s + delim;
    if(_offset + offset + ss.length()  > _stream->Peekable()) return false;
    return _stream->Update(_offset + offset, ss.data(), ss.length());
}

// Update string at offset position
// append delimit string (e.g. "\r\n")
bool RandomWriter::String(size_t offset, const std::string& s, const char* delim) const
{
    if(!_stream) return false;
    std::string ss = s + delim;
    if(_offset + offset + ss.length() > _stream->Size()) return false;
    return _stream->Update(_offset + offset, ss.data(), ss.length());
}

NETB_END

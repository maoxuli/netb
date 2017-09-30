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

#include "StreamBuffer.hpp"
#include <cstring>
#include <cassert>

NETB_BEGIN

// Initialize with initial size and limit size
StreamBuffer::StreamBuffer(size_t init, size_t limit)
: _bytes(init)
, _limit(limit)
, _opos(0)
, _rpos(0)
, _wpos(0)
{
    assert(limit >= init);   
}

// Initialize with initial data, initial size and limit size
StreamBuffer::StreamBuffer(const void* p, size_t n, size_t init, size_t limit)
: _bytes(init)
, _limit(limit)
, _opos(0)
, _rpos(0)
{
    assert(init >= n);
    assert(limit >= init);
    memcpy(Begin(), p, n);
    _wpos = n;
}

// Copy constructor
// Discard the flushed data
StreamBuffer::StreamBuffer(const StreamBuffer& b)
: _bytes(b.Size())
, _limit(b._limit)
, _opos(0)
, _rpos(0)
{
    memcpy(Begin(), b.Begin() + b._opos, b._wpos - b._opos);
    _wpos = b._wpos - b._opos;
}

// Copy constructor
// Discard the flushed data
StreamBuffer::StreamBuffer(const StreamBuffer* b)
: _bytes(b->Size())
, _limit(b->_limit) 
, _opos(0)
, _rpos(0)
{
    assert(b);
    memcpy(Begin(), b->Begin() + b->_opos, b->_wpos - b->_opos);
    _wpos = b->_wpos - b->_opos;
}

// Assignment operator
// Discard the flushed data
StreamBuffer& StreamBuffer::operator=(const StreamBuffer& b)
{
    _bytes.resize(b.Size());
    memcpy(Begin(), b.Begin() + _opos, b._wpos - b._opos);
    _limit = b._limit;
    _opos = 0;
    _rpos = b._rpos - b._opos;
    _wpos = b._wpos - b._opos;
    return *this;
}

// Actually write, copy data into the buffer and move write position forward
bool StreamBuffer::Write(const void* p, size_t n, size_t* offset)
{
    if(!Writable(n)) return false;
    memcpy(Write(), p, n);
    return Write(n, offset);
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit char
// Todo: recover from errors
bool StreamBuffer::Write(const void* p, size_t n, const char delim, size_t* offset)
{
    return Write(p, n, offset) && Write(&delim, sizeof(delim));
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit string
// Todo: recover from errors
bool StreamBuffer::Write(const void* p, size_t n, const char* delim, size_t* offset)
{
    return Write(p, n, offset) && Write(delim, strlen(delim));
}

// return the length of accessible data 
// after current sequential reading position and before next delimit string
// return -1 if the delim is not found
ssize_t StreamBuffer::Readable(const char delim) const
{
    if(Readable() < sizeof(delim)) return -1;
    const char* p1 = (const char*)Read();
    const char* p2 = std::find(p1, (const char*)Write(), delim);
    if(p2 == Write()) return -1;
    return p2 - p1;
}

// return the length of accessible data 
// after current sequential reading position and before next delimit string
// return -1 if the delim is not found
ssize_t StreamBuffer::Readable(const char* delim) const
{
    if(Readable() < strlen(delim)) return -1;
    const char* p1 = (const char*)Read();
    const char* p2 = std::find_first_of(p1, (const char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write()) return -1;
    return p2 - p1;
}

// Actually read, copy data from the buffer and move read position forward
bool StreamBuffer::Read(void* p, size_t n, size_t* offset)
{
    if(Readable() < n) return false;
    memcpy(p, Read(), n);
    return Read(n, offset);
}

// return the length of peekable data with offset and before the delimit character
// return -1 if the offset position is out of range of peekable data or delim is not found
ssize_t StreamBuffer::Peekable(size_t offset, const char delim) const
{
    if(Peekable(offset) < sizeof(delim)) return -1;
    const char* p1 = (const char*)Peekable(offset);
    const char* p2 = std::find(p1, (const char*)Write(), delim);
    if(p2 == Write()) return -1;
    return p2 - p1;
}

// return the length of peekable data with offset and before the delimit string
// return -1 if the offset position is out of range of peekable data or delim is not found
ssize_t StreamBuffer::Peekable(size_t offset, const char* delim) const
{
    if(Peekable(offset) < strlen(delim)) return -1;
    const char* p1 = (const char*)Peekable(offset);
    const char* p2 = std::find_first_of(p1, (const char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write()) return -1;
    return p2 - p1;
}

// Peek the peekable data at offset position 
bool StreamBuffer::Peek(size_t offset, void* p, size_t n)
{
    if(Peekable(offset) < n) return false;
    memcpy(p, Peek(offset), n);
    return true;
}

// Update the peekable data at offset position
// this function must be used when you know what you are doing
// improperly update may destroy the data structure in the buffer
bool StreamBuffer::Update(size_t offset, const void* p, size_t n)
{
    if(Peekable(offset) < n) return false;
    memcpy(Peek(offset), p, n);
    return true;
}

NETB_END

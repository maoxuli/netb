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
#include <cassert>

NETB_BEGIN

// Initialize with initial size and limit size
StreamBuffer::StreamBuffer(size_t init, size_t limit)
: _bytes(init)
, _limit(limit)
, _read_index(0)
, _write_index(0)
{
    assert(limit >= init);   
}

// Initialize with initial data, initial size and limit size
StreamBuffer::StreamBuffer(const void* p, size_t n, size_t init, size_t limit)
: _bytes(init)
, _limit(limit)
, _read_index(0)
{
    assert(init >= n);
    assert(limit >= init);
    memcpy(Begin(), p, n);
    _write_index = n;
}

// Copy constructor
// Deep copy and move data to the beginning
StreamBuffer::StreamBuffer(const StreamBuffer& b)
: _bytes(b.Readable())
, _limit(b._limit)
{
    memcpy(Begin(), b.Read(), b.Readable());
    _read_index = 0;
    _write_index = b.Readable();
}

// Copy constructor
// Deep copy and move data to the beginning
StreamBuffer::StreamBuffer(const StreamBuffer* b)
: _bytes(b->Readable())
, _limit(b->_limit)
{
    memcpy(Begin(), b->Read(), b->Readable());
    _read_index = 0;
    _write_index = b->Readable();
}

StreamBuffer::~StreamBuffer()
{
    
}

// Assignment operator
// Deep copy and move data to the beginning 
StreamBuffer& StreamBuffer::operator=(const StreamBuffer& b)
{
    if(_limit < b.Size())
    {
        _limit = b.Size();
    }
    if(_bytes.size() < b.Readable())
    {
        _bytes.resize(b.Readable());
    }
    memcpy(Begin(), b.Read(), b.Readable());
    _read_index = 0;
    _write_index = b.Readable();
    return *this;
}

// Actually write, copy data into the buffer and move write position forward
bool StreamBuffer::Write(const void* p, size_t n)
{
    if(!Writable(n))
    {
        return false;
    }
    memcpy(Write(), p, n);
    Write(n);
    return true;
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit char
bool StreamBuffer::Write(const void* p, size_t n, const char delim)
{
    if(Write(p, n))
    {
        return Write(&delim, sizeof(char));
    }
    return false;
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit string
bool StreamBuffer::Write(const void* p, size_t n, const char* delim)
{
    if(Write(p, n))
    {
        return Write(delim, strlen(delim));
    }
    return false;
}

// Available data to read
// Before next delimit string
ssize_t StreamBuffer::Readable(const char* delim) const
{
    if(Readable() < strlen(delim))
    {
        return -1;
    }
    const char* p1 = (const char*)Read();
    const char* p2 = std::find_first_of(p1, (const char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Actually read, copy data from the buffer and move read position forward
bool StreamBuffer::Read(void* p, size_t n)
{
    if(Readable() < n)
    {
        return false;
    }
    memcpy(p, Read(), n);
    Read(n);
    return true;
}

// Addressable data from position that offset the reading position
// to next delimit string
ssize_t StreamBuffer::Addressable(const char* delim, size_t offset) const
{
    if(Addressable(offset) < strlen(delim))
    {
        return -1;
    }
    const char* p1 = (const char*)Address(offset);
    const char* p2 = std::find_first_of(p1, (const char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Peek data at position that offset to the reading position
// Copy data from buffer but not affect reading and writing position
bool StreamBuffer::Peek(void* p, size_t n, size_t offset)
{
    if(Addressable(offset) < n) 
    {
        return false;
    }
    memcpy(p, Address(offset), n);
    return true;
}

// Update data at position that offset to the reading position
// Update data in buffer but not affect reading and writing position
bool StreamBuffer::Update(void* p, size_t n, size_t offset)
{
    if(Addressable(offset) < n) 
    {
        return false;
    }
    memcpy(Address(offset), p, n);
    return true;
}

NETB_END

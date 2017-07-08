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

#include "ByteBuffer.hpp"
#include <cassert>

NET_BASE_BEGIN

// Initialize with initial size and limit size
ByteBuffer::ByteBuffer(size_t init, size_t limit)
: mBytes(init)
, mLimit(limit)
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(limit >= init);   
}

// Initialize with initial data, initial size and limit size
ByteBuffer::ByteBuffer(const void* p, size_t n, size_t init, size_t limit)
: mBytes(init)
, mLimit(limit)
, mReadIndex(0)
{
    assert(init >= n);
    assert(limit >= init);
    memcpy(Begin(), p, n);
    mWriteIndex = n;
}

// Initialize with another ByteBuffer object
// Deep copy and move data to the beginning
ByteBuffer::ByteBuffer(const ByteBuffer& b)
: mBytes(b.Readable())
, mLimit(b.mLimit)
{
    memcpy(Begin(), b.Read(), b.Readable());
    mReadIndex = 0;
    mWriteIndex = b.Readable();
}

// Initialize with another ByteBuffer object
// Deep copy and move data to the beginning
ByteBuffer::ByteBuffer(const ByteBuffer* b)
: mBytes(b->Readable())
, mLimit(b->mLimit)
{
    memcpy(Begin(), b->Read(), b->Readable());
    mReadIndex = 0;
    mWriteIndex = b->Readable();
}

// Initialize with another StreamBuffer object
// Deep copy and move data to the beginning
ByteBuffer::ByteBuffer(const StreamBuffer& s)
: mBytes(s.Readable())
, mLimit(s.Size()) // !!!
{
    memcpy(Begin(), s.Read(), s.Readable());
    mReadIndex = 0;
    mWriteIndex = s.Readable();
}

// Initialize with another StreamBuffer object
// Deep copy and move data to the beginning
ByteBuffer::ByteBuffer(const StreamBuffer* s)
: mBytes(s->Readable())
, mLimit(s->Size()) // !!!
{
    memcpy(Begin(), s->Read(), s->Readable());
    mReadIndex = 0;
    mWriteIndex = s->Readable();
}

ByteBuffer::~ByteBuffer()
{
    
}

// Assignment with another ByteBuffer object
// Deep copy and move data to the beginning 
ByteBuffer& ByteBuffer::operator=(const ByteBuffer& b)
{
    if(mLimit < b.Size())
    {
        mLimit = b.Size();
    }
    if(mBytes.size() < b.Readable())
    {
        mBytes.resize(b.Readable());
    }
    memcpy(Begin(), b.Read(), b.Readable());
    mReadIndex = 0;
    mWriteIndex = b.Readable();
    return *this;
}

// Assignment with another StreamBuffer object
// Deep copy and move data to the beginning 
ByteBuffer& ByteBuffer::operator=(const StreamBuffer& s)
{
    if(mLimit < s.Size())
    {
        mLimit = s.Size();
    }
    if(mBytes.size() < s.Readable())
    {
        mBytes.resize(s.Readable());
    }
    memcpy(Begin(), s.Read(), s.Readable());
    mReadIndex = 0;
    mWriteIndex = s.Readable();
    return *this;
}

// Actually write, copy data into the buffer and move write position forward
bool ByteBuffer::Write(const void* p, size_t n)
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
bool ByteBuffer::Write(const void* p, size_t n, const char delim)
{
    if(Write(p, n))
    {
        return Write(&delim, sizeof(char));
    }
    return false;
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit string
bool ByteBuffer::Write(const void* p, size_t n, const char* delim)
{
    if(Write(p, n))
    {
        return Write(delim, strlen(delim));
    }
    return false;
}

// Available data to read
// Before next delimit char
ssize_t ByteBuffer::Readable(const char delim) const
{
    if(Readable() < sizeof(delim))
    {
        return -1;
    }
    const char* p1 = (const char*)Read();
    const char* p2 = std::find(p1, (const char*)Write(), delim);
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Available data to read
// Before next delimit string
ssize_t ByteBuffer::Readable(const char* delim) const
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
bool ByteBuffer::Read(void* p, size_t n)
{
    if(Readable() < n)
    {
        return false;
    }
    memcpy(p, Read(), n);
    Read(n);
    return true;
}

// Available data to peek, using offset for random access
// Before next delimit char
ssize_t ByteBuffer::Peekable(const char delim, size_t offset) const
{
    if(Peekable(offset) < sizeof(delim))
    {
        return -1;
    }
    const char* p1 = (const char*)Peek(offset);
    const char* p2 = std::find(p1, (const char*)Write(), delim);
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Available data to peek, using offset for random access
// Before next delimit string
ssize_t ByteBuffer::Peekable(const char* delim, size_t offset) const
{
    if(Peekable(offset) < strlen(delim))
    {
        return -1;
    }
    const char* p1 = (const char*)Peek(offset);
    const char* p2 = std::find_first_of(p1, (const char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Peek data, copy data from the buffer but not move read position
// using offset for random access
bool ByteBuffer::Peek(void* p, size_t n, size_t offset)
{
    if(Peekable(offset) < n) 
    {
        return false;
    }
    memcpy(p, Peek(offset), n);
    return true;
}

// Update data, replace data in the buffer
// using offset for random access
bool ByteBuffer::Replace(void* p, size_t n, size_t offset)
{
    if(Peekable(offset) < n) 
    {
        return false;
    }
    memcpy(Peek(offset), p, n);
    return true;
}

NET_BASE_END

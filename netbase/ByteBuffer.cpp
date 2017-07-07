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

// Constructor with inital size and limit size
ByteBuffer::ByteBuffer(size_t init, size_t limit)
: mBytes(init) // Initial buffer size
, mLimit(limit) // Limit of memory occupancy
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(limit >= init);   
}

// Contructor with inital data, initial size and limit size
ByteBuffer::ByteBuffer(const void* p, size_t n, size_t init, size_t limit)
: mBytes(init) // Initial buffer size
, mLimit(limit) // limit of footprint
, mReadIndex(0)
{
    assert(init >= n);
    assert(limit >= init);
    memcpy(Begin(), p, n);
    mWriteIndex = n;
}

// Initialize with a ByteBuffer object
// Copy data and get a resizable buffer
// Can write and read data to and from the buffer
ByteBuffer::ByteBuffer(const ByteBuffer& b)
: mBytes(b.Readable())
, mLimit(b.mLimit)
{
    memcpy(Begin(), b.Read(), b.Readable());
    mReadIndex = 0;
    mWriteIndex = b.Readable();
}

// Initialize with a ByteBuffer object
// Copy data and get a resizable buffer
// Can write and read data to and from the buffer
ByteBuffer::ByteBuffer(const ByteBuffer* b)
: mBytes(b->Readable())
, mLimit(b->mLimit)
{
    memcpy(Begin(), b->Read(), b->Readable());
    mReadIndex = 0;
    mWriteIndex = b->Readable();
}

// Initialize with a stream object
// Initially construct a buffer only for readable data
// Can resize up to the size of the stream object
ByteBuffer::ByteBuffer(const ByteStream& s)
: mBytes(s.Readable())
, mLimit(s.Size())
{
    memcpy(Begin(), s.Read(), s.Readable());
    mReadIndex = 0;
    mWriteIndex = s.Readable();
}

// Initialize with a stream object
// Initially construct a buffer only for readable data
// Can resize up to the size of the stream object
ByteBuffer::ByteBuffer(const ByteStream* s)
: mBytes(s->Readable())
, mLimit(s->Size())
{
    memcpy(Begin(), s->Read(), s->Readable());
    mReadIndex = 0;
    mWriteIndex = s->Readable();
}

ByteBuffer::~ByteBuffer()
{
    
}

// Copy from another buffer
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

// Copy from another stream
ByteBuffer& ByteBuffer::operator=(const ByteStream& s)
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

// Write n bytes
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

bool ByteBuffer::Write(const void* p, size_t n, const char delim)
{
    if(Write(p, n))
    {
        return Write(&delim, sizeof(char));
    }
    return false;
}

bool ByteBuffer::Write(const void* p, size_t n, const char* delim)
{
    if(Write(p, n))
    {
        return Write(delim, strlen(delim));
    }
    return false;
}

ssize_t ByteBuffer::Readable(const char delim, size_t offset) const
{
    if(Readable(offset) < sizeof(delim))
    {
        return -1;
    }
    
    const unsigned char* p1 = (const unsigned char*)Peek(offset);
    const unsigned char* p2 = std::find(p1, (const unsigned char*)Write(), delim);
    if(p2 == Write())
    {
        return -1;
    }

    return p2 - p1;
}

ssize_t ByteBuffer::Readable(const char* delim, size_t offset) const
{
    if(Readable(offset) < strlen(delim))
    {
        return -1;
    }
    
    const unsigned char* p1 = (const unsigned char*)Peek(offset);
    const unsigned char* p2 = std::find_first_of(p1, (const unsigned char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write())
    {
        return -1;
    }
    
    return p2 - p1;
}

// Read into a bytes array
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

bool ByteBuffer::Peek(void* p, size_t n, size_t offset)
{
    if(Readable(offset) < n) 
    {
        return false;
    }

    memcpy(p, Peek(offset), n);
    return true;
}


bool ByteBuffer::Update(void* p, size_t n, size_t offset)
{
    if(Readable(offset) < n) 
    {
        return false;
    }

    memcpy(Peek(offset), p, n);
    return false;
}

NET_BASE_END

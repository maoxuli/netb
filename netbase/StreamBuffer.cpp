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

NET_BASE_BEGIN

// Initialize internal buffer with initial size 
StreamBuffer::StreamBuffer(size_t size)
: mOwn(true)
, mSize(size)
, mBytes((unsigned char*)malloc(mSize))
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(mBytes != NULL);
}

// Initialize internal bufffer with initial data
StreamBuffer::StreamBuffer(const void* p, size_t data_len)
: mOwn(true)
, mSize(data_len * 2)
, mBytes((unsigned char*)malloc(mSize))
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(mBytes != NULL);
    memcpy(mBytes, p, data_len);
    mWriteIndex = data_len;
}

// Initialize with external buffer
StreamBuffer::StreamBuffer(const void* p, size_t size, size_t data_len)
: mOwn(false)
, mSize(size)
, mBytes((unsigned char*)p)
, mReadIndex(0)
, mWriteIndex(data_len)
{
    assert(mBytes != NULL);
    assert(mWriteIndex <= mSize);
}

// Deep copy
StreamBuffer::StreamBuffer(const StreamBuffer& b)
: mOwn(true)
, mSize(b.mSize)
, mBytes((unsigned char*)malloc(mSize))
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(mBytes != NULL);
    if(b.Readable() > 0) 
    {
        memcpy(mBytes, b.Read(), b.Readable());
        mWriteIndex = b.Readable();
    }
}

// Deep copy
StreamBuffer::StreamBuffer(const StreamBuffer* b)
: mOwn(true)
, mSize(b->mSize)
, mBytes((unsigned char*)malloc(mSize))
, mReadIndex(0)
, mWriteIndex(0)
{
    assert(mBytes != NULL);
    if(b->Readable() > 0) 
    {
        memcpy(mBytes, b->Read(), b->Readable());
        mWriteIndex = b->Readable();
    }
}

StreamBuffer::~StreamBuffer()
{
    if(mOwn && mBytes != NULL)
    {
        free(mBytes);
    }
}

// Deep copy
StreamBuffer& StreamBuffer::operator=(const StreamBuffer& b)
{
    if(!mOwn) 
    {
        mOwn = true;
        mSize = b.mSize;
        mBytes = (unsigned char*)malloc(mSize);
        assert(mBytes != NULL);
    }
    else if(mSize < b.Readable())
    {
        mSize = b.mSize;
        mBytes = (unsigned char*)realloc(mBytes, mSize);
        assert(mBytes != NULL);
    }

    mReadIndex = 0;
    if(b.Readable() > 0)
    {
        memcpy(mBytes, b.Read(), b.Readable());
        mWriteIndex = b.Readable();
    }

    return *this;
}

// Write n bytes
bool StreamBuffer::Write(const void* p, size_t n)
{
    if(Reserve(n))
    {
        memcpy(Write(), p, n);
        Write(n);
        return true;
    }
    return false;
}

bool StreamBuffer::Write(const void* p, size_t n, const char delim)
{
    if(Write(p, n))
    {
        return Write(&delim, sizeof(char));
    }
    return false;
}

bool StreamBuffer::Write(const void* p, size_t n, const char* delim)
{
    if(Write(p, n))
    {
        return Write(delim, strlen(delim));
    }
    return false;
}

ssize_t StreamBuffer::Readable(const char delim, size_t offset)
{
    if(Readable() < sizeof(delim) + offset)
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

ssize_t StreamBuffer::Readable(const char* delim, size_t offset)
{
    if(Readable() < strlen(delim) + offset)
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

bool StreamBuffer::Peek(void* p, size_t n, size_t offset)
{
    if(Readable() - offset < n) 
    {
        return false;
    }

    memcpy(p, Peek(offset), n);
    return true;
}


bool StreamBuffer::Update(void* p, size_t n, size_t offset)
{
    if(Readable() - offset < n) 
    {
        return false;
    }

    memcpy(Peek(offset), p, n);
    return true;
}

NET_BASE_END

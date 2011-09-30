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

// limit the footprint of the buffer
// 0 is no limit
StreamBuffer::StreamBuffer(size_t init, size_t limit)
: mBytes(init) // Initial buffer size
, mLimit(limit * 1024) // Limit of footprint
, mReadIndex(0)
, mWriteIndex(0)
{
    
}

// A buffer loading given data on init
StreamBuffer::StreamBuffer(const void* p, size_t n, size_t init, size_t limit)
: mBytes(n * init) // Initial buffer size
, mLimit(limit * 1024) // limit of footprint
, mReadIndex(0)
{
    memcpy(Begin(), p, n);
    mWriteIndex = n;
}


StreamBuffer::StreamBuffer(const StreamBuffer* buf)
: mBytes(buf->Size())
, mLimit(buf->mLimit)
{
    memcpy(Begin(), buf->Read(), buf->Readable());
    mReadIndex = 0;
    mWriteIndex = buf->Readable();
}

StreamBuffer::StreamBuffer(const StreamBuffer& buf)
: mBytes(buf.Size())
, mLimit(buf.mLimit)
{
    memcpy(Begin(), buf.Read(), buf.Readable());
    mReadIndex = 0;
    mWriteIndex = buf.Readable();
}

StreamBuffer::~StreamBuffer()
{
    
}

StreamBuffer& StreamBuffer::operator=(const StreamBuffer& buf)
{
    mBytes.resize(buf.Size());
    mLimit = buf.mLimit;
    memcpy(Begin(), buf.Read(), buf.Readable());
    mReadIndex = 0;
    mWriteIndex = buf.Readable();
    return *this;
}

// Write n bytes
bool StreamBuffer::Write(const void* p, size_t n)
{
    if(!Reserve(n))
    {
        return false;
    }

    memcpy(Write(), p, n);
    Write(n);
    return true;
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
    if(Readable() < n + offset) 
    {
        return false;
    }

    memcpy(p, Peek(offset), n);
    return true;
}


bool StreamBuffer::Update(void* p, size_t n, size_t offset)
{
    if(Readable() < n + offset) 
    {
        return false;
    }

    memcpy(Peek(offset), p, n);
    return false;
}

NET_BASE_END

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

#include "ByteWrapper.hpp"
#include <cassert>

NET_BASE_BEGIN

// Initialize with external buffer and initial data
ByteWrapper::ByteWrapper(const void* p, size_t size, size_t& data_len)
: mBytes((unsigned char*)p)
, mSize(size)
, mWriteIndex(data_len)
, mReadIndex(0)
{
    assert(mBytes != NULL);
    assert(mSize >= mWriteIndex);
}

ByteWrapper::~ByteWrapper()
{

}

// Actually write, copy data into the buffer and move write position forward
bool ByteWrapper::Write(const void* p, size_t n)
{
    if(Writable(n))
    {
        memcpy(Write(), p, n);
        Write(n);
        return true;
    }
    return false;
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit char
bool ByteWrapper::Write(const void* p, size_t n, const char delim)
{
    if(Write(p, n))
    {
        return Write(&delim, sizeof(char));
    }
    return false;
}

// Actually write, copy data into the buffer and move write position forward
// Append a delimit string
bool ByteWrapper::Write(const void* p, size_t n, const char* delim)
{
    if(Write(p, n))
    {
        return Write(delim, strlen(delim));
    }
    return false;
}

// Available data to read
// Before next delimit char
ssize_t ByteWrapper::Readable(const char delim) const
{
    if(Readable() < sizeof(delim))
    {
        return -1;
    }
    const unsigned char* p1 = (const unsigned char*)Read();
    const unsigned char* p2 = std::find(p1, (const unsigned char*)Write(), delim);
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Available data to read
// Before next delimit string
ssize_t ByteWrapper::Readable(const char* delim) const
{
    if(Readable() < strlen(delim))
    {
        return -1;
    }
    const unsigned char* p1 = (const unsigned char*)Read();
    const unsigned char* p2 = std::find_first_of(p1, (const unsigned char*)Write(), delim, delim + strlen(delim));
    if(p2 == Write())
    {
        return -1;
    }
    return p2 - p1;
}

// Actually read, copy data from the buffer and move read position forward
bool ByteWrapper::Read(void* p, size_t n)
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
ssize_t ByteWrapper::Peekable(const char delim, size_t offset) const
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
ssize_t ByteWrapper::Peekable(const char* delim, size_t offset) const
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
bool ByteWrapper::Peek(void* p, size_t n, size_t offset)
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
bool ByteWrapper::Replace(void* p, size_t n, size_t offset)
{
    if(Peekable(offset) < n) 
    {
        return false;
    }
    memcpy(Peek(offset), p, n);
    return true;
}

NET_BASE_END

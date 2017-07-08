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

#ifndef NET_BASE_BYTE_WRAPPER_HPP
#define NET_BASE_BYTE_WRAPPER_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"

NET_BASE_BEGIN

//
// A wrapper of external byte sequence, i.e. a memory block
//
class ByteWrapper : public StreamBuffer, Uncopyable
{
public:
    // Initialize with external buffer and initial data
    ByteWrapper(const void* p, size_t size, size_t& data_len);
    ~ByteWrapper();

    // Readable() + Writable()
    size_t Size() const
    {
        return mSize - mReadIndex;
    }

    // Readable() == 0 ?
    bool Empty() const 
    {
        return mWriteIndex == mReadIndex;
    }

    // Set Empty() == true
    void Clear()
    {
        mReadIndex = 0;
        mWriteIndex = 0; 
    }

    // Available space to write
    size_t Writable() const
    {
        return mSize - mWriteIndex;
    }

    // Ensure Writable() == n
    bool Writable(size_t n)
    {
        if(n == 0 || n <= mSize - mWriteIndex)
        {
            return true;
        }
        return false;
    }

    // Pointer to write position 
    // For external use, such as directly copy data into the buffer 
    const void* Write() const
    {
        return mBytes + mWriteIndex;
    }

    void* Write()
    {
        return mBytes + mWriteIndex;
    }

    // Virtually write, move write position forward
    bool Write(size_t n)
    {
        if(n == 0 || mSize < mWriteIndex + n)
        {
            return false;
        }
        mWriteIndex += n;
        return true;
    }

    // Actually write, copy data into the buffer and move write position forward
    bool Write(const void* p, size_t n);
    bool Write(const void* p, size_t n, const char delim); // Append a delimit char
    bool Write(const void* p, size_t n, const char* delim); // Append a delimit string

    // Available data to read
    size_t Readable() const
    {
        return mWriteIndex - mReadIndex;
    }

    // Available data to read
    ssize_t Readable(const char delim) const; // Before next delimit char
    ssize_t Readable(const char* delim) const; // Before next delimit string

    // Pointer to read position
    // For external use, such as directly copy data from the buffer
    const void* Read() const
    {
        return mBytes + mReadIndex;
    }

    void* Read() 
    {
        return mBytes + mReadIndex;
    }

    // Virtually read, move read position forward
    bool Read(size_t n)
    {
        if(mWriteIndex < mReadIndex + n)
        {
            return false;
        }

        mReadIndex += n;
        return true;
    }
    
    bool Remove(size_t n)
    {
        return Read(n);
    }

    // Actually read, copy data from the buffer and move read position forward
    bool Read(void* p, size_t n);

    // Available data to peek, using offset for random access
    ssize_t Peekable(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex - offset;
    }

    // Available data to peek, using offset for random access
    ssize_t Peekable(const char delim, size_t offset = 0) const; // Before next delimit char
    ssize_t Peekable(const char* delim, size_t offset = 0) const; // Before next delimit string

    // Pointer to peek position, using offset for random access
    // For external use, such as directly copy data from the buffer
    const void* Peek(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex < offset ? NULL : mBytes + mReadIndex + offset;
    }

    void* Peek(size_t offset = 0) 
    {
        return mWriteIndex - mReadIndex < offset ? NULL : mBytes + mReadIndex + offset;
    }

    // Peek data, copy data from the buffer but not move read position
    // using offset for random access
    bool Peek(void* p, size_t n, size_t offset = 0);

    // Update data, replace data in the buffer
    // using offset for random access
    bool Replace(void* p, size_t n, size_t offset = 0);

protected:
    unsigned char* mBytes;  // Pointer of external memory block
    size_t mSize;           // Allocated size of the memory block
    size_t& mWriteIndex;    // Current write position, length of current data
    size_t mReadIndex;      // Current read position
};

NET_BASE_END

#endif

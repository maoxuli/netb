/*
 * Copyright (C) 2010-2017, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NET_BASE_STREAM_BUFFER_HPP
#define NET_BASE_STREAM_BUFFER_HPP

// Buffer size, can be re-defined in Config.hpp
#define DEFAULT_BUFFER_SIZE     4096 // 4K bytes, enough for two MTU
#define MAX_BUFFER_SIZE        (1024 * 1024) // 1M bytes

#include "Config.hpp"
#include <cstddef>
#include <vector>

NET_BASE_BEGIN

//
// A StreamBuffer is a buffer that supports streaming-like read and write, 
// as well as randomly peek and update. 
//
class StreamBuffer
{
public:
    ~StreamBuffer();

    // Initialize with initial size and limit size
    StreamBuffer(size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Initialize with initial data, initial size and limit size
    StreamBuffer(const void* p, size_t n, size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Copy constructor
    // Deep copy and move data to the beginning
    StreamBuffer(const StreamBuffer& b);
    explicit StreamBuffer(const StreamBuffer* b);

    // Assignment operator
    // Deep copy and move data to the beginning 
    StreamBuffer& operator=(const StreamBuffer& b);

    // Swap, without data copy
    StreamBuffer& Swap(StreamBuffer& b)
    {
        mBytes.swap(b.mBytes);
        std::swap(mLimit, b.mLimit);
        std::swap(mReadIndex, b.mReadIndex);
        std::swap(mWriteIndex, b.mWriteIndex);
        return *this;
    }

    StreamBuffer& Swap(StreamBuffer* b)
    {
        mBytes.swap(b->mBytes);
        std::swap(mLimit, b->mLimit);
        std::swap(mReadIndex, b->mReadIndex);
        std::swap(mWriteIndex, b->mWriteIndex);
        return *this;
    }

    // Readable() + Writable()
    size_t Size() const 
    {
        return mBytes.size() - mReadIndex;
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
        return mBytes.size() - mWriteIndex;
    }

    // Try to ensure Writable() >= n
    bool Writable(size_t n)
    {
        if(n == 0 || mBytes.size() - mWriteIndex >= n)
        {
            return true;
        }
        if(mLimit > 0) 
        {
            if(mWriteIndex - mReadIndex + n > mLimit) // the buffer is overflow
            {
                return false;
            } 

            if(mWriteIndex + n > mLimit) // Occupancy is beyond limit, compact
            {
                Compact();
            }
        } 
        mBytes.resize(mWriteIndex + n);
        return mBytes.size() - mWriteIndex >= n;
    }

    // Pointer to write position 
    // For external use, such as directly copy data into the buffer 
    const void* Write() const
    {
        return Begin() + mWriteIndex;
    }

    void* Write()
    {
        return Begin() + mWriteIndex;
    }

    // Virtually write, move write position forward
    bool Write(size_t n)
    {
        if(mBytes.size() - mWriteIndex < n)
        {
            return false;
        }
        mWriteIndex += n;
        return true;
    }

    // Actually write, copy data into the buffer and move write position forward
    bool Write(const void* p, size_t n);
    bool Write(const void* p, size_t n, const char delim); // Append delimit char
    bool Write(const void* p, size_t n, const char* delim); // Append delimit string

    // Available data to read
    size_t Readable() const
    {
        return mWriteIndex - mReadIndex;
    }

    // Check if Readable() >= n
    bool Readable(size_t n) const
    {
        return mWriteIndex - mReadIndex >= n;
    }

    // Available data to read before next delimit char or string
    ssize_t Readable(const char delim) const;
    ssize_t Readable(const char* delim) const; 

    // Pointer to read position
    // For external use, such as directly copy data from the buffer
    const void* Read() const
    {
        return Begin() + mReadIndex;
    }

    void* Read() 
    {
        return Begin() + mReadIndex;
    }

    // Virtually read, move read position forward
    bool Read(size_t n)
    {
        if(mWriteIndex - mReadIndex < n)
        {
            return false;
        }
        mReadIndex += n;
        if(mReadIndex == mWriteIndex)
        {
            mReadIndex = 0;
            mWriteIndex = 0;
        }
        return true;
    }

    // Actually read, copy data from the buffer and move read position forward
    bool Read(void* p, size_t n);

    // Available data from position that offset to the reading position
    ssize_t Lockable(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex - offset;
    }

    // Available data from position that offset to the reading position 
    // to next delimit char or string
    ssize_t Lockable(const char delim, size_t offset = 0) const; 
    ssize_t Lockable(const char* delim, size_t offset = 0) const; 

    // Pointer to position that offset to the reading position 
    // For external use, such as peek or update data in buffer 
    const void* Lock(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex < offset ? NULL : Begin() + mReadIndex + offset;
    }

    void* Lock(size_t offset = 0) 
    {
        return mWriteIndex - mReadIndex < offset ? NULL : Begin() + mReadIndex + offset;
    }

    // Peek data at position that offset to the reading position
    // Copy data from buffer but not affect reading and writing position
    bool Peek(void* p, size_t n, size_t offset = 0);

    // Update data at position that offset to the reading position
    // Update data in buffer but not affect reading and writing position
    bool Update(void* p, size_t n, size_t offset = 0);

protected:
    
    //
    //        |                        -size()-                          |
    // vector |##########################################################|.............|
    //      begin()                                                     end()      capacity()
    //
    //
    //
    //                 |                   -Size()-                      |
    // Buffer |--------|xxxxxxxxxxxxxxxxxxxxxxxxx|***********************|.............|
    //        |        |       -Readable()-      |       -Writable()-    |
    //      Begin() mReadIndex              mWriteIndex                  
    //
    //
    
    std::vector<unsigned char> mBytes;
    size_t mLimit; // Limit of the max footprint of the buffer
    size_t mReadIndex; // Index of first readable byte
    size_t mWriteIndex; // Index of first writable byte

    const unsigned char* Begin() const
    {
        return &mBytes[0];
    }

    unsigned char* Begin()
    {
        return &mBytes[0];
    }

    // Move data to the beginning
    void Compact()
    {
        std::rotate(mBytes.begin(), mBytes.begin() + mReadIndex, mBytes.begin() + mWriteIndex - mReadIndex);
        mWriteIndex -= mReadIndex;
        mReadIndex = 0;
    }
};

NET_BASE_END

#endif

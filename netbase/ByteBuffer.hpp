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

#ifndef NET_BASE_BYTE_BUFFER_HPP
#define NET_BASE_BYTE_BUFFER_HPP

// Buffer size, can be re-defined in Config.hpp
#define DEFAULT_BUFFER_SIZE     4096 // 4K bytes, enough for two MTU
#define MAX_BUFFER_SIZE        (1024 * 1024) // 1M bytes

#include "Config.hpp"
#include "StreamBuffer.hpp"
#include <vector>

NET_BASE_BEGIN

//
// It is a stream, and 
// it is a buffer, and
// it is a byte sequence buffer
//
class ByteBuffer : public StreamBuffer
{
public:
    ~ByteBuffer();

    // Initialize with initial size and limit size
    ByteBuffer(size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Initialize with initial data, initial size and limit size
    ByteBuffer(const void* p, size_t n, size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Initialize with another ByteBuffer object
    // Deep copy and move data to the beginning
    ByteBuffer(const ByteBuffer& b);
    ByteBuffer(const ByteBuffer* b);

    // Initialize with another StreamBuffer object
    // Deep copy and move data to the beginning
    ByteBuffer(const StreamBuffer& s);
    ByteBuffer(const StreamBuffer* s);

    // Assignment with another ByteBuffer or StreamBuffer object
    // Deep copy and move data to the beginning 
    ByteBuffer& operator=(const ByteBuffer& b);
    ByteBuffer& operator=(const StreamBuffer& s);

    // Swap with another ByteBuffer object
    // No data copy
    ByteBuffer& Swap(ByteBuffer& b)
    {
        return Swap(&b);
    }

    ByteBuffer& Swap(ByteBuffer* b)
    {
        mBytes.swap(b->mBytes);
        std::swap(mLimit, b->mLimit);
        std::swap(mReadIndex, b->mReadIndex);
        std::swap(mWriteIndex, b->mWriteIndex);
        return *this;
    }

    // Swap with another StreamBuffer object
    // No data copy if another object is a ByteBuffer
    ByteBuffer& Swap(StreamBuffer& s)
    {   
        return Swap(&s);
    }

    ByteBuffer& Swap(StreamBuffer* s)
    {
        ByteBuffer* b = dynamic_cast<ByteBuffer*>(s);
        if ( b  != NULL )
        {
            mBytes.swap(b->mBytes);
            std::swap(mLimit, b->mLimit);
            std::swap(mReadIndex, b->mReadIndex);
            std::swap(mWriteIndex, b->mWriteIndex);
            return *this;
        }
        return *this = *s;
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

    // Ensure Writable() == n
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

    // Available data to read
    ssize_t Readable(const char delimt) const; // Before next delimit char
    ssize_t Readable(const char* delim) const; // Before next delimit string

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
        return mWriteIndex - mReadIndex < offset ? NULL : Begin() + mReadIndex + offset;
    }

    void* Peek(size_t offset = 0) 
    {
        return mWriteIndex - mReadIndex < offset ? NULL : Begin() + mReadIndex + offset;
    }

    // Peek data, copy data from the buffer but not move read position
    // using offset for random access
    bool Peek(void* p, size_t n, size_t offset = 0);

    // Update data, replace data in the buffer
    // using offset for random access
    bool Replace(void* p, size_t n, size_t offset = 0);

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

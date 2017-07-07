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

#include "Config.hpp"
#include "ByteStream.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

NET_BASE_BEGIN

//
// Stream buffer is basically a bytes buffer, with random access. 
// The buffer is designed particularly for network IO and message packaging. 
// It is not thread safe, so can be used within a single thread. That means the 
// network IO and prottocol message packaging must be done in a same thread. 
// To be specific, the buffer should be used immediately in ReceivedCallback, 
// and Send should be done in the same thread of network IO. 
//
// It is a vary sized buffer based on internal vary sized std::vector. so 
// write may be successfully always until reach the max limit of the buffer size. 
// Meanwhile, any memory position (pointer) is timely valid. It may be invalid 
// with next time resizing (usually with memory re-allocation).
// 
// According to typical protocol message packaging strategies, the buffer support 
// writing and reading data with a fixed size, or writing data with a ending 
// delimit and reading data to the delimit. 
//
// bool StreamBuffer::Write(void* p, size_t n);
// bool StreamBuffer::Write(void* p, size_t n, delimit);
// size_t StreamBuffer::Readable(delimit); // get data size before the delimit
// bool StreamBuffer::Read(void* p, size_t); 
//
// As a IO buffer, it must expose the memory pointer for reading and writing to 
// IO operations, to avoid more memory copy. 
// void* Write(); // expose the writing position (pointer) for external writing
// bool Write(size_t n);  // adjust buffer size according external writing
// void* Read(); // expose the reading position (pointer) for external reading 
// bool Read(size_t n); // Adjust buffer size according to external reading
// 
// The buffer also support to peek and update data on random position, which is 
// useful in message packaging. 
// 
// The buffer works in bytes basis, all parameters of size or offset are of bytes 
// count. The exposed pointers are all void* type to support any pointer types.
// There is no specific data type information other than bytes sequence, so 
// there is no endinaness concerns.  
//  
// The buffer is based on a internal std::vector that can resize by request. 
// In a typical continuous writing and reading, the buffer may  
//
class ByteBuffer : public ByteStream
{
public:
    virtual ~ByteBuffer();

    // Initialize with initial size and limit size
    ByteBuffer(size_t init = 4096, size_t limit = 1024 * 1024);

    // Initialize with initial data, initial size and limit size
    ByteBuffer(const void* p, size_t n, size_t init = 4096, size_t limit = 1024 * 1024);

    // Initialize with another ByteBuffer object
    // Deep copy and move data to the beginning
    ByteBuffer(const ByteBuffer& b);
    ByteBuffer(const ByteBuffer* b);

    // Initialize with another ByteStream object
    // Deep copy and move data to the beginning
    ByteBuffer(const ByteStream& s);
    ByteBuffer(const ByteStream* s);

    // Assignment with another ByteBuffer or ByteStream object
    // Deep copy and move data to the beginning 
    ByteBuffer& operator=(const ByteBuffer& b);
    ByteBuffer& operator=(const ByteStream& s);

    // Swap with another ByteBuffer object
    // No data copy
    ByteBuffer& Swap(ByteBuffer& b)
    {
        mBytes.swap(b.mBytes);
        std::swap(mLimit, b.mLimit);
        std::swap(mReadIndex, b.mReadIndex);
        std::swap(mWriteIndex, b.mWriteIndex);
        return *this;
    }

    ByteBuffer& Swap(ByteBuffer* b)
    {
        mBytes.swap(b->mBytes);
        std::swap(mLimit, b->mLimit);
        std::swap(mReadIndex, b->mReadIndex);
        std::swap(mWriteIndex, b->mWriteIndex);
        return *this;
    }

    // Swap with another ByteStream object
    // No data copy if another object is a ByteBuffer
    ByteBuffer& Swap(ByteStream& s)
    {   
        return Swap(&s);
    }

    ByteBuffer& Swap(ByteStream* s)
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

    // Is empty?
    bool Empty() const 
    {
        return mWriteIndex == mReadIndex;
    }

    // Clear the buffer
    // Todo: Thrink memory occupancy if necessary
    void Clear()
    {
        mReadIndex = 0;
        mWriteIndex = 0; 
    }

    // Size of the buffer
    // Readable size and writable size
    size_t Size() const 
    {
        return mBytes.size() - mReadIndex;
    }

    // Check and try to get vailable space to write
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

            if(mWriteIndex + n > mLimit) // Occupancy is beyond limit, shrink
            {
                Shrink();
            }
        } 

        mBytes.resize(mWriteIndex + n);
        return mBytes.size() - mWriteIndex >= n;
    }

    // Check available space to write
    size_t Writable() const
    {
        return mBytes.size() - mWriteIndex;
    }

    // Write n bytes
    bool Write(const void* p, size_t n);

    // Write n bytes with delimit char or string
    bool Write(const void* p, size_t n, const char delim);
    bool Write(const void* p, size_t n, const char* delim);

    // Write with only size of bytes
    // move forward write position only
    bool Write(size_t n)
    {
        if(mBytes.size() - mWriteIndex < n)
        {
            return false;
        }
        mWriteIndex += n;
        return true;
    }

    // Write without argument
    // return a pointer of write position for external use
    // unsigned char src[100];
    // buf.Reserve(100);
    // memcpy(buf.Write(), src, 100);
    // buf.Write(100);
    
    // const pointer used as a position
    const void* Write() const
    {
        return Begin() + mWriteIndex;
    }

    // pointer used to write
    void* Write()
    {
        return Begin() + mWriteIndex;
    }

    // The available bytes to read
    size_t Readable(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex - offset;
    }

    // Readable before the delimit
    ssize_t Readable(const char delimt, size_t offset = 0) const;
    ssize_t Readable(const char* delim, size_t offset = 0) const;

    // Read with buffer and size
    // actual read and move forward read index
    bool Read(void* p, size_t n);

    // Read with only data size 
    // move forward read position only
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
    
    // equal to read(n)
    bool Remove(size_t n)
    {
        return Read(n);
    }

    // Read with no argument
    // return read position for external use
    // 
    // unsigned char dst[20];
    // memcpy(dst, buf.Read(), 20);
    // buf.Read(20); [or buf.Remove(20);]
    //
    const void* Read() const
    {
        return Begin() + mReadIndex;
    }

    void* Read() 
    {
        return Begin() + mReadIndex;
    }

    // Peek data in buffer on given offset from reading position 
    // return first byte position of peek for external use
    const void* Peek(size_t offset = 0) const
    {
        return mWriteIndex - mReadIndex > offset ? Begin() + mReadIndex + offset : NULL;
    }

    // for update
    void* Peek(size_t offset)
    {
        return mWriteIndex - mReadIndex > offset ? Begin() + mReadIndex + offset : NULL;
    }

    // Peek bytes
    bool Peek(void* p, size_t n, size_t offset = 0);

    // Update bytes
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
    //                        -Readable()-              -Writable()-
    //      Begin() mReadIndex              mWriteIndex                  
    //
    //
    
    const unsigned char* Begin() const
    {
        return &mBytes[0];
    }

    unsigned char* Begin()
    {
        return &mBytes[0];
    }

    void Shrink()
    {
        std::rotate(mBytes.begin(), mBytes.begin() + mReadIndex, mBytes.begin() + mWriteIndex - mReadIndex);
        mWriteIndex -= mReadIndex;
        mReadIndex = 0;
    }

    std::vector<unsigned char> mBytes;
    size_t mLimit; // Limit of the max footprint of the buffer
    size_t mReadIndex; // Index of first readable byte
    size_t mWriteIndex; // Index of first writable byte
};

NET_BASE_END

#endif

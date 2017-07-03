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

#ifndef NET_BASE_STREAM_BUFFER_HPP
#define NET_BASE_STREAM_BUFFER_HPP

#include "Config.hpp"
#include <cstddef>
#include <cstdint>
#include <cassert>

NET_BASE_BEGIN

//
// A memory block denoted with a pointer and size is usually used in network 
// programming to carry data, or transfer data. Naturally, at some points, we 
// need to write or read data from the memory. As a carrier of protocol data, 
// some features as below are usually expected:
//
// 1. The size of the memory is enought to hold the protocol data, or it can be 
//    resized automatically according to requirments. 
// 2. Write or read data in a manner of "streaming", that means the positions of 
//    last read and last write are tracted and kept. 
// 3. An easy interface to the plain pointer and size that denotes the memory block.
// 
// StreamBuffer is designed from above perspectives. Basically, it works in two 
// manners: 
// 
// 1. StreamBuffer object is initiated firstly, it initalizes and owns the internal 
//    memory block. Writing or reading data are always done via the interface of the 
//    object. A pointer and size can be exposed to external to read or write data, 
//    but the extenal can not own and manage the memory block directly. 
//    Such objects are instantiated with below constructor: 
//    
//    // A buffer with a initial size
//    StreamBuffer(size_t size);
//
//    // A buffer with inital data, which will be copied into the internal memory
//    StreamBuffer(const void* p, size_t data_len);
// 
// 2. A memory block is initialized firstly, and some data maybe has been written into 
//    the memory block already. Then a StreamBuffer object is instantiated to manage 
//    the existing memory block, i.e. reading and writing data. The object does not 
//    own the memory block, all functions of the object effect on the external memory 
//    block, thus the memory block (the pointer and size) must be kept valid during 
//    the lifetime of StreamBuffer object. 
//    Such objects are instantiated with below constructor: 
// 
//    // The external memory block is denoted by p and size,
//    // data_len denotes the size of initial data in the memory block
//    StreamBuffer(const void* p, size_t size, size_t data_len); 
// 
// The buffer object is designed particularly for network IO and message packaging. 
// According to typical protocol message packaging strategies, it supports writing 
// and reading data with a fixed size, or with a ending delimit, as below:
// 
//    bool StreamBuffer::Write(void* p, size_t n);
//    bool StreamBuffer::Write(void* p, size_t n, const char/char* delimit);
//
//    size_t StreamBuffer::Readable(const char/char* delimit); // size to the delimit
//    bool StreamBuffer::Read(void* p, size_t); 
//
// As a IO buffer, it must expose the memory pointer for reading and writing to 
// avoid more memory copies. 
// 
//    void* Write(); // expose the writing position (pointer) for external writing
//    bool Write(size_t n);  // adjust buffer size according external writing
//
//    void* Read(); // expose the reading position (pointer) for external reading 
//    bool Read(size_t n); // Adjust buffer size according to external reading
// 
// The buffer also support to peek and update data on random position, which is 
// useful in message packaging. 
// 
// The buffer works in bytes basis, all parameters of size or offset are of bytes 
// count. The exposed pointers are all void* type to support any pointer types.
// There is no specific data type information other than bytes sequence, so there 
// is no endinaness concerns.  

// StreamBuffer is not thread safe, so can be used within a single thread. That 
// means the network IO and prottocol message packaging must be done in a same thread. 
// To be specific, the buffer should be used immediately in ReceivedCallback, 
// and Send should be done in the same thread of network IO. 
//
// It is a vary sized buffer based on internal vary sized std::vector. so 
// write may be successfully always until reach the max limit of the buffer size. 
// Meanwhile, any memory position (pointer) is timely valid. It may be invalid 
// with next time resizing (usually with memory re-allocation).
//

#define BUFFER_SIZE_STEP    8096        // 8K bytes, enough for 4 MTUs
#define BUFFER_SIZE_LIMIT   1024*1024   // 1G bytes

class StreamBuffer
{
public:
    ~StreamBuffer();

    // Initialize internal buffer with initial size
    StreamBuffer(size_t size = BUFFER_SIZE_STEP);

    // Initialize internal buffer with initial data (p, data_len)
    StreamBuffer(const void* p, size_t data_len);

    // Buffer is externally initialized
    StreamBuffer(const void* p, size_t size, size_t data_len);

    // Copy constructor, deep copy
    StreamBuffer(const StreamBuffer& b);

    // Copy from a pointer, deep copy
    StreamBuffer(const StreamBuffer* b);

    // Assignment, deep copy
    StreamBuffer& operator = (const StreamBuffer& b);

    // Swap two buffers
    StreamBuffer& Swap(StreamBuffer& b)
    {
        if(mOwn && b.mOwn)
        {
            std::swap(mSize, b.mSize);
            std::swap(mBytes, b.mBytes);
            std::swap(mReadIndex, b.mReadIndex);
            std::swap(mWriteIndex, b.mWriteIndex);
        }
        else if(mOwn) // !b.mOwn
        {
            *this = b;
        }
        else if(b.mOwn)  // !mOwn
        {
            b = *this;
        }
        return *this;
    }
    
    // Swap two buffers
    StreamBuffer& Swap(StreamBuffer* b)
    {
        if(mOwn && b->mOwn)
        {
            std::swap(mSize, b->mSize);
            std::swap(mBytes, b->mBytes);
            std::swap(mReadIndex, b->mReadIndex);
            std::swap(mWriteIndex, b->mWriteIndex);
        }
        else if(mOwn) // !b.mOwn
        {
            *this = *b;
        }
        else if(b->mOwn)  // !mOwn
        {
            *b = *this;
        }
        return *this;
    }

    // Allocated memory size
    size_t Size() const
    {
        return mSize;
    }

    // Clear the buffer
    void Clear()
    {
        mReadIndex = 0;
        mWriteIndex = 0; 
    }

    // Is empty?
    bool Empty() const 
    {
        return mWriteIndex == mReadIndex;
    }

    // Reserve free space to write
    // Ensure the buffer has enough writing space
    // Once reach the limit footprint, shrink 
    bool Reserve(size_t n)
    {
        // No need to move data or resize buffer
        if(n == 0 || (mSize - mWriteIndex) >= n)
        {
            return true;
        }

        // External memory can not be moved or resized
        if(!mOwn)
        {
            return false;
        }

        // The spare space before data is larger than a step, and 
        // total spare space is enough to keep at leat step/2 spare space
        // Move data to the beginning
        if(mReadIndex >= BUFFER_SIZE_STEP && (mSize - mWriteIndex + mReadIndex - BUFFER_SIZE_STEP / 2) >= n)
        {
            memcpy(mBytes, mBytes + mReadIndex, mWriteIndex - mReadIndex);
            mWriteIndex = mWriteIndex - mReadIndex;
            mReadIndex = 0;
        }
        
        // resize the buffer to keep at leat step/2 spare space
        size_t steps = (mSize - mWriteIndex + BUFFER_SIZE_STEP / 2) >= n ? 1 : (n / BUFFER_SIZE_STEP + 1);
        size_t rsize = steps * BUFFER_SIZE_STEP + mSize;
        if(rsize > BUFFER_SIZE_LIMIT)
        {
            return false;
        }
        mBytes = (unsigned char*)realloc(mBytes, rsize);
        assert(mBytes != NULL);
        return true;
    }

    // Available free space to write
    size_t Writable() const
    {
        return mSize - mWriteIndex;
    }

    // Write n bytes
    bool Write(const void* p, size_t n);
    bool Write(const void* p, size_t n, const char delim);
    bool Write(const void* p, size_t n, const char* delim);

    // Write with only size of bytes
    // move forward write position only
    bool Write(size_t n)
    {
        if(mSize < mWriteIndex + n)
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
    void* Write()
    {
        return mBytes + mWriteIndex;
    }
    
    // const pointer used as a position
    const void* Write() const
    {
        return mBytes + mWriteIndex;
    }

    // The available bytes to read
    size_t Readable() const
    {
        return mWriteIndex - mReadIndex;
    }

    // Readable before the delimit
    ssize_t Readable(const char delim, size_t offset = 0);
    ssize_t Readable(const char* delim, size_t offset = 0);

    // Read with buffer and size
    // actual read and move forward read index
    bool Read(void* p, size_t n);

    // Read with only data size 
    // move forward read position only
    bool Read(size_t n)
    {
        if(mWriteIndex < mReadIndex + n)
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
        return mBytes + mReadIndex;
    }

    void* Read() 
    {
        return mBytes + mReadIndex;
    }

    bool Peek(void* p, size_t n, size_t offset = 0);

    // Peek data in buffer on given offset from reading position 
    // return first byte position of peek for external use
    const void* Peek(size_t offset = 0) const
    {
        return mReadIndex + offset < mWriteIndex ? mBytes + mReadIndex + offset : NULL;
    }

    // for update
    void* Peek(size_t offset)
    {
        return mReadIndex + offset < mWriteIndex ? mBytes + mReadIndex + offset : NULL;
    }

    // update bytes
    bool Update(void* p, size_t n, size_t offset = 0);

private:
    bool mOwn;
    size_t mSize;           // Allocated size of memory block
    unsigned char* mBytes;  // Pointer of the memory block
    size_t mReadIndex;      // Index of first byte of data
    size_t mWriteIndex;     // Index of next byte of last data byte
};

NET_BASE_END

#endif

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

#ifndef NET_BASE_BYTE_STREAM_HPP
#define NET_BASE_BYTE_STREAM_HPP

#include "Config.hpp"
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
class ByteStream
{
public:
    virtual ~ByteStream() { }

    // Is empty?
    virtual bool Empty() const = 0;

    // Clear the buffer
    virtual void Clear() = 0;

    // Size of the stream
    // Readable size  and writable size
    virtual size_t Size() const = 0;

    // Check and try to get available space to write
    virtual bool Writable(size_t n) = 0;

    // Check available space to write
    virtual size_t Writable() const = 0;

    // Pointer to write position
    virtual const void* Write() const = 0;
    virtual void* Write() = 0;

    // Write with only size of bytes
    // move forward write position only
    virtual bool Write(size_t n) = 0;

    // Write with buffer and size
    // Actuallly write n bytes and move forward position
    virtual bool Write(const void* p, size_t n) = 0;

    // Write n bytes with delimit char or string
    virtual bool Write(const void* p, size_t n, const char delim) = 0;
    virtual bool Write(const void* p, size_t n, const char* delim) = 0;

    // Available bytes to read
    virtual size_t Readable(size_t offset = 0) const = 0;

    // Available bytes to read before the delimit char or string
    virtual ssize_t Readable(const char delim, size_t offset = 0) const = 0;
    virtual ssize_t Readable(const char* delim, size_t offset = 0) const = 0;

    // Pointer to read position
    virtual const void* Read() const = 0;
    virtual void* Read() = 0;

    // Read with only data size 
    // move forward read position only
    virtual bool Read(size_t n) = 0;
    virtual bool Remove(size_t n) = 0;

    // Read with buffer and size
    // actual read and move forward read index
    virtual bool Read(void* p, size_t n) = 0;

    // Random position for peek or update
    virtual const void* Peek(size_t offset = 0) const = 0;
    virtual void* Peek(size_t offset) = 0;

    // Peek data
    virtual bool Peek(void* p, size_t n, size_t offset = 0) = 0;

    // Update bytes
    virtual bool Update(void* p, size_t n, size_t offset = 0) = 0;
};

NET_BASE_END

#endif

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

NET_BASE_BEGIN

//
// It is a stream, and
// it is a buffer.
// 
class StreamBuffer
{
public:
    virtual ~StreamBuffer() { }

    // Readable() + Writable()
    virtual size_t Size() const = 0;

    // Readable() == 0 ?
    virtual bool Empty() const = 0;

    // Set Empty() == true
    virtual void Clear() = 0;

    // Available space to write
    virtual size_t Writable() const = 0;

    // Ensure Writable() == n
    virtual bool Writable(size_t n) = 0;

    // Pointer to write position 
    // For external use, such as directly copy data into the buffer 
    virtual const void* Write() const = 0;
    virtual void* Write() = 0;

    // Virtually write, move write position forward
    virtual bool Write(size_t n) = 0;

    // Actually write, copy data into the buffer and move write position forward
    virtual bool Write(const void* p, size_t n) = 0;
    virtual bool Write(const void* p, size_t n, const char delim) = 0; // Append delimit char
    virtual bool Write(const void* p, size_t n, const char* delim) = 0; // Append delimit string

    // Available data to read
    virtual size_t Readable() const = 0;
    virtual ssize_t Readable(const char delim) const = 0; // Before next delimit char
    virtual ssize_t Readable(const char* delim) const = 0; // Before next delimit string

    // Pointer to read position
    // For external use, such as directly copy data from the buffer
    virtual const void* Read() const = 0;
    virtual void* Read() = 0;

    // Virtually read, move read position forward
    virtual bool Read(size_t n) = 0;
    virtual bool Remove(size_t n) = 0;

    // Actually read, copy data from the buffer and move read position forward
    virtual bool Read(void* p, size_t n) = 0;

    // Available data to peek, using offset for random access
    virtual ssize_t Peekable(size_t offset = 0) const = 0;
    virtual ssize_t Peekable(const char delim, size_t offset = 0) const = 0; // Before next delimit char
    virtual ssize_t Peekable(const char* delim, size_t offset = 0) const = 0; // Before next delimit string

    // Pointer to peek position, using offset for random access
    // For external use, such as directly copy data from the buffer
    virtual const void* Peek(size_t offset = 0) const = 0;
    virtual void* Peek(size_t offset = 0) = 0;

    // Peek data, copy data from the buffer but not move read position
    // using offset for random access
    virtual bool Peek(void* p, size_t n, size_t offset = 0) = 0;

    // Replace data, data in the buffer is covered
    // using offset for random access
    virtual bool Replace(void* p, size_t n, size_t offset = 0) = 0;
};

NET_BASE_END

#endif

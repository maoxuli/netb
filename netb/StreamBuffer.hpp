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

#ifndef NETB_STREAM_BUFFER_HPP
#define NETB_STREAM_BUFFER_HPP

#include "Config.hpp"
#include <cstddef>
#include <vector>

// Default buffer size, can be defined in Config.hpp
#if !defined(DEFAULT_BUFFER_SIZE)
#define DEFAULT_BUFFER_SIZE     4096 // 4K bytes, enough for two MTU
#endif

// Max buffer size, can be defined in Config.hpp
#if !defined(MAX_BUFFER_SIZE)
#define MAX_BUFFER_SIZE        (1024 * 1024) // 1M bytes
#endif

NETB_BEGIN

//
// StreamBuffer is a byte buffer that supports streaming-like read and write, 
// as well as randomly peek and update. 
//
class StreamBuffer
{
public:
    virtual ~StreamBuffer();

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
        _bytes.swap(b._bytes);
        std::swap(_limit, b._limit);
        std::swap(_read_index, b._read_index);
        std::swap(_write_index, b._write_index);
        return *this;
    }

    StreamBuffer& Swap(StreamBuffer* b)
    {
        _bytes.swap(b->_bytes);
        std::swap(_limit, b->_limit);
        std::swap(_read_index, b->_read_index);
        std::swap(_write_index, b->_write_index);
        return *this;
    }

    // Readable() + Writable()
    size_t Size() const 
    {
        return _bytes.size() - _read_index;
    }

    // Readable() == 0 ?
    bool Empty() const 
    {
        return _write_index == _read_index;
    }

    // Set Empty() == true
    void Clear()
    {
        _read_index = 0;
        _write_index = 0; 
    }

    // Available space to write
    size_t Writable() const
    {
        return _bytes.size() - _write_index;
    }

    // Try to ensure Writable() >= n
    bool Writable(size_t n)
    {
        if(n == 0 || _bytes.size() - _write_index >= n)
        {
            return true;
        }
        if(_limit > 0) 
        {
            if(_write_index - _read_index + n > _limit) // the buffer is overflow
            {
                return false;
            } 

            if(_write_index + n > _limit) // Occupancy is beyond limit, compact
            {
                Compact();
            }
        } 
        _bytes.resize(_write_index + n);
        return _bytes.size() - _write_index >= n;
    }

    // Pointer to write position 
    // For external use, such as directly copy data into the buffer 
    const void* Write() const
    {
        return Begin() + _write_index;
    }

    void* Write()
    {
        return Begin() + _write_index;
    }

    // Virtually write, move write position forward
    bool Write(size_t n)
    {
        if(_bytes.size() - _write_index < n)
        {
            return false;
        }
        _write_index += n;
        return true;
    }

    // Actually write, copy data into the buffer and move write position forward
    bool Write(const void* p, size_t n);
    bool Write(const void* p, size_t n, const char delim); // Append delimit char
    bool Write(const void* p, size_t n, const char* delim); // Append delimit string

    // Available data to read
    size_t Readable() const
    {
        return _write_index - _read_index;
    }

    // Check if Readable() >= n
    bool Readable(size_t n) const
    {
        return _write_index - _read_index >= n;
    }

    // Available data to read before next delimit char or string
    ssize_t Readable(const char* delim) const; 

    // Pointer to read position
    // For external use, such as directly copy data from the buffer
    const void* Read() const
    {
        return Begin() + _read_index;
    }

    void* Read() 
    {
        return Begin() + _read_index;
    }

    // Virtually read, move read position forward
    bool Read(size_t n)
    {
        if(_write_index - _read_index < n)
        {
            return false;
        }
        _read_index += n;
        if(_read_index == _write_index)
        {
            _read_index = 0;
            _write_index = 0;
        }
        return true;
    }

    // Actually read, copy data from the buffer and move read position forward
    bool Read(void* p, size_t n);

    // Addressable data from position that offset to the reading position
    ssize_t Addressable(size_t offset = 0) const
    {
        return _write_index - _read_index - offset;
    }

    // Addressable data from position that offset to the reading position 
    // to next delimit char or string
    ssize_t Addressable(const char* delim, size_t offset = 0) const; 

    // Pointer to position that offset to the reading position 
    // For external use, such as peek or update data in buffer 
    const void* Address(size_t offset = 0) const
    {
        return _write_index - _read_index < offset ? nullptr : Begin() + _read_index + offset;
    }

    void* Address(size_t offset = 0) 
    {
        return _write_index - _read_index < offset ? nullptr : Begin() + _read_index + offset;
    }

    // Peek data at position that offset to the reading position
    // Copy data from buffer but not affect reading and writing position
    bool Peek(void* p, size_t n, size_t offset = 0);

    // Update data at position that offset to the reading position
    // Update data in buffer but not affect reading and writing position
    bool Update(void* p, size_t n, size_t offset = 0);

private:
    
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
    //      Begin() _read_index              _write_index                  
    //
    //
    
    std::vector<unsigned char> _bytes;
    size_t _limit; // Limit of the max footprint of the buffer
    size_t _read_index; // Index of first readable byte
    size_t _write_index; // Index of first writable byte

    const unsigned char* Begin() const
    {
        return &_bytes[0];
    }

    unsigned char* Begin()
    {
        return &_bytes[0];
    }

    // Move data to the beginning
    void Compact()
    {
        std::rotate(_bytes.begin(), _bytes.begin() + _read_index, 
                    _bytes.begin() + _write_index - _read_index);
        _write_index -= _read_index;
        _read_index = 0;
    }
};

NETB_END

#endif

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

// Default buffer size
// can be defined in Config.hpp
#if !defined(DEFAULT_BUFFER_SIZE)
#define DEFAULT_BUFFER_SIZE     4096 // 4K bytes, enough for two MTU
#endif

// Max buffer size
// can be defined in Config.hpp
// used to limit the memory occupancy
#if !defined(MAX_BUFFER_SIZE)
#define MAX_BUFFER_SIZE        (1024 * 1024) // 1M bytes
#endif

NETB_BEGIN

//
// It is eighter a stream or a buffer; It is neighter a stream nor a buffer; 
// It is StreamBuffer.
// 
// StreamBuffer is a byte buffer supports both sequnential reading and 
// random peeking/updating. 
//
class StreamBuffer
{
public:
    // Initialize with initial size and memory limit
    StreamBuffer(size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Initialize with initial data, initial size and memory limit
    StreamBuffer(const void* p, size_t n, size_t init = DEFAULT_BUFFER_SIZE, size_t limit = MAX_BUFFER_SIZE);

    // Copy constructor
    // Discard the flushed data
    StreamBuffer(const StreamBuffer& b);
    explicit StreamBuffer(const StreamBuffer* b);

    // Assignment operator
    // Discard the flushed data
    StreamBuffer& operator=(const StreamBuffer& b);

    // Swap, no copy
    StreamBuffer& Swap(StreamBuffer& b)
    {
        _bytes.swap(b._bytes);
        std::swap(_limit, b._limit);
        std::swap(_opos, b._opos);
        std::swap(_rpos, b._rpos);
        std::swap(_wpos, b._wpos);
        return *this;
    }

    // Swap, no copy 
    StreamBuffer& Swap(StreamBuffer* b)
    {
        _bytes.swap(b->_bytes);
        std::swap(_limit, b->_limit);
        std::swap(_opos, b->_opos);
        std::swap(_rpos, b->_rpos);
        std::swap(_wpos, b->_wpos);
        return *this;
    }

    // the size of the buffer include the length accessible data 
    // and the length of writable space
    size_t Size() const 
    {
        return _bytes.size() - _opos;
    }

    // empty buffer is a buffer with no accessible data 
    bool Empty() const 
    {
        return _opos == _wpos;
    }

    // clear accessible data in the buffer
    // to make the buffer empty
    void Clear()
    {
        _wpos = _rpos = _opos = 0;
    }

    // accessible data in buffer may be dicarded for any reason 
    // it is determined externally when and how much data is not necessary 
    // to keep any more and should be discarded
    ssize_t Flush(size_t n)
    {
        if(n > _wpos - _opos) return -1;
        _opos += n;
        if(_opos == _wpos)
        {
            _wpos = _rpos = _opos = 0;
        }
        return _wpos - _opos;
    }

    // discard data before current sequential reading position 
    // it is externally determined when the data before current reading position 
    // is not necessary to keep any more and should be discarded.
    ssize_t Flush() 
    {
        _opos = _rpos;
        if(_opos == _wpos)
        {
            _wpos = _rpos = _opos = 0;
        }
        return _wpos - _opos;
    }

    // Dump for debug
    void Dump() const 
    {
        std::cout << "opos:" << _opos << "\n";
        std::cout << "rpos:" << _rpos << "\n";
        std::cout << "wpos:" << _wpos << "\n";
        for(int i = _opos; i < _wpos; i++)
        {
            std::cout << i << ":" << (int)(*(Begin() + i)) << " ";
        }
        std::cout << std::endl;
    }

    // Sequential writing is the basic feature of a data buffer. 
    // data is always written from the next byte of last written byte. 
 
    // return the length of writable space, which is the range between 
    // _wpos position and the end of the container
    size_t Writable() const
    {
        return _bytes.size() - _wpos;
    }

    // check if the writalbe space larger than given length, 
    // if not, try to resize the buffer to enlarge the writable space.
    bool Writable(size_t n)
    {
        if(n == 0 || _bytes.size() - _wpos >= n) return true;
        if(_limit > 0) 
        {
            if(_wpos - _opos + n > _limit) return false; // the buffer is overflow
            if(_wpos + n > _limit) // Occupancy is beyond limit, reclaim flushed memory
            {
                Reclaim();
            }
        } 
        _bytes.resize(_wpos + n);
        return _bytes.size() - _wpos >= n;
    }

    // return the memory pointer of writable space 
    // may be used for external writing of data into the buffer
    // return also current relative position indicated by offset
    const void* Write(size_t* offset = 0) const
    {
        if(offset) *offset = _wpos;
        return Begin() + _wpos;
    }

    // return the memory pointer of writable space
    // may be used for external writing of data into the buffer
    // return also current relative position indicated by offset
    void* Write(size_t* offset = 0)
    {
        if(offset) *offset = _wpos;
        return Begin() + _wpos;
    }

    // virtually write, move the write position forward
    // usually used after external writing to adjust write position
    // return also current relative position indicated by offset
    bool Write(size_t n, size_t* offset = 0)
    {
        if(_wpos + n > _bytes.size()) return false;
        if(offset) *offset = _wpos;
        _wpos += n;
        return true;
    }

    // actually write data into buffer
    // copy data into the buffer and move write position forward
    // return also current relative position indicated by offset
    bool Write(const void* p, size_t n, size_t* offset = 0);

    // actually write data into buffer
    // append a delimit char or string as the boundary of the data
    bool Write(const void* p, size_t n, const char delim, size_t* offset = 0); 
    bool Write(const void* p, size_t n, const char* delim, size_t* offset = 0);

    // Sequential reading is an usual way to access data in the buffer. 
    // the accessible data is the range between _opos position and _wpos 
    // position. _rpos is used internally to keep current reading position. 
    // Sequential reading can be started over by moving current reading 
    // position back to the first byte of accessible data. And at any point 
    // of sequential reading, the data before current reading position can 
    // be discarded (flushed away), so that it is not accessible any more. 
    
    // return the length of accessible data 
    // after current sequential reading position
    size_t Readable() const
    {
        return _wpos - _rpos;
    }

    // return the length of accessible data 
    // after current sequential reading position and before next delimit character
    // return -1 if the delim is not found
    ssize_t Readable(const char delim) const;

    // return the length of accessible data 
    // after current sequential reading position  and before next delimit string
    // return -1 if the delim is not found
    ssize_t Readable(const char* delim) const; 

    // return the memory pointer of current sequential reading position
    // may be used for external sequential reading data from the buffer
    // return also current relative position indicated by offset
    const void* Read(size_t* offset = 0) const
    {
        if(offset) *offset = _rpos;
        return Begin() + _rpos;
    }

    // return the memory pointer of current sequential reading position
    // may be used for external sequential reading data from the buffer
    void* Read(size_t* offset = 0) 
    {
        if(offset) *offset = _rpos;
        return Begin() + _rpos;
    }

    // virtual sequentila reading, move read position forward
    // usually used after external sequential reading
    bool Read(size_t n, size_t* offset = 0)
    {
        if(_rpos + n > _wpos) return false;
        if(offset) *offset = _rpos;
        _rpos += n;
        return true;
    }

    // actual seqential reading data from buffer
    // copy data from the buffer and move read position forward
    bool Read(void* p, size_t n, size_t* offset = 0);

    // Sometimes the data in buffer is not used sequentially so the sequential 
    // reading is not enough for access of buffer data. Random access is reading or 
    // changing some data at a given position of the accessible data. The accessible 
    // data between the range of _opos and _wpos is referred as peekable data in terms 
    // of random access. An offset is used in random access functions to indicate the 
    // random position in accessible data, which refers the number of bytes off the 
    // first byte of accessible data. Peeking is reading some accessible data from 
    // the random position and updating is changing some addressable data from the 
    // random position.

    // return the length of peekable data starting from offset position
    // return -1 if the offset position is out of range of peekable data
    ssize_t Peekable(size_t offset = 0) const
    {
        if(offset > _wpos - _opos) return false;
        return _wpos - _opos - offset;
    }

    // return the length of peekable data with offset and before the delimit character
    // return -1 if the offset position is out of range of peekable data or delim is not found
    ssize_t Peekable(size_t offset, const char delim) const;

    // return the length of peekable data with offset and before the delimit string
    // return -1 if the offset position is out of range of peekable data or delim is not found
    ssize_t Peekable(size_t offset, const char* delim) const;

    // Memory pointer of peekable position with offset to the first byte of peekable data
    // this pointer can be used for externally access of the peekable data
    const void* Peek(size_t offset = 0) const
    {
        if(offset > _wpos - _opos) return 0;
        return Begin() + _opos + offset;
    }

    void* Peek(size_t offset = 0)
    {
        if(offset > _wpos - _opos) return 0;
        return Begin() + _opos + offset;
    }

    // Peek the peekable data at offset position 
    bool Peek(size_t offset, void* p, size_t n);

    // Update the peekable data at offset position
    // this function must be used when you know what you are doing
    // improperly update may destroy the data structure in the buffer
    bool Update(size_t offset, const void* p, size_t n);

private:
    
    //
    // std::vector is used as a resizing bytes container. The buffer is the range from 
    // an origin position (_opos) to the end of vector. The range between the beginning
    // of the vector and the origin position is the data that has ever been written into 
    // the buffer but now has been discarded (flushed away). _wpos is the next byte of 
    // last written byte, so the range between _wpos and the end of the vector is the 
    // writable space. The range between _opos and _wpos is the data that has been 
    // written into the buffer and has not been flushed away, i.e. the accessible data 
    // in the buffer. Sequential reading of the accessible data is supported by the 
    // buffer, _rpos is used to indicate current reading position. So the range between 
    // _rpos and _wpos is the readable data. 
    //   
    //
    //        |                       -size()-                           |
    // vector |##########################################################|..........|
    //      begin()                                                     end()   capacity()
    //
    //
    //
    //         [Flushed]|                    -Size()-                    |
    // Buffer |.........|**************|xxxxxxxxxxxxxxxx|----------------|..........|
    //        |         |              |  -Readable()-  |  -Writable()-  |
    //     Begin()    _opos          _rpos            _wpos                 
    //
    //
    
    // bytes container
    std::vector<unsigned char> _bytes;
    size_t _limit;  // limit of the max memory occupancy
    size_t _opos;   // origin position
    size_t _rpos;   // sequential reading position 
    size_t _wpos;   // writing position

    // pointer of the first byte of the bytes container
    const unsigned char* Begin() const
    {
        return &_bytes[0];
    }

    // pointer of the first byte of the bytes container
    unsigned char* Begin()
    {
        return &_bytes[0];
    }

    // the range between the beginning of the container and _opos is not used by 
    // the buffer anymore but it occupies the memory. So in an approperiate time 
    // we need to reclaim this memory by moving accessible data between _opos and 
    // _wpos to the beginning of the container. 
    void Reclaim()
    {
        std::rotate(_bytes.begin(), _bytes.begin() + _opos, 
                    _bytes.begin() + _wpos - _opos);
        _wpos -= _opos;
        _rpos -= _opos;
        _opos = 0;
    }
};

NETB_END

#endif

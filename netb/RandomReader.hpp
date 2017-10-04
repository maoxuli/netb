/*
 * Copyright (C) 2010-2016, Maoxu Li. http://maoxuli.com/dev
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

#ifndef NETB_RANDOM_READER_HPP
#define NETB_RANDOM_READER_HPP

#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// RandomReader is the wrapper of random peeking interface of 
// a StreamBuffer object.
//
class RandomReader : private Uncopyable
{
public:
    RandomReader();
    explicit RandomReader(StreamBuffer* buf, size_t offset = 0);
    explicit RandomReader(StreamBuffer& buf, size_t offset = 0); 
    virtual ~RandomReader();

    RandomReader& Attach(StreamBuffer* buf, size_t offset = 0);
    RandomReader& Attach(StreamBuffer& buf, size_t offset = 0); 

    StreamBuffer* Buffer() const { return _stream; }

    // Peek n bytes at offset position
    bool Bytes(size_t offset, void* p, size_t n) const;

    // Peek integer at offset position
    // with given integer type (with indicated length)
    bool Integer(size_t offset, int8_t& v) const;
    bool Integer(size_t offset, uint8_t& v) const;
    bool Integer(size_t offset, int16_t& v) const;
    bool Integer(size_t offset, uint16_t& v) const;
    bool Integer(size_t offset, int32_t& v) const;
    bool Integer(size_t offset, uint32_t& v) const;
    bool Integer(size_t offset, int64_t& v) const;
    bool Integer(size_t offset, uint64_t& v) const;

    // Peek at offset position
    // bool, float, double (with default length)
    // Todo: compatible formats in memory
    bool Bool(size_t offset, bool& v) const;
    bool Float(size_t offset, float& v) const;
    bool Double(size_t offset, double& v) const;

    // Peek string
    // include all peekable data
    bool String(size_t offset, std::string& s) const;
    
    // Peek string 
    // that is n bytes length
    bool String(size_t offset, std::string& s, size_t n) const;

    // Peek string
    // to delimit character (e.g. '\0')
    bool String(size_t offset, std::string& s, const char delim) const;
    
    // Peek string
    // to delimit string (e.g. '\r\n')
    bool String(size_t offset, std::string& s, const char* delim) const;

protected:
    StreamBuffer* _stream;
    size_t _offset;
};

NETB_END

#endif

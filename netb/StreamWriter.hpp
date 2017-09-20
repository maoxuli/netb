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

#ifndef NETB_STREAM_WRITER_HPP
#define NETB_STREAM_WRITER_HPP

#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// StreamWriter is the wrapper of sequential writing interface of 
// a StreamBuffer object. 
//
class StreamWriter : private Uncopyable
{
public:
    StreamWriter();
    explicit StreamWriter(StreamBuffer* buf, bool clear = false);
    explicit StreamWriter(StreamBuffer& buf, bool clear = false);
    virtual ~StreamWriter();

    StreamWriter& Attach(StreamBuffer* buf, bool clear = false);
    StreamWriter& Attach(StreamBuffer& buf, bool clear = false);

    StreamBuffer* Buffer() const { return _stream; }

    // write n bytes
    bool Bytes(const void* p, size_t n, size_t* offset = 0) const;

    // write integer 
    // integer with given type (with indicated length) 
    // Todo: endianess concerns for multi-bytes types 
    bool Integer(int8_t v, size_t* offset = 0) const;
    bool Integer(uint8_t v, size_t* offset = 0) const;
    bool Integer(int16_t v, size_t* offset = 0) const;
    bool Integer(uint16_t v, size_t* offset = 0) const;
    bool Integer(int32_t v, size_t* offset = 0) const;
    bool Integer(uint32_t v, size_t* offset = 0) const;
    bool Integer(int64_t v, size_t* offset = 0) const;
    bool Integer(uint64_t v, size_t* offset = 0) const;

    // write bool, float, double (with default length)
    // Todo: compatible format in memory
    bool Bool(bool v, size_t* offset = 0) const;
    bool Float(float v, size_t* offset = 0) const;
    bool Double(double v, size_t* offset = 0) const;

    // write string
    // all data in the string
    bool String(const std::string& s, size_t* offset = 0) const;

    // write string
    // append delimit character (e.g. '\0')
    bool String(const std::string& s, const char delim, size_t* offset = 0) const;

    // write string
    // append delimit string (e.g. "\r\n")
    bool String(const std::string& s, const char* delim, size_t* offset = 0) const;

protected:
    StreamBuffer* _stream;
};

NETB_END

#endif

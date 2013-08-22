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

#ifndef NET_BASE_STREAM_WRITER_HPP
#define NET_BASE_STREAM_WRITER_HPP

#include "StreamBuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

NET_BASE_BEGIN

//
// StreamReader and StreamWriter are designed as tool classes to read from and 
// write to a StreamBuffer. The reader and writer work on a buffer object but 
// not own the buffer. 
// Reader read data from the buffer sequentially, and writer write data to the 
// buffer sequentially. In terms of protocol message packaging, these operations 
// are kind of serialization.
// Serialization read data from buffer and assign the value to variables in 
// protocol message packet, while write the value of variables into buffer. 
// Reader and Writer support serialization of same types of variables, and 
// particularly, the operations of reading and writing have compatable names, 
// i.e., SerializeXXX(). These may let protocol message packet using a some function 
// to read from buffer or write to buffer, which determined by using StreamReader 
// or StreamWriter.  
//
class StreamWriter
{
private:
    StreamWriter(const StreamWriter*);
    StreamWriter(const StreamWriter&);
    void operator=(const StreamWriter&);

public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamWriter();
    StreamWriter(StreamBuffer* buf);
    StreamWriter(StreamBuffer& buf);
    ~StreamWriter();

    StreamWriter& Attach(StreamBuffer* buf);
    StreamWriter& Attach(StreamBuffer& buf);

    // Write the n bits from low end of the buffer
    bool SerializeBits(const void* p, size_t n);

    // n bytes
    bool SerializeBytes(const void* p, size_t n);

    // Bytes for integer, determined by integer type
    // Todo: endianess concerns for multi-bytes types 
    bool SerializeInteger(int8_t v);
    bool SerializeInteger(uint8_t v);
    bool SerializeInteger(int16_t v);
    bool SerializeInteger(uint16_t v);
    bool SerializeInteger(int32_t v);
    bool SerializeInteger(uint32_t v);
    bool SerializeInteger(int64_t v);
    bool SerializeInteger(uint64_t v);

    // Bytes for bool, float, and double
    // Todo: presentation formats for these types
    bool SerializeBool(bool b);
    bool SerializeFloat(float v);
    bool SerializeDouble(double v);

    // Writer n bytes of string from the begining
    bool SerializeString(const std::string& s, size_t n);

    // Write bytes in a string (not include terminate \0), plus a delimit character
    bool SerializeString(const std::string& s, const char delim);

    // Write bytes in a string (not include terminate \0), plus a delimit string
    bool SerializeString(const std::string& s, const char* delim);

private:
    StreamBuffer* mStream;
};

NET_BASE_END

#endif

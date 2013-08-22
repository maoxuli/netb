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

#ifndef NET_BASE_STREAM_PEEKER_HPP
#define NET_BASE_STREAM_PEEKER_HPP

#include "StreamBuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

NET_BASE_BEGIN

//
// StreamPeeker
//
class StreamPeeker
{
private:
    StreamPeeker(const StreamPeeker*);
    StreamPeeker(const StreamPeeker&);
    StreamPeeker& operator=(const StreamPeeker&);

public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamPeeker();
    StreamPeeker(StreamBuffer* buf);
    StreamPeeker(StreamBuffer& buf);
    ~StreamPeeker();

    StreamPeeker& Attach(StreamBuffer* buf);
    StreamPeeker& Attach(StreamBuffer& buf);

    // Peek n bits from the low end
    bool SerializeBits(void* p, size_t n) { return false; }

    // n bytes
    bool SerializeBytes(void* p, size_t n);

    // Bytes for integer, determined by integer type
    // Todo: endianess concerns for multi-bytes types 
    bool SerializeInteger(int8_t& v);
    bool SerializeInteger(uint8_t& v);
    bool SerializeInteger(int16_t& v);
    bool SerializeInteger(uint16_t& v);
    bool SerializeInteger(int32_t& v);
    bool SerializeInteger(uint32_t& v);
    bool SerializeInteger(int64_t& v);
    bool SerializeInteger(uint64_t& v);

    // Bytes for bool, float, and double
    // Todo: presentation formats for these types
    bool SerializeBool(bool& b);
    bool SerializeFloat(float& v);
    bool SerializeDouble(double& v);

    // Read a string that has n bytes length
    bool SerializeString(std::string& s, size_t n);

    // Read a string, with bytes before a delimit character
    bool SerializeString(std::string& s, const char delim);

    // Read a string, with bytes before a delimit string
    bool SerializeString(std::string& s, const char* delim);

private:
    StreamBuffer* mStream;
    size_t mOffset;
};

NET_BASE_END

#endif

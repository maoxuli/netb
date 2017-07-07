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

#include "Uncopyable.hpp"
#include "ByteStream.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

NET_BASE_BEGIN

//
// ByteStream is designed as the major interface to hold and transfer data. The 
// implementation objects include ByteBuffer and ByteWrapper. The former owns a 
// internal memory block, while the latter hold a pointer to a external memory 
// block. ByteStream interface defined functions to 'streamingly' read and write 
// data, as well as 'randomly' peek and update data. All the functions work on 
// 'byte' level, which means the functions always manipulate one or more bytes 
// per calling. This is inconvenient in some cases, e.g. we may need to read or 
// write a 'integer number', a 'float number', or a 'string', from or to the 
// stream buffer. Then, we have to translate all those known 'data type' to kind 
// of 'byte sequence' every time using the interface. 
// 
// StreamReader, StreamWriter, StreamPeeker, and maybe other objects are designed 
// as tool object to facilitate reading and writing known 'data type' from and to
// a BytemStream object. These tool objects work on a existing ByteStream object, 
// i.e. holding a pointer to the external object, hence need the external object 
// keep valid during all operations. 
//
// These tool objects are designed to have 'compatible' inteface so that protocol 
// data may be packed or unpacked from buffer based on same code, as below:
//
// class Packet 
// {
// private: 
//     int a;
//     float b;
//     bool c;
//     std::string d;
//     unsigned char data[100];
//     unsigned short data_len;
// 
// public:
//     void ToStream(ByteStream* stream)
//     {
//         Serialize(StreamWriter(stream));
//     }
//
//     void FromStream(ByteStream* stream)
//     {
//         Serialize(StreamReader(stream));
//     }
// 
//     template<type T> 
//     void Serialize(const T& serializer)
//     {
//         serializer.SerializeInteger(a);
//         serializer.SerializeFloat(b);
//         serializer.SerializeBool(c);
//         serializer.SerializeString(d);
//         serializer.SerializeBytes(data, data_len);
//      }
// };
//
// To ensure these strategie working properly, the interfaces of StreamReader, 
// StreamWriter, StreamPeeker and other possible objects must be 'compatible'.
// The means their member functions have 'compatible' signatures, and the known 
// 'data type' is writen into and read from the buffer using same rule. 
//
// StreamWriter is used to write data into the stream.
// StreamReader is used to read data from the stream.
// StreamPeeker is used to read data from the stream, but the data is not removed 
// from the stream, i.e., still kept in the stream. 
// 
class StreamPeeker : Uncopyable
{
public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamPeeker();
    StreamPeeker(ByteStream* buf);
    StreamPeeker(ByteStream& buf);
    ~StreamPeeker();

    StreamPeeker& Attach(ByteStream* buf);
    StreamPeeker& Attach(ByteStream& buf);

    // Peek n bits from the low end or current byte
    bool SerializeBits(void* p, size_t n);

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
    ByteStream* mStream;
    size_t mOffset;
};

NET_BASE_END

#endif

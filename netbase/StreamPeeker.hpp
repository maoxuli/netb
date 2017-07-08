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
#include "StreamBuffer.hpp"
#include <cstdint>
#include <string>

NET_BASE_BEGIN

//
// StreamBuffer is designed as the major interface to hold and transfer data. The 
// implementation objects include ByteBuffer and ByteWrapper. The former owns a 
// internal memory block, while the latter hold a pointer to a external memory 
// block. StreamBuffer interface defined functions to 'streamingly' read and write 
// data, as well as 'randomly' peek and update data. The interface of StreamBuffer 
// works on 'byte' level, which means the functions always manipulate one or more 
// bytes. This is inconvenient in some cases, for example, we may need to read or 
// write a 'integer number', a 'float number', or a 'string', from the buffer or 
// to the buffer. In those cases, we have to translate all known 'data type' to 
// a kind of 'byte sequence' to adapt to the StreamBuffer interface. 
// 
// StreamReader, StreamWriter, and StreamPeeker are designed as tool objects to 
// facilitate reading and writing known 'data type' from and to a StreamBuffer 
// object, which is also known as serialization in network protocol programming. 
// These tool objects work on a existing StreamBuffer object, holding a pointer 
// to the external object, hence need the external object keep valid during all 
// operations. 
//
// To make network protocol programming further easier, these tool objects are 
// designed to have 'compatible' inteface so that protocol data may be packed 
// and unpacked from StreamBuffer based on the same code, somehow as below:
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
//     void ToBuffer(StreamBuffer* buf)
//     {
//         Serialize(StreamWriter(buf));
//     }
//
//     void FromBuffer(StreamBuffer* buf)
//     {
//         Serialize(StreamReader(buf));
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
// To ensure this strategy working properly, the interfaces of StreamReader, 
// StreamWriter, StreamPeeker and other possible tool objects must be 'compatible'.
// That means their member functions have 'compatible' signatures, and the known 
// 'data type' is writen into and read from the buffer using same rule. 
//
// StreamWriter is used to write data into the buffer.
// StreamReader is used to read data from the buffer.
// StreamPeeker is used to read data from the buffer, but the data is not removed 
// from the buffer, i.e., still kept in the buffer. 
//
class StreamPeeker : Uncopyable
{
public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamPeeker();
    StreamPeeker(StreamBuffer* buf);
    StreamPeeker(StreamBuffer& buf);
    ~StreamPeeker();

    StreamPeeker& Attach(StreamBuffer* buf);
    StreamPeeker& Attach(StreamBuffer& buf);

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
    StreamBuffer* mStream;
    size_t mOffset;
};

NET_BASE_END

#endif

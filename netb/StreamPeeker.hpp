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

#ifndef NETB_STREAM_PEEKER_HPP
#define NETB_STREAM_PEEKER_HPP

#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"
#include <cstdint>
#include <string>

NETB_BEGIN

//
// StreamBuffer is designed as an object to hold and transfer data. It defined 
// functions to 'streamingly' read and write data, as well as 'randomly' peek 
// and update data. The interface of StreamBuffer works on 'byte' level, which 
// means the functions always take a byte sequece (one or more bytes) as input 
// or output parameters. This is inconvenient in some cases, for example, we 
// may need to write or read 'integer number', 'float number', or 'string text'
// to/from the buffer. In these cases, we have to translate the known 'data type' 
// to a kind of 'byte sequence' to adapt to StreamBuffer's interface. 
// 
// StreamWriter, StreamReader, and StreamPeeker are designed as tool classes to 
// facilitate reading and writing known 'data type' from and to a StreamBuffer 
// object, which is also known as serialization in network protocol programming. 
// These tool classes work on a existing StreamBuffer object, holding a pointer 
// to the external object, hence need the external object keep valid during all 
// operations. 
//
// To make network protocol programming further easier, these tool objects are 
// designed to have 'compatible' inteface so that protocol data may be packed 
// and unpacked to/from StreamBuffer based on the same code, somehow as below:
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
//         serializer.Integer(a);
//         serializer.Float(b);
//         serializer.Bool(c);
//         serializer.String(d);
//         serializer.Bytes(data, data_len);
//      }
// };
//
// To ensure this strategy working properly, the interfaces of StreamWriter, 
// StreamReader, and StreamPeeker must be 'compatible'. That means their member 
// functions have 'compatible' signatures, and the known 'data types' are writen 
// and read to/from the buffer using the same rules. 
//
// StreamWriter is used to write data into the buffer.
// StreamReader is used to read data from the buffer.
// StreamPeeker is used to peek data from the buffer, while keep data in buffer.
//
class StreamPeeker : private Uncopyable
{
public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamPeeker();
    explicit StreamPeeker(StreamBuffer* buf);
    explicit StreamPeeker(StreamBuffer& buf);
    virtual ~StreamPeeker();

    StreamPeeker& Attach(StreamBuffer* buf);
    StreamPeeker& Attach(StreamBuffer& buf);

    // Peek n bits from the low end or current byte
    bool Bits(void* p, size_t n);

    // n bytes
    bool Bytes(void* p, size_t n);

    // Bytes for integer, determined by integer type
    // Todo: endianess concerns for multi-bytes types 
    bool Integer(int8_t& v);
    bool Integer(uint8_t& v);
    bool Integer(int16_t& v);
    bool Integer(uint16_t& v);
    bool Integer(int32_t& v);
    bool Integer(uint32_t& v);
    bool Integer(int64_t& v);
    bool Integer(uint64_t& v);

    // Bytes for bool, float, and double
    // Todo: presentation formats for these types
    bool Bool(bool& b);
    bool Float(float& v);
    bool Double(double& v);

    // Read all as a string
    bool String(std::string& s);

    // Read a string that has n bytes length
    bool String(std::string& s, size_t n);

    // Read a string, with bytes before a delimit string
    bool String(std::string& s, const char* delim);

protected:
    StreamBuffer* _stream;
    size_t _offset;
};

NETB_END

#endif

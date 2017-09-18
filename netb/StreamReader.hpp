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

#ifndef NETB_STREAM_READER_HPP
#define NETB_STREAM_READER_HPP

#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// StreamReader is the wrapper of sequential reading interface of 
// a StreamBuffer object.
//
class StreamReader : private Uncopyable
{
public:
    StreamReader();
    explicit StreamReader(StreamBuffer* buf, bool reset = false);
    explicit StreamReader(StreamBuffer& buf, bool reset = false); 
    virtual ~StreamReader();

    StreamReader& Attach(StreamBuffer* buf, bool reset = false);
    StreamReader& Attach(StreamBuffer& buf, bool reset = false);

    StreamBuffer* Buffer() const { return _stream; }

    // Read n bytes
    bool Bytes(void* p, size_t n);

    // Read integer 
    // integer with given type (with indicated length) 
    // Todo: endianess concerns for multi-bytes types 
    bool Integer(int8_t& v);
    bool Integer(uint8_t& v);
    bool Integer(int16_t& v);
    bool Integer(uint16_t& v);
    bool Integer(int32_t& v);
    bool Integer(uint32_t& v);
    bool Integer(int64_t& v);
    bool Integer(uint64_t& v);

    // Read bool, float, double
    // Todo: compatible format in memory
    bool Bool(bool& v);
    bool Float(float& v);
    bool Double(double& v);

    // Read string
    // include all readalbe data 
    bool String(std::string& s);
    
    // Read string 
    // that is n bytes length
    bool String(std::string& s, size_t n);

    // Read string 
    // to delimit character ('\0')
    bool String(std::string& s, const char delim);

    // Read string 
    // to delimit string ('\r\n')
    bool String(std::string& s, const char* delim);

protected:
    StreamBuffer* _stream;
};

NETB_END

#endif

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

#ifndef NETB_RANDOM_WRITER_HPP
#define NETB_RANDOM_WRITER_HPP

#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"

NETB_BEGIN

//
// RandomWriter is the wrapper of random updating interface of 
// a StreamBuffer object. 
// 
class RandomWriter : private Uncopyable
{
public:
    RandomWriter();
    explicit RandomWriter(StreamBuffer* buf, size_t offset = 0);
    explicit RandomWriter(StreamBuffer& buf, size_t offset = 0);
    virtual ~RandomWriter();

    RandomWriter& Attach(StreamBuffer* buf, size_t offset = 0);
    RandomWriter& Attach(StreamBuffer& buf, size_t offset = 0);

    StreamBuffer* Buffer() const { return _stream; }

    // Update n bytes at offset position
    bool Bytes(size_t offset, const void* p, size_t n);

    // Update at offset position
    // integer number with given type (with indicated length) 
    bool Integer(size_t offset, int8_t v);
    bool Integer(size_t offset, uint8_t v);
    bool Integer(size_t offset, int16_t v);
    bool Integer(size_t offset, uint16_t v);
    bool Integer(size_t offset, int32_t v);
    bool Integer(size_t offset, uint32_t v);
    bool Integer(size_t offset, int64_t v);
    bool Integer(size_t offset, uint64_t v);

    // Update at offset position
    // bool, float, double (with default length)
    // Todo: compatible format in memory
    bool Bool(size_t offset, bool v);
    bool Float(size_t offset, float v);
    bool Double(size_t offset, double v);

    // Update string at offset position
    // all data in string
    bool String(size_t offset, const std::string& s);

    // Update string at offset position
    // append delimit character (e.g. '\0')
    bool String(size_t offset, const std::string& s, const char delim);

    // Update string at offset position
    // append delimit string (e.g. "\r\n")
    bool String(size_t offset, const std::string& s, const char* delim);

protected:
    StreamBuffer* _stream;
    size_t _offset;
};

NETB_END

#endif

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

#include "StreamReader.h"
#include <cassert>

NET_BASE_BEGIN

StreamReader::StreamReader()
: mStream(NULL)
{

}

StreamReader::StreamReader(StreamBuffer* buf)
: mStream(buf)
{

}

StreamReader::StreamReader(StreamBuffer& buf)
: mStream(&buf)
{

}

StreamReader::~StreamReader()
{

}

StreamReader& StreamReader::Attach(StreamBuffer* buf)
{
    mStream = buf;
    return *this;
}

StreamReader& StreamReader::Attach(StreamBuffer& buf)
{
    mStream = &buf;
    return *this;
}

bool StreamReader::SerializeBytes(void* p, size_t n)
{
    if(mStream == NULL) return false;
    if(mStream->Readable() < n) return false;

    return mStream->Read(p, n);
}

bool StreamReader::SerializeString(std::string& s, size_t n)
{
    if(mStream == NULL) return false;
    if(mStream->Readable() < n) return false;

    const unsigned char* p = (const unsigned char*)mStream->Read();
    std::string(p, p + n).swap(s);
    mStream->Read(n);
    return true;
}

bool StreamReader::SerializeString(std::string& s, const char delim)
{
    if(mStream == NULL) return false;
    if(mStream->Readable() < sizeof(delim)) return false;

    ssize_t n = mStream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;

    const unsigned char* p = (const unsigned char*)mStream->Read();
    std::string(p, p + n).swap(s);
    mStream->Read(n + sizeof(delim));
    return true;
}

bool StreamReader::SerializeString(std::string& s, const char* delim)
{
    if(mStream == NULL) return false;
    if(mStream->Readable() < strlen(delim)) return false;

    ssize_t n = mStream->Readable(delim);
    if(n < 0) return false;
    if(n == 0) return true;
    
    const unsigned char* p = (const unsigned char*)mStream->Read();
    std::string(p, p + n).swap(s);
    mStream->Read(n + strlen(delim));
    return true;
}

NET_BASE_END

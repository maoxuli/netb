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

#include "StreamWriter.h"
#include <cassert>

NET_BASE_BEGIN

StreamWriter::StreamWriter()
: mStream(NULL)
{

}

StreamWriter::StreamWriter(StreamBuffer* buf)
: mStream(buf)
{

}

StreamWriter::StreamWriter(StreamBuffer& buf)
: mStream(&buf)
{

}

StreamWriter::~StreamWriter()
{

}

StreamWriter& StreamWriter::Attach(StreamBuffer* buf)
{
    mStream = buf;
    return *this;
}

StreamWriter& StreamWriter::Attach(StreamBuffer& buf)
{
    mStream = &buf;
    return *this;
}

bool StreamWriter::SerializeBytes(const void* p, size_t n)
{
    if(mStream == NULL) return false;
    return mStream->Write(p, n);
}

bool StreamWriter::SerializeString(const std::string& s, size_t n)
{
    if(mStream == NULL) return false;
    return mStream->Write(s.data(), n);
}

bool StreamWriter::SerializeString(const std::string& s, const char delim)
{
    if(mStream == NULL) return false;
    return mStream->Write(s.data(), s.length()) && 
           mStream->Write(&delim, sizeof(char));
}

bool StreamWriter::SerializeString(const std::string& s, const char* delim)
{
    if(mStream == NULL) return false;
    return mStream->Write(s.data(), s.length()) && 
           mStream->Write(delim, strlen(delim));
}

NET_BASE_END

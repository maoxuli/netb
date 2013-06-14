/*
 * Copyright (C) 2013, Maoxu Li. Email: maoxu@lebula.com
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

#include "HttpMessage.hpp"
#include "StreamBuffer.hpp"
#include "StreamReader.hpp"
#include "StreamWriter.hpp"
#include <sstream>
#include <cassert>

NET_BASE_BEGIN

const char* HttpMessage::CRLF = "\r\n";

HttpMessage::HttpMessage(const char* version) 
: mVersion(version)
, mBodyLen(0)
, mBody(NULL)
, mState(MSG_READY)
{

}

HttpMessage::~HttpMessage()
{
    Reset();
}

void HttpMessage::Reset()
{
    for(std::vector<Header*>::iterator it = mHeaders.begin(); it != mHeaders.end(); ++it)
    {
        Header* p = *it;
        delete p;
    }
    mHeaders.clear();

    if(!mBody)
    {
        delete[] mBody; 
        mBody = NULL;
        mBodyLen = 0;
    }

    mVersion = "HTTP/1.1";
    mState = MSG_READY;
}

void HttpMessage::SetHeader(const char* key, const char* value)
{
    std::vector<Header*>::iterator it(mHeaders.begin());
    while(it != mHeaders.end())
    {
        Header* p = *it;
        if(key == p->key)
        {
            p->value = value;
            return;
        }
        ++it;
    }

    mHeaders.push_back(new Header(key, value));
}

void HttpMessage::RemoveHeader(const char* key) 
{
    std::vector<Header*>::iterator it(mHeaders.begin());
    while(it != mHeaders.end())
    {
        Header* p = *it;
        if(key == p->key)
        {
            it = mHeaders.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

const char* HttpMessage::GetHeader(const char* key) const
{
    std::vector<Header*>::const_iterator it(mHeaders.begin());
    while(it != mHeaders.end())
    {
        Header* p = *it;
        if(key == p->key)
        {
            return p->value.c_str();
        }
        ++it;
    }

    return NULL;
}

long HttpMessage::GetHeaderAsInt(const char* key) const
{
    assert(false);
    return 0;
}

double HttpMessage::GetHeaderAsFloat(const char* key) const
{
    assert(false);
    return 0;
}

size_t HttpMessage::GetBodyLen() const
{
    return mBodyLen;
}

bool HttpMessage::GetBody(void* buf, size_t n) const
{
    if(mBodyLen > n)
    {
        return false;
    }

    memcpy(buf, mBody, mBodyLen);
    return true;
}

std::string HttpMessage::Dump() const 
{
    // <verb> SP <url> SP <protocol/version> CRLF
    // <headers> CRLF 
    // <buf>
    std::ostringstream oss;

    oss << StartLine() << "\r\n";

    for(std::vector<Header*>::const_iterator it = mHeaders.begin(); it != mHeaders.end(); ++it)
    {
        Header* p = *it;
        oss << p->key << ":" << p->value << "\r\n";
    }

    oss << CRLF;

    oss << "Body [" << mBodyLen << "]" << "\r\n";

    return oss.str();
}

// Todo: error handling, caused by limit of buffer
bool HttpMessage::ToBuffer(StreamBuffer* buf) const
{
    StreamWriter stream(buf);
    if(!stream.SerializeString(StartLine(), CRLF)) return false;

    for(std::vector<Header*>::const_iterator it = mHeaders.begin();
        it != mHeaders.end(); ++it)
    {
        Header* p = *it;
        if(!stream.SerializeString(p->key, ':')
           || !stream.SerializeString(p->value, CRLF))
        {
            return false;
        }
    }
    stream.SerializeString(CRLF, (size_t)2);

    if(mBodyLen > 0 && mBody != NULL)
    {
        stream.SerializeBytes(mBody, mBodyLen);
    }

    return true;
}

// Parse a HTTP message from a buffer
// This function will keep state and loop to complete messages
// Once a message is completed, return true
bool HttpMessage::FromBuffer(StreamBuffer* buf)
{
    if(mState == MSG_READY) ReadStartLine(buf);
    if(mState == MSG_READ_HEADER) ReadHeader(buf);
    if(mState == MSG_READ_BODY) ReadBody(buf);

    return (mState == MSG_OKAY);
}

// Read start line
// parsed by derived class
void HttpMessage::ReadStartLine(StreamBuffer* buf)
{
    assert(mState == MSG_READY);

    std::string line;
    if(StreamReader(buf).SerializeString(line, CRLF))
    {
        StartLine(line);
        mState = MSG_READ_HEADER;
    }
}

// Read header lines
// Read each complete line, until a blank line
void HttpMessage::ReadHeader(StreamBuffer* buf)
{
    assert(mState == MSG_READ_HEADER);

    // Read all complete lines
    std::string line;
    while(StreamReader(buf).SerializeString(line, CRLF))
    {       
        if(line.empty()) // Blank line, Separator line of headers and body, headers are completd
        {
            mBodyLen = GetHeaderAsInt("Content-Length");
            mState = mBodyLen > 0 ? MSG_READ_BODY : MSG_OKAY;
            break;
        }

        // parse line with key and value
        size_t pos = line.find(":");
        assert(pos != std::string::npos);
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        SetHeader(key.c_str(), value.c_str());
    }
}

// Read http message body
void HttpMessage::ReadBody(StreamBuffer* buf)
{
    assert(mState == MSG_READ_BODY);
    assert(mBodyLen > 0);
    if(mBody == NULL) mBody = new unsigned char[mBodyLen];
    if(StreamReader(buf).SerializeBytes(mBody, mBodyLen))
    {
        mState = MSG_OKAY;
    }
}

//////////////////////////////////////////////////////////////////////////

HttpRequest::HttpRequest(const char* version) 
: HttpMessage(version)
{
    mMethod = "GET";
}

HttpRequest::HttpRequest(const char* method, const char* url, const char* version)
: HttpMessage(version)
, mMethod(method)
, mUrl(url)
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::Reset() 
{
    HttpMessage::Reset();
    mMethod = "GET";
    mUrl = "";
}

const char* HttpRequest::GetMethod() const 
{
    return mMethod.c_str();
}

void HttpRequest::SetMethod(const char* method)
{
    mMethod = method;
}

const char* HttpRequest::GetUrl() const
{
    return mUrl.c_str();
}

void HttpRequest::SetUrl(const char* url)
{
    mUrl = url;
}

// Parse start line
// <verb> SP <url> SP <protocol/version> CRLF
bool HttpRequest::StartLine(const std::string& line)
{
    if(line.empty())
    {
        return false;
    }

    // Split sring with space
    std::istringstream iss(line);
    iss >> mMethod >> mUrl >> mVersion;
    return true;
}

// <verb> SP <url> SP <protocol/version> CRLF
std::string HttpRequest::StartLine() const
{
    std::ostringstream oss;
    oss << mMethod << " " << mUrl << " " << mVersion;
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponse(const char* version) 
: HttpMessage(version) 
{
    mCode = 200;
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::Reset() 
{
    HttpMessage::Reset();
    mCode = 200;
    mPhrase = "OK";
}

void HttpResponse::SetStatus(int code, const char* phrase)
{
    mCode = code;
    mPhrase = phrase;
}

int HttpResponse::GetCode() const
{
    return mCode;
}

const char* HttpResponse::GetPhrase() const
{
    return (mPhrase.empty() ? sDefaultPhrases[mCode] : mPhrase.c_str());
}


// HTTP response status code and default phrase
std::map<int, const char*> HttpResponse::sDefaultPhrases = 
{
    { 100, "Continue" },

    { 200, "OK" },
    { 201, "Created" },
    { 250, "Low on Storage Space" },

    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Moved Temporarily" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 305, "Use Proxy" },

    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Time-out" },
    { 410, "Gone" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Request Entity Too Large" },
    { 414, "Request-URI Too Large" },
    { 415, "Unsupported Media Type" },
    { 451, "Parameter Not Understood" },
    { 452, "Conference Not Found" },
    { 453, "Not Enough Bandwidth" },
    { 454, "Session Not Found" },
    { 455, "Method Not Valid in This State" },
    { 456, "Header Field Not Valid for Resource" },
    { 457, "Invalid Range" },
    { 458, "Parameter Is Read-Only" },
    { 459, "Aggregate operation not allowed" },
    { 460, "Only aggregate operation allowed" },
    { 461, "Unsupported transport" },
    { 462, "Destination unreachable" },

    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Time-out" },
    { 505, "Rtsp Version not supported" },
    { 551, "Option not supported" },
    {   0, NULL }
};

// HTTP/1.1 code phrase CRLF
bool HttpResponse::StartLine(const std::string& line)
{
    if(line.empty())
    {
        return false;
    }

    std::istringstream iss(line);
    iss >> mVersion >> mCode >> mPhrase;
    return true;
}

std::string HttpResponse::StartLine() const
{
    // <protocol>/<version> code phrase CRLF
    std::ostringstream oss;
    oss << mVersion << " " << mCode << " " << (mPhrase.empty() ? sDefaultPhrases[mCode] : mPhrase);
    return oss.str();
}

NET_BASE_END

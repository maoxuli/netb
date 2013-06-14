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

#ifndef NET_BASE_HTTP_MESSAGE_HPP
#define NET_BASE_HTTP_MESSAGE_HPP

#include "Config.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <map>

NET_BASE_BEGIN

class StreamBuffer;
class HttpMessage
{
public:
	HttpMessage(const char* version = "HTTP/1.1");	
	virtual ~HttpMessage();
	
	enum Type 
	{
		UNKNOWN_MESSAGE, 
		REQUEST_MESSAGE, 
		RESPONSE_MESSAGE
	};

	virtual Type GetType() const { return UNKNOWN_MESSAGE; }
	const char* GetVersion() const { return mVersion.c_str(); }

	const char* GetHeader(const char* key) const;
	long GetHeaderAsInt(const char* key) const;
	double GetHeaderAsFloat(const char* key) const;

	size_t GetBodyLen() const;
	bool GetBody(void* buf, size_t n) const;
	
	void SetHeader(const char* key, const char* value);
	void SetHeader(const char* key, long value);
	void SetHeader(const char* key, double value);
	void RemoveHeader(const char* key);
	
	virtual void Reset();
	bool FromBuffer(StreamBuffer* buf);
	bool ToBuffer(StreamBuffer* buf) const;

	virtual std::string Dump() const;

protected:
	static const char* CRLF; // "\r\n"
	std::string mVersion; // "HTTP/1.0", "HTTP/1.1", "HTTP/2.0"
	
	struct Header
	{
		Header(const char* k, const char* v) : key(k), value(v) { }
		std::string key;
		std::string value;
	};
	std::vector<Header*> mHeaders; // keep order with vector

	size_t mBodyLen;
	unsigned char* mBody;

protected:
	// State to parse HTTP message
    enum State
    {
        MSG_READY,        // Ready to receive a new packet
        MSG_READ_HEADER,  // Reading header lines
        MSG_READ_BODY,    // Reading body
        MSG_OKAY          // Complete a message packet
    };
    State mState;

	void ReadStartLine(StreamBuffer* buf);
	void ReadHeader(StreamBuffer* buf);
	void ReadBody(StreamBuffer* buf);

	// Parse start line and composite start line
	// implement by derived classes
	virtual bool StartLine(const std::string& line) = 0;
	virtual std::string StartLine() const = 0;
};

class HttpRequest : public HttpMessage
{
public:
	HttpRequest(const char* version = "HTTP/1.1");
    HttpRequest(const char* method, const char* url, const char* version = "HTTP/1.1");
	virtual ~HttpRequest();

	virtual Type GetType() const { return REQUEST_MESSAGE; }
	virtual void Reset();

	const char* GetMethod() const;
	void SetMethod(const char* method);

	const char* GetUrl() const;
	void SetUrl(const char* url);

private:
	std::string	mMethod;
	std::string mUrl;

	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

class HttpResponse : public HttpMessage
{
public:
	HttpResponse(const char* version = "HTTP/1.1");
	virtual ~HttpResponse();

	virtual Type GetType() const { return RESPONSE_MESSAGE; }
	virtual void Reset();

	void SetStatus(int code, const char* phrase = "");
	int GetCode() const;
	const char* GetPhrase() const;
	
private:
	int mCode;
	std::string mPhrase;
	static std::map<int, const char*> sDefaultPhrases;

	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

NET_BASE_END

#endif 

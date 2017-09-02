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

#ifndef NETB_HTTP_MESSAGE_H
#define NETB_HTTP_MESSAGE_H

#include "StreamBuffer.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <map>

NETB_BEGIN

// 
// HTTP message 
// Todo: copy constructor with deep copy
// 
class HttpMessage
{
public:
	HttpMessage(const char* version = "HTTP/1.1");	
	virtual ~HttpMessage();
	
	enum class TYPE 
	{
		UNKNOWN, 
		REQUEST, 
		RESPONSE
	};

	virtual TYPE GetType() const { return TYPE::UNKNOWN; }
	const char* GetVersion() const { return _version.c_str(); }

	const char* GetHeader(const char* key) const;
	long GetHeaderAsInt(const char* key) const;
	double GetHeaderAsFloat(const char* key) const;

	size_t GetBodyLen() const;
	const void* GetBody() const;
	
	void SetHeader(const char* key, const char* value);
	void SetHeader(const char* key, long value);
	void SetHeader(const char* key, double value);
	void RemoveHeader(const char* key);
	
	virtual void Reset();
	bool FromBuffer(StreamBuffer* buf);
	bool ToBuffer(StreamBuffer* buf) const;

	virtual std::string ToString() const;

protected:
	static const char* CRLF; // "\r\n"
	std::string _version; // "HTTP/1.0", "HTTP/1.1", "HTTP/2.0"
	
	struct Header
	{
		Header(const char* k, const char* v) : key(k), value(v) { }
		std::string key;
		std::string value;
	};
	std::vector<Header*> _headers; // keep order with vector

	size_t _body_len;
	unsigned char* _body;

protected:
	// State to parse HTTP message
    enum class STATE
    {
        READY,   // Ready to parse start line
        HEADER,  // Reading header lines
        BODY,    // Reading body
        OKAY     // Complete a message packet
    };
    STATE _state;

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

	virtual TYPE GetType() const { return TYPE::REQUEST; }
	virtual void Reset();

	const char* GetMethod() const;
	void SetMethod(const char* method);

	const char* GetUrl() const;
	void SetUrl(const char* url);

private:
	std::string	_method;
	std::string _url;

	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

class HttpResponse : public HttpMessage
{
public:
	HttpResponse(const char* version = "HTTP/1.1");
	virtual ~HttpResponse();

	virtual TYPE GetType() const { return TYPE::RESPONSE; }
	virtual void Reset();

	void SetStatus(int code, const char* phrase = "");
	int GetCode() const;
	const char* GetPhrase() const;
	
private:
	int _code;
	std::string _phrase;
	static std::map<int, const char*> s_default_phrases;

	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

NETB_END

#endif 

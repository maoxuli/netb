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

#ifndef NETB_HTTP_MESSAGE_HPP
#define NETB_HTTP_MESSAGE_HPP

#include "Config.hpp"
#include "Uncopyable.hpp"
#include "StreamBuffer.hpp"
#include "StreamReader.hpp"
#include "StreamWriter.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <map>

NETB_BEGIN

// 
// Class for HTTP message
// 
class HttpMessage : private Uncopyable
{
public:
	HttpMessage(const char* version = "HTTP/1.1");	
	virtual ~HttpMessage();
	
	// get type
	enum class TYPE { UNKNOWN = 0, REQUEST = 1, RESPONSE = 2 };
	virtual TYPE GetType() const { return TYPE::UNKNOWN; }

	// Reset 
	virtual void Reset();

	// get and set version
	const char* GetVersion() const;
	void SetVersion(const char* version);

	// get header
	const char* GetHeader(const char* key) const;
	long GetHeaderAsInt(const char* key) const;
	double GetHeaderAsFloat(const char* key) const;

	// set header
	void SetHeader(const char* key, const char* value);
	void SetHeader(const char* key, long value);
	void SetHeader(const char* key, double value);
	void RemoveHeader(const char* key);

	// get and set body
	size_t GetBodyLen() const;
	const void* GetBody() const;
	void SetBody(const void* p, size_t n);

	// pack and unpack
	bool FromBuffer(StreamBuffer* buf);
	bool ToBuffer(StreamBuffer* buf) const;

	// output for log or diagnosis
	std::string String() const;

protected:
	static const char* CRLF; // "\r\n"
	std::string _version; // "HTTP/1.0", "HTTP/1.1", "HTTP/2.0"
	
	struct Header
	{
		Header(const char* k, const char* v) 
		: key(k), value(v) { }

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

	void ReadStartLine(const StreamReader& stream);
	void ReadHeader(const StreamReader& stream);
	void ReadBody(const StreamReader& stream);

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

	// get and set method
	const char* GetMethod() const;
	void SetMethod(const char* method);

	// get and set url
	const char* GetUrl() const;
	void SetUrl(const char* url);

private:
	std::string	_method;
	std::string _url;

	// Pack and unpack start line
	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

class HttpResponse : public HttpMessage
{
public:
	HttpResponse(const char* version = "HTTP/1.1");
	HttpResponse(int code, const char* phrase = "", const char* version = "HTTP/1.1");
	virtual ~HttpResponse();

	virtual TYPE GetType() const { return TYPE::RESPONSE; }
	virtual void Reset();

	// get and set 
	int GetCode() const;
	const char* GetPhrase() const;
	void SetStatus(int code, const char* phrase = "");

private:
	int _code;
	std::string _phrase;
	static std::map<int, const char*> s_default_phrases;

	// pack and unpack start line
	virtual bool StartLine(const std::string& line);
	virtual std::string StartLine() const;
};

NETB_END

#endif 

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

#include "Exception.hpp"

NETB_BEGIN

// Exception
Exception::Exception(const std::string& msg, int code)
: std::exception()
, _message(msg)
, _code(code) 
{

}

Exception::~Exception() noexcept
{

}

// Error class 
const class ErrorClass& Exception::Class() const 
{
    return ErrorClass();
}

// To string for log
std::string Exception::Report() const 
{
    std::ostringstream oss;
    oss << Class().Name();
    if(!_message.empty()) oss << ":" << _message;
    if(_code > 0) oss << ":" << _code;
    oss << ".";
    return oss.str();
}

////////////////////////////////////////////////////////////////////////

// Logic exception
LogicException::LogicException(const std::string& msg, int code)
: Exception(msg, code)
, std::logic_error(Error::MessageStream() << msg << ":" << code)
{

}

LogicException::~LogicException() noexcept
{

}

// Error class
const class ErrorClass& LogicException::Class() const 
{
    return LogicError();
}

// Implementation for logic error class 
IMPLEMENT_ERROR_CLASS(LogicError, "LogicError", LogicException)

////////////////////////////////////////////////////////////////////////

// Runtime exception
RuntimeException::RuntimeException(const std::string& msg, int code)
: Exception(msg, code)
, std::runtime_error(Error::MessageStream() << msg << ":" << code)
{

}

RuntimeException::~RuntimeException() noexcept
{

}

// Error class  
const class ErrorClass& RuntimeException::Class() const 
{
    return RuntimeError();
}

// Implementation for runtime error class 
IMPLEMENT_ERROR_CLASS(RuntimeError, "RuntimeError", RuntimeException)

NETB_END

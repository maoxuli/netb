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
: _message(msg)
, _code(code) 
{

}

Exception::~Exception() noexcept
{

}

// what() needs a static string
const std::string& Exception::MakeWhat() const
{
    static std::string s;
    std::ostringstream oss;
    oss << _message << " : " << _code;
    s = oss.str();
    return s;
}

// what
const char* Exception::what() const noexcept
{
    return MakeWhat().c_str();
}

// To string for log
std::string Exception::Report() const 
{
    std::ostringstream oss;
    oss << "Error";
    if(!_message.empty()) oss << " : " << _message;
    if(_code > 0) oss << " : " << _code;
    oss << ".";
    return oss.str();
}

////////////////////////////////////////////////////////////////////////

// Logic exception
LogicException::LogicException(const std::string& msg, int code)
: Exception(msg, code)
, std::logic_error(MakeWhat())
{

}

LogicException::~LogicException() noexcept
{

}

// what
const char* LogicException::what() const noexcept
{
    return std::logic_error::what();
}

// To string for log
std::string LogicException::Report() const 
{
    std::ostringstream oss;
    oss << "Logic Error";
    if(!_message.empty()) oss << " : " << _message;
    if(_code > 0) oss << " : " << _code;
    oss << ".";
    return oss.str();
}

////////////////////////////////////////////////////////////////////////

// Runtime exception
RuntimeException::RuntimeException(const std::string& msg, int code)
: Exception(msg, code)
, std::runtime_error(MakeWhat())
{

}

RuntimeException::~RuntimeException() noexcept
{

}

// what
const char* RuntimeException::what() const noexcept
{
    return std::runtime_error::what();
}

// To string for log
std::string RuntimeException::Report() const 
{
    std::ostringstream oss;
    oss << "Runtime Error";
    if(!_message.empty()) oss << " : " << _message;
    if(_code > 0) oss << " : " << _code;
    oss << ".";
    return oss.str();
}

NETB_END

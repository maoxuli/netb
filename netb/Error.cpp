/*
 * Copyright (C) 2017, Maoxu Li. http://maoxuli.com/dev
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

#include "Error.hpp"
#include "Exception.hpp"

NETB_BEGIN

// No error
Error::Error() noexcept
: _class(0)
, _message("")
, _code(0)
{

}

// Unclassified error
Error::Error(const std::string& msg, int code) noexcept
: _class(&GeneralError())
, _message(msg)
, _code(code)

{

}

// Error classification, message, and code
Error::Error(const class ErrorClass& cls, const std::string& msg, int code) noexcept
: _class(&cls)
, _message(msg)
, _code(code)
{

}

Error::~Error() noexcept
{

}

const class ErrorClass& Error::Class() const noexcept 
{
    if(_class) 
    {
        return *_class;
    }
    return ErrorClass();
}

// Set to no error, empty error
void Error::Reset() noexcept
{
    _class = 0;
    _message = "";
    _code = 0; 
}

void Error::Set(const std::string& msg, int code) noexcept
{
    _message = msg;
    _code = code; 
    if(!_class) _class = &GeneralError();
}

void Error::Set(const class ErrorClass& cls, const std::string& msg, int code) noexcept
{
    _class = &cls;
    _message = msg;
    _code = code; 
}

void Error::SetClass(const class ErrorClass& cls) noexcept
{
    _class = &cls;
}

void Error::SetMessage(const std::string& msg) noexcept
{
    _message = msg;
    if(!_class) _class = &GeneralError();
}

void Error::SetCode(int code) noexcept
{
    _code = code;
    if(!_class) _class = &GeneralError();
}

std::string Error::Report() const noexcept
{
    if(!_class) return "No error.";
    std::ostringstream oss;
    oss << _class->Name();
    if(_code > 0) oss << ":" << _code;
    if(!_message.empty()) oss << ":" << _message;
    oss << ".";
    return oss.str();
}

NETB_END

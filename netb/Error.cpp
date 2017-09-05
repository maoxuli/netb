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

#include "Error.hpp"
#include "Exception.hpp"

NETB_BEGIN

//
// Error
//
Error::Error() noexcept
: _class(NULL)
, _info("")
, _code(0)
{

}

Error::Error(const std::string& info, int code) noexcept
: _class(&ErrorClass())
, _info(info)
, _code(code)

{

}

Error::Error(const class ErrorClass& cls, const std::string& info, int code) noexcept
: _class(&cls)
, _info(info)
, _code(code)
{

}

Error::~Error() noexcept
{

}

void Error::Reset() noexcept
{
    _class = NULL;
    _info = "";
    _code = 0; 
}

void Error::Set(const std::string& info, int code) noexcept
{
    _info = info;
    _code = code; 
    if(!_class) _class = &ErrorClass();
}

void Error::Set(const class ErrorClass& cls, const std::string& info, int code) noexcept
{
    _class = &cls;
    _info = info;
    _code = code; 
}

void Error::SetClass(const class ErrorClass& cls) noexcept
{
    _class = &cls;
}

void Error::SetInfo(const std::string& info) noexcept
{
    _info = info;
    if(!_class) _class = &ErrorClass();
}

void Error::SetCode(int code) noexcept
{
    _code = code;
    if(!_class) _class = &ErrorClass();
}

//
// ErrorClass
//
const char* ErrorClass::Name() const noexcept
{
    return "Error";
}

void ErrorClass::Throw(const Error& e) const noexcept
{
    if(e) throw Exception(e.Info(), e.Code());
}

const class ErrorClass& ErrorClass() noexcept
{
    static class ErrorClass sErrorClass;
    return sErrorClass;
}

//
// Implement other error classes
//
IMPLEMENT_ERROR_CLASS(SocketError, "SocketError", SocketException)
IMPLEMENT_ERROR_CLASS(AddressError, "AddressError", AddressException)

NETB_END

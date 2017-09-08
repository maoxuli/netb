/*
 * Copyright (C) 2010 Maoxu Li. All rights reserved. maoxu@lebula.com
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

#ifndef NETB_EXCEPTION_HPP
#define NETB_EXCEPTION_HPP

#include "Config.hpp"
#include "Error.hpp"
#include <stdexcept>

NETB_BEGIN

//
// Declare base exception
//
class Exception : public std::exception
{
public: 
    Exception(const std::string& info = "", int code = 0) noexcept
        : std::exception(), _info(info), _code(code) { }
    virtual ~Exception() noexcept { }
    virtual const char* what() const noexcept { return _info.c_str(); }
    virtual std::string Info() const noexcept { return _info; }
    virtual int Code() const noexcept { return _code; }
    virtual const class ErrorClass& Class() const noexcept;

private:
    std::string _info;
    int _code;
};

//
// Macros to declare and implement other exceptions
//
#define DECLARE_EXCEPTION(CLS, BASE)                                        \
    class CLS : public BASE                                                 \
    {                                                                       \
    public:                                                                 \
        CLS(const std::string& info = "", int code = 0) noexcept            \
            : BASE(info, code) { }                                          \
        ~CLS() noexcept { }                                                 \
        const class ErrorClass& Class() const noexcept;                     \
    };

#define IMPLEMENT_EXCEPTION(CLS, EC)                                        \
    const class ErrorClass& CLS::Class() const noexcept                     \
    {                                                                       \
        return EC();                                                        \
    }                                      


// System error, errors that occurred in system calling, always with an error code
// Marcros to declare exception, declare error class, and set error object 
DECLARE_EXCEPTION(SystemException, Exception)
DECLARE_ERROR_CLASS(SystemError, ErrorClass)
#define SET_SYSTEM_ERROR(e, info) do{ if(e) e->Set(SystemError(), info, ErrorCode::Current()); } while(0) // no trailing ;

// Logic error, errors of logic that produce unexcepcted results
// Marcros to declare exception, declare error class, and set error object
DECLARE_EXCEPTION(LogicException, Exception)
DECLARE_ERROR_CLASS(LogicError, ErrorClass)
#define SET_LOGIC_ERROR(e, info) do{ if(e) e->Set(LogicError(), info); } while(0) // no trailing ;

NETB_END

#endif

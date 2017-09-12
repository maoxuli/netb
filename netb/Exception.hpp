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
#include <exception>

NETB_BEGIN

//
// Base class for exceptions
//
class Exception : public std::exception
{
public: 
    Exception(const std::string& msg = "", int code = 0) noexcept
        : std::exception(), _message(msg), _code(code) { }
    
    // Get
    virtual const char* what() const noexcept { return _message.c_str(); }
    virtual std::string Message() const noexcept { return _message; }
    virtual int Code() const noexcept { return _code; }

    // Bridge to an Error object
    virtual const class ErrorClass& Class() const noexcept;

    // To string for log or display
    virtual std::string ToString() const noexcept;
private:
    std::string _message;
    int _code;
};

//
// Declare and implement other exceptions
//
#define DECLARE_EXCEPTION(CLS, BASE)                                        \
    class CLS : public BASE                                                 \
    {                                                                       \
    public:                                                                 \
        CLS(const std::string& msg = "", int code = 0) noexcept             \
            : BASE(msg, code) { }                                           \
        const class ErrorClass& Class() const noexcept;                     \
    };

#define IMPLEMENT_EXCEPTION(CLS, EC)                                        \
    const class ErrorClass& CLS::Class() const noexcept                     \
    {                                                                       \
        return EC();                                                        \
    }                                      


// Declare a new exception and associated error classification
// System error, errors that occurred in system calling, always with an error code
// Declare exception, error classification, and macro to set error object given by a pointer
DECLARE_EXCEPTION(SystemException, Exception)
DECLARE_ERROR_CLASS(SystemError, ErrorClass)
#define SET_SYSTEM_ERROR(e, msg) do{ if(e) e->Set(SystemError(), \
                        (Error::MessageStream() << msg), ErrorCode::Current()); } while(0) // no trailing ;

// Declare a new exception and associated error classification
// Logic error, errors of logic that produce unexcepcted results
// Declare exception, error classification, and macro to set error object given by a pointer
DECLARE_EXCEPTION(LogicException, Exception)
DECLARE_ERROR_CLASS(LogicError, ErrorClass)
#define SET_LOGIC_ERROR(e, msg) do{ if(e) e->Set(LogicError(), \
                       (Error::MessageStream() << msg)); } while(0) // no trailing ;

NETB_END

#endif

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
#include <stdexcept>

NETB_BEGIN

//
// NetB follows the logic of stl exception handling, e.g., classify exceptions 
// into logic errors and runtime errors. NetB declared some exception classes 
// accordingly but extended with an optional error code. 
// 
// The NetB exception class is so declared that it is always a subclass of a 
// coresponding standard stl exception, if it exists. 
//
class Exception : public std::exception
{
public: 
    Exception(const std::string& msg = "", int code = 0);
    virtual ~Exception() noexcept;
    
    // Get
    virtual const char* what() const noexcept { return _message.c_str(); }
    virtual std::string Message() const { return _message; }
    virtual int Code() const { return _code; }

    // Bridge to an Error object
    virtual const class ErrorClass& Class() const;

    // To string for log or display
    virtual std::string Report() const;
    
private:
    std::string _message;
    int _code;
};

//
// Logic exception 
//
class LogicException : public Exception, public std::logic_error
{
public: 
    LogicException(const std::string& msg = "", int code = 0);
    virtual ~LogicException() noexcept;

    // what
    virtual const char* what() const noexcept { return logic_error::what(); }

    // Bridge to an error class 
    virtual const class ErrorClass& Class() const;
};

// Declare an error class for logic exception 
// and macro to set error object with error class of logic error 
DECLARE_ERROR_CLASS(LogicError, ErrorClass)
#define SET_LOGIC_ERROR(e, msg, code) do{ if(e) e->Set(LogicError(), \
                       (Error::MessageStream() << msg), code); } while(0) // no trailing ;

//
// Runtime exception 
// 
class RuntimeException : public Exception, public std::runtime_error 
{
public: 
    RuntimeException(const std::string& msg = "", int code = 0);
    virtual ~RuntimeException() noexcept; 

    // what
    virtual const char* what() const noexcept { return std::runtime_error::what(); }

    // Bridge to an error class
    virtual const class ErrorClass& Class() const; 
};

// Declare an error class for runtime exception 
// and macro to set error object with error class of runtime error
DECLARE_ERROR_CLASS(RuntimeError, ErrorClass)
#define SET_RUNTIME_ERROR(e, msg, code) do{ if(e) e->Set(RuntimeError(), \
                         (Error::MessageStream() << msg), code); } while(0) // no trailing ;

//
// Macros to declare and implement non-standard exceptions
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

NETB_END

#endif

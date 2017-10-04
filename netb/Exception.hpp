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

#ifndef NETB_EXCEPTION_HPP
#define NETB_EXCEPTION_HPP

#include "Config.hpp"
#include <exception>
#include <stdexcept>

NETB_BEGIN

//
// NetB follows the logic of stl exception handling, e.g., classify exceptions 
// into logic errors and runtime errors. NetB declared some exception classes 
// accordingly but extended with error message and error code. 
// 
// The NetB exception class is so declared that it is always a subclass of a 
// coresponding standard stl exception, if it exists. The what() interface in  
// std::exception returns error message and error code. 
//
class Exception : public std::exception
{
public: 
    Exception(const std::string& msg = "", int code = 0);
    virtual ~Exception() noexcept;
    
    // Get
    virtual std::string Message() const { return _message; }
    virtual int Code() const { return _code; }

    // To string
    virtual const char* what() const noexcept;
    virtual std::string Report() const;
    
protected:
    std::string _message;
    int _code;

    const std::string& MakeWhat() const; 
};

//
// Logic exception 
//
class LogicException : public Exception, public std::logic_error
{
public: 
    LogicException(const std::string& msg = "", int code = 0);
    virtual ~LogicException() noexcept;

    // To string
    virtual const char* what() const noexcept;
    virtual std::string Report() const; 
};

//
// Runtime exception 
// 
class RuntimeException : public Exception, public std::runtime_error 
{
public: 
    RuntimeException(const std::string& msg = "", int code = 0);
    virtual ~RuntimeException() noexcept; 

    // To string
    virtual const char* what() const noexcept;
    virtual std::string Report() const;
};

NETB_END

#endif

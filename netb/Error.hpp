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

#ifndef NETB_ERROR_HPP
#define NETB_ERROR_HPP

#include "Config.hpp"
#include "ErrorClass.hpp"

NETB_BEGIN

//
// Error object is defined by Error class, which is used to return details 
// of error status from functions to caller. It holds a text message as 
// description of the error and an optional associated error code defined 
// by system or the library. 
//
// Error object also holds an error class object, which is defined by 
// ErrorClass or its subclasses, to indicate classification of the error. 
// Error class object is a bridge between an error object and an exception. 
// Usually a subclass of ErrorClass will be declared for each of exceptions. 
// Error class has a member method to throw associated exception. 
// 
// A static object for each ErrorClass and its subclasses is initiated   
// when it is referenced for the first time. It is then referenced by all 
// error objects to indicate error classifications. For convenience a same 
// name function of ErrorClass and its subclasses is defined to get the 
// reference of the error class object.  
//
class Error
{
public:
    // Error with nothing indicates no error
    Error() noexcept;

    // Only text message and error code, denotes an unclassified error
    // The classification is set by default
    Error(const std::string& msg, int code = 0) noexcept;

    // Classification of an error is denoted by a ErrorClass (or its subclass) object
    Error(const class ErrorClass& cls, const std::string& msg = "", int code = 0) noexcept;

    // Destructor
    ~Error() noexcept;

    // Set and Get error status
    // Empty Error object indicates no error
    operator bool() const noexcept { return _class; }
    bool Empty() const noexcept { return !_class; }
    void Reset() noexcept; // set to empty

    // Get
    const class ErrorClass& Class() const noexcept;
    const std::string& Message() const noexcept { return _message; }
    int Code() const noexcept { return _code; }

    // Set 
    void Set(const std::string& msg, int code = 0) noexcept; // unclassified error by default
    void Set(const class ErrorClass& cls, const std::string& msg = "", int code = 0) noexcept;
    void SetClass(const class ErrorClass& cls) noexcept;
    void SetMessage(const std::string& msg) noexcept;
    void SetCode(int code) noexcept;

    // To string for log or display
    std::string Report() const noexcept;

private:
    const class ErrorClass* _class; // classification, ErrorClass or its subclass
    std::string _message;
    int _code;

public:
    // Helper class for formatting error message
    class MessageStream
    {
    public:
        operator std::string() const
        {
            return stream.str();
        }
 
        template<typename T>
        MessageStream& operator << (const T& value)
        {
            stream << value;
            return *this;
        }

    private:
        std::ostringstream stream;
    };
};

// Set error object given by a pointer
#define RESET_ERROR(e) do{ if(e) e->Reset(); } while(0) // no trailing ;
#define SET_ERROR_CLASS(e, cls) do{ if(e) e->SetClass(cls); } while(0) // no trailing ;
#define SET_ERROR_MESSAGE(e, msg) do{ if(e) e->SetMessage((Error::MessageStream() << msg)); } while(0) // no trailing ;
#define SET_ERROR_CODE(e, code) do{ if(e) e->SetCode(code); } while(0) // no trailing ;

NETB_END

#endif

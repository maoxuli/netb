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

#ifndef NETB_ERROR_CLASS_HPP
#define NETB_ERROR_CLASS_HPP

#include "Config.hpp"

NETB_BEGIN

//
// The classification of an error is denoted by an object of ErrorClass or its subclasses. 
// ErrorClass provides a name for the error classification. A subclass of ErrorClass is 
// usually declared for a target exception. So ErrorClass and it subclasses have a member
// method to throw the target exceptions. The error class object is a bridge between error 
// object and a exception, so that approperiate exception can be throwed based on an error 
// object.   
//
// Base error class indicate no error
// not throw exception 
class ErrorClass
{
public:
    virtual const char* Name() const;
    virtual void Throw(const class Error& e) const;
};
const class ErrorClass& ErrorClass();

// Macro to throw exception based on an error object
#define THROW_ERROR(e) do{ e.Class().Throw(e); } while(0) // no trailing ;

// 
// Macros to declare and implement a subclass of ErrorClass
//
#define DECLARE_ERROR_CLASS(CLS, BASE)                              \
    class CLS : public BASE                                         \
    {                                                               \
    public:                                                         \
        virtual const char* Name() const;                           \
        virtual void Throw(const Error& e) const;                   \
    };                                                              \
    const class CLS& CLS();

#define IMPLEMENT_ERROR_CLASS(CLS, NAME, EXCEPTION)                 \
    const char* CLS::Name() const                                   \
    {                                                               \
        return NAME;                                                \
    }                                                               \
    void CLS::Throw(const Error& e) const                           \
    {                                                               \
        if(e) throw EXCEPTION(e.Message(), e.Code());               \
    }                                                               \
    const class CLS& CLS()                                          \
    {                                                               \
        static class CLS s##CLS;                                    \
        return s##CLS;                                              \
    }                                                    

//
// Declare error classes for exceptions
// 
DECLARE_ERROR_CLASS(GeneralError, ErrorClass)   // unclassified error
DECLARE_ERROR_CLASS(LogicError, ErrorClass)     // logic error
DECLARE_ERROR_CLASS(RuntimeError, ErrorClass)   // runtime error

// Macro to set an error object as an unclassified error
#define SET_ERROR(e, msg, code) do{ if(e) e->Set(GeneralError(), \
                 (Error::MessageStream() << msg), code); } while(0) 

// macro to set error object with error classes of logic error 
#define SET_LOGIC_ERROR(e, msg, code) do{ if(e) e->Set(LogicError(), \
                       (Error::MessageStream() << msg), code); } while(0) 

// macro to set error object with error classes of runtime error                        
#define SET_RUNTIME_ERROR(e, msg, code) do{ if(e) e->Set(RuntimeError(), \
                         (Error::MessageStream() << msg), code); } while(0)                        

NETB_END

#endif

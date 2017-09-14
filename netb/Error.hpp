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

NETB_BEGIN

//
// Error object is used to transfer error status from functions to caller. 
// It holds a text message and an optional associated code to describe the 
// error, and a ErrorClass object to classify the error. ErrorClass object 
// is also a bridge between an error object and an exception. The formats 
// of error message and code are not determined. They are usually varied on 
// error classifications.
//
class Error
{
public:
    // Error with nothing indicates no error
    Error() noexcept;

    // Only text message and error code denote an unclassified error
    // The classification is set by default
    Error(const std::string& msg, int code = 0) noexcept;

    // Classification of an error is denoted by a ErrorClass (or its subclass) object
    Error(const class ErrorClass& cls, const std::string& msg = "", int code = 0) noexcept;

    // Destructor
    ~Error() noexcept;

    // Error status
    // Empty Error object indicates no error
    bool Empty() const noexcept { return !_class; }
    operator bool() const noexcept { return _class; }

    // Get
    const class ErrorClass& Class() const noexcept;
    const std::string& Message() const noexcept { return _message; }
    int Code() const noexcept { return _code; }

    // Set 
    void Reset() noexcept; // Set to empty
    void Set(const std::string& msg, int code = 0) noexcept; // unclassified error by default
    void Set(const class ErrorClass& cls, const std::string& msg = 0, int code = 0) noexcept;
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
#define SET_ERROR_CLASS(e, cls) do{ if(e) e->SetClass(cls); } while(0) // no trailing ;
#define SET_ERROR_MESSAGE(e, msg) do{ if(e) e->SetMessage((Error::MessageStream() << msg)); } while(0) // no trailing ;
#define SET_ERROR_CODE(e, code) do{ if(e) e->SetCode(code); } while(0) // no trailing ;

// Throw exception based on an error object
#define THROW_ERROR(e) do{ e.Class().Throw(e); } while(0) // no trailing ;

//
// The classification of an error is denoted by an ErrorClass or its subclass object. 
// ErrorClass provides a name for an error classification and established a static 
// mapping to an exception or its subclass object. By this mean, an ErrorClass subclass 
// is usually declared for an exception subclass.
//
class ErrorClass
{
public:
    virtual const char* Name() const noexcept;
    virtual void Throw(const class Error& e) const;
};
const class ErrorClass& ErrorClass() noexcept;

// Set an error object as an unclassified error, with error message and code
#define SET_ERROR(e, msg, code) do{ if(e) e->Set(ErrorClass(), (Error::MessageStream() << msg), code); } while(0) // no trailing ;

//
// A dummy subclass of ErrorClass is declared to indicate no error
// 
class NoError : public ErrorClass
{
public:
    const char* Name() const noexcept;
    void Throw(const Error& e) const noexcept;
};
const class NoError& NoError() noexcept;

// 
// Declare and implement a subclass of ErrorClass
//
#define DECLARE_ERROR_CLASS(CLS, BASE)                              \
    class CLS : public BASE                                         \
    {                                                               \
    public:                                                         \
        const char* Name() const noexcept;                          \
        void Throw(const Error& e) const;                           \
    };                                                              \
    const class CLS& CLS() noexcept;

#define IMPLEMENT_ERROR_CLASS(CLS, NAME, EXCEPTION)                 \
    const char* CLS::Name() const noexcept                          \
    {                                                               \
        return NAME;                                                \
    }                                                               \
    void CLS::Throw(const Error& e) const                           \
    {                                                               \
        if(e) throw EXCEPTION(e.Message(), e.Code());               \
    }                                                               \
    const class CLS& CLS() noexcept                                 \
    {                                                               \
        static class CLS s##CLS;                                    \
        return s##CLS;                                              \
    }                                                    

NETB_END

#endif

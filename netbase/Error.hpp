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

#ifndef NET_BASE_ERROR_HPP
#define NET_BASE_ERROR_HPP

#include "Config.hpp"

NET_BASE_BEGIN

//
// ErrorClass is used for error classfification. 
//
class ErrorClass
{
public:
    virtual ~ErrorClass() noexcept { }
    virtual const char* Name() const noexcept;
    virtual void Throw(const class Error& e) const noexcept;
};
const class ErrorClass& ErrorClass() noexcept;

//
// Error object is used to transfer error status from functions to caller. 
// As a general error, it holds a text info to descript the error status. 
// As a runtime error, it holds the error code. Actually user may also define 
// error codes for non-runtime errors. 
//
class Error
{
public:
    // Error Info, Error Code
    Error() noexcept;
    Error(const std::string& info, int code = 0) noexcept;
    Error(const class ErrorClass& cls, const std::string& info = "", int code = 0) noexcept;
    ~Error() noexcept;

    // Error status
    operator bool() const { return _class != NULL; }

    // Get
    const class ErrorClass& Class() const { return *_class; }
    const std::string& Info() const { return _info; }
    int Code() const { return _code; }

    // Set 
    void Reset() noexcept;
    void Set(const std::string& info, int code = 0) noexcept;
    void Set(const class ErrorClass& cls, const std::string& info = 0, int code = 0) noexcept;
    void SetClass(const class ErrorClass& cls) noexcept;
    void SetInfo(const std::string& info) noexcept;
    void SetCode(int code) noexcept;

private:
    const class ErrorClass* _class;
    std::string _info;
    int _code;
};

// Usually error is returned as an out parameter (pointer) of functions. 
// Using this macro to set error object for flexibility
#define SET_ERROR_CLASS(e, cls) if(e) e->SetClass(cls);
#define SET_ERROR_INFO(e, info) if(e) e->SetInfo(info);
#define SET_ERROR_CODE(e, code) if(e) e->SetCode(code);

#define SET_ERROR(e, info, code) if(e) e->Set(ErrorClass(), info, code);

// Throw exception based on an error
#define THROW_ERROR(e) e.Class().Throw(e);

// 
// Macros to declare and implement new error class
//
#define DECLARE_ERROR_CLASS(CLS, BASE)                              \
    class CLS : public BASE                                         \
    {                                                               \
    public:                                                         \
        const char* Name() const noexcept;                          \
        void Throw(const Error& e) const noexcept;                  \
    };                                                              \
    const class ErrorClass& CLS() noexcept;

#define IMPLEMENT_ERROR_CLASS(CLS, NAME, EXCEPTION)                 \
    const char* CLS::Name() const noexcept                          \
    {                                                               \
        return NAME;                                                \
    }                                                               \
    void CLS::Throw(const Error& e) const noexcept                  \
    {                                                               \
        if(e) throw EXCEPTION(e.Info(), e.Code());                  \
    }                                                               \
    const class ErrorClass& CLS() noexcept                          \
    {                                                               \
        static class CLS s##CLS;                                    \
        return s##CLS;                                              \
    }                                                              

//
// Declare other error classes 
//
DECLARE_ERROR_CLASS(SocketError, ErrorClass)
DECLARE_ERROR_CLASS(AddressError, ErrorClass)
DECLARE_ERROR_CLASS(ThreadError, ErrorClass)

// Macros to set errors
#define SET_SOCKET_ERROR(e, info, code) if(e) e->Set(SocketError(), info, code);
#define SET_ADDRESS_ERROR(e, info, code) if(e) e->Set(AddressError(), info, code);
#define SET_THREAD_ERROR(e, info, code) if(e) e->Set(ThreadError(), info, code);

NET_BASE_END

#endif

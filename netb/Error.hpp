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
// It holds a text info to descript the error and a optional associated code 
// number. The formats of info and code are not determined. They are varied 
// on error classifications.
//
class Error
{
public:
    // No error, or empty error
    Error() noexcept;
    Error(const std::string& info, int code = 0) noexcept;
    Error(const class ErrorClass& cls, const std::string& info = "", int code = 0) noexcept;
    ~Error() noexcept;

    // Error status
    operator bool() const noexcept { return _class != NULL; }
    bool Empty() const noexcept { return _class == NULL; }

    // Get
    const class ErrorClass& Class() const noexcept;
    const std::string& Info() const noexcept { return _info; }
    int Code() const noexcept { return _code; }

    // Set 
    void Reset() noexcept; // Set to empty
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

// Macros to set error object given by pointer
#define SET_ERROR_CLASS(e, cls) do{ if(e) e->SetClass(cls); } while(0) // no trailing ;
#define SET_ERROR_INFO(e, info) do{ if(e) e->SetInfo(info); } while(0) // no trailing ;
#define SET_ERROR_CODE(e, code) do{ if(e) e->SetCode(code); } while(0) // no trailing ;

// Throw exception based on an error
#define THROW_ERROR(e) do{ e.Class().Throw(e); } while(0)

//
// ErrorClass is used for error classfification. 
// The base class indicate unclassified errors and throw Exception
//
class ErrorClass
{
public:
    virtual ~ErrorClass() noexcept { }
    virtual const char* Name() const noexcept;
    virtual void Throw(const class Error& e) const noexcept;
};
const class ErrorClass& ErrorClass() noexcept;

// Macro to set error object with unclassified error class
#define SET_ERROR(e, info, code) do{ if(e) e->Set(ErrorClass(), info, code); } while(0) // no trailing ;

//
// A dummy error class, used to indicate no error
// 
class NoError : public ErrorClass
{
public:
    const char* Name() const noexcept;
    void Throw(const Error& e) const noexcept;
};
const class ErrorClass& NoError() noexcept;

// 
// Macros to declare and implement other error classes
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

NETB_END

#endif

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

class Exception : public std::runtime_error
{
public: 
    Exception(const std::string& info = "", int code = 0) noexcept
        : std::runtime_error(info), _code(code) { }
    virtual ~Exception() noexcept { }
    virtual const class ErrorClass& Class() const noexcept;
    virtual std::string Info() const noexcept { return runtime_error::what(); }
    virtual int Code() const noexcept { return _code; }
private:
    int _code;
};

//
// Macros to declare and implement exceptions
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

//
// Declare other exceptions
//
DECLARE_EXCEPTION(SocketException, Exception)
DECLARE_EXCEPTION(AddressException, Exception)
DECLARE_EXCEPTION(ThreadException, Exception)

NETB_END

#endif

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

#include "Error.hpp"
#include "Exception.hpp"

NETB_BEGIN

// base class of error classification indicate no error
const char* ErrorClass::Name() const
{
    return "";
}

// not throw 
void ErrorClass::Throw(const Error& e) const
{

}

// A single object of error class is defined 
const class ErrorClass& ErrorClass()  
{
    static class ErrorClass sErrorClass;
    return sErrorClass;
}

// Implementation for error classes  
IMPLEMENT_ERROR_CLASS(GeneralError, "Error", Exception)
IMPLEMENT_ERROR_CLASS(LogicError, "LogicError", LogicException)
IMPLEMENT_ERROR_CLASS(RuntimeError, "RuntimeError", RuntimeException)

NETB_END

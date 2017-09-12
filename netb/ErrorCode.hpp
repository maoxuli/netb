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

#ifndef NETB_ERROR_CODE_HPP
#define NETB_ERROR_CODE_HPP

#include "Config.hpp"

NETB_BEGIN

//
// ErrorCode is a wrapper class for system error codes and runtime last-error. 
// It defines an error with a customized name (function's name), and establish 
// a static mapping from the name to system dependent error codes. It also 
// hides the difference of last-errors across operating systems. ErrorCode is 
// a tool class consists of a set of static member functions corresponding to 
// system's last-error and all system defined error codes.  
//
class ErrorCode
{
public:  
    // System's last-error
    static int Current(); 
    static void SetCurrent(int code);

    // Text description for last-error or given error code
    static std::string Description();
    static std::string Description(int code);
    
    // Interrupted, error returned in block function calling
    static int Interrupted();               // error code
    static bool IsInterrupted();            // last-error
    static bool IsInterrupted(int code);    // given code

    // In progress, error returned in non-block function calling
    // e.g. socket connect() 
    static int InProgress(); 
    static bool IsInProgress(); 
    static bool IsInProgress(int code); 
    
    // Would block, error returned in non-block function calling
    // e.g. socket I/O functions
    static int WouldBlock(); 
    static bool IsWouldBlock(); 
    static bool IsWouldBlock(int code); 
};

NETB_END

#endif

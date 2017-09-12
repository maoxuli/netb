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
// ErrorCode is a wrapper of system error codes and current error info.
//
class ErrorCode
{
public:  
    // Current error code (errno or GetLastError())
    static int Current();   // get current code
    static void SetCurrent(int code); // set current code

    // Text description for error code
    static std::string Description(); // for current code
    static std::string Description(int code); // for given code
    
    // System interruption in block mode
    static int Interrupted(); // get error code
    static bool IsInterrupted(); // for current code
    static bool IsInterrupted(int code); // for given code

    // Connect is in progress in non-block mode
    // EINPROGRESS/WSAWOULDBLOCK
    static int InProgress(); // get error code
    static bool IsInProgress(); // for current code
    static bool IsInProgress(int code); // for given code
    
    // I/O is not ready in non-block mode
    // EWOULDBLOCK/EAGAIN/ASAWOULDBLOCK
    static int WouldBlock(); // get error code
    static bool IsWouldBlock(); // for current code
    static bool IsWouldBlock(int code); // for given code
};

NETB_END

#endif

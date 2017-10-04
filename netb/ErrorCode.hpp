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

#ifndef NETB_ERROR_CODE_HPP
#define NETB_ERROR_CODE_HPP

#include "Config.hpp"
#include <errno.h>

NETB_BEGIN

//
// Once a system calling is failed, an errno/last-error is usually set. 
// ErrorCode namespace include the error code defined by system and the 
// associated text description of each error code. The errno/last-error 
// is returned by function Current().
//
namespace ErrorCode
{

//
// Once a system calling is failed, errno/last-error is set, which can 
// be retrieved by Current() function. The current error code can be 
// reset too, in some cases to pass error status to following actions. 
// 

int Current();  // current error code
void SetCurrent(int code);

// 
// Each error code defined by system has an associated text description, 
// which is usually on global basis, rather a specific function.
// 

std::string Description(); // for current error code
std::string Description(int code);

//
// Mostly it is not necessary for application code to aware the error 
// code name. But sometimes it is. Here is some useful error code name 
// defined for cross-platforms. 
//

enum 
{
#ifdef _WIN32
    TIMEDOUT    = WSAETIMEDOUT, 
#else 
    BADF        = EBADF,
    TIMEDOUT    = ETIMEDOUT,
    INVAL       = EINVAL,
    ACCES       = EACCES,
    PROTOTYPE   = EPROTOTYPE,
    NOBUFS      = ENOBUFS,
    NOMEM       = ENOMEM
#endif
};

};

NETB_END

#endif

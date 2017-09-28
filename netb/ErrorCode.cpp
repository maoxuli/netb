/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#include "ErrorCode.hpp"

NETB_BEGIN

namespace ErrorCode 
{

// Get system last-error
int Current()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

// Set system last error
void SetCurrent(int code)
{
#ifdef _WIN32
    WSASetLastError(code);
#else
    errno = code;
#endif    
}

// Text description of last-error
std::string Description()
{
    return Description(Current());
}

// Text description of given error code
std::string Description(int code)
{
    assert(false);
    return "";
}

} // namespace ErrorCode

NETB_END

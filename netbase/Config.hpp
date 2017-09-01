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

#ifndef NET_BASE_CONFIG_HPP
#define NET_BASE_CONFIG_HPP

//
// Namespace may be changed to adapt to project
//
#define NET_BASE_NAMESPACE

#ifdef NET_BASE_NAMESPACE
#   define NET_BASE_BEGIN   namespace netbase {
#   define NET_BASE_END     }
#else
#   define NET_BASE_BEGIN
#   define NET_BASE_END
#endif

//
// Header files that used everywhere
// 
#include <iostream>
#include <sstream>
#include <cassert>

#endif

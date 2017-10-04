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

#ifndef NETB_SOCKET_ERROR_HPP
#define NETB_SOCKET_ERROR_HPP

#include "Config.hpp"
#include "Error.hpp"
#include "ErrorClass.hpp"
#include "ErrorCode.hpp"

NETB_BEGIN

//
// Socket API usually return -1 (SOCKET_ERROR) and set errno/last-error 
// on failure. The most simple handling on the errors is to return the 
// error code and context to the caller, who will determin the following 
// actions. To do this, we usually need to set a error object with current 
// error code, error message, and error classification. 
//
// The fact is there is also some error status returned by socket API that
// may be handled with always the same following actions, for example 
// repeating the calling, waiting for a while to detect status, and so on. 
// In these cases, we should provide more checking on current error status 
// to determine the direct actions. 
// 
// Some functions useful for abovementioned error handling of socket API 
// are implemented in the name sapce of SocketError.  
//

namespace SocketError 
{

//
// Once a socket API returned with error code, the general action following 
// is determined by the code. In general, if the error can not be recovered 
// locally, the reasonable action is to return the error code and details to 
// caller. The functions below is used to set the error object to hold current 
// error code and error message. The error codes are not classified in these 
// functions so the error object is set to unclassified (or default class). 
// 

void SetError(Error* e, const std::string& msg, int code);
inline void SetError(Error* e, const std::string& msg)
{
    SetError(e, msg, ErrorCode::Current());
}

//
// Using name space to call the function above is not convenient. 
// Marcro below may be used to do the same thing. Only for the function on current error code, i.e., 
// SocketError::SetError(e, msg);
//
#define SET_SOCKET_ERROR(e, msg) do{ SocketError::SetError(e, (Error::MessageStream() << msg)); } while(0) 

//
// Sorting error codes to classify the errors is helpful for error handling 
// base on the reuturned error object. One possible way is to sort the error 
// codes in socket API directly into error classifications by their general 
// meaning. It is direct and simple. But the better way maybe is sorting the 
// error codes returned by socket API in the context of the function that 
// just returned. Less comparison is needed and more context information is 
// available. 
// 
// The marcro below may be used to declare error object setting functions 
// in terms of the function that just returned, i.e., declare same functions 
// in the same name space of the function that returned the error. 
// 

#define DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(FUNC)                 \
    namespace FUNC                                                  \
    {                                                               \
        void SetError(Error* e, const std::string& msg, int code);  \
        inline void SetError(Error* e, const std::string& msg)      \
        {                                                           \
            SetError(e, msg, ErrorCode::Current());                 \
        }                                                           \
    } 

//
// We declare set socket error functions in terms of the function that 
// just retuned. Accordingly, sorting the error codes and set the error 
// object need to be done in each function declared here. 
// 

DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Select)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Open)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Close)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Shutdown)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Bind)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Connect)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Listen)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Accept)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Send)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Receive)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Name)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Control)
DECLARE_SET_SOCKET_ERROR_FOR_FUNCTION(Option)

//
// Using two levels of namespaces to call the functions above are not covenient, e.g., 
// SocketError::Connect::SetError(e, ...);
//
// Macros below may be used to do the same things. Only for the functions on current error code, e.g., 
// SocketError::Open::SetError(e, msg);
// 

#define SET_SOCKET_SELECT_ERROR(e, msg) do{ SocketError::Select::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_OPEN_ERROR(e, msg) do{ SocketError::Open::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_CLOSE_ERROR(e, msg) do{ SocketError::Close::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_SHUTDOWN_ERROR(e, msg) do{ SocketError::Shutdown::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_BIND_ERROR(e, msg) do{ SocketError::Bind::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_CONNECT_ERROR(e, msg) do{ SocketError::Connect::SetError(e, (Error::MessageStream() << msg)); } while(0)
#define SET_SOCKET_LISTEN_ERROR(e, msg) do{ SocketError::Listen::SetError(e, (Error::MessageStream() << msg)); } while(0)
#define SET_SOCKET_ACCEPT_ERROR(e, msg) do{ SocketError::Accept::SetError(e, (Error::MessageStream() << msg)); } while(0)
#define SET_SOCKET_SEND_ERROR(e, msg) do{ SocketError::Send::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_RECEIVE_ERROR(e, msg) do{ SocketError::Receive::SetError(e, (Error::MessageStream() << msg)); } while(0) 
#define SET_SOCKET_NAME_ERROR(e, msg) do{ SocketError::Name::SetError(e, (Error::MessageStream() << msg)); } while(0)
#define SET_SOCKET_CONTROL_ERROR(e, msg) do{ SocketError::Control::SetError(e, (Error::MessageStream() << msg)); } while(0)
#define SET_SOCKET_OPTION_ERROR(e, msg) do{ SocketError::Option::SetError(e, (Error::MessageStream() << msg)); } while(0) 

//
// While in some cases, socket API returned with an error that may be recovered 
// with almost the same following actions, for example, repeating the calling, 
// or waiting for a while to detect the status. so it is necessary in some cases 
// to check particular error code to determine possilbe following actions. 
//
// System interruption is one of the general status that may cause many socket 
// functions returned. The usual action for this error status is to repeat the 
// calling. So it is necessary to check this error sometimes if a function may 
// raise it.  
//

bool Interrupted(int code);
inline bool Interrupted() 
{
    return Interrupted(ErrorCode::Current());
}

// 
// Would block is one of the general status that may cause many socket I/O 
// functions working in non-block mode returned. It usually means the system 
// is not ready to complete the I/O. The usual action for this error status is 
// to repeat the calling after a while. So it is necessary to check this error 
// sometimes if a I/O function may reaise it. 
//

bool WouldBlock(int code);
inline bool WouldBlock()
{
    return WouldBlock(ErrorCode::Current());
}

//
// Socket connect working in non-block mode may return error status to 
// indicate the connecting is still in progress. The following actions may be 
// waiting for a while and detect the socket status. 
//

bool InProgress(int code);
inline bool InProgress()
{
    return InProgress(ErrorCode::Current());
}

//
// If you decide to do some more checking on the error code returned by a socket 
// function, just like above, declare your error checking function the name space 
// of SocketError. Particularly, if you wanna to declare an error checking function 
// only working on calling a particular socket function, you may declare the error 
// chekcing function in the same name space of the function.  
// 

} // namespace SocketError


NETB_END

#endif

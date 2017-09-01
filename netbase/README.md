# NetBase

NetBase aims to a lightweight C++ wrapper of socket API, rather than a C++ framework for socket programming. It is desinged in an incremental style and works in a building blocks style, which provide practical workaround to overcome the "heavyweight" issue of most C++ socket API libraries. 

## Three levels of error handling

1. Check errors by catching exceptions
Exception is standard mechanism for C++ to handle errors, and by default, NetBase API will throw exceptions on errors. The caller may handle erros with "try...catch..." clause. 

2. Check errors by out parater
To avoid using exception, the caller may pass a Error object into the NetBase API, and then the API will not throw any exception. The Error object works as an out parameter to return the datails of errors. 

3. Only check success and failure
If the caller only care if the NetBase API is working properly, and does not care the details of errors, a NULL can be passed to the out parameter of Error object, then the API will neither throw exception nor return error object. The return value of the API usually carry information that may indicate if the API is successful or failed.  

It is worth to mention that, the three levels of error handling can be mixed to use as requirements. That means you may catch an exception and then ignore it or return it by error object, and you may get error by out parameter and then throw a exception based on the error objet. The design supports extensions to classify errors and associate a classfication of error to an exception. Please refer files of Error, Exception, and ErrorCode for details about the error handling.  

## Three levels of socket API wrapper

NetBase starts from a very thin wrapper of socket API that consists of a series of C++ classes, including Socket, SocketSelector, SocketAddress, and StreamBuffer. Each of these classes is designed to wrap an relative independent aspect of socket API, and works indepently as much as possible. Socket is a class that wraps a socket descriptor and all possible operations on the descriptor. SocketSelector is a class that wraps select operation on multiple socket descriptors. SocketAddress is a wrapper class of address structures used in socket programming. StreamBuffer is a byte buffer that may used in socket I/O. particularly, the class of Socket is designed as a complete wrapper of all possible operations for all kinds of socket descriptors (with different domain, type, and protocol), rather than the common part of different sockets like that in most socket libraries. This leads to a important follow-up design rule: Socket is used in composition mode (protected or private derivation or member object), rather than public derivation, to extend classes for particular kind of socket. With Socket class, NetBase privides a complete wrapper of socket API, by which you may complete any socket programming without any extension. With this thin wrapper, socket programming is still in a similar mode to original socket API, but with concise code and safe guarantee. 

NetBase further wraps the popular socket protocols with some C++ classess. TcpAcceptor and TcpSocket wrap socket API for TCP protocol. UdpSocket wrap socket API for UDP protocol. They follow the genral operation flows of particular socket protocol and hide some details of socket API. Basically, these classes are still a simple wrapper of socket API, working in either block or non-block mode, lacking of a complete support of popular asynchronous I/O mode. 

Asynchronous, however, is a important feature that is necessary in most network applicaitons, and thus is necessary in NetBase, as a library for network programming. NetBase further extends socket wrapper classess to support async I/O, that includes AsyncTcpAcceptor and AsyncTcpSocket, AsyncUdpSocket. They derive from the non-async classes and only extend the features of async I/O.    

It is worth to mention again that, NetBase is designed as only socket API rather than network programming framework. The classess for TCP and UDP programming, either in block or non-block mode, sync or async I/O, provide only socket API wrapper and there is no application logic. Different from most socket API libraries, it is easy for NetBase to mix using sync and async I/O, because the objects are designed and used in a building block mode, and you may mix them as freely.   

## One event loop per thread

Async mode socket API needs a mechanism of async driving, usually multi-threading. EventLoop, EventSelector, and EventHandler are designed as a basic aysnc driving mode. EvenLoopThread is used to creating separate thread for aync mode socket I/O.  

## Streaming buffer for socket I/O

Steaming buffer is an important feature for async I/O. The data is usually write into the buffer asynchronously and notifiy the application, then the data is read from the buffer. Particularly, the buffer usually support streaming like read and write, i.e., the data in buffer is not necessary completely processed in one I/O cycle, and can be accumulated for next cycle. StreamBuffer is designed as a byte buffer working in streaming style. StreamBuffer is the basic object used in NetBase to transfer and buffer data in socket API. StreamWrite, StreamReader, StreamPeeker are tool classes for writing and reading data into/from buffer, which are useful in message packaging and unpackaging. 

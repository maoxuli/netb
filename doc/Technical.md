# NetB Technical Discussion 

*Note: It is not necessary for you to understand anything in this document, this is merely for those curious about the initial ideas and inner logic of NetB.*

## Using NetB never means binding to it.

In general, a C++ library is more suitable for a kind of software framework, rather thank components with interface. A C++ network (or specifically socket) library then usually builds a class hierarchy and programming will based on those classes. It needs to be determined at the beginning using the C++ classes or using the original socket API, using this class or using that class. Mixing of all availabe implementations is not that convenient, if not impossible. NetB is making effert to break these limitations. It is designed as functional blocks, rather than a framework, so that you may switch among original socket API and NetB classes with different features at any points of your programming. 

NetB designed a basic class named Socket, but different from those in most network libraries, this Socket is not only a base class of the class hierarchy to implement common functions. It is a complete wrapper of socket API and keeps the complete logic of original socket API, with only extensions for crossing platforms and more powserful error handling with optional C++ standard exceptions. It can complete everything original socket API can do. So it is always easy to mix or switch between original socket API and Socket object in network programming on a socket descriptor. For example, establish a TCP connection with Socket object, and then complete data I/O with original socket API or third-party libraries, or vice versa. 

NetB further designed classes based on Socket to work on specific protocols or support particular I/O features. For example, TcpAcceptor wrapps a socket descriptor working as TCP server to accept incomming connections, while AsyncTcpAcceptor extends TcpAcceptor to accept incomming connections in asynchronous mode. These classes are also coorperative with original socket API, or with each other, which makes it possible to have a socket working in various I/O modes with different wrapper objects. The capability of mixing and swtiching makes it easy to integrate NetB classes in programming with other libraries.  

The classes in NetB for other functions, including socket address, socket I/O demultiplexer, I/O stream buffer, event-driven I/O notifications, and error handling, are also designed in a loose coupling style. The final purpose is to make NetB working as a collection of functional blocks. You may use one or more of the blocks, use this block or that block, use one at first and switch to another later. Using NetB never means binding to it.  

Please refer to the examples of "tcp", "udp", and "echo", which illustrate the using of different classes to complete the same functions. 

## StreamBuffer is designed for protocol message serialization  

As usual in most network libraries, NetB implements a wrapper class for I/O buffer. Socket API has no particluar specifications for a I/O buffer, except for it is a block of memory denoted by a starting address and a writable size. The usual step of socket I/O is to copy user's data to or from the I/O buffer at a properiate time. It is necessary because the data processing and the socket I/O may be not synchronous, e.g. data may not be prossed completly in a I/O cycle. A possible strategy to increase socket I/O performance is to avoid or reduce user's data copy. This may be realized with a dedicated designed I/O buffer that works in a streaming style, e.g., the data not processed is kept in buffer and the new data is written into the buffer at the following address. Socket I/O and data processing are on the same buffer thus avoid data copy. NetB designed such a buffer named StreamBuffer, which supports streaming style data writing and reading, as well as position based random data peeking and updating.   

To keep simple, StreamBuffer is designed with only interface for reading and writing on a sequence of bytes, rather than the known data types, such as integer, float, or string. Some helper classes are thus designed to support known-type data reading and writing. The interface of the helper classes are designed to be suitable for protocol message serialization. The design of StreamBuffer and the tool classes also makes it easy to coorperate with a plain memory buffer, or third-party I/O buffer implementations.  

Please refer to the examples of "http" and "dns", which illustrate the using of StreamBuffer and the helper classes to pack and unpack a protocol message.  

## Cooperating with thir-party event-driven notification libraries  

NetB has an internal implementation of a simplified event-driven notification mechanism and the asynchronous I/O socket classes based on it. It provides a convenient way to introduce asynchfronous I/O mode in network applications. However, if you would like to use the third-party event-driven notification libraries, such as libevent, libev, and libuv, what you need to do is just working on the non-async socket classes.  












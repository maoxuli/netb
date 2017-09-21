# NetB - Base and Blocks for Network Programming  

NetB is a C++ library (not framework) that implements object-oritented interface for network programming. Basically, it was designed with features include: 

**1. A loose coupling object-oriented socket API**         

NetB was not designed with a complicated object hierarchy for socket programming, as usual in most similar libraries. It however implemented a series of loose coupling objects, which may work independently or coorperatively. This feature let NetB working in various weights: from a very thin wrappper of original socket API to a complete object-oritented socket interface with various I/O modes and detailed error handling. It also made it easy to program mixing NetB with any other third-party network libraries. Using NetB never means binding to NetB. 

**2. Error handling with exceptions is not the only option**    

Error handling is necessary in any serious programming, while C++ standard exception is an angel for some programmers and an evil for others. It is always a hard decision for C++ library to throw exceptions or not. NetB supports exceptions, if you like it. NetB does not throw any exceptions, if you don't like it. But those are not the most great when you may determine a single NetB calling throwing exceptions or not. Without exceptions, the only return value is however too weak. NetB introduced a error object in the calling that may raise errors to provide the same power of error handling as exceptions. Error handling styles are not determined on NetB library level, but on per-calling of functions, which means you may switch at any points.  

**3. Asynchronous I/O with event-driven notifications**  

Asynchronous I/O is a basis for high performance network services. Event-driven notification and socket complexing are basic mechanisms of asynchronous socket I/O. Some great implementations can be found in open-source libraries, such as libevent, libev, and libuv. NetB did not mind to re-invent the wheels, but implemented a similiar while more simple interface in object-oriented style. For example, the performance is not the most concern of NetB, the I/O events are always based on "select" on all platforms, rather than the native alternatives with better performance. NetB only focused on the events of network I/O (i.e., socket ready events). NetB supports an event loop per thread, but gurantees the safe of functional calling across threads.   

**4. An I/O buffer supports network protocol serialization**   

A necessary function of network programming is to transfer protocol message over networks, and the workflows of messaging in different applications almost keep unchanged: assembling a protocol message, packing the message into an I/O buffer, sending the data in buffer over a socket, receiving data from socket into an I/O buffer, unpacking a protocol message from the data in buffer, and processing the protocol message. It is obviously that, to get better performance, data should be copied as less as possible in the workflow. An application protocol I/O may be across multiple network I/Os, so a better buffer in such cases should supports writing and reading continously (or streamingly) to avoid copying data per netowrk I/O. In addition, the application protocols are various and packing and unpacking of a prototocol message may be not always a sequential process, so the buffer should also support random access. NetB provided an implementation of such I/O buffer and a set of tools to simplify the packing and unpacking of application protocol messages.    

**5. Essential support of important network protocols**    

NetB did not implement application frameworks for network protcols as usual. It only implemented some wrappers of protocol messages and messaging flows. Tha fact is the application frameworks using network protocols are always various dramatically, with the only unchanged protocol message formats and messaging flows. NetB provides essential support of impotant application protcols, such as HTTP, DNS, and STUN, to make it easy enough to assemble any kind of network applications.  

## Build NetB

NetB is currently in dense development and the supporting platforms will include Linux, FreeBSD, Mac OSX, Solaris,and Windows. While to date, the major work of development is on Mac OSX EI Capitan. The pre-release version on Mac OSX and Linux should be completed recently. Please follow our updates. 

1. Build NetB Static Library

The target of "all" in make file is used to build a static library, which by default produces an output file libnetb.a in "lib" folder, based on all source code in "netb" folder.   

Build NetB static library with:  
```shell
make
or
make all  
```

2. Build NetB Examples  

A set of examples have been included to illustrate the using and important features of NetB. Target of "examples" in make file will build all examples in "examples" folder. Other targets with the same names of subfolders in "examples" will build examples in each subfolder. 

Build all NetB examples with:    
```shell
make examples
```

Build examples in "examples/echo" folder with:  
```shell
make echo 
```

3. Clean  

As usual, target "clean" will remove all midlle files produced in building, and "cleanall" will remove all output files. 

```shell
make clean
```

```shell
make cleanall
```

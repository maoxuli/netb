# NetB

## Naming conventions 

File names, class names, and function names (including member functions of class) follow Passcal naming convention. variable names, including object names, always use lower case characters and under-scores. Member variable names of class use under-score prefix. 

## Error handling 

NetB supports error handling either with or without exceptions. It introduced an error object used together with return values to catch details of error status, providing the same power of error handing as the exceptions. The two error handling styles may be mixed and switched at any points. The foundamental implementation is in classes listed below:  

- Exception (and sub-classes)  
- ErrorClass (and sub-classes)  
- Error  
- ErrorCode  
- SocketError  

## A very thin wrapper of socket API  

In some cases, a thin wrapper of socket API that only for the purpose of cross-platform has been enough. The files below include simple C++ wrappers of socket descriptor and associated operations, socket address, and socket multiplexing. They may work independently or coorperatively.    

- Socket  
- SocketSelector  
- SocketAddress  

## Object-oriented socket API  

- TcpAcceptor  
- TcpSocket  
- UdpSocket  

## Event-driven notifications  

I/O demultiplexing with event-driven notifications is the major way to implement asynchronous I/O. NetB introduced the callback style asynchronous I/O interface, which is driven by internal socket ready events.  The model is an event handler per socket, and an event loop per thread. The implementation is in classes listed below: 

- EventHandler 
- EventLoop    
- EventLoopThread  

## Asynchronous Socket I/O   

- AsyncTcpAcceptor  
- AsyncTcpSocket  
- AsyncUdpSocket 

## I/O buffer and protocol message serialization    

- StreamBuffer  
- StreamWriter  
- StreamReader  
- RandomWriter
- RandomReader  

## Application layer protocols

- HttpMessage  
- DnsRecord  
- DnsMessage  

# NetB

## Naming conventions 

File names, class names, and function names (including member functions of class) follow Passcal naming convention. variable names, including object names, always use lower case characters and under-scores. Member variable names of class use under-score prefix. 

## Error handling 

NetB supports error handling either with or without exceptions. It defined an error object used with return values to catch details of error status. The two error handling methods may be used separately or mixed. The foundamental implementation is in classes listed below:  

- Error
- Exception  
- ErrorCode  

## Asynchronous I/O facility  

Socket multiplexing and multi-threading is the major way to implement asynchronous I/O. NetB introduced the callback style asynchronous I/O interface, which is driven by internal socket multiplexing and multi-threading. The implementation is in classes listed below: 

- EventSelector  
- EventHandler  
- SocketPipe  
- EventLoop  
- EventLoopThread  

## A very thin wrapper of socket API  

In some cases, a thin wrapper of socket API that supports cross-platform has been enough. The files below include the wrappers of socket descriptor and associated operations, socket multiplexing, and socket address.  

- Socket  
- SocketSelector  
- SocketAddress  

## Object-oriented socket API  

- TcpAcceptor  
- TcpSocket  
- UdpSocket  

## Asynchronous Socket I/O  

- AsyncTcpAcceptor  
- AsyncTcpSocket  
- AsyncUdpSocket 

## I/O buffer and message packaging  

- StreamBuffer  
- StreamWriter  
- StreamReader  
- StreamPeeker  

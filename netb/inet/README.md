# NetB INet

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

## Application layer protocols

- HttpMessage  
- DnsRecord  
- DnsMessage  
- StunMessage  
- RtspMessage  

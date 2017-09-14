# NetB Examples  

## Simple UDP server and client based on Socket  

Socket is only a thin wrapper of original socket API. UDP programming based on Socket is summarized as below:  

1. Open an UDP socket, send data to given address  
2. Open an UDP socket and bind to a local address, then receive data from any address  
3. Open an UDP socket and associate (connect) a peer address, then only send and receive data to/from the associated address.  

Either as server or client, the I/O is usually always performed on a single UDP socket. So multi-thread I/O model for UDP server is more complicated than TCP server. The general practice is open a UDP socket and perform all I/O on it in a single thread. 

Let's take a look at the source code of a simple UDP echo server as below. It opens an UDP socket by initiation a Socket object and bind to a local address, then receive data over the socket and send back the data to its orginal address.  

```C++
// UDP echo server
// Error handling with exceptions
try
{
    Socket udps(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    udps.Bind(SocketAddress(8080, AF_INET));

    SocketAddress addr;
    char* buf = new char[2048];
    ssize_t ret;
    while((ret = udps.ReceiveFrom(buf, 2048, &addr)) > 0)
    {
        udps.SendTo(buf, ret, &addr);
    }
    delete [] buf;
}
catch(const Exception& ex)
{
    // Error handling
}
```  
*The complete version of this code is in UdpServer1.cpp.*  
*UdpServer2.cpp includes the version without exception.*  

Below is the source code of an UDP echo client. It opens a socket and send data to server address directly. During this process, the system will bind a random local address to the socket implicitly, so the data sent back from server can be received in the following operations.  

```C++
// UDP echo client
// Error handling with exceptions
Socket udpc;
try
{
    udpc.Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
}
catch(const Exception& ex)
{
    // Error handling
    return;
}

Error e;
SocketAddress addr("", 8080, AF_INET);
std::string msg = "Hello";
char* buf = new char[2048];
ssize_t ret = 0;
if((ret = udpc.SendTo(msg.data(), msg.length(), &addr, 0, &e)) <= 0 ||
   (ret = udpc.ReceiveFrom(buf, 2048, &addr, 0, &e)) <= 0)
{
    // Error handling
}
else 
{
    // Process received data in buf
}
delete [] buf;
```
*The complete version of this code is in UdpClient1.cpp.*  
*UdpClient2.cpp includes the version without exception.*  

## Simplify UDP programming with UdpSocket 

UdpSocket is a wrapper class specific to UDP programming, which further simplified network programming for UDP protocol. Below is the TCP echo server based on UdpSocket. 

*UdpServer3.cpp*  
*UdpServer4.cpp* 

*UdpClient3.cpp*   
*UdpClient4.cpp*  

## Synchoronous I/O with timeout  

*UdpServer5.cpp*  
*UdpServer6.cpp*  

*UdpClient5.cpp*   
*UdpClient6.cpp*  

## Asynchronous I/O with AsyncUdpSocket  

Please refer examples of echo for more details in asynchronous I/O.  

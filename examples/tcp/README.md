# NetB Examples  

## Simple TCP server and client based on Socket  

Socket is a simple but a complete wrapper class for socket descriptor and the associated operations,based on which we can implement a general TCP server that accepts incomming connections and perform data I/O on the connections. The work flow is almost same as the original socket API, as listed below:  

1. Open a TCP socket  
2. Bind a local address  
3. Listen on incomming connections  
4. Accept an incomming connection  
5. Perform I/O on accepted connection  

Let's first take a look at the source code below. It includes two possible implementations of TCP echo server with a Socket object.  

```C++
// TCP echo server
// Error handling with exceptions
try
{
    Socket tcps(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    tcps.Bind(SocketAddress(8080, AF_INET));
    tcps.Listen();

    Socket conn;
    char* buf = new char[2048];
    while(conn.Attach(tcps.Accept()))
    {
        ssize_t ret;
        while((ret = conn.Receive(buf, 2048)) > 0)
        {
            conn.Send(buf, ret);
        }
    }
    delete [] buf;
}
catch(const Exception& ex)
{
    std::cout << "Exception: " << ex.Report() << std::endl;
}
```

```C++
// TCP echo server
// Error handling with return values and error object
Error e;
Socket tcps;
if(!tcps.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &e) || 
   !tcps.Bind(SocketAddress(8080, AF_INET), &e) || 
   !tcps.Listen(-1, &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

Socket conn;
char* buf = new char[2048];
while(conn.Attach(tcps.Accept(&e)))
{
    ssize_t ret;
    while((ret = conn.Receive(buf, 2048)) > 0)
    {
        conn.Send(buf, ret);
    }
}
delete [] buf;
std::cout << "Error: " << e.Report() << std::endl;
```

As the comments pointed out, the essential difference between the two implementations is error handling style. The former is based on exceptions, while the latter is based on return values. Socket usually implements two overloading functions for an operation that may cause errors, one throws an exception on error while the other returns status value and an error object. This makes it easy to write code with or without exceptions. Actually, the design of exception object and error object support swithing of error handling style at any point by throwing an exception based on an returned error object, or return an error object based on an catched exception. Please refer the section of error handling for more details.   

In both implementations, a Socket object with successful instatiation and initialization works as TCP server to accept incomming connections, while another Socket object is used to manage the established connection and perform data I/O. The I/O operations of both sockets work in block mode and the server loops to do: wait for and accept a connection, receive data and send back the data. We may summarized the features of this TCP sever as: single thread, block mode, synchronous I/O.  

Actually, Socket also supports non-block mode on I/O operations. The function calling will return immediately, either success or failure. If the return status indicates I/O was just not ready, rather than errors occurred,the application should try the operation again repeatedly in a loop or on I/O ready events monitored by select() or similar mechanisms. Please refer the section of asynchronous I/O for more details.  

Now let's take a look at the source code of corresponding TCP client based on also a Socket object, which connect to TCP server, send message to server and then receive the message from server. It is also works in single thread, block mode, and synchronous I/O. Here is the version with error handling with return values and error object.  

```C++
// TCP echo client
// Error handling with return values and error object
Error e;
Socket tcpc;
if(!tcpc.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &e) ||  
   !tcpc.Connect(SocketAddress("", 8080, AF_INET), &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

std::string msg = "Hello";
char* buf = new char[2048];
ssize_t ret = 0;
if((ret = tcpc.Send(msg.data(), msg.length(),0,  &e)) <= 0 ||
   (ret = tcpc.Receive(buf, 2048, 0, &e)) <= 0)
{
    std::cout << "Error: " << e.Report() << std::endl;
}
else
{
    std::cout << "Received: " << std::string(buf, ret) << std::endl;
}
delete [] buf;
```

Please find the complete source code of above TCP server and TCP client in:  

TcpServer1.cpp  
TcpClient1.cpp  
TcpServer2.cpp  
TcpClient2.cpp  

## Simplify TCP programming with TcpAcceptor and TcpSocket    

TcpAcceptor and TcpSocket are wrapper classes specific to TCP protocol, which further simplified socket programming for TCP protocol. Let's take a look at the corresponding source code as below:    

```C++
// TCP echo server
// Error handling with exceptions
try
{
    TcpAcceptor acceptor(SocketAddress(8080, AF_INET));
    acceptor.Open();

    TcpSocket conn;
    char* buf = new char[2048];
    while(conn.Connected(acceptor.Accept()))
    { 
        int ret;
        while((ret = conn.Receive(buf, 2048)) > 0)
        {
            conn.Send(buf, ret);
        }
    }
    delete [] buf;
}
catch(const Exception& ex)
{
    std::cout << "Exception: " << ex.Report() << std::endl;
}
```

```C++
// TCP echo server
// Error handling with return values and error object
Error e;
TcpAcceptor tcps(SocketAddress(8080, AF_INET));
if(!tcps.Open(&e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

TcpSocket conn;
char* buf = new char[2048];
while(conn.Connected(tcps.Accept(&e), 0, &e))
{
    int ret;
    while((ret = conn.Receive(buf, 2048)) > 0)
    {
        conn.Send(buf, ret);
    }
}
delete [] buf;
std::cout << "Error: " << e.Report() << std::endl;
```

Accordingly, the source code for TCP client is shown as below:  

```C++
// TCP echo client
// Error handling with return values and error object
Error e;
TcpSocket tcpc;
if(!tcpc.Connect(SocketAddress("", 8080, AF_INET), &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

std::string msg = "Hello";
char* buf = new char[2048];
ssize_t ret = 0;
if((ret = tcpc.Send(msg.data(), msg.length(), &e)) <= 0 ||
   (ret = tcpc.Receive(buf, 2048, &e)) <= 0)
{
    std::cout << "Error: " << e.Report() << std::endl;
}
else
{
    std::cout << "Received: " << std::string(buf, ret) << std::endl;
}
delete [] buf;
return 0;
```

Apparently, The TCP server and TCP client based on TcpAcceptor and TcpSocket are still working in single thread, block mode, and synchronous I/O.   

Please find the complete source code of above TCP server and TCP client in:  

TcpServer3.cpp  
TcpClient3.cpp  
TcpServer4.cpp  
TcpClient4.cpp  

## Asynchronous I/O with AsyncTcpAcceptor and AsyncTcpSocket  

Concurrent I/O is a important feature for high performance network applications. It involves socket I/O multiplexing and multithreading. AsyncTcpAcceptor and AsyncTcpSocket implement a callback style asynchronous I/O mechanism, which accept incomming connections and perform I/O asynchronously. The source code below is only for the purpose of showing the work flow of asynchronous I/O, thus omitted some details and error handling.  

```C++
// Demo for work flow only
// TCP server need to manage the list of connections
AsyncTcpSocketList connections;

// Accepted callback
bool OnAccepted(SOCKET s, const SocketAddress* addr)
{
    AsyncTcpSocket* conn = AsyncTcpSocket(s, addr);
    conn.SetReceivedCallback(OnReceived); // set async I/O callback
    conn.Connected();
    connections.Add(conn); // add connection to the list
    return true;
}

// Connected callback
void OnConnected(AsyncTcpSocket* conn, bool connected)
{
    if(!connected)
    {
        connections.Remove(conn); // remove closed connection
    }
}

// Received callback
void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
{
    conn->Send(buf); // send back data
}

// Open TCP server
AsyncTcpAcceptor tcps(SocketAddress(8080, AF_INET));
tcps.SetAcceptedCallback(OnAccepted); // set async connection callback
tcps.Open();
```

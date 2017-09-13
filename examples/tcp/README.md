# NetB Examples

## Simple TCP server and client based on Socket

In object-oriented paradigm, the socket descriptor and associated operations are wrapped into an object, i.e. a Socket object in this case. Socket is a simple but a complete wrapper, based which we may implement a general TCP server that accepts incomming connections and perform data I/O on established connections. The work flow is almost same as the original socket API, as is listed below:

1. Open a TCP socket for TCP server
2. Bind a local address to the socket
3. Start to listen on incomming connections
4. Accept an incomming connection
5. Perform I/O on the established connection

Let's first take a look at the two possible implementations. 

```C++
try
{
    Socket tcps(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    tcps.Bind(SocketAddress(8080, AF_INET));
    tcps.Listen();
    Socket conn;
    char* buf = new char[2048];
    while(conn.Attach(tcps.Accept()).Valid())
    {
        ssize_t ret;
        try // catch I/O exceptions
        {
            while((ret = conn.Receive(buf, 2048)) > 0)
            {
                conn.Send(buf, ret);
            }
        }
        catch(const Exception& ex)
        {
            std::cout << "I/O Exception: " << ex.Report() << std::endl;
        }
    }
}
catch(const Exception& ex)
{
    std::cout << "Exception: " << ex.Report() << std::endl;
}
```

```C++
Error e;
Socket tcps;
if(!tcps.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &e) || 
   !tcps.Bind(SocketAddress(8080, AF_INET, &e) || 
   !tcps.Listen(-1, &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

Sokcet conn;
char* buf = new char[2048];
while(conn.Attach(Accept(nullptr, &e)).Valid())
{
    ssize_t ret;
    while((ret = conn.Receive(buf, 2048, nullptr)) > 0) // ignore I/O errors
    {
        conn.Send(buf, ret, nullptr); // ignore I/O errors
    }
}
std::cout << "Error: " << e.Report() << std::endl;
return;
```

We should notice that the essential difference between the two implementations is the ways of error handling. The former is based on exceptions, while the latter is based on return values. Socket implements a overloading function for the important operations, which returns a status value and an error object, rather than throwing an exception. It makes it easy to write codes with or without exceptions. Actually, we may switch the error handling styles at any point the codes by throwing an exception based on an error object, or return an error object based on an exception. Please refer the section of error handling for more details. 

In both implementations, a Socket object with successful instatiation and initialization works as TCP server to accept incomming connections, and nother Socket object is used to manage the established connection and perform data I/O. Both work in block mode by default: wait for and accept a connection, receive data and send back the data, repeatedly. We may summarized the features of this TCP sever as: single thread, block mode, synchronous I/O. 

Socket also supports non-block mode calling on I/O functions. The function will return immediately, either success or failure. If the return status indicates I/O was not ready, rather than errors occurred,the application should try again repeatedly in a loop or on I/O ready events monitored by select() or similar mechanisms. Please refer the section of asynchronous I/O for more details.  

Now let's take a look at the source code of TCP client based on a Socket object, which connect to TCP server, send some data to server and receive the data from server. It is also works in single thread, block mode,and synchronous I/O.

```C++
Error e;
Socket tcpc;
if(!tcpc.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP) ||  
   !tcpc.Connect(SocketAddress("", 8080, AF_INET, &e), &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

std::string msg = "Hello";
char* buf = new char[2048];
ssize_t ret = 0;
if((ret = tcps.Send(msg.data(), msg.length(), &e)) <= 0 ||
   (ret = tcps.Receive(buf, 2048, &e)) <= 0)
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}
std::cout << "Received: " << std::string(buf, ret) << std::endl;
return;
```

The complete source code of above TCP server and TCP client with exceptions is in TcpServer1.cpp and TcpClient1.cpp. The no exception version is in TcpServer2.cpp and TcpClient2.cpp.

## Simplify TCP programming with TcpAcceptor and TcpSocket  

TcpAcceptor and TcpSocket are wrapper class specific to TCP protocol that further simplified socket programming for TCP protocol. Let's take a look at the source code of server:  

```C++
// Error handling with exceptions
try
{
    TcpAcceptor acceptor(SocketAddress(8080, AF_INET));
    acceptor.Open();
    TcpSocket conn;
    while(conn.Connected(acceptor.Accept()))
    {
        int ret;
        try // catch I/O exceptions
        {
            while((ret = conn.Receive(buf, 2048)) > 0)
            {
                conn.Send(buf, ret);
            }
        }
        catch(const Exception& ex)
        {
            std::cout << "I/O Exception: " << ex.Report() << std::endl;
        }
    }
}

// Error handling with return values
Error e;
TcpAcceptor tcps(SocketAddress(8080, AF_INET, nullptr));
if(!tcps.Open(&e))
{
    std::cout << "Error: " << e.Reprot() << std::endl;
}
TcpSocket conn;
while(conn.Connected(tcps.Accept(&e), nullptr, &e))
{
    int ret;
    while((ret = conn.Receive(buf, 2048, nullptr)) > 0) // ignore I/O errors
    {
        conn.Send(buf, ret, nullptr); // ignore I/O errors
    }
}
std::cout << "Error: " << e.Reprot() << std::endl;
```
Accordingly, the source code for TCP client is shown as below:

```C++
Error e;
TcpSocket tcpc;
if(!tcps.Connect(SocketAddress("", 8080, AF_INET, &e), &e))
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}

std::string msg = "Hello";
char* buf = new char[2048];
ssize_t ret = 0;
if((ret = tcps.Send(msg.data(), msg.length(), &e)) <= 0 ||
   (ret = tcps.Receive(buf, 2048, &e)) <= 0)
{
    std::cout << "Error: " << e.Report() << std::endl;
    return;
}
std::cout << "Received: " << std::string(buf, ret) << std::endl;
return;

```

Apparently, both above TCP server and TCP client based on TcpAcceptor and TcpSocket are still working in single thread, block mode, and synchronous I/O. 

## Asynchronous I/O with AsyncTcpAcceptor and AsyncTcpSocket

Concurrency is a key point in high performance network applications. It involves socket I/O multiplexing and multithreading. AsyncTcpAcceptor implements a callback style asynchronous mechanism to accept incomming connections and perform I/O asynchronously. The source code below is only for the purpose of showing work flow, thus omitted all details and error handling. 

```C++
// TCP server need manage the list of connection
AsyncTcpSocketList list;

// Connected callback, remove closed connection
void OnConnected(AsyncTcpSocket* conn, bool connected)
{
    if(!connected)
    list.Remove(conn);
}
// Received callback, send back the received data
void OnReceived(AsyncTcpSocket* conn, StreamBuffer* buf)
{
    conn->Send(buf);
}

// Accepted callback, set async I/O callback
bool OnAccepted(SOCKET s, const SocketAddress* addr)
{
    AsyncTcpSocket* conn = AsyncTcpSocket(s, addr);
    conn.SetReceivedCallback(OnReceived);
    conn.Connected();
    list.Add(conn);
}

// Open TCP server, set async connection callback
AsyncTcpAcceptor tcps(SocketAddress(8080));
tcps.SetAcceptedCallback(OnAccepted);
tcps.Open();
```
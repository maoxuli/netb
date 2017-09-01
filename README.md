# NetBase

A lightweight C++ socket API.

## Lightweight, Versatile, and Flexible 

1. Exception, error object, or just return value

NetBase supports errer handling with exception as standard suggested. For example, TcpAcceptor::Open() ma throw various exceptions and we may handle them as below:

```C++
try
{
    TcpAcceptor acceptor(8080);
    acceptor.Open();
}
catch(const AddressException& ex)
{
    std::cout << ex.Class().Name() << ":" << ex.Info() << ":" << ex.Code() << "\n";
}
catch(const SocketException& ex)
{
    std::cout << ex.Class().Name() << ":" << ex.Info() << ":" << ex.Code() << "\n";
}
catch(const Exception& ex)
{
    std::cout << ex.Class().Name() << ":" << ex.Info() << ":" << ex.Code() << "\n";
}
```

But sometimes, we may want to avoid exception handling in the program. NetBase supports an out parameter in most API to return the error status. 

```C++
TcpAcceptor acceptor(8080);
Error e;
if(!acceptor.Open(&e))
{
    std::cout << e.Class().Name() << ":" << e.Info() << ":" e.Code() << "\n";
}
```

Actually, at any point of the program, it is easy for caller to switch between exception and error return. For example, check errors with return object and throw an exception: 

```C++
TcpAcceptor acceptor(8080);
Error e;
if(!acceptor.Open(&e))
{
    e.Class().Throw();
}
```

And catch an exception then return an error object: 

```C++
try
{
    TcpAcceptor acceptor(8080);
    acceptor.Open();
}
catch(const Exception& ex)
{
    e.Set(ex.Class(), ex.Info(), ex.Code());
}
```

2. Block or non-block, sync or async

TCP server may work in synchronous mode in current thread:  

```C++
TcpAcceptor acceptor(8080);
if(acceptor.Open())
{
    while(true)
    {
        OnAccept(acceptor.Accept());
    }
}
```

While, the established connections my work in async mode in a separate thread:  

```C++
EventLoopThread thread;
EventLoop* loop = thread.Start();
std::vector<AsyncTcpSocket*> sockets;
void OnAccept(SOCKET s)
{
    AsyncTcpSocket* sock = new AsyncTcpSocket(loop, s);
    sock->SetClosedCallback(OnClosed)
    sock->SetReceivedCallback(OnReveived);
    sock->Connected();
    sockets.push_back(sock);
}

void OnClosed(AsyncTcpSocket*)
{
    // erase from list and delete the object
}

void OnReceived(AsyncTcpSocket*, StreamBuffer*)
{
    // Handle received data
}
```

## Build the library

Currently NetBase is in the process of developing and only supports Linux.

1. Building on Linux

make  
make all  
make examples  
make clean  
make cleanall  

2. Building on Windows  

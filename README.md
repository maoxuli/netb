# NetB - Base and Blocks for Network Programming  

Have you very imagined a C++ library for network programming has mixed features as below: 

1. A very thin C++ wrapper of socket API that only focused on cross-platform.
2. An object-oriented wrapper of socket API that simplified socket programming.
3. A socket I/O interface that supports block mode, non-block mode, and timeout mode. 
4. An asynchronous socket I/O that integrated sockets multiplexing and multi-threading. 
5. A real library not framework that consists of loose coupling blocks. 
5. A C++ socket library that supports standard error handling with exceptions. 
6. A C++ socket library that supports error handling without exception. 
7. A lightweight library that does not depend on any third-party codes. 

NetB is building a such versatile while flexible network programming library. Please find more details in README files in source code folder and examples folder.  

## Build NetB

Currently NetB is in dense development and the first supported platforms should Linux and Windows. To date, the major compilation and debug work is completed on Mac OSX EI Capitan. The pre-release version on Linux and Windows should be completed recently. Please follow our updates. 

1. Build NetB static library

The target of "all" in make file is used to build a static library of NetB. Using command below will produce the default output libnetb.a in lib folder. All source code for this static library is in netb folder.  

```shell
make
or
make all  
```

2. Build examples  

A set of examples have been included to illustrate the important features of NetB. Target of "examples" in make file will build all examples in "examples" folder. Some other targets were defined to build the examples in each subfolders. For example,  

Build all examples with command:  
```shell
make examples
```

Build examples in "echo" folder with command:  
```shell
make echo 
```

3. Clean  

As usual, target "clean" will remove all midlle files produced by build process, and "cleanall" will remove all output files of building. 

```shell
make clean
```

```shell
make cleanall
```

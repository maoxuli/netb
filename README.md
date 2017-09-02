# NetB

Base and Blocks for Network Programming

## Versatile and Flexible  

1. W/ or W/O Exceptions  

Exceptions provide a modern way for error handling, while many programmers do not like exceptions, especially those in C++. It is a hard choice for C++ library to support exceptions or not. Sometimes it is required by the project to use exceptions even you hate it, and sometimes vice versa. Some C++ codes switch the interface between w/ or w/o exceptions by compiling macros to workaround this issue. NetB tried a new way of error handling that mixed up exceptions and errors in return values.   

2. Multiple I/O Modes  

Except for the original block or non-block modes for socket API, NetB also supported a block mode with timeout. It is a balanced mode between block or non-block mode, while satisfy most use cases. NetB supported complete asynchronous I/O mode with internal support of multithreading and I/O bufferring.  

3. Base and Blocks for Network Programming  

Most wrappers of socket API provided a set of tightly coupled classes, which made the API more like a framework. NetB was designed in an incremental style and tried to reduce the coupling among classes. That means, you may use only the classes you want, and it usually works well by herself. You don't need to change your codes too much to adapt to the introducing of NetB.   

## Build the library

Currently NetB is in the process of development and only supports Linux OS.

1. Building on Linux  

make  
make all  
make examples  
make clean  
make cleanall  

2. Building on Windows   

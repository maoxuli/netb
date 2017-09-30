# NetB Base

## Error handling 

NetB supports error handling either with or without exceptions. It introduced an error object used together with return values to catch details of error status, providing the same power of error handing as the exceptions. The two error handling styles may be mixed and switched at any points. The foundamental implementation is in classes listed below:  

- Exception (and sub-classes)  
- ErrorClass (and sub-classes)  
- Error  
- ErrorCode  

## I/O buffer and protocol message serialization    

- StreamBuffer  
- StreamWriter  
- StreamReader  
- RandomWriter
- RandomReader  

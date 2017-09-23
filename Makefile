#
# Todo: depedency rules on header files
#

OBJDIR := build
LIBDIR := lib
BINDIR := bin
INCDIR := netb
SRCDIR := netb
OUT := libnetb.a

CXX := g++
CXXFLAGS := -g -Wall -std=c++11
CPPFLAGS := -I./$(INCDIR)

AR := ar
ARFLAGS := rcs

INC := $(INCDIR)/Config.hpp \
	   $(INCDIR)/Uncopyable.hpp \
	   $(INCDIR)/ErrorCode.hpp \
	   $(INCDIR)/Error.hpp \
	   $(INCDIR)/Exception.hpp \
	   $(INCDIR)/StreamBuffer.hpp \
	   $(INCDIR)/SocketConfig.hpp \
	   $(INCDIR)/SocketAddress.hpp \
	   $(INCDIR)/SocketSelector.hpp \
	   $(INCDIR)/Socket.hpp \
	   $(INCDIR)/TcpAcceptor.hpp \
	   $(INCDIR)/TcpSocket.hpp \
	   $(INCDIR)/UdpSocket.hpp \
	   $(INCDIR)/SocketPipe.hpp \
	   $(INCDIR)/EventHandler.hpp \
	   $(INCDIR)/EventLoop.hpp \
	   $(INCDIR)/EventLoopThread.hpp \
	   $(INCDIR)/AsyncTcpAcceptor.hpp \
	   $(INCDIR)/AsyncTcpSocket.hpp \
	   $(INCDIR)/AsyncUdpSocket.hpp \
	   $(INCDIR)/StreamReader.hpp \
	   $(INCDIR)/StreamWriter.hpp \
	   $(INCDIR)/RandomReader.hpp \
	   $(INCDIR)/RandomWriter.hpp \
	   $(INCDIR)/HttpMessage.hpp \
	   $(INCDIR)/DnsRecord.hpp \
	   $(INCDIR)/DnsMessage.hpp
	  
OBJ	:= $(OBJDIR)/ErrorCode.o \
	   $(OBJDIR)/Error.o \
	   $(OBJDIR)/Exception.o \
	   $(OBJDIR)/StreamBuffer.o \
	   $(OBJDIR)/SocketConfig.o \
	   $(OBJDIR)/SocketAddress.o \
	   $(OBJDIR)/SocketSelector.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/TcpAcceptor.o \
	   $(OBJDIR)/TcpSocket.o \
	   $(OBJDIR)/UdpSocket.o \
	   $(OBJDIR)/SocketPipe.o \
	   $(OBJDIR)/EventHandler.o \
	   $(OBJDIR)/EventLoop.o \
	   $(OBJDIR)/EventLoopThread.o \
	   $(OBJDIR)/AsyncTcpAcceptor.o \
	   $(OBJDIR)/AsyncTcpSocket.o \
	   $(OBJDIR)/AsyncUdpSocket.o \
	   $(OBJDIR)/StreamReader.o \
	   $(OBJDIR)/StreamWriter.o \
	   $(OBJDIR)/RandomReader.o \
	   $(OBJDIR)/RandomWriter.o \
	   $(OBJDIR)/HttpMessage.o \
	   $(OBJDIR)/DnsRecord.o \
	   $(OBJDIR)/DnsMessage.o

all: $(LIBDIR)/$(OUT)

$(LIBDIR)/$(OUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: examples tcp udp echo http dns clean cleanall

examples: tcp udp echo http dns

tcp:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcps1 examples/tcp/TcpServer1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcpc1 examples/tcp/TcpClient1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcps2 examples/tcp/TcpServer2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcpc2 examples/tcp/TcpClient2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcps3 examples/tcp/TcpServer3.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcpc3 examples/tcp/TcpClient3.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcps4 examples/tcp/TcpServer4.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/tcpc4 examples/tcp/TcpClient4.cpp 

udp:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udps1 examples/udp/UdpServer1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udpc1 examples/udp/UdpClient1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udps2 examples/udp/UdpServer2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udpc2 examples/udp/UdpClient2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udps3 examples/udp/UdpServer3.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/udpc3 examples/udp/UdpClient3.cpp 

echo:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echos1 examples/echo/EchoServer1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoc1 examples/echo/EchoClient1.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echos2 examples/echo/EchoServer2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoc2 examples/echo/EchoClient2.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echos3 examples/echo/EchoServer3.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoc3 examples/echo/EchoClient3.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echos4 examples/echo/EchoServer4.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoc4 examples/echo/EchoClient4.cpp 

#http:
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/https examples/http/HttpServer.cpp 
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpc examples/http/HttpClient.cpp 

dns:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/dnsr examples/dns/DnsResolver.cpp 
	
clean:
	rm -f $(OBJDIR)/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)
	rm -r $(BINDIR)/*.dSYM
	rm -f $(BINDIR)/tcp*
	rm -f $(BINDIR)/udp*
	rm -f $(BINDIR)/echo*
	rm -f $(BINDIR)/http*
	rm -f $(BINDIR)/dns*

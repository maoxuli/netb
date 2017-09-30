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
CXXFLAGS := -g -Wall -std=c++11 -pthread
CPPFLAGS := -I./$(INCDIR)/base -I./$(INCDIR)/inet

AR := ar
ARFLAGS := rcs

INC := $(INCDIR)/base/Config.hpp \
	   $(INCDIR)/base/Uncopyable.hpp \
	   $(INCDIR)/base/Exception.hpp \
	   $(INCDIR)/base/ErrorClass.hpp \
	   $(INCDIR)/base/Error.hpp \
	   $(INCDIR)/base/ErrorCode.hpp \
	   $(INCDIR)/base/StreamBuffer.hpp \
	   $(INCDIR)/base/StreamReader.hpp \
	   $(INCDIR)/base/StreamWriter.hpp \
	   $(INCDIR)/base/RandomReader.hpp \
	   $(INCDIR)/base/RandomWriter.hpp \
	   $(INCDIR)/inet/SocketConfig.hpp \
	   $(INCDIR)/inet/SocketError.hpp \
	   $(INCDIR)/inet/SocketAddress.hpp \
	   $(INCDIR)/inet/SocketSelector.hpp \
	   $(INCDIR)/inet/Socket.hpp \
	   $(INCDIR)/inet/SocketPipe.hpp \
	   $(INCDIR)/inet/EventHandler.hpp \
	   $(INCDIR)/inet/EventLoop.hpp \
	   $(INCDIR)/inet/EventLoopThread.hpp \
	   $(INCDIR)/inet/TcpAcceptor.hpp \
	   $(INCDIR)/inet/TcpSocket.hpp \
	   $(INCDIR)/inet/UdpSocket.hpp \
	   $(INCDIR)/inet/AsyncTcpAcceptor.hpp \
	   $(INCDIR)/inet/AsyncTcpSocket.hpp \
	   $(INCDIR)/inet/AsyncUdpSocket.hpp \
	   $(INCDIR)/inet/HttpMessage.hpp \
	   $(INCDIR)/inet/DnsRecord.hpp \
	   $(INCDIR)/inet/DnsMessage.hpp
	  
OBJ	:= $(OBJDIR)/base/Exception.o \
	   $(OBJDIR)/base/ErrorClass.o \
	   $(OBJDIR)/base/Error.o \
	   $(OBJDIR)/base/ErrorCode.o \
	   $(OBJDIR)/base/StreamBuffer.o \
	   $(OBJDIR)/base/StreamReader.o \
	   $(OBJDIR)/base/StreamWriter.o \
	   $(OBJDIR)/base/RandomReader.o \
	   $(OBJDIR)/base/RandomWriter.o \
	   $(OBJDIR)/inet/SocketConfig.o \
	   $(OBJDIR)/inet/SocketError.o \
	   $(OBJDIR)/inet/SocketAddress.o \
	   $(OBJDIR)/inet/SocketSelector.o \
	   $(OBJDIR)/inet/Socket.o \
	   $(OBJDIR)/inet/SocketPipe.o \
	   $(OBJDIR)/inet/EventHandler.o \
	   $(OBJDIR)/inet/EventLoop.o \
	   $(OBJDIR)/inet/EventLoopThread.o \
	   $(OBJDIR)/inet/TcpAcceptor.o \
	   $(OBJDIR)/inet/TcpSocket.o \
	   $(OBJDIR)/inet/UdpSocket.o \
	   $(OBJDIR)/inet/AsyncTcpAcceptor.o \
	   $(OBJDIR)/inet/AsyncTcpSocket.o \
	   $(OBJDIR)/inet/AsyncUdpSocket.o \
	   $(OBJDIR)/inet/HttpMessage.o \
	   $(OBJDIR)/inet/DnsRecord.o \
	   $(OBJDIR)/inet/DnsMessage.o

all: $(LIBDIR)/$(OUT)

$(LIBDIR)/$(OUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJDIR)/base/%.o: $(SRCDIR)/base/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/inet/%.o: $(SRCDIR)/inet/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: examples tcp udp echo http dns clean cleanall

examples: tcp udp echo http dns

tcp:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcps1 examples/tcp/TcpServer1.cpp $(LIBDIR)/$(OUT) 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcpc1 examples/tcp/TcpClient1.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcps2 examples/tcp/TcpServer2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcpc2 examples/tcp/TcpClient2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcps3 examples/tcp/TcpServer3.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcpc3 examples/tcp/TcpClient3.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcps4 examples/tcp/TcpServer4.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/tcpc4 examples/tcp/TcpClient4.cpp $(LIBDIR)/$(OUT)

udp:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udps1 examples/udp/UdpServer1.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udpc1 examples/udp/UdpClient1.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udps2 examples/udp/UdpServer2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udpc2 examples/udp/UdpClient2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udps3 examples/udp/UdpServer3.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/udpc3 examples/udp/UdpClient3.cpp $(LIBDIR)/$(OUT)

echo:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echos1 examples/echo/EchoServer1.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echoc1 examples/echo/EchoClient1.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echos2 examples/echo/EchoServer2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echoc2 examples/echo/EchoClient2.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echos3 examples/echo/EchoServer3.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echoc3 examples/echo/EchoClient3.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echos4 examples/echo/EchoServer4.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/echoc4 examples/echo/EchoClient4.cpp $(LIBDIR)/$(OUT)

http:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/https examples/http/HttpServer.cpp $(LIBDIR)/$(OUT)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/httpc examples/http/HttpClient.cpp $(LIBDIR)/$(OUT)

dns:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/dnsr examples/dns/DnsResolver.cpp $(LIBDIR)/$(OUT)

clean:
	rm -f $(OBJDIR)/base/*.o
	rm -f $(OBJDIR)/inet/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)
	rm -fr $(BINDIR)/*.dSYM
	rm -f $(BINDIR)/tcp*
	rm -f $(BINDIR)/udp*
	rm -f $(BINDIR)/echo*
	rm -f $(BINDIR)/http*
	rm -f $(BINDIR)/dns*

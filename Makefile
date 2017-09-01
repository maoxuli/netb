#
# Todo: depedency rules on header files
#

OBJDIR := build
LIBDIR := lib
BINDIR := bin
INCDIR := netbase
SRCDIR := netbase
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
	   $(INCDIR)/StreamWriter.hpp \
	   $(INCDIR)/StreamReader.hpp \
	   $(INCDIR)/StreamPeeker.hpp \
	   $(INCDIR)/SocketConfig.hpp \
	   $(INCDIR)/SocketAddress.hpp \
	   $(INCDIR)/SocketSelector.hpp \
	   $(INCDIR)/Socket.hpp \
	   $(INCDIR)/TcpAcceptor.hpp \
	   $(INCDIR)/TcpSocket.hpp \
	   $(INCDIR)/UdpSocket.hpp \
	   $(INCDIR)/SocketPipe.hpp \
	   $(INCDIR)/EventHandler.hpp \
	   $(INCDIR)/EventSelector.hpp \
	   $(INCDIR)/EventLoop.hpp \
	   $(INCDIR)/EventLoopThread.hpp \
	   $(INCDIR)/AsyncTcpAcceptor.hpp \
	   $(INCDIR)/AsyncTcpSocket.hpp \
	   $(INCDIR)/AsyncUdpSocket.hpp
	  
OBJ	:= $(OBJDIR)/ErrorCode.o \
	   $(OBJDIR)/Error.o \
	   $(OBJDIR)/Exception.o \
	   $(OBJDIR)/StreamBuffer.o \
	   $(OBJDIR)/StreamWriter.o \
	   $(OBJDIR)/StreamReader.o \
	   $(OBJDIR)/StreamPeeker.o \
	   $(OBJDIR)/SocketConfig.o \
	   $(OBJDIR)/SocketAddress.o \
	   $(OBJDIR)/SocketSelector.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/TcpAcceptor.o \
	   $(OBJDIR)/TcpSocket.o \
	   $(OBJDIR)/UdpSocket.o \
	   $(OBJDIR)/SocketPipe.o \
	   $(OBJDIR)/EventHandler.o \
	   $(OBJDIR)/EventSelector.o \
	   $(OBJDIR)/EventLoop.o \
	   $(OBJDIR)/EventLoopThread.o \
	   $(OBJDIR)/AsyncTcpAcceptor.o \
	   $(OBJDIR)/AsyncTcpSocket.o \
	   $(OBJDIR)/AsyncUdpSocket.o

all: $(LIBDIR)/$(OUT)

$(LIBDIR)/$(OUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: examples clean cleanall

examples:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpserver examples/http/HttpMessage.cpp examples/http/HttpServer.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpclient examples/http/HttpMessage.cpp examples/http/HttpClient.cpp 
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoserver examples/echo/EchoServer.cpp 
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoclient examples/echo/EchoClient.cpp 
	
clean:
	rm -f $(OBJDIR)/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)
	rm -r $(BINDIR)/*.dSYM
	rm -f $(BINDIR)/httpserver
	rm -f $(BINDIR)/httpclient
	rm -f $(BINDIR)/echoserver
	rm -f $(BINDIR)/echoclient

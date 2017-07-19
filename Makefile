#
# Todo: depedency rules on header files
#

INCDIR := netbase
SRCDIR := netbase
OBJDIR := build
LIBDIR := lib
BINDIR := bin
OUT := libnetb.a

CXX := g++
CXXFLAGS := -g -Wall -std=c++11
CPPFLAGS := -I./$(INCDIR)

AR := ar
ARFLAGS := rcs

INC := $(INCDIR)/Config.h \
	   $(INCDIR)/SocketDef.h \
	   $(INCDIR)/SocketError.h \
	   $(INCDIR)/Socket.h \
	   $(INCDIR)/SocketSelector.h \
	   $(INCDIR)/SocketAddress.h \
	   $(INCDIR)/StreamBuffer.h \
	   $(INCDIR)/StreamWriter.h \
	   $(INCDIR)/StreamReader.h \
	   $(INCDIR)/StreamPeeker.h \
	   $(INCDIR)/TcpAcceptor.h \
	   $(INCDIR)/TcpSocket.h \
	   $(INCDIR)/UdpSocket.h \
	   $(INCDIR)/SocketPipe.h \
	   $(INCDIR)/EventHandler.h \
	   $(INCDIR)/EventSelector.h \
	   $(INCDIR)/EventLoop.h \
	   $(INCDIR)/EventLoopThread.h \
	   $(INCDIR)/AsyncAcceptor.h \
	   $(INCDIR)/AsyncTcpSocket.h \
	   $(INCDIR)/AsyncUdpSocket.h
	  
OBJ	:= $(OBJDIR)/SocketError.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/SocketSelector.o \
	   $(OBJDIR)/SocketAddress.o \
	   $(OBJDIR)/StreamBuffer.o \
	   $(OBJDIR)/StreamWriter.o \
	   $(OBJDIR)/StreamReader.o \
	   $(OBJDIR)/StreamPeeker.o \
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
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoserver examples/echo/EchoServer.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/echoclient examples/echo/EchoClient.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpclient examples/http/HttpMessage.cpp examples/http/HttpClient.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpserver examples/http/HttpMessage.cpp examples/http/HttpServer.cpp 

clean:
	rm -f $(OBJDIR)/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)
	rm -r $(BINDIR)/*.dSYM
	rm -f $(BINDIR)/echoserver
	rm -f $(BINDIR)/echoclient
	rm -f $(BINDIR)/httpserver
	rm -f $(BINDIR)/httpclient

#
# Todo: 
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

INC := $(INCDIR)/Config.hpp \
	   $(INCDIR)/SocketError.hpp \
	   $(INCDIR)/Socket.hpp \
	   $(INCDIR)/SocketSelector.hpp \
	   $(INCDIR)/SocketAddress.hpp \
	   $(INCDIR)/StreamBuffer.hpp \
	   $(INCDIR)/StreamReader.hpp \
	   $(INCDIR)/StreamWriter.hpp \
	   $(INCDIR)/TcpSocket.hpp \
	   $(INCDIR)/UdpSocket.hpp \
	   $(INCDIR)/RawSocket.hpp \
	   $(INCDIR)/SocketPipe.hpp \
	   $(INCDIR)/EventHandler.hpp \
	   $(INCDIR)/EventSource.hpp \
	   $(INCDIR)/EventSelector.hpp \
	   $(INCDIR)/EventLoop.hpp \
	   $(INCDIR)/EventLoopThread.hpp \
	   $(INCDIR)/TcpListener.hpp \
	   $(INCDIR)/TcpConnector.hpp \
	   $(INCDIR)/TcpConnection.hpp \
	   $(INCDIR)/UdpTransceiver.hpp
	  
OBJ	:= $(OBJDIR)/SocketError.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/SocketSelector.o \
	   $(OBJDIR)/SocketAddress.o \
	   $(OBJDIR)/StreamBuffer.o \
	   $(OBJDIR)/StreamReader.o \
	   $(OBJDIR)/StreamWriter.o \
	   $(OBJDIR)/TcpSocket.o \
	   $(OBJDIR)/UdpSocket.o \
	   $(OBJDIR)/RawSocket.o \
	   $(OBJDIR)/SocketPipe.o \
	   $(OBJDIR)/EventHandler.o \
	   $(OBJDIR)/EventSelector.o \
	   $(OBJDIR)/EventLoop.o \
	   $(OBJDIR)/EventLoopThread.o \
	   $(OBJDIR)/TcpListener.o \
	   $(OBJDIR)/TcpConnector.o \
	   $(OBJDIR)/TcpConnection.o \
	   $(OBJDIR)/UdpTransceiver.o	   

all: $(LIBDIR)/$(OUT)

$(LIBDIR)/$(OUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: examples clean cleanall

examples:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpclient examples/http/HttpMessage.cpp examples/http/HttpClient.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIR)/$(OUT) -o $(BINDIR)/httpserver examples/http/HttpMessage.cpp examples/http/HttpServer.cpp 

clean:
	rm -f $(OBJDIR)/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)
	rm -r $(BINDIR)/*.dSYM
	rm -f $(BINDIR)/httpclient
	rm -f $(BINDIR)/httpserver

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
	   $(INCDIR)/RawSocket.hpp
	  
OBJ	:= $(OBJDIR)/SocketError.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/SocketSelector.o \
	   $(OBJDIR)/SocketAddress.o \
	   $(OBJDIR)/StreamBuffer.o \
	   $(OBJDIR)/StreamReader.o \
	   $(OBJDIR)/StreamWriter.o \
	   $(OBJDIR)/TcpSocket.o \
	   $(OBJDIR)/UdpSocket.o \
	   $(OBJDIR)/RawSocket.o

all: $(LIBDIR)/$(OUT)

$(LIBDIR)/$(OUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean cleanall

clean:
	rm -f $(OBJDIR)/*.o

cleanall: clean
	rm -f $(LIBDIR)/$(OUT)

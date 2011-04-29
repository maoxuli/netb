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
	   $(INCDIR)/SocketSelector.hpp 
	  
OBJ	:= $(OBJDIR)/SocketError.o \
	   $(OBJDIR)/Socket.o \
	   $(OBJDIR)/SocketSelector.o

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

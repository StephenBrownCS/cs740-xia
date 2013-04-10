include ../../xia-core/xia.mk

# These lines are no longer needed
#CC=g++
#CFLAGS=-I../xia-core/XIASocket/API -g
#LDFLAGS =  -lprotobuf -lpthread
#LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py

LDFLAGS += $(LIBS)

all: VideoServer

VideoServer: VideoServer.cpp
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoServer.cpp $(LDFLAGS)

clean:
	rm VideoServer

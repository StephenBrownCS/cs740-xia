include ../../xia-core/xia.mk

# These lines are no longer Needed
#CC=g++
#CFLAGS=-I../../xia-core/api/xsocket
#CFLAGS=-g
#LDFLAGS =  -lprotobuf -lpthread
#LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py
#XIADIR = /home/xia/xia-core

LDFLAGS += $(LIBS)

all: VideoClient

VideoClient: VideoClient.cpp
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.cpp $(LDFLAGS)

#libXsocket.a:
#	ln -s ../../xia-core/api/xsocket/libXsocket.a .

#libXsocket.so:
#	ln -s ../../xia-core/api/xsocket/libXsocket.so .

#_xsocket.so:
#	ln -s ../../xia-core/api/socket/API/python/_xsocket.so .

#xsocket.py:
#	ln -s ../../xia/core/api/socket/API/python/xsocket.py .


clean:
	rm VideoClient

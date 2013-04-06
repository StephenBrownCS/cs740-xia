
CC=g++
CFLAGS=-I../xia-core/XIASocket/API -g
LDFLAGS =  -lprotobuf -lpthread
LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py

all: $(LIBFILES) VideoServer

VideoServer: VideoServer.cpp libXsocket.a
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoServer.cpp libXsocket.a $(LDFLAGS)

libXsocket.a:
	ln -s ../xia-core/XIASocket/API/libXsocket.a .

libXsocket.so:
	ln -s ../xia-core/XIASocket/API/libXsocket.so .

_xsocket.so:
	ln -s ../xia-core/XIASocket/API/python/_xsocket.so .

xsocket.py:
	ln -s ../xia-core/XIASocket/API/python/xsocket.py .


clean:
	rm -f $(LIBFILES)
	rm VideoServer



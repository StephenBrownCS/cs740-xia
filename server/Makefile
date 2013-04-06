
CC=g++
CFLAGS=-I../../XIASocket/API -g
LDFLAGS =  -lprotobuf -lpthread
LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py

all: $(LIBFILES) VideoServer

VideoServer: VideoServer.cpp libXsocket.a
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoServer.cpp libXsocket.a $(LDFLAGS)

libXsocket.a:
	ln -s ../../XIASocket/API/libXsocket.a .

libXsocket.so:
	ln -s ../../XIASocket/API/libXsocket.so .

_xsocket.so:
	ln -s ../../XIASocket/API/python/_xsocket.so .

xsocket.py:
	ln -s ../../XIASocket/API/python/xsocket.py .


clean:
	rm -f $(LIBFILES)
	rm VideoServer



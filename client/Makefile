.PHONY: clean

CC=g++
CFLAGS=-I../XIASocket/API
LDFLAGS =  -lprotobuf -lpthread
LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py

libXsocket.a:
	ln -s ../XIASocket/API/libXsocket.a .

libXsocket.so:
	ln -s ../XIASocket/API/libXsocket.so .

_xsocket.so:
	ln -s ../XIASocket/API/python/_xsocket.so .

xsocket.py:
	ln -s ../XIASocket/API/python/xsocket.py .

all: $(LIBFILES)
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.cpp libXsocket.a  ../../XIASocket/API/*.o $(LDFLAGS)

libXsocket.a:
	ln -s ../XIASocket/API/libXsocket.a .



clean:
	rm *.o



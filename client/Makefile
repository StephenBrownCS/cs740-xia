.PHONY: clean

CC=g++
CFLAGS=-I../XIASocket/API
LDFLAGS =  -lprotobuf -lpthread

all:
	g++ -Wall $(CFLAGS) VideoClient.cpp

libXsocket.a:
	ln -s ../XIASocket/API/libXsocket.a .



clean:
	rm *.o



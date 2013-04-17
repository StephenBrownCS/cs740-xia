include ../../xia-core/xia.mk

# These lines are no longer Needed
#CC=g++
#CFLAGS=-I../../xia-core/api/xsocket
#CFLAGS=-g
#LDFLAGS =  -lprotobuf -lpthread
#LIBFILES = libXsocket.a libXsocket.so _xsocket.so xsocket.py
#XIADIR = /home/xia/xia-core

LIBS += -lsydneyaudio -lvorbis -ltheoradec -logg -lSDL

ifeq "$(UNAME)" "Linux"
INCLUDE=
LIBS += -lasound
endif

ifeq "$(UNAME)" "Darwin"
#INCLUDE=-I/opt/local/include
LIBS += -lSDLmain -framework Carbon -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Cocoa
endif

CFLAGS += -g
LDFLAGS += $(LIBS)

all: VideoClient XChunkSocketStream

VideoClient: VideoClient.cpp Utility XChunkSocketStream
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.cpp Utility.o XChunkSocketStream.o PloggOggDecoder.o $(LDFLAGS)

Utility: Utility.h Utility.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Utility.cpp $(LDFLAGS)

XChunkSocketStream: XChunkSocketStream.cpp XChunkSocketStream.h PloggOggDecoder Utility
	g++ -Wall -Wextra $(CFLAGS) -c XChunkSocketStream.cpp PloggOggDecoder.cpp $(LDFLAGS)

PloggOggDecoder: PloggOggDecoder.cpp PloggOggDecoder.h
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) -o PloggOggDecoder.o PloggOggDecoder.cpp $(LDFLAGS)

clean:
	rm VideoClient *.o *~

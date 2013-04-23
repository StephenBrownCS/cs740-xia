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

all: VideoClient

VideoClient: VideoClient.cpp Utility XChunkSocketStream ClientConfig
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.cpp Utility.o XChunkSocketStream.o PloggOggDecoder.o Chunk.o ChunkQueue.o ChunkFetcher.o ClientConfig.o $(LDFLAGS)

Utility: Utility.h Utility.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Utility.cpp $(LDFLAGS)

Chunk: Chunk.h Chunk.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Chunk.cpp $(LDFLAGS)

XChunkSocketStream: XChunkSocketStream.cpp XChunkSocketStream.h PloggOggDecoder Utility Chunk ChunkFetcher
	g++ -Wall -Wextra $(CFLAGS) -c XChunkSocketStream.cpp PloggOggDecoder.cpp $(LDFLAGS)

PloggOggDecoder: PloggOggDecoder.cpp PloggOggDecoder.h
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) -o PloggOggDecoder.o PloggOggDecoder.cpp $(LDFLAGS)

ChunkQueue: ChunkQueue.h ChunkQueue.cpp Chunk
	g++ -Wall -Wextra $(CFLAGS) -c ChunkQueue.cpp $(LDFLAGS)

ChunkFetcher: ChunkFetcher.h ChunkFetcher.cpp ChunkQueue ClientConfig
	g++ -Wall -Wextra $(CFLAGS) -c ChunkFetcher.cpp $(LDFLAGS)

ClientConfig: ClientConfig.h ClientConfig.cpp
	g++ -Wall -Wextra $(CFLAGS) -c ClientConfig.cpp $(LDFLAGS)

clean:
	rm VideoClient *.o *~

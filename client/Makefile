include ../../xia-core/xia.mk


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

CLIENT_TARGETS = Utility Chunk XChunkSocketStream PloggOggDecoder ChunkQueue ChunkFetcher ClientConfig VideoInformation VideoClient

all:$(CLIENT_TARGETS)

VideoClient: VideoClient.cpp Utility.h XChunkSocketStream.h ClientConfig.h PloggOggDecoder.h VideoInformation.h
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.cpp Utility.o XChunkSocketStream.o PloggOggDecoder.o Chunk.o ChunkQueue.o ChunkFetcher.o ClientConfig.o VideoInformation.o $(LDFLAGS)

Utility: Utility.h Utility.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Utility.cpp

Chunk: Chunk.h Chunk.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Chunk.cpp

XChunkSocketStream: XChunkSocketStream.cpp XChunkSocketStream.h PloggOggDecoder.h Utility.h Chunk.h ChunkFetcher.h
	g++ -Wall -Wextra $(CFLAGS) -c XChunkSocketStream.cpp PloggOggDecoder.cpp

PloggOggDecoder: PloggOggDecoder.cpp PloggOggDecoder.h
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) PloggOggDecoder.cpp

ChunkQueue: ChunkQueue.h ChunkQueue.cpp Chunk.h
	g++ -Wall -Wextra $(CFLAGS) -c ChunkQueue.cpp

ChunkFetcher: ChunkFetcher.h ChunkFetcher.cpp ChunkQueue.h ClientConfig.h VideoInformation.h
	g++ -Wall -Wextra $(CFLAGS) -c ChunkFetcher.cpp

ClientConfig: ClientConfig.h ClientConfig.cpp
	g++ -Wall -Wextra $(CFLAGS) -c ClientConfig.cpp
	
VideoInformation: VideoInformation.h VideoInformation.cpp
	g++ -Wall -Wextra $(CFLAGS) -c VideoInformation.cpp

clean:
	rm VideoClient *.o *~

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

CLIENT_TARGETS = Utility.o Chunk.o XChunkSocketStream.o PloggOggDecoder.o ChunkQueue.o ChunkFetcher.o ClientConfig.o VideoInformation.o VideoClient.o

all: VideoClient

VideoClient: $(CLIENT_TARGETS)
	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoClient.o Utility.o XChunkSocketStream.o PloggOggDecoder.o Chunk.o ChunkQueue.o ChunkFetcher.o ClientConfig.o VideoInformation.o $(LDFLAGS)

VideoClient.o: Utility.h ClientConfig.h ChunkFetcher.h ChunkQueue.h PloggOggDecoder.h Chunk.h XChunkSocketStream.h VideoClient.cpp
	g++ -Wall -Wextra $(CFLAGS) -c VideoClient.cpp
	
Utility.o: Utility.h Utility.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Utility.cpp

Chunk.o: Chunk.h Chunk.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Chunk.cpp

XChunkSocketStream.o: XChunkSocketStream.cpp XChunkSocketStream.h PloggOggDecoder.h Utility.h Chunk.h ClientConfig.h ChunkFetcher.h
	g++ -Wall -Wextra $(CFLAGS) -c XChunkSocketStream.cpp PloggOggDecoder.cpp

PloggOggDecoder.o: PloggOggDecoder.cpp PloggOggDecoder.h
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) PloggOggDecoder.cpp

ChunkQueue.o: ChunkQueue.h ChunkQueue.cpp Chunk.h
	g++ -Wall -Wextra $(CFLAGS) -c ChunkQueue.cpp

ChunkFetcher.o: ChunkFetcher.h ChunkFetcher.cpp ChunkQueue.h ClientConfig.h VideoInformation.h
	g++ -Wall -Wextra $(CFLAGS) -c ChunkFetcher.cpp

ClientConfig.o: ClientConfig.h ClientConfig.cpp
	g++ -Wall -Wextra $(CFLAGS) -c ClientConfig.cpp
	
VideoInformation.o: VideoInformation.h VideoInformation.cpp
	g++ -Wall -Wextra $(CFLAGS) -c VideoInformation.cpp

clean:
	rm VideoClient *.o *~

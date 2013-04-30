include ../../xia-core/xia.mk

CFLAGS += -g
LDFLAGS += $(LIBS)

all: ContentServer CidDirectoryServer

# Video Server - Both Content and Directory in one program
#VideoServer: VideoServer.o Utility.o
#	g++ -Wall -Wextra $(CFLAGS) -o $@ VideoServer.o Utility.o $(LDFLAGS)

#VideoServer.o: VideoServer.cpp Utility.h
#	g++ -Wall -Wextra $(CFLAGS) -c VideoServer.cpp


# Just a host for CIDs
ContentServer: ContentServer.o Utility.o
	g++ -Wall -Wextra $(CFLAGS) -o $@ ContentServer.o Utility.o $(LDFLAGS)

ContentServer.o: ContentServer.cpp Utility.h
	g++ -Wall -Wextra $(CFLAGS) -c ContentServer.cpp


# Just the CID directory functionality
CidDirectoryServer: CidDirectoryServer.o Utility.o
	g++ -Wall -Wextra $(CFLAGS) -o $@ CidDirectoryServer.o Utility.o $(LDFLAGS)

CidDirectoryServer.o: CidDirectoryServer.cpp Utility.h
	g++ -Wall -Wextra $(CFLAGS) -c CidDirectoryServer.cpp

Utility.o: Utility.h Utility.cpp
	g++ -Wall -Wextra $(CFLAGS) -c Utility.cpp


clean:
	rm *.o CidDirectoryServer ContentServer

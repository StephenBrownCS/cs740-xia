UNAME=$(shell uname -s)
ifeq "$(UNAME)" "Linux"
INCLUDE=
LIBS=-lasound
endif

ifeq "$(UNAME)" "Darwin"
#INCLUDE=-I/opt/local/include
LIBS=-lSDLmain -framework Carbon -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Cocoa
endif

all: player_example

player_example.o: player_example.c 
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) -o player_example.o player_example.c

player_example: player_example.o 
	g++ -g -I/opt/local/include -L/opt/local/lib -o player_example player_example.o -lvorbis -ltheoradec -logg -lSDL $(LIBS)

clean: 
	rm *.o player_example

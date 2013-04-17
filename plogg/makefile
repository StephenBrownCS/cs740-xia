UNAME=$(shell uname -s)
ifeq "$(UNAME)" "Linux"
INCLUDE=
LIBS=-lasound
endif

ifeq "$(UNAME)" "Darwin"
#INCLUDE=-I/opt/local/include
LIBS=-lSDLmain -framework Carbon -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Cocoa
endif

all: plogg

plogg.o: plogg.cpp 
	g++ -g -I/opt/local/include -L/opt/local/lib -c $(INCLUDE) -o plogg.o plogg.cpp

plogg: plogg.o 
	g++ -g -I/opt/local/include -L/opt/local/lib -o plogg plogg.o -lsydneyaudio -lvorbis -ltheoradec -logg -lSDL $(LIBS)

clean: 
	rm *.o plogg

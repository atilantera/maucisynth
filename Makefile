CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f

all: main

main: EventBuffer.o main.o SynthGui.o
	$(LD) $(LIBS) EventBuffer.o main.o SynthGui.o -o maucisynth

EventBuffer.o: EventBuffer.cpp EventBuffer.h
	$(CC) $(CFLAGS) -o EventBuffer.o -c EventBuffer.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -o main.o -c main.cpp

SynthGui.o: SynthGui.cpp SynthGui.h
	$(CC) $(CFLAGS) -o SynthGui.o -c SynthGui.cpp

clean:
	$(RM) *.o
	$(RM) ./main

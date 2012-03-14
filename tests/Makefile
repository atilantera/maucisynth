CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f

all: LfoTest

LfoTest: LfoTest.o ../LowFrequencyOscillator.o ../Oscillator.o
	$(LD) $(LIBS) LfoTest.o ../LowFrequencyOscillator.o ../Oscillator.o \
	-o lfotest
	
LfoTest.o: LfoTest.cpp
	$(CC) $(CFLAGS) -o LfoTest.o -c LfoTest.cpp
	
	

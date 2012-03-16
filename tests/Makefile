CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f

all: LfoTest MainOscillatorTest

LfoTest: LfoTest.o ../LowFrequencyOscillator.o ../Oscillator.o
	$(LD) $(LIBS) LfoTest.o ../LowFrequencyOscillator.o ../Oscillator.o \
	-o lfotest

LfoTest.o: LfoTest.cpp
	$(CC) $(CFLAGS) -o LfoTest.o -c LfoTest.cpp
	
MainOscillatorTest: MainOscillatorTest.o ../MainOscillator.o \
    ../Oscillator.o
	$(LD) $(LIBS) MainOscillatorTest.o ../MainOscillator.o \
	../Oscillator.o -o mainoscillatortest

MainOscillatorTest.o: MainOscillatorTest.cpp
	$(CC) $(CFLAGS) -o MainOscillatorTest.o -c MainOscillatorTest.cpp
	

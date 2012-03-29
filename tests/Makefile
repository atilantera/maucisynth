CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f

all: testall

testall: LfoTest.o MainOscillatorTest.o TestAll.o
	$(LD) $(LIBS) LfoTest.o MainOscillatorTest.o TestAll.o \
	../LowFrequencyOscillator.o ../MainOscillator.o ../Oscillator.o \
	-o testall

LfoTest.o: LfoTest.h LfoTest.cpp
	$(CC) $(CFLAGS) -o LfoTest.o -c LfoTest.cpp
	
MainOscillatorTest.o: MainOscillatorTest.h MainOscillatorTest.cpp
	$(CC) $(CFLAGS) -o MainOscillatorTest.o -c MainOscillatorTest.cpp

TestAll.o: TestAll.cpp
	$(CC) $(CFLAGS) -o TestAll.o -c TestAll.cpp
	
clean:
	$(RM) *.o
	$(RM) ./testall

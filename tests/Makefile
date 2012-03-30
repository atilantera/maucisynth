CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f
MAINOBJECTS=../LowFrequencyOscillator.o ../MainOscillator.o ../Oscillator.o

all: testall

testall: LfoTest.o MainOscillatorTest.o TestAll.o $(MAINOBJECTS)
	$(LD) $(LIBS) LfoTest.o MainOscillatorTest.o TestAll.o $(MAINOBJECTS) \
	-o testall

LfoTest.o: LfoTest.h LfoTest.cpp
	$(CC) $(CFLAGS) -c LfoTest.cpp
	
MainOscillatorTest.o: MainOscillatorTest.h MainOscillatorTest.cpp
	$(CC) $(CFLAGS) -c MainOscillatorTest.cpp

TestAll.o: TestAll.cpp
	$(CC) $(CFLAGS) -c TestAll.cpp
	
clean:
	$(RM) *.o
	$(RM) ./testall

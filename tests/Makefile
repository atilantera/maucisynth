CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g -pg
LIBS=-ljack -lm -pg
RM=rm -f
MAINOBJECTS=../LowFrequencyOscillator.o ../MainOscillator.o ../Oscillator.o \
../SynthParameters.o

all: performanceTests unitTests

performanceTests: PerformanceTests.o $(MAINOBJECTS)
	$(LD) $(LIBS) PerformanceTests.o $(MAINOBJECTS) \
	-o performancetests

unitTests: LfoTest.o MainOscillatorTest.o UnitTests.o $(MAINOBJECTS)
	$(LD) $(LIBS) LfoTest.o MainOscillatorTest.o UnitTests.o $(MAINOBJECTS) \
	-o unittests

LfoTest.o: LfoTest.h LfoTest.cpp
	$(CC) $(CFLAGS) -c LfoTest.cpp
	
MainOscillatorTest.o: MainOscillatorTest.cpp MainOscillatorTest.h
	$(CC) $(CFLAGS) -c MainOscillatorTest.cpp

PerformanceTests.o: PerformanceTests.cpp PerformanceTests.h
	$(CC) $(CFLAGS) -c PerformanceTests.cpp

UnitTests.o: UnitTests.cpp
	$(CC) $(CFLAGS) -c UnitTests.cpp
	
clean:
	$(RM) *.o
	$(RM) ./unittests ./performancetests

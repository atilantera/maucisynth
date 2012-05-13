AS=as
CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g -pg
LIBS=-pg -ljack -lm -lrt
RM=rm -f
MAINOBJECTS=../LowFrequencyOscillator.o ../MainOscillator.o ../Oscillator.o \
../SynthParameters.o

all: performanceTests unitTests

performanceTests: PerformanceTests.o AsmFunctions.o $(MAINOBJECTS)
	$(LD) -o performancetests PerformanceTests.o AsmFunctions.o $(MAINOBJECTS) $(LIBS)
	

unitTests: LfoTest.o MainOscillatorTest.o UnitTests.o $(MAINOBJECTS)
	$(LD) $(LIBS) LfoTest.o MainOscillatorTest.o UnitTests.o $(MAINOBJECTS) \
	-o unittests
	
AsmFunctions.o: AsmFunctions.s
	$(AS) -o AsmFunctions.o AsmFunctions.s

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

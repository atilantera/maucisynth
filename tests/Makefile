AS=as
CC=g++
LD=g++
CFLAGS=-std=c++0x -g -pg
LIBS=-pg -ljack -lm -lrt
RM=rm -f

all: chorustest envelopetest 

chorustest: ChorusTest.o ../ChorusEffect.o
	$(LD) $(LIBS) ChorusTest.o ../ChorusEffect.o \
	-o chorustest

envelopetest: EnvelopeTest.o ../EnvelopeGenerator.o
	$(LD) $(LIBS) EnvelopeTest.o ../EnvelopeGenerator.o \
	-o envelopetest
	
clean:
	$(RM) *.o
	$(RM) ./unittests ./performancetests

ChorusTest.o: ChorusTest.cpp
	$(CC) $(CFLAGS) -c ChorusTest.cpp
	
EnvelopeTest.o: EnvelopeTest.cpp
	$(CC) $(CFLAGS) -c EnvelopeTest.cpp
	

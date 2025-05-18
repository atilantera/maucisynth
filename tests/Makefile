AS=as
CC=g++
LD=g++
CFLAGS=-std=c++0x -g -pg
LIBS=-pg -ljack -lm -lrt
RM=rm -f

all: envelopetest

envelopetest: EnvelopeTest.o ../EnvelopeGenerator.o
	$(LD) $(LIBS) EnvelopeTest.o ../EnvelopeGenerator.o \
	-o envelopetest
	
clean:
	$(RM) *.o
	$(RM) ./unittests ./performancetests

EnvelopeTest.o: EnvelopeTest.cpp
	$(CC) $(CFLAGS) -c EnvelopeTest.cpp
	

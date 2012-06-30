CC=gcc
LD=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
LIBS=-lm

all: freqanalysis

freqanalysis: freqanalysis.o wav.o
	$(LD) -o freqanalysis freqanalysis.o wav.o $(LIBS)

freqanalysis.o: freqanalysis.c
	$(CC) $(CFLAGS) -c freqanalysis.c

wav.o: wav.c
	$(CC) $(CFLAGS) -c wav.c


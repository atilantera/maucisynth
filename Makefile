CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g -pg \
        `pkg-config --cflags gtk+-2.0` 
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0` -pg
RM=rm -f



all: main
	cd tests && make && cd ..
	
main: EventBuffer.o main.o LowFrequencyOscillator.o LowpassFilter.o \
	MainOscillator.o Oscillator.o Synthesizer.o SynthGui.o SynthParameters.o
	$(LD) $(LIBS) EventBuffer.o main.o LowFrequencyOscillator.o \
	LowpassFilter.o MainOscillator.o Oscillator.o Synthesizer.o SynthGui.o \
	SynthParameters.o -o maucisynth

EventBuffer.o: EventBuffer.cpp EventBuffer.h tests/testing.h
	$(CC) $(CFLAGS) -c EventBuffer.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

LowFrequencyOscillator.o: LowFrequencyOscillator.cpp LowFrequencyOscillator.h
	$(CC) $(CFLAGS) -c LowFrequencyOscillator.cpp

LowpassFilter.o: LowpassFilter.cpp LowpassFilter.h
	$(CC) $(CFLAGS) -c LowpassFilter.cpp

MainOscillator.o: MainOscillator.cpp MainOscillator.h
	$(CC) $(CFLAGS) -c MainOscillator.cpp

Oscillator.o: Oscillator.cpp Oscillator.h
	$(CC) $(CFLAGS) -c Oscillator.cpp

Synthesizer.o: Synthesizer.cpp Synthesizer.h SynthParameters.h
	$(CC) $(CFLAGS) -c Synthesizer.cpp

SynthGui.o: SynthGui.cpp SynthGui.h
	$(CC) $(CFLAGS) -c SynthGui.cpp

SynthParameters.o: SynthParameters.cpp SynthParameters.h
	$(CC) $(CFLAGS) -c SynthParameters.cpp

clean:
	$(RM) *.o
	$(RM) ./main

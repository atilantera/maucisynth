CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm `pkg-config --libs gtk+-2.0`
RM=rm -f

all: main
	cd tests && make && cd ..
	
main: EventBuffer.o main.o LowFrequencyOscillator.o \
	MainOscillator.o Oscillator.o Synthesizer.o SynthGui.o 
	$(LD) $(LIBS) EventBuffer.o main.o LowFrequencyOscillator.o \
	MainOscillator.o Oscillator.o Synthesizer.o SynthGui.o \
	-o maucisynth

EventBuffer.o: EventBuffer.cpp EventBuffer.h tests/testing.h
	$(CC) $(CFLAGS) -o EventBuffer.o -c EventBuffer.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -o main.o -c main.cpp

LowFrequencyOscillator.o: LowFrequencyOscillator.cpp LowFrequencyOscillator.h
	$(CC) $(CFLAGS) -o LowFrequencyOscillator.o -c LowFrequencyOscillator.cpp

MainOscillator.o: MainOscillator.cpp MainOscillator.h
	$(CC) $(CFLAGS) -o MainOscillator.o -c MainOscillator.cpp

Oscillator.o: Oscillator.cpp Oscillator.h
	$(CC) $(CFLAGS) -o Oscillator.o -c Oscillator.cpp

Synthesizer.o: Synthesizer.cpp Synthesizer.h SynthParameters.h
	$(CC) $(CFLAGS) -o Synthesizer.o -c Synthesizer.cpp

SynthGui.o: SynthGui.cpp SynthGui.h
	$(CC) $(CFLAGS) -o SynthGui.o -c SynthGui.cpp

clean:
	$(RM) *.o
	$(RM) ./main

CC=g++
LD=g++

# CFLAGS:
# -std=c++0x
#     version of C++ standard
# -Wall -Wextra -pedantic
#     produces lots of warnings for the developer
# -march=native
#     compiles for the processor architecture of the current system
# -O2
#     optimization level
# -g -pg
#     add debug and profiling data
# `pkg-config --cflags gtk+-2.0`
#     bindings for GTK+ 2.0 library

# Meticulous development options
#CFLAGS= -std=c++0x -Wall -Wextra -pedantic -march=native -O2 -g -pg \
#        `pkg-config --cflags gtk+-2.0`

# Quick development
CFLAGS= -std=c++0x -march=native -g -pg `pkg-config --cflags gtk+-2.0`
LIBS=-ljack -lm -pg `pkg-config --libs gtk+-2.0`
OBJFILES=ChorusEffect.o EnvelopeGenerator.o EventBuffer.o main.o \
        LowFrequencyOscillator.o LowpassFilter.o MainOscillator.o \
        Oscillator.o OscillatorGroup.o \
		Synthesizer.o SynthGui.o SynthParameters.o
EXECUTABLE=maucisynth
RM=rm -f



all: main
	
main: $(OBJFILES)
	$(LD) -o $(EXECUTABLE) $(OBJFILES) $(LIBS)

#EnvelopeGenerator.o: EnvelopeGenerator.cpp EnvelopeGenerator.h
#	$(CC) $(CFLAGS) -c EnvelopeGenerator.cpp

ChorusEffect.o: ChorusEffect.cpp ChorusEffect.h
	$(CC) $(CFLAGS) -c ChorusEffect.cpp

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

OscillatorGroup.o: OscillatorGroup.cpp OscillatorGroup.h
	$(CC) $(CFLAGS) -c OscillatorGroup.cpp

Synthesizer.o: Synthesizer.cpp Synthesizer.h SynthParameters.h
	$(CC) $(CFLAGS) -c Synthesizer.cpp

SynthGui.o: SynthGui.cpp SynthGui.h
	$(CC) $(CFLAGS) -c SynthGui.cpp

SynthParameters.o: SynthParameters.cpp SynthParameters.h
	$(CC) $(CFLAGS) -c SynthParameters.cpp

clean:
	$(RM) *.o
	$(RM) ./$(EXECUTABLE)

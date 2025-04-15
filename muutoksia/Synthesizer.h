/*
 * Synthesizer.h
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 *
 * The Synthesizer class receives messages (parameter changes, notes) from
 * GUI and JACK. It keeps track which oscillators are playing notes, and
 * delegates the actual sound synthesis to the MainOscillator and
 * LowFrequencyOscillator classes. Then it mixes the sound of the oscillators
 * and sends it to JACK.
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_

#define SYNTH_TESTING 1

#include <cstring>
#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/thread.h>
#include <pthread.h>

#include "EventBuffer.h"
#include "LowFrequencyOscillator.h"
#include "LowpassFilter.h"
#include "MainOscillator.h"
#include "Oscillator.h"
#include "SynthGui.h"
#include "SynthParameters.h"

const unsigned int POLYPHONY = 8;

class Synthesizer {
public:
	Synthesizer(EventBuffer & b, SynthParameters & p);
	~Synthesizer();

	bool isActive();

private:
	// JACK I/O
	jack_port_t * jackOutputPort;
	jack_port_t * jackMidiInPort;
	jack_client_t * jackClient;
	bool synthIsRunning;
	static Synthesizer * synthInstance;
	unsigned int samplerate;
	unsigned int bufferLength;
	unsigned char midiInChannel;
	bool samplerateChanged;
	bool bufferLengthChanged;
	pthread_mutex_t jackCallbackLock;

	// EventBuffer stores messages from SynthGui.
	EventBuffer & events;

	// SynthParameters stores all oscillator and filter parameters.
	// It is shared with SynthGui.
	SynthParameters & parameters;

	// Oscillators generate their sound to these buffers
	float * oscillatorBuffer;
	float * lfoBuffer;

	// Oscillator and note data
	MainOscillator * oscillator1[POLYPHONY];
	LowFrequencyOscillator * lfo1[POLYPHONY];
	NoteSource noteSource[POLYPHONY];
	unsigned char noteKey[POLYPHONY];

	// Filters
	LowpassFilter filter;

	bool initJack();
	static int jackCallback(jack_nframes_t nframes, void * arg);
	static int updateSamplerate(jack_nframes_t nframes, void *arg);
	static int updateBufferLength(jack_nframes_t nframes, void *arg);

	void checkJackExceptions();
	void generateSound(jack_nframes_t nframes);

    void processEvents(jack_nframes_t nframes);
	void processGuiEvents();
	void processMidiEvents(jack_nframes_t nframes);
	void processMidiControlChange(unsigned char type, unsigned char value);
	void processNoteOn(unsigned char key, unsigned char velocity,
        unsigned short time, NoteSource source);
	void processNoteOff(unsigned char key, unsigned short time,
        NoteSource source);
	void processFastMute(NoteSource source);

#ifdef SYNTH_TESTING
	// for testing purposes
	EnvelopePhase osc1curPhase[POLYPHONY];
	EnvelopePhase osc1prevPhase[POLYPHONY];
	void printOscillatorPhases();
#endif

};

#endif /* SYNTHESIZER_H_ */

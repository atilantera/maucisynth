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
#include "OscillatorGroup.h"
#include "SynthGui.h"
#include "SynthParameters.h"

#define SYNTH_TESTING 1

// How many notes can be played simultaneously
const unsigned int POLYPHONY = 8;

// How many notes from the same key can be played during the processing
// of one JACK buffer.
// (Usually buffer size is 16..4096 samples, which is 0.4 .. 9.3 msec
// @ 44100 Hz samplerate.)
const unsigned int MAX_TREMOLO_NOTES = 4;

// Number of possible MIDI note pitches.
// 0   = double contra C (C-1 or C,,,)
// 60  = middle / 1 line C (C4 or c')
// 69  = 440 Hz A (A4 or a')
// 127 = 6 line G (G9 or g'''''')
const unsigned int MIDI_PITCHES = 128;

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

	// MIDI note buffer for method processMidiEvents().
	// Value 128 in sortedNoteVelocity means the event is "note off".
	// Otherwise the event is "note on".
	unsigned short * sortedNoteTime;
	unsigned char * sortedNoteVelocity;
	unsigned int * sortedNoteCount;

	// Oscillators generate their sound to these buffers
	float * oscillatorBuffer;
	float * lfoBuffer;

	// Oscillator and note data
	OscillatorGroup * oscillatorGroup[POLYPHONY];

	// Filters
	LowpassFilter filter;

	bool initJack();
	static int jackCallback(jack_nframes_t nframes, void * arg);
	static int updateSamplerate(jack_nframes_t nframes, void *arg);
	static int updateBufferLength(jack_nframes_t nframes, void *arg);

	void checkJackExceptions();

	void generateSound(jack_nframes_t nframes);
	void playMidiNotes(OscillatorGroup * group, bool useDedicatedLfo,
			float * outputBuffer, jack_nframes_t nframes);

	bool renderNote(OscillatorGroup * group, bool useDedicatedLfo,
			float * outputBuffer, unsigned int rangeStart,
			unsigned int rangeEnd);

	void processGuiEvents();
	void processMidiEvents(jack_nframes_t nframes);
	void processMidiControlChange(unsigned char type, unsigned char value);

	void processNoteOn(unsigned char key, unsigned char velocity,
		unsigned short time, NoteSource source);

	void processNoteOff(unsigned char key, unsigned short time,
		NoteSource source);

	void processFastMute(NoteSource source);
	void processParameterChange(unsigned int parameter,
		unsigned int parameterValue);

#ifdef SYNTH_TESTING
	// for testing purposes
	EnvelopePhase osc1curPhase[POLYPHONY];
	EnvelopePhase osc1prevPhase[POLYPHONY];
	void printOscillatorPhases();
	void printSortedNotes();
#endif

};

#endif /* SYNTHESIZER_H_ */

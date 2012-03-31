/*
 * Synthesizer.h
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_

#include <cstring>
#include <iostream>
#include <jack/jack.h>
#include <jack/thread.h>
#include <pthread.h>

#include "EventBuffer.h"
#include "LowFrequencyOscillator.h"
#include "LowpassFilter.h"
#include "MainOscillator.h"
#include "Oscillator.h"
#include "SynthGui.h"
#include "SynthParameters.h"

const unsigned int POLYPHONY = 2;

class Synthesizer {
public:
	Synthesizer(EventBuffer & b, SynthGui & g);
	~Synthesizer();

	bool isActive();

private:
	// JACK I/O
	jack_port_t * jackOutputPort;
	jack_client_t * jackClient;
	bool synthIsRunning;
	static Synthesizer * synthInstance;
	unsigned int samplerate;
	unsigned int bufferLength;
	bool samplerateChanged;
	bool bufferLengthChanged;
	pthread_mutex_t jackCallbackLock;

	// EventBuffer stores messages from GUI
	EventBuffer & events;

	// Reference to GUI is needes for SynthGui.endExecution().
	SynthGui & gui;

	float * oscillatorBuffer;
	float * lfoBuffer;

	MainOscillator * oscillator1[POLYPHONY];
	LowFrequencyOscillator * lfo1[POLYPHONY];
	NoteSource noteSource[POLYPHONY];
	unsigned char noteKey[POLYPHONY];

	OscillatorParameters osc1parameters;

	LfoFrequencyType lfo1frequencyType;
	float lfo1fixedFrequency;
	float lfo1relativeFrequency;

	LowpassFilter filter;

	// Master volume of the synthesizer output
	float mainVolume;

	void initJack();
	static int jackCallback(jack_nframes_t nframes, void * arg);
	static int updateSamplerate(jack_nframes_t nframes, void *arg);
	static int updateBufferLength(jack_nframes_t nframes, void *arg);

	void checkJackExceptions();
	void generateSound(jack_nframes_t nframes);

	void processEvents();
	void processNoteOn(unsigned char key, unsigned char velocity,
			NoteSource source);
	void processNoteOff(unsigned char key, NoteSource source);
	void processFastMute(NoteSource source);
	void processParameterChange(unsigned int parameter,
		unsigned int parameterValue);
};

#endif /* SYNTHESIZER_H_ */

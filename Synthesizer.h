/*
 * Synthesizer.h
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_

#include <jack/jack.h>
#include <jack/thread.h>

#include "EventBuffer.h"
#include "LowFrequencyOscillator.h"
#include "MainOscillator.h"
#include "SynthParameters.h"

const unsigned int POLYPHONY = 32;

class Synthesizer {
public:
	Synthesizer(EventBuffer & b);
	~Synthesizer();

	bool isActive();

private:
	// JACK I/O
	jack_port_t * jackOutputPort;
	jack_client_t * jackClient;
	bool jackIsRunning;
	static Synthesizer * synthInstance;
	unsigned int samplerate;
	unsigned int bufferLength;

	// EventBuffer stores messages from GUI
	EventBuffer & events;

	float * oscillatorBuffer;
	float * lfoBuffer;

	MainOscillator * oscillatorTable[POLYPHONY];
	LowFrequencyOscillator * lfoTable[POLYPHONY];

	void initJack();
	static int jackCallback(jack_nframes_t nframes, void * arg);
	static int updateSamplerate(jack_nframes_t nframes, void *arg);
	static int updateBufferLength(jack_nframes_t nframes, void *arg);

	void generateSound(jack_nframes_t nframes);

	void processEvents();
	void processNoteOn(unsigned char key, unsigned char velocity,
		unsigned char noteSource);
	void processNoteOff(unsigned char key, unsigned char noteSource);
	void processFastMute(unsigned char noteSource);
	void processParameterChange(unsigned int parameter,
		unsigned int parameterValue);

};

#endif /* SYNTHESIZER_H_ */

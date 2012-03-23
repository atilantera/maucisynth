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

class Synthesizer {
public:
	Synthesizer(EventBuffer & b);
	~Synthesizer();

	bool isActive();

private:
	// Since the synthesizer is monotimbral, all the main oscillators have
	// the same waveform and LFO modulation type
	WaveformType waveform;
	LfoModulationTarget modulation;

	jack_port_t * jackOutputPort;
	jack_client_t * jackClient;
	bool jackIsRunning;
	static Synthesizer * synthInstance;
	unsigned int samplerate;
	unsigned int bufferLength;

	static int jackCallback(jack_nframes_t nframes, void * arg);



	EventBuffer & events;

	void generateSound(jack_nframes_t nframes);

};

#endif /* SYNTHESIZER_H_ */

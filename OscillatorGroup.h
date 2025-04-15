/*
 * OscillatorGroup.h
 *
 * Oscillator group is group of oscillators that is playing one note together.
 * It is a data structure needed by class Synthesizer.
 *
 *  Created on: 9.7.2012
 *      Author: alkim
 */

#ifndef OSCILLATORGROUP_H_
#define OSCILLATORGROUP_H_

#include <cstdlib>
#include "LowFrequencyOscillator.h"
#include "MainOscillator.h"
#include "SynthParameters.h"

class OscillatorGroup {
public:
	OscillatorGroup(SynthParameters & parameters);
	~OscillatorGroup();

	MainOscillator * osc1;
	LowFrequencyOscillator * lfo1;
	NoteSource noteSource;
	unsigned char notePitch; // MIDI note pitch (0..127)
	bool isPlaying;          // isPlaying == false means the oscillator is free
	                         // for new notes
};

#endif /* OSCILLATORGROUP_H_ */

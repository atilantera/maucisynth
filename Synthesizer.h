/*
 * Synthesizer.h
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_

enum controllerType {
	OSC1_WAVEFORM           =  0,
	OSC1_ATTACK             =  1,
	OSC1_DECAY              =  2,
	OSC1_SUSTAIN            =  3,
	OSC1_RELEASE            =  4,
	LFO1_FREQUENCY_TYPE     =  5,
	LFO1_FIXED_FREQUENCY    =  6,
	LFO1_RELATIVE_FREQUENCY =  7,
	LFO1_TARGET_TYPE        =  8,
	LFO1_MODULATION_AMOUNT  =  9,
	FILTER_LOWPASS          = 20
};

enum waveformType {
	SINE      = 0,
	TRIANGLE  = 1,
	SAWTOOTH  = 2,
	PULSE     = 3,
	ABS_SINE  = 4
};

enum lfoFrequencyType {
	FIXED    = 0,
	RELATIVE = 1
};

enum lfoTargetType {
	NONE        = 0,
	FREQUENCY   = 1,
	AMPLITUDE   = 2,
	PULSE_WIDTH = 3
};

enum NoteSource { computerKeyboard = 1, JACK = 2};

const int maxVelocity = 127;

class Synthesizer {
public:
	Synthesizer();
	virtual ~Synthesizer();
};

#endif /* SYNTHESIZER_H_ */

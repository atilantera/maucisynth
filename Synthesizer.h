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
	LFO_FIXED    = 0,
	LFO_RELATIVE = 1
};

enum lfoTargetType {
	LFO_TO_NONE        = 0,
	LTO_TO_FREQUENCY   = 1,
	LFO_TO_AMPLITUDE   = 2,
	LFO_TO_PULSE_WIDTH = 3
};

class Synthesizer {
public:
	Synthesizer();
	virtual ~Synthesizer();
};

#endif /* SYNTHESIZER_H_ */

/*
 * SynthParameters.h
 *
 * This file contains constants for synthesizer parameters.
 * It is used by SynthGui to send messages to Synthesizer via
 * EventBuffer.
 *
 * Also LowFrequencyOscillator and MainOscillator need some
 * definitions here.
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef SYNTHPARAMETERS_H_
#define SYNTHPARAMETERS_H_

// NOTE: If you modify the enum ControllerType, you must also
// change the method Synthesizer.processParameterChange().

enum ControllerType {
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

enum WaveformType {
	SINE      = 0,
	TRIANGLE  = 1,
	SAWTOOTH  = 2,
	PULSE     = 3,
	ABS_SINE  = 4
};

const unsigned int maxWaveformType = 4;

enum LfoFrequencyType {
	FIXED    = 0,
	RELATIVE = 1
};

enum LfoModulationTarget {
	NONE        = 0,
	FREQUENCY   = 1,
	AMPLITUDE   = 2,
	PULSE_WIDTH = 3
};

enum NoteSource { computerKeyboard = 1, JACK = 2};

const unsigned int maxVelocity = 127;

const float MinFixedLfoFrequency = 0.1;
const float MaxFixedLfoFrequency = 50;

const float MinRelativeLfoFrequency = 0.05;
const float MaxRelativeLfoFrequency = 0.5;

const float MinLfoModulation = 0.01;
const float MaxLfoModulation = 1;

// LowPassFrequency is in Hz.
const unsigned int MinLowpassFrequency = 100;
const unsigned int MaxLowpassFrequency = 10000;

// ADSR times are in milliseconds.
const unsigned int MinAttackTime = 5;
const unsigned int MaxAttackTime = 1000;

const unsigned int MinDecayTime  = 5;
const unsigned int MaxDecayTime  = 1000;

const unsigned int MinSustainVolume = 1;
const unsigned int MaxSustainVolume = 100;

const unsigned int MinReleaseTime   = 5;
const unsigned int MaxReleaseTime   = 3000;

// Class OscillatorParameters has parameters of one MainOscillator-
// LowFrequencyOscillator combination.
class OscillatorParameters {
public:
	WaveformType waveform;

	unsigned int attackTime;
	unsigned int decayTime;
	float sustainVolume;
	unsigned int releaseTime;

	LfoModulationTarget modulation;
	LfoFrequencyType lfoFrequencyType;
	float modulationAmount;
	float fixedLfoFrequency;
	float relativeLfoFrequency;
};


#endif /* SYNTHPARAMETERS_H_ */

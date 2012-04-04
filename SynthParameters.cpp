/*
 * SynthParameters.cpp
 *
 *  Created on: 31.3.2012
 *      Author: alkim
 */

#include "SynthParameters.h"

OscillatorParameters::OscillatorParameters()
{
	waveform = SINE;
	attackTime = MinAttackTime;
	decayTime = MinDecayTime;
	sustainVolume = MinSustainVolume;
	releaseTime = MinReleaseTime;

	lfoModulationTarget = NONE;
	lfoFrequencyType = FIXED;
	lfoModulationAmount = 0;
	lfoFixedFrequency = MinFixedLfoFrequency;
	lfoRelativeFrequency = MinRelativeLfoFrequency;
}

SynthParameters::SynthParameters()
{
	lowpassFrequency = MinLowpassFrequency;
	mainVolume = 1;
}


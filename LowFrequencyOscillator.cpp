/*
 * LowFrequencyOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "LowFrequencyOscillator.h"

LowFrequencyOscillator::LowFrequencyOscillator(OscillatorParameters & p,
	MainOscillator & m):
globalParameters(p), mainOscillator(m)
{
	fixedFrequency = globalParameters.lfoFixedFrequency;
	setFrequency(fixedFrequency);
}


void LowFrequencyOscillator::updateRelativeFrequency(float mainOscFrequency)
{
	setFrequency(globalParameters.lfoRelativeFrequency * mainOscFrequency);
}

void LowFrequencyOscillator::generateSound(float buffer[])
{
	refreshParameters();

	// With pulse width modulation, 70% on-pulse / 30% off-pulse sounds
    // the same to the ear as 30% on-pulse / 70% off-pulse.
    // Therefore in a single sine wave lfo cycle there is _two_ points
    // where the wave sounds the same, and therefore we need to split
    // the frequency.

	float increase = anglePerSample;
    if (globalParameters.lfoModulationTarget == PULSE_WIDTH) {
    	increase *= 0.5;
    }

    for (unsigned int i = 0; i < bufferLength; i++) {
		// (int)(x + 0.5) is faster than calling lroundf()
		buffer[i] = sineTable[(int)(angle * WAVE_TABLE_LENGTH + 0.5)];
		angle += increase;
		if (angle >= 1) {
			angle -= 1;
		}
	}
}

void LowFrequencyOscillator::refreshParameters()
{
	if (frequencyType != globalParameters.lfoFrequencyType) {
		frequencyType = globalParameters.lfoFrequencyType;
		if (frequencyType == FIXED) {
			fixedFrequency = globalParameters.lfoFixedFrequency;
			setFrequency(fixedFrequency);
		}
		else {
			relativeFrequency = globalParameters.lfoRelativeFrequency;
			mainOscFrequency = mainOscillator.getFrequency();
			setFrequency(relativeFrequency * mainOscFrequency);
		}
		return;
	}

	if (frequencyType == FIXED &&
		fixedFrequency != globalParameters.lfoFixedFrequency)
	{
		fixedFrequency = globalParameters.lfoFixedFrequency;
		setFrequency(fixedFrequency);
	}

	if (frequencyType == RELATIVE &&
		(relativeFrequency != globalParameters.lfoRelativeFrequency ||
		 mainOscFrequency != mainOscillator.getFrequency()))
	{
		relativeFrequency = globalParameters.lfoRelativeFrequency;
		mainOscFrequency = mainOscillator.getFrequency();
		setFrequency(relativeFrequency * mainOscFrequency);
	}
}

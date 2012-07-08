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

// Generates oscillation.
// buffer - output buffer for sound / oscillation
// rangeStart - index of first sample in the buffer to generate
// rangeEnd - index of last sample + 1
void LowFrequencyOscillator::generateSound(float buffer[],
unsigned int rangeStart, unsigned int rangeEnd)
{
	refreshParameters();

	// With pulse width modulation, 70% on-pulse / 30% off-pulse sounds
    // the same to the ear as 30% on-pulse / 70% off-pulse.
    // Therefore in a single sine wave lfo cycle there is _two_ points
    // where the wave sounds the same, and therefore we need to split
    // the frequency.

	float tableLength = WAVE_TABLE_LENGTH;
	float index = angle * tableLength;
	float increase = anglePerSample * tableLength;

    if (globalParameters.lfoModulationTarget == PULSE_WIDTH) {
    	increase *= 0.5;
    }

    float * endPtr = buffer + rangeEnd;
    buffer += rangeStart;
    while (buffer < endPtr) {
		// (int)(x + 0.5) may be even three times faster than calling lroundf()
		*buffer++ = sineTable[(int)index];
		index += increase;
		if (index >= tableLength) {
			index -= tableLength;
		}
	}
    angle = index / tableLength;
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

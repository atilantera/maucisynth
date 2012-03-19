/*
 * LowFrequencyOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "LowFrequencyOscillator.h"

LowFrequencyOscillator::LowFrequencyOscillator() {
	modulationTarget = NONE;
}

LowFrequencyOscillator::~LowFrequencyOscillator() {
}

void LowFrequencyOscillator::setModulationTarget(LfoModulationTarget target)
{
	modulationTarget = target;
}

void LowFrequencyOscillator::generateSound(float buffer[]) {
	// With pulse width modulation, 70% on-pulse / 30% off-pulse sounds
    // the same to the ear as 30% on-pulse / 70% off-pulse.
    // Therefore in a single sine wave lfo cycle there is _two_ points
    // where the wave sounds the same, and therefore we need to split
    // the frequency.
	float increase = anglePerSample;
    if (modulationTarget == PULSE_WIDTH) {
    	increase *= 0.5;
    }

    for (unsigned int i = 0; i < bufferLength; i++) {
		// (int)(x + 0.5) is faster than calling lroundf()
		buffer[i] = sineTable[(int)(angle * WAVE_TABLE_LENGTH + 0.5)];
		angle += increase;
		if (angle > 1) {
			angle -= 1;
		}
	}
}

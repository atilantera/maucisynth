/*
 * LowFrequencyOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "LowFrequencyOscillator.h"

LowFrequencyOscillator::LowFrequencyOscillator() {
	// TODO Auto-generated constructor stub

}

LowFrequencyOscillator::~LowFrequencyOscillator() {
	// TODO Auto-generated destructor stub
}

void LowFrequencyOscillator::setModulationTarget(lfoModulationTarget target)
{
	modulationTarget = target;
}

void LowFrequencyOscillator::generateSound(float * buffer, int bufferLength) {
	float * ptr;
	float * endPtr;
	int index;

	// With pulse width modulation, 70% on-pulse / 30% off-pulse sounds
    // the same to the ear as 30% on-pulse / 70% off-pulse.
    // Therefore in a single sine wave lfo cycle there is _two_ points
    // where the wave sounds the same, and therefore we need to split
    // the frequency.
	float increase = radiansPerSample;
    if (modulationTarget == PULSE_WIDTH) {
    	increase *= 0.5;
    }

    float waveTableConstant = samplerate / TWO_PI;
    endPtr = buffer + bufferLength;

    for (ptr = buffer; ptr < endPtr; ptr++) {
		// (int)(x + 0.5) is faster than calling lroundf()
		index = (int)(angle * waveTableConstant + 0.5);
		*ptr = sineTable[index];
		angle += increase;
		if (angle > TWO_PI) {
			angle -= TWO_PI;
		}
	}
}

/*
 * LowFrequencyOscillator.h
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef LOWFREQUENCYOSCILLATOR_H_
#define LOWFREQUENCYOSCILLATOR_H_

#include "Oscillator.h"
#include "SynthParameters.h"

class LowFrequencyOscillator : public Oscillator {
public:
	LowFrequencyOscillator(OscillatorParameters & p);
	virtual ~LowFrequencyOscillator() {}

	void updateRelativeFrequency(float mainOscFrequency);
	void generateSound(float buffer[]);

private:
	OscillatorParameters & globalParameters;

};

#endif /* LOWFREQUENCYOSCILLATOR_H_ */

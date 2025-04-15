/*
 * LowFrequencyOscillator.h
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef LOWFREQUENCYOSCILLATOR_H_
#define LOWFREQUENCYOSCILLATOR_H_

#include "Oscillator.h"
#include "MainOscillator.h"
#include "SynthParameters.h"

class LowFrequencyOscillator : public Oscillator {
public:
	LowFrequencyOscillator(OscillatorParameters & p, MainOscillator & m);
	virtual ~LowFrequencyOscillator() {}

	void setRelativeOscillator(MainOscillator * osc);
	void updateRelativeFrequency(float mainOscFrequency);
	void generateSound(float buffer[], unsigned int firstSample,
			unsigned int lastSample);

private:
	OscillatorParameters & globalParameters;
	MainOscillator & mainOscillator;

	// These are copies of fields in globalParameters
	LfoFrequencyType frequencyType;
	float fixedFrequency;
	float relativeFrequency;

	float mainOscFrequency;

	inline void refreshParameters();

};

#endif /* LOWFREQUENCYOSCILLATOR_H_ */

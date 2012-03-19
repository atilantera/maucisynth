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
	LowFrequencyOscillator();
	virtual ~LowFrequencyOscillator();

	void setModulationTarget(LfoModulationTarget target);
	void generateSound(float * buffer);

private:
	LfoModulationTarget modulationTarget;

};

#endif /* LOWFREQUENCYOSCILLATOR_H_ */

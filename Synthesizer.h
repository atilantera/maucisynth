/*
 * Synthesizer.h
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_

#include "LowFrequencyOscillator.h"
#include "MainOscillator.h"
#include "SynthParameters.h"

class Synthesizer {
public:
	Synthesizer();
	virtual ~Synthesizer();
};

#endif /* SYNTHESIZER_H_ */

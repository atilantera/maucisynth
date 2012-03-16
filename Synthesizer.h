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

private:
	// Since the synthesizer is monotimbral, all the main oscillators have
	// the same waveform and LFO modulation type
	WaveformType waveform;
	LfoModulationTarget modulation;
};

#endif /* SYNTHESIZER_H_ */

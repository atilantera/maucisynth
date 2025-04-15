/*
 * OscillatorGroup.cpp
 *
 *  Created on: 9.7.2012
 *      Author: alkim
 */

#include "OscillatorGroup.h"

OscillatorGroup::OscillatorGroup(SynthParameters & parameters)
{
	osc1 = NULL;
	lfo1 = NULL;
	osc1 = new MainOscillator(parameters.osc1);
	lfo1 = new LowFrequencyOscillator(parameters.osc1, *osc1);
	isPlaying = false;

}

OscillatorGroup::~OscillatorGroup() {
	delete osc1;
	delete lfo1;
}


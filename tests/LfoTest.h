/*
 * LfoTest.h
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#ifndef LFOTEST_H_
#define LFOTEST_H_

#include <math.h>
#include <iostream>
#include "../LowFrequencyOscillator.h"

class LfoTest : public LowFrequencyOscillator {
public:
	LfoTest();
	void testAll();
	void testSineTable();
	void testFrequencyRandomness();
	void testLFO();

private:
	OscillatorParameters parameters;
	MainOscillator mainOscillator;
};

#endif /* LFOTEST_H_ */

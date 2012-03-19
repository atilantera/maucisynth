/*
 * TestAll.cpp
 * Contains all test suites for maucisynth.
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#include "LfoTest.h"
#include "MainOscillatorTest.h"

int main()
{
	std::cout << "Testing LowFrequencyOscillator" << std::endl;
	LfoTest lfo;
	lfo.testAll();

	std::cout << "Testing MainOscillator" << std::endl;
	MainOscillatorTest mainOsc;
	if (mainOsc.initTests() == false) {
		return 1;
	}
	mainOsc.testAll();
	mainOsc.finishTesting();

	std::cout << "Testing finished." << std::endl;
	return 0;
}

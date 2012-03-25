/*
 * TestAll.cpp
 * Contains all test suites for maucisynth.
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#include "LfoTest.h"
#include "MainOscillatorTest.h"
#include "ValueSetTest.h"

int main()
{
	// Classes LowFrequencyOscillator and Oscillator
	std::cout << "Testing LowFrequencyOscillator" << std::endl;
	LfoTest lfo;
	lfo.testAll();

	// Classes MainOscillator and Oscillator
	std::cout << "Testing MainOscillator" << std::endl;
	MainOscillatorTest mainOsc;
	if (mainOsc.initTests() == false) {
		return 1;
	}
	mainOsc.testAll();
	mainOsc.finishTesting();

	// Classes ValueSet and KeyValueSet
	std::cout << "Testing ValueSet and KeyValueSet" << std::endl;
	ValueSetTest vsTest;
	vsTest.testAll();

	std::cout << "Testing finished." << std::endl;
	return 0;
}

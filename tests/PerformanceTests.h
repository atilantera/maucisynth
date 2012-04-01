/*
 * PerformanceTests.h
 *
 * This class tests sound synthesis performance.
 * Usage:
 * ./performancetests && gprof -Q performancetests > performance-results.txt
 *
 *  Created on: 31.3.2012
 *      Author: alkim
 */

#ifndef PERFORMANCETESTS_H_
#define PERFORMANCETESTS_H_

#include <iostream>
#include "../MainOscillator.h"

const unsigned int POLYPHONY = 20;
const unsigned int BUFFER_LENGTH = 128;
const unsigned int SAMPLE_RATE = 44100;

class PerformanceTests {
public:
	PerformanceTests();
	virtual ~PerformanceTests();

	void renderingSpeedTest();

private:
	OscillatorParameters parameters;
	MainOscillator * oscillators[POLYPHONY];
	float * outputBuffer;
	float * modulatorBuffer;
};

#endif /* PERFORMANCETESTS_H_ */

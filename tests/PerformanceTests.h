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

#include <ctime>
#include <iostream>
#include <string>
#include "../MainOscillator.h"

const unsigned int POLYPHONY = 20;
const unsigned int BUFFER_LENGTH = 128;
const unsigned int SAMPLE_RATE = 44100;

class PerformanceTests {
public:
	PerformanceTests();
	virtual ~PerformanceTests();

    void runWithParameters(int argc, char ** argv);
    void printUsageText();
	void renderingSpeedTest();
    void tableVsSinTest();

private:
	OscillatorParameters parameters;
	MainOscillator * oscillators[POLYPHONY];
	float * outputBuffer;
	float * modulatorBuffer;

    double secondDifference(struct timespec t1, struct timespec t0);
};

#endif /* PERFORMANCETESTS_H_ */

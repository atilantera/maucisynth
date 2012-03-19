/*
 * MainOscillatorTest.h
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#ifndef MAINOSCILLATORTEST_H_
#define MAINOSCILLATORTEST_H_

#include <fstream>
#include <stdlib.h>
#include "../MainOscillator.h"

class MainOscillatorTest : public MainOscillator {
public:
	MainOscillatorTest();
	~MainOscillatorTest();

	bool initTests();
	void testAll();
	void finishTesting();

	void testBasicSynthesis();
	void testFrequencyModulation(bool generateOutput);
	void testAmplitudeModulation(bool generateOutput);
	void testPulseWidthModulation(bool generateOutput);

private:
	float * outputBuffer;
	float * modulatorBuffer;
	unsigned int testBufferLength;

	std::ofstream testFile;

	// Right values for testFrequencyModulation
	float * frequencyModulatedSine;
	float * frequencyModulatedTriangle;
	float * frequencyModulatedSawtooth;
	float * frequencyModulatedPulse;
	float * frequencyModulatedAbsSine;
	float * pulseModulatedPulse;
	float * amplitudeModulatedPulse;

	bool initTestData();
};


#endif /* MAINOSCILLATORTEST_H_ */

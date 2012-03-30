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

	void testBasicSynthesis(bool generateOutput);
	void testFrequencyModulation(bool generateOutput);
	void testAmplitudeModulation(bool generateOutput);
	void testPulseWidthModulation(bool generateOutput);
	void testAdsrCurve(bool generateOutput);
	void testFastMute(bool generateOutput);

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

	OscillatorParameters parameters;

	bool initTestData();
	void setWaveform(WaveformType w);
	void setADSR(unsigned int a, unsigned int d, float s, unsigned int r);
	void printBuffer(const char * message, unsigned int samples);
};


#endif /* MAINOSCILLATORTEST_H_ */

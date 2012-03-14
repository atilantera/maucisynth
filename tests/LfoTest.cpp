/*
 * LfoTest.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include "../LowFrequencyOscillator.h"

class OscillatorTester : public Oscillator {
public:
	OscillatorTester();
	void testSineTable();
	void testFrequencyRandomness();
	void testLFO();
};

OscillatorTester::OscillatorTester()
{
	// Random seed must be a constant so that test can be exactly repeatable
	srandom(0);
}

void OscillatorTester::testSineTable() {
	int step = WAVE_TABLE_LENGTH / 32;
	float angle;
	for (int i = 0; i < 32; i++) {
		angle = (float)i / 32 * 2 * M_PI;
		if (fabs(sineTable[i * step] - sinf(angle)) > 0.002) {
			std::cout << "testSineTable failed!" << std::endl;
			return;
		}
	}
	std::cout << "testSineTable passed" << std::endl;
}

void OscillatorTester::testFrequencyRandomness()
{
	float testFrequency = 100;
	float minValue = testFrequency * (1 - randomDetune);
	float maxValue = testFrequency * (1 + randomDetune);

	for (int i = 0; i < 100; i++) {
		setFrequency(testFrequency);
		if (!(minValue <= frequency && frequency <= maxValue)) {
			std::cout << "testFrequencyRandomness failed! " <<
				" randomDetune=" << randomDetune << " range=[" << minValue <<
				", " << maxValue << "]" << ", but got frequency: " <<
				frequency << std::endl;
			return;
		}
	}
	std::cout << "testFrequencyRandomness passed" << std::endl;
}

void OscillatorTester::testLFO() {
	int bufferLength = 40;
	float soundBuffer[40];
	LowFrequencyOscillator osc;

	int i;
	float angle;
	float frequency;

	Oscillator::setSamplerate(bufferLength);


	for (frequency = 1; frequency < 10; frequency *= 1.9) {
		osc.setFrequency(frequency);
		osc.generateSound(soundBuffer, bufferLength);
		for (i = 0; i < bufferLength; i++) {
			angle = (float)i / 40 * 2 * M_PI * frequency;
			if (fabs(soundBuffer[i] - sinf(angle)) > 0.01 * frequency) {
				std::cout << "testLFO failed! frequency=" << frequency <<
					" soundBuffer[" << i << "]=" << soundBuffer[i] <<
					" sinf=" << sinf(angle) << std::endl;
				return;
			}
		}
	}
	std::cout << "testLFO passed" << std::endl;
}

int main(int argc, char ** argv) {
	OscillatorTester tester;
	tester.testSineTable();
	tester.testFrequencyRandomness();
	tester.testLFO();
}

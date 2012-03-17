/*
 * LfoTest.cpp
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#include "LfoTest.h"

LfoTest::LfoTest()
{
	// Random seed must be a constant so that test can be exactly repeatable
	srandom(0);
}

void LfoTest::testAll()
{
	testSineTable();
	testFrequencyRandomness();
	testLFO();
}

void LfoTest::testSineTable() {
	int step = WAVE_TABLE_LENGTH / 32;
	float x;
	for (int i = 0; i < WAVE_TABLE_LENGTH; i += step) {
		x = i * 2 * M_PI / WAVE_TABLE_LENGTH;
		if (fabs(sineTable[i] - sinf(x)) > 0.002) {
			std::cout << "testSineTable failed!" << std::endl;
			return;
		}
	}
}

void LfoTest::testFrequencyRandomness()
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
}

void LfoTest::testLFO() {
	int bufferLength = 40;
	float soundBuffer[40];
	LowFrequencyOscillator osc;

	int i;
	float x;
	float frequency;

	Oscillator::setSamplerate(bufferLength);

	for (frequency = 1; frequency < 10; frequency *= 1.9) {
		setFrequency(frequency);
		generateSound(soundBuffer, bufferLength);
		for (i = 0; i < bufferLength; i++) {
			x = (float)i / 40 * 2 * M_PI * frequency;
			if (fabs(soundBuffer[i] - sinf(x)) > 0.01 * frequency) {
				std::cout << "testLFO failed! frequency=" << frequency <<
					" soundBuffer[" << i << "]=" << soundBuffer[i] <<
					" sinf=" << sinf(x) << std::endl;
				return;
			}
		}
	}
}

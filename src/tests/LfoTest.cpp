/*
 * LfoTest.cpp
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#include "LfoTest.h"

const unsigned int testBufferLength = 40;

LfoTest::LfoTest() : LowFrequencyOscillator(parameters, mainOscillator),
mainOscillator(parameters)
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
	for (unsigned int i = 0; i < WAVE_TABLE_LENGTH; i += step) {
		x = i * 2 * M_PI / WAVE_TABLE_LENGTH;
		if (fabs(sineTable[i] - sinf(x)) > 0.002) {
			std::cout << "LfoTest::testSineTable failed!" << std::endl;
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
			std::cout << "LfoTest::testFrequencyRandomness failed! " <<
				" randomDetune=" << randomDetune << " range=[" << minValue <<
				", " << maxValue << "]" << ", but got frequency: " <<
				frequency << std::endl;
			return;
		}
	}
}

void LfoTest::testLFO() {
	float soundBuffer[testBufferLength];

	Oscillator::setBufferLength(testBufferLength);
	Oscillator::setSamplerate(bufferLength);
	parameters.lfoModulationTarget = NONE;

	unsigned int i;
	float x, testFrequency;



	for (testFrequency = 1; testFrequency < 10; testFrequency *= 1.9) {
		this->setFrequency(testFrequency);
		generateSound(soundBuffer);

		for (i = 0; i < testBufferLength; i++) {

			x = (float)i / 40 * 2 * M_PI * testFrequency;

			if (fabs(soundBuffer[i] - sinf(x)) > 0.01 * testFrequency) {

				std::cout << "testLFO failed! testFrequency=" <<
					testFrequency << " soundBuffer[" << i << "]=" <<
					soundBuffer[i] << " sinf=" << sinf(x) << std::endl;
				return;
			}
		}
	}
}

/*
 * PerformanceTests.cpp
 *
 *  Created on: 31.3.2012
 *      Author: alkim
 */

#include "PerformanceTests.h"

int main()
{
	PerformanceTests ptests;
	ptests.renderingSpeedTest();
	return 0;
}

PerformanceTests::PerformanceTests()
{
	unsigned int i;
	for (i = 0; i < POLYPHONY; i++) {
		oscillators[i] = new MainOscillator(parameters);
	}

	outputBuffer = new float[BUFFER_LENGTH];
	modulatorBuffer = new float[BUFFER_LENGTH];
	Oscillator::setBufferLength(BUFFER_LENGTH);
	Oscillator::setSamplerate(SAMPLE_RATE);
}

PerformanceTests::~PerformanceTests() {
	unsigned int i;
	for (i = 0; i < POLYPHONY; i++) {
		delete oscillators[i];
	}
	delete [] outputBuffer;
	delete [] modulatorBuffer;
}

// Tests rendering speed of maximum number of simultaneous notes
// with simultaneous conditions:
// 1. envelope phase is SUSTAIN
// 2. no LFO modulation is used
void PerformanceTests::renderingSpeedTest()
{
	std::cout << "Rendering speed test begins!" << std::endl;

	parameters.attackTime = 0;
	parameters.decayTime = 0;
	parameters.sustainVolume = MaxSustainVolume;
	parameters.releaseTime = 0;


	unsigned int bufferCount = 50000;
	std::cout << "Running " << bufferCount << " buffers, each of " <<
		BUFFER_LENGTH << " samples." << std::endl;

	unsigned int osc;
	for (osc = 0; osc < POLYPHONY; osc++) {
		oscillators[osc]->noteOn((unsigned char)
			(maxVelocity * osc / POLYPHONY), 100, true);
	}

	unsigned int buffer;
	bool noteFinished;

	std::cout << "Testing synthesizeFromWavetable()" << std::endl;
	parameters.waveform = ABS_SINE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizeTriangleWave()" << std::endl;
	parameters.waveform = TRIANGLE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizeSawtoothWave()" << std::endl;
	parameters.waveform = SAWTOOTH;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizePulseWave()" << std::endl;
	parameters.waveform = PULSE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	for (osc = 0; osc < POLYPHONY; osc++) {
		oscillators[osc]->muteFast();
	}

	std::cout << "Rendering speed test ends." << std::endl;
}


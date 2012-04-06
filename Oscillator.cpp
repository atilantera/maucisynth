/*
 * Oscillator.cpp
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#include "Oscillator.h"

float Oscillator::sineTable[WAVE_TABLE_LENGTH];
float Oscillator::absSineTable[WAVE_TABLE_LENGTH];
float Oscillator::baseFrequency[128];
bool  Oscillator::staticDataInitialized = false;
float Oscillator::randomDetune = 0.001;
unsigned int Oscillator::samplerate = 44100;
unsigned int Oscillator::bufferLength = 128;

Oscillator::Oscillator() {
	if (!staticDataInitialized) {
		srandom(0);
		generateWaveTables();
		generateBaseFrequencies();
		staticDataInitialized = true;
	}
	frequency = 1;
	pulseWidth = 0.5;
	angle = 0;
	anglePerSample = 0.1;
}

Oscillator::~Oscillator() { }

// Sets global samplerate for all oscillators
// (LowFrequencyOscillator, MainOscillator).
void Oscillator::setSamplerate(unsigned int samplesPerSecond)
{
	if (samplesPerSecond > 1) {
		samplerate = samplesPerSecond;
	}
}

// Sets global buffer length for all oscillators
// (LowFrequencyOscillator, MainOscillator).
void Oscillator::setBufferLength(unsigned int lengthInSamples)
{
	if (lengthInSamples > 1) {
		bufferLength = lengthInSamples;
	}
}

void Oscillator::setFrequency(float f)
{
	if (f <= 0) {
		return;
	}

	float randomness = (float)(random() % 2048 - 1024) / 1024;
	frequency = f * (1 + randomDetune * randomness);
	anglePerSample = frequency / samplerate;
}

float Oscillator::getFrequency() const
{
	return frequency;
}

void Oscillator::debugCheckTables(const char * message)
{
	std::cout << "Oscillator::debugCheckTables(" << message << "): ";
	unsigned int i;

	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		if (sineTable[i] < -1 && sineTable[i] > 1) {
			std::cout << "sineTable[" << i << "] = " << sineTable[i] << "!" <<
				std::endl;
			return;
		}
		if (absSineTable[i] < -1 || absSineTable[i] > 1) {
			std::cout << "absSineTable[" << i << "] = " << absSineTable[i]
			    << "!" << std::endl;
			return;
		}

	}
	std::cout << "ok." << std::endl;
}
// Generates waveform tables
void Oscillator::generateWaveTables()
{
	unsigned int i;
	float x, increase;

	// Sine wave
	x = 0;
	increase = 2 * M_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		sineTable[i] = sinf(x);
		x += increase;
	}

	// abs(sin)
	x = 0;
	increase = M_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		absSineTable[i] = -1 + 2 * fabs(sinf(x));
		x += increase;
	}
}

// Generates base frequencies for every MIDI key.
void Oscillator::generateBaseFrequencies()
{

	// Standard A key above piano middle C is 440 Hz.
	// It is assigned to MIDI key number 69 (between 0 and 127).
	baseFrequency[9]  = 13.75;
	baseFrequency[21] = 27.5;
	baseFrequency[33] = 55;
	baseFrequency[45] = 110;
	baseFrequency[57] = 220;
	baseFrequency[69] = 440;
	baseFrequency[81] = 880;
	baseFrequency[93] = 1760;
	baseFrequency[105] = 3520;
	baseFrequency[117] = 7040;

	float semitoneUp = powf(2, 1.0 / 12);
	float semitoneDown = 1 / semitoneUp;
	float f;
	int i, j;

	// Keys 0..119
	for (i = 0; i <= 108; i += 12) {

		f = baseFrequency[i + 9];
		for (j = 8; j >= 0; j--) {
			f *= semitoneDown;
			baseFrequency[i + j] = f;
		}

		f = baseFrequency[i + 9];
		for (j = 10; j < 12; j++) {
			f *= semitoneUp;
			baseFrequency[i + j] = f;
		}

	}

	// Keys 120..127
	f = baseFrequency[120];
	for (i = 121; i < 128; i++) {
		f *= semitoneUp;
		baseFrequency[i] = f;
	}
}

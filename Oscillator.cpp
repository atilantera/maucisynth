/*
 * Oscillator.cpp
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#include "Oscillator.h"

float Oscillator::sineTable[WAVE_TABLE_LENGTH];
float Oscillator::triangleTable[WAVE_TABLE_LENGTH];
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
	if (f < 1) {
		return;
	}

	float randomness = (float)(random() % 2048 - 1024) / 1024;
	frequency = f * (1 + randomDetune * randomness);
	angle = 0;
	anglePerSample = frequency / samplerate;
}

float Oscillator::getFrequency() const
{
	return frequency;
}

// Generates waveform tables
void Oscillator::generateWaveTables()
{
	int i, b;
	float x, increase;

	// Sine wave
	x = 0;
	increase = 2 * M_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		sineTable[i] = sinf(x);
		x += increase;
	}

	// Triangle wave
	b = WAVE_TABLE_LENGTH / 4;
	x = 0;
	increase = 4.0 / (float)(WAVE_TABLE_LENGTH);
	for (i = 0; i < b; i++) {
		triangleTable[i] = x;
		x += increase;
	}
	b *= 3;
	for (; i < b; i++) {
		triangleTable[i] = x;
		x -= increase;
	}
	for (; i < WAVE_TABLE_LENGTH; i++) {
		triangleTable[i] = x;
		x += increase;
	}

	// abs(sin)
	x = 0;
	increase = M_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		absSineTable[i] = -1 + fabs(sinf(x)) * 2;
		x += increase;
	}
}

// Generates base frequencies for every MIDI key.
void Oscillator::generateBaseFrequencies()
{
	float semitone = powf(2, (float)1/12);
	float octaveCoefficent;
	int i, j;

	// Standard A key above piano middle C is 440 Hz.
	// It is assigned to MIDI key number 69 (between 0 and 127).
	baseFrequency[69] = 440;

	// notes 70..80
	for (i = 0; i < 11; i++) {
		baseFrequency[70 + i] = baseFrequency[69 + i] * semitone;
	}

	// notes 9 .. 68
	octaveCoefficent = 0.5;
	for (j = 1; j < 7; j++) {
		for (i = 0; i < 12; i++) {
			baseFrequency[69 - j * 12 + i] =
			    baseFrequency[69 + i] * octaveCoefficent;
		}
		octaveCoefficent *= 0.5;
	}

	// notes 81 .. 116
	octaveCoefficent = 2;
	for (j = 1; j < 4; j++) {
		for (i = 0; i < 12; i++) {
			baseFrequency[69 + j * 12 + i] =
				baseFrequency[69 + i] * octaveCoefficent;
		}
		octaveCoefficent *= 2;
	}

	// notes 117 .. 127
	octaveCoefficent = 16;
	for (i = 0; i < 11; i++) {
		baseFrequency[117 + i] = baseFrequency[69 + i] * octaveCoefficent;
	}

	// notes 0..8
	octaveCoefficent = (float)1/32;
	for (i = 0; i < 12; i++) {
		baseFrequency[i] = baseFrequency[60 + i] * octaveCoefficent;
	}
}

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
bool Oscillator::staticDataInitialized = false;
float Oscillator::randomDetune = 0.000001;

Oscillator::Oscillator() {
	if (!staticDataInitialized) {
		generateWaveTables();
		generateBaseFrequencies();
		staticDataInitialized = true;
	}

}

Oscillator::~Oscillator() {
	// TODO Auto-generated destructor stub
}

// Generates waveform tables
void Oscillator::generateWaveTables()
{
	int i, b;
	float angle, increase;

	// Sine wave
	angle = 0;
	increase = TWO_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		sineTable[i] = sinf(angle);
		angle += increase;
	}

	// Triangle wave
	b = WAVE_TABLE_LENGTH / 4;
	angle = 0;
	increase = 4.0 / (float)(WAVE_TABLE_LENGTH);
	for (i = 0; i < b; i++) {
		triangleTable[i] = angle;
		angle += increase;
	}
	b *= 3;
	for (; i < b; i++) {
		triangleTable[i] = angle;
		angle -= increase;
	}
	for (; i < WAVE_TABLE_LENGTH; i++) {
		triangleTable[i] = angle;
		angle += increase;
	}

	// abs(sin)
	angle = 0;
	increase = 0.5 * TWO_PI / WAVE_TABLE_LENGTH;
	for (i = 0; i < WAVE_TABLE_LENGTH; i++) {
		absSineTable[i] = -1 + fabs(sinf(angle)) * 2;
		angle += increase;
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

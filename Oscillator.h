/*
 * Oscillator.h
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

// Length of wave tables in samples.
// Oscillator::baseFrequency[0] ~= 8 Hz.
// Typical samplerate is 44100 .. 96000 Hz.
// Therefore 96000 / 8 = 12000 samples would be sufficient to reproduce
// high-resolution waves at 8 Hz while samplerate is 96 kHz.
// 12288 = 3 * 4096. (If system memory page size is 4 kB, it's equally 3 pages.)
const int WAVE_TABLE_LENGTH = 12288;

class Oscillator {
public:
	Oscillator();
	virtual ~Oscillator();

	static void setSamplerate(int samplesPerSecond);
	void setFrequency(float frequency);
	virtual void generateSound() { }

protected:
	void generateWaveTables();
	void generateBaseFrequencies();

	// Value of the last sample played.
	// If the note of the oscillator ends in Attack or Decay phases,
	// the current peak amplitude is not sustain * peakAmplitude.
	float lastSample;

	float frequency;
	float pulseWidth;

	// angle is amount of oscillator cycle, 0..1.
	// (It would be just a nuisance to have angle in radians: angle is either
	// projected into index of precalculated waveform table or used in
	// generating sawtooth or pulse wave, where trigonometric function are not
	// needed.)
	float angle;
	float anglePerSample;

	// Samplerate that all oscillators use. Samples per second.
	static int samplerate;

	// Precalculated waveform tables
	static float sineTable[WAVE_TABLE_LENGTH];
	static float triangleTable[WAVE_TABLE_LENGTH];
	static float absSineTable[WAVE_TABLE_LENGTH];

	// Precalculated base frequencies for all MIDI keys
	static float baseFrequency[128];
	static bool staticDataInitialized;

	// In acoustic and analogue electronic instruments, two adjacent notes with
	// the same nominal key frequency do not have the exact playing frequency.
	// This is just random variance brought by nature. Here field randomDetune
	// simulates this: if randomDetune = 0.001, the note played by an oscillator
	// is the nominal frequency +-0.1% (which is quite much).
	static float randomDetune;
};

#endif /* OSCILLATOR_H_ */

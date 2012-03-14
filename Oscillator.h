/*
 * Oscillator.h
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <math.h>
#include "SoundBuffer.h"

// Length of wave tables in samples. 44100 samples could store sine wave
// at 1.0 Hz when the samplerate is 44.1 kHz.
const int WAVE_TABLE_LENGTH = 65536;

const float TWO_PI = 6.283185308;

class Oscillator {
public:
	Oscillator();
	virtual ~Oscillator();

	void generateSound();

protected:
	void generateWaveTables();
	void generateBaseFrequencies();

	// Value of the last sample played.
	// If the note of the oscillator ends in Attack or Decay phases,
	// the current peak amplitude is not sustain * peakAmplitude.
	float lastSample;

	float frequency;           // osc_frequency
	float pulseWidth;          // osc_pulse_width
	float angle;               // osc_angle
	float radiansPerSample;    // osc_radians_per_sample

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

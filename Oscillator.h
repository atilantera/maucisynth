/*
 * Oscillator.h
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Length of wave tables in samples.
// Oscillator::baseFrequency[0] ~= 8 Hz.
// Typical samplerate is 44100 Hz.
// Therefore 44100 / 8 = 5512 samples would be sufficient to reproduce
// high-resolution waves at 8 Hz when samplerate is 44,1 kHz.
// 4096 samples will fit nicely to four 4k memory pages when
// sizeof(float) == 4.
const int WAVE_TABLE_LENGTH = 4096;

class Oscillator {
public:
	Oscillator();
	virtual ~Oscillator();

	static void setSamplerate(unsigned int samplesPerSecond);
	static void setBufferLength(unsigned int lengthInSamples);

	void setFrequency(float frequency);
	virtual void generateSound() { }

protected:
	void generateWaveTables();
	void generateBaseFrequencies();

	// Oscillator base frequency is in Hz (cycles per second).
	float frequency;

	// Pulse width is used with pulse wave only. 0 <= pulseWidth <= 1.
	float pulseWidth;

	// angle is amount of oscillator cycle, 0..1.
	// (It would be just a nuisance to have angle in radians: angle is either
	// projected into index of precalculated waveform table or used in
	// generating sawtooth or pulse wave, where trigonometric function are not
	// needed.)
	float angle;
	float anglePerSample;

	// Samplerate that all oscillators use. Samples per second.
	static unsigned int samplerate;

	// Length of output buffer and possible modulator buffer in samples
	// for method generateSound().
	static unsigned int bufferLength;

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

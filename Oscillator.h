/*
 * Oscillator.h
 *
 *  Created on: 12.3.2012
 *      Author: alkim
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <math.h>

// Length of wave tables in samples. 44100 samples could store sine wave
// at 1.0 Hz when the samplerate is 44.1 kHz.
const int WAVE_TABLE_LENGTH = 65536;

const float TWO_PI = 6.283185308;

class Oscillator {
public:
	Oscillator();
	virtual ~Oscillator();


protected:
	void generateWaveTables();
	void generateBaseFrequencies();

	// variable                   variable in synth.c
	int waveform;              // osc_waveform
	int envelopePhase;         // osc_envelope_phase
	int previousEnvelopePhase; // osc_precious_envelope_phase
	float envelopeAmplitude;   // osc_envelope_amplitude
	bool isProducingSound;     // osc_mute

	// ADSR parameters. Time in samples. Sustain volume is 0..1 times
	// peak amplitude (between attack and decay).
	// variable                   variable in synth.c
	int attack;                // osc_attack_time
	int decay;                 // osc_decay_time
	float sustain;             // osc_sustain_volume
	int release;               // osc_release_time

	// Which keyboard key this oscillator is currently playing
	// variable                   variable in synth.c
	unsigned char key;         // osc_key
	int keySource;             // osc_key_source

	// Peak amplitude of the oscillator. MIDI note velocity determines
	// this. It is coefficent between 0..1.
	float peakAmplitude;        // osc_max_amplitude

	// Playing time of current envelope phase (in samples)
	int phaseTime;             // osc_time

	// Value of the last sample played. When the synthesizer gets an
	// "All notes off" message, it means that all oscillators must be muted
	// suddenly. To prevent a crack or pop, all oscillators produce quick
	// linear transition from last sample value to zero.
	float lastSample;

	// Precalculated waveform tables
	static float sineTable[WAVE_TABLE_LENGTH];
	static float triangleTable[WAVE_TABLE_LENGTH];
	static float absSineTable[WAVE_TABLE_LENGTH];

	// Precalculated base frequencies for all MIDI keys
	static float baseFrequency[128];

	static bool staticDataInitialized;
	static float randomDetune;




};

#endif /* OSCILLATOR_H_ */

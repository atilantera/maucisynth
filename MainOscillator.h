/*
 * MainOscillator.h
 *
 * Main oscillator produces signal that is part of the sound.
 * Main oscillator can be modulated by a Low Frequency Oscillator (LFO).
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef MAINOSCILLATOR_H_
#define MAINOSCILLATOR_H_

#include "Oscillator.h"
#include "SynthParameters.h"

class MainOscillator : public Oscillator {
public:
	MainOscillator();
	virtual ~MainOscillator();

	void generateSound(float * buffer, int bufferLength);

private:
	// variable                   variable in synth.c
	int waveform;              // osc_waveform
	int envelopePhase;         // osc_envelope_phase
	int previousEnvelopePhase; // osc_precious_envelope_phase
	float envelopeAmplitude;   // osc_envelope_amplitude
	bool isProducingSound;     // osc_mute

	// Amplitude envelope (ADSR curve) parameters.
	// There are four adjacent phases of amplitude:
	// first attack, then decay, then sustain and finally release.
	// Attack: amplitude grows from 0 to peakAmplitude. Time in samples.
	int attack;                // osc_attack_time

	// Decay: amplitude decreases from peakAmplitude to
	//       sustain * peakAmplitude. Time in samples.
	int decay;                 // osc_decay_time

	// Sustain: amplitude stays in sustain * peakAmplitude as long as the
	// key is pressed. Note that sustain is relative value, not time.
	float sustain;             // osc_sustain_volume

	// Release: amplitude decreases to zero when the key is released.
	// Release time is in samples.
	int release;               // osc_release_time

	// Which keyboard key this oscillator is currently playing
	// variable                   variable in synth.c
	unsigned char key;         // osc_key
	int keySource;             // osc_key_source

	// Peak amplitude of the oscillator. MIDI note velocity determines
	// this. It is coefficent between 0..1.
	float peakAmplitude;        // osc_max_amplitude

	// Playing time of current envelope phase (in samples)
	// This must be memorized if the note that the oscillator is playing
	// ends before the amplitude envelope reaches

	int phaseTime;             // osc_time

};

#endif /* MAINOSCILLATOR_H_ */

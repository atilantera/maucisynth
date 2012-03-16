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

#ifdef TESTING
#include <iostream>
#endif

#include "Oscillator.h"
#include "SynthParameters.h"

class MainOscillator : public Oscillator {
public:
	MainOscillator();
	virtual ~MainOscillator();

	static void setWaveform(WaveformType w);
	static void setModulationTarget(LfoModulationTarget m);

	void noteOn(unsigned char noteKey, unsigned char noteVelocity,
			NoteSource source);
	void noteOff();
	void generateSound(float * outputBuffer, float * modulatorBuffer,
		unsigned int bufferLength);

private:
	enum EnvelopePhase { ATTACK, DECAY, SUSTAIN, RELEASE, QUIET };

	static WaveformType waveform;

	// synthesisFunction points to synthesizeFromWavetable() or
	// synthesizeSawtooth() or synthesizePulseWave().
	static void (* synthesisFunction) (float *, float *, unsigned int);

	// wavetable points to sineTable[] or triangleTable[] or
	// absSineTable[] in class Oscillator.
	static float * wavetable;

	static LfoModulationTarget modulation;

	EnvelopePhase envelopePhase;         // osc_envelope_phase
	EnvelopePhase previousEnvelopePhase; // osc_precious_envelope_phase
	float envelopeAmplitude;   // osc_envelope_amplitude

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
	NoteSource noteSource;             // osc_key_source

	// Peak amplitude of the oscillator. MIDI note velocity determines
	// this. It is coefficent between 0..1.
	float peakAmplitude;        // osc_max_amplitude

	// Playing time of current envelope phase (in samples)
	// This must be memorized if the note that the oscillator is playing
	// ends before the amplitude envelope reaches

	int phaseTime;             // osc_time

	static void synthesizeFromWavetable(float * outputBuffer, float * modulatorBuffer,
		unsigned int bufferLength);

	static void synthesizeSawtooth(float * outputBuffer, float * modulatorBuffer,
		unsigned int bufferLength);

	static void synthesizePulseWave(float * outputBuffer, float * modulatorBuffer,
		unsigned int bufferLength);

};

#endif /* MAINOSCILLATOR_H_ */

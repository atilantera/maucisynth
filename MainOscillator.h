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

#include "tests/testing.h"

class MainOscillator : public Oscillator {
public:
	MainOscillator();
	virtual ~MainOscillator();

	void setWaveform(WaveformType w);
	void setModulationTarget(LfoModulationTarget m);
	void setModulationAmount(float a);

	void noteOn(unsigned char noteKey, unsigned char noteVelocity);
	void noteOff();
	void generateSound(float outputBuffer[], const float modulatorBuffer[],
		bool & noteFinished);

protected:
	enum EnvelopePhase { ATTACK, DECAY, SUSTAIN, RELEASE, OFF };

	WaveformType waveform;

	// wavetable points to sineTable[] or triangleTable[] or
	// absSineTable[] in class Oscillator.
	float * wavetable;

	LfoModulationTarget modulation;
	float modulationAmount;

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
	float peakAmplitude;

	// Playing time of current envelope phase (in samples)
	// This must be memorized if the note that the oscillator is playing
	// ends before the amplitude envelope reaches
	int phaseTime;             // osc_time

	// Method synthesizePulseWave must have pulse width memorized between
	// adjacent calls when pulse width modulation is on.
	// pulseWidth is in radians (0 .. TWO_PI).
	float pulseWidth;

	void synthesizeFromWavetable(float outputBuffer[],
		const float modulatorBuffer[]);

	void synthesizeSawtooth(float outputBuffer[],
		const float modulatorBuffer[]);

	void synthesizePulseWave(float outputBuffer[],
		const float modulatorBuffer[]);

	void applyAmplitudeModulation(float outputBuffer[],
		const float modulatorBuffer[]);

};

#endif /* MAINOSCILLATOR_H_ */

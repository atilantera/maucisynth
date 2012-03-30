/*
 * MainOscillator.h
 *
 * Main oscillator produces signal that is part of the sound.
 * Main oscillator can be modulated by a Low Frequency Oscillator (LFO).
 *
 * Note: MainOscillator calls LowFrequencyOscillator.setRelativeFrequency(),
 * but not any other methods of the LFO class.
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef MAINOSCILLATOR_H_
#define MAINOSCILLATOR_H_

#include "Oscillator.h"
#include "LowFrequencyOscillator.h"
#include "SynthParameters.h"

#include "tests/testing.h"

enum EnvelopePhase { ATTACK, DECAY, SUSTAIN, RELEASE, OFF, FAST_MUTE };

class MainOscillator : public Oscillator {
public:
	MainOscillator(OscillatorParameters & p);
	virtual ~MainOscillator();

	void setDedicatedLfo(LowFrequencyOscillator * lfo);
	void noteOn(unsigned char noteKey, unsigned char noteVelocity);
	void noteOff();
	void muteFast();

	void generateSound(float outputBuffer[], const float modulatorBuffer[],
		bool & noteFinished);

	EnvelopePhase getEnvelopePhase() const;

protected:
	// Parameters: Amplitude envelope (ADSR curve) parameters.
	// Attack, decay and release are in samples.
	// Sustain is relative amplitude: 0..1.
	unsigned int attackTime;
	unsigned int decayTime;
	float sustainVolume;
	unsigned int releaseTime;

	OscillatorParameters & globalParameters;
	LowFrequencyOscillator * dedicatedLfo;

	// Dynamic parameter: Peak amplitude of the oscillator. MIDI note velocity
	// determines this. It is coefficent between 0..1.
	float peakAmplitude;


	// Field wavetable depends on field waveform. See setWaveform().
	// It points to sineTable[] or triangleTable[] or
	// absSineTable[] in class Oscillator.
	float * wavetable;

	EnvelopePhase envelopePhase;         // osc_envelope_phase
	EnvelopePhase previousEnvelopePhase; // osc_precious_envelope_phase
	float envelopeAmplitude;   // osc_envelope_amplitude

	// Playing time of current envelope phase (in samples).
	// Used to determine when envelope phase changes to another
	unsigned int envelopePhaseTime;

	// Method synthesizePulseWave must have pulse width memorized between
	// adjacent calls when pulse width modulation is on.
	// pulseWidth is in radians (0 .. TWO_PI).
	float pulseWidth;

	// Methods synthesizeTriangleWave and synthesizeSawtooth and must have
	// their own anglePerSample memorized. This is because with frequency
	// modulation, those methods change angle increase only one or two
	// times per cycle.
	float triangleAnglePerSample;
	float sawtoothAnglePerSample;

	// Value of the last sample played. This must be memorized because
	// if the note of the oscillator ends in Attack or Decay phases,
	// the current peak amplitude is not sustain * peakAmplitude.
	// Going directly from attack/decay to release results in audible crack.
	float lastSample;

	void setFrequency(float f);

	void synthesizeFromWavetable(float outputBuffer[],
		const float modulatorBuffer[]);

	void synthesizeTriangleWave(float outputBuffer[],
		const float modulatorBuffer[]);

	void synthesizeSawtoothWave(float outputBuffer[],
		const float modulatorBuffer[]);

	void synthesizePulseWave(float outputBuffer[],
		const float modulatorBuffer[]);

	void applyAmplitudeModulation(float outputBuffer[],
		const float modulatorBuffer[]);

	void applyEnvelope(float outputBuffer[]);

	unsigned int applyAttack(float outputBuffer[], unsigned int i);
	unsigned int applyDecay(float outputBuffer[], unsigned int i);
	unsigned int applyRelease(float outputBuffer[], unsigned int i);

	void applyFastMute(float outputBuffer[]);

};

#endif /* MAINOSCILLATOR_H_ */

/*
 * MainOscillator.h
 *
 * Main oscillator produces sound of a single note.
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

#include <cstring>
#include "Oscillator.h"
#include "SynthParameters.h"

#include "tests/testing.h"

#define USE_OPTIMIZATIONS 1

enum EnvelopePhase { ATTACK, DECAY, SUSTAIN, RELEASE, OFF, FAST_MUTE,
    RETRIGGER };

class MainOscillator : public Oscillator {
public:
	MainOscillator(OscillatorParameters & p);
	virtual ~MainOscillator();

	void noteOn(unsigned char noteKey, unsigned char noteVelocity,
        bool retrigger);
	void noteOff();
	void muteFast();

	void generateSound(float outputBuffer[], const float modulatorBuffer[],
		unsigned int rangeStart, unsigned int rangeEnd, bool & noteFinished);

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

	// Dynamic parameter: Peak amplitude of the oscillator. MIDI note velocity
	// determines this. It is coefficent between 0..1.
	float peakAmplitude;

	// Field wavetable depends on field waveform. See setWaveform().
	// It points to sineTable[] or triangleTable[] or
	// absSineTable[] in class Oscillator.
	float * wavetable;

    // envelopePhase is the current ADSR curve phase.
   	EnvelopePhase envelopePhase;

    // previousEnvelopePhase is used to determine if envelopePhase
    // changed to RELEASE from ATTACK or DECAY instead of SUSTAIN.
    // This happens when the duration of the note is shorter than
    // duration of attack and decay phases together (i.e. method
    // noteOff() is called when envelopePhase = ATTACK or DECAY.
	EnvelopePhase previousEnvelopePhase;

    // Last amplitude value of the current envelope curve played.
    // This is needed for appropriate transition from attack or decay
    // phase to release phase.
	float envelopeAmplitude;

    // applySustain needs also this.
    float previousEnvelopeAmplitude;

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
	float lastSampleValue;

	void setFrequency(float f);

	void synthesizeFromWavetable(float outputBuffer[],
		const float modulatorBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	void synthesizeTriangleWave(float outputBuffer[],
		const float modulatorBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	void synthesizeSawtoothWave(float outputBuffer[],
		const float modulatorBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	void synthesizePulseWave(float outputBuffer[],
		const float modulatorBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	void applyAmplitudeModulation(float outputBuffer[],
		const float modulatorBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	static void initializeEnvelopeTables();

	void applyEnvelope(float outputBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	unsigned int applyAttack(float outputBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

    unsigned int applyRetrigger(float outputBuffer[], unsigned int rangeStart,
    	unsigned int rangeEnd);

	unsigned int applyDecay(float outputBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	unsigned int applySustain(float outputBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	unsigned int applyRelease(float outputBuffer[], unsigned int rangeStart,
		unsigned int rangeEnd);

	void applyFastMute(float outputBuffer[]);
};

#endif /* MAINOSCILLATOR_H_ */

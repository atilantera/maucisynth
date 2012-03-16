/*
 * MainOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "MainOscillator.h"

WaveformType MainOscillator::waveform = SINE;
void (* MainOscillator::synthesisFunction)
    (float *, float *, unsigned int) = MainOscillator::synthesizeFromWavetable;

float * MainOscillator::wavetable = Oscillator::sineTable;
LfoModulationTarget MainOscillator::modulation = NONE;

MainOscillator::MainOscillator()
{
	envelopePhase = QUIET;
	previousEnvelopePhase = QUIET;
	envelopeAmplitude = 0;
	attack = 100;
	decay = 100;
	sustain = 50;
	release = 500;
	key = 0;
	peakAmplitude = 0;
	phaseTime = 0;
}

MainOscillator::~MainOscillator() {
}

// Sets waveform type of all MainOscillators
void MainOscillator::setWaveform(WaveformType w)
{
	waveform = w;
	switch (waveform) {

	case SINE:
		synthesisFunction = synthesizeFromWavetable;
		wavetable = sineTable;
		break;

	case TRIANGLE:
		synthesisFunction = synthesizeFromWavetable;
		wavetable = triangleTable;
		break;

	case ABS_SINE:
		synthesisFunction = synthesizeFromWavetable;
		wavetable = absSineTable;
		break;

	case SAWTOOTH:
		synthesisFunction = synthesizeSawtooth;
		break;

	case PULSE:
		synthesisFunction = synthesizePulseWave;
		break;

	}
}

// Sets LFO modulation target of all MainOscillators
void MainOscillator::setModulationTarget(LfoModulationTarget m)
{
	modulation = m;
}

// Sets oscillator base frequency and maximum amplitude.
// Sets envelope curve to the beginning of attack phase.
void MainOscillator::noteOn(unsigned char noteKey, unsigned char noteVelocity,
	NoteSource source)
{
	// TODO
}

// Sets envelope curve to the beginning of release phase.
void MainOscillator::noteOff()
{
	// TODO
}

//
void MainOscillator::generateSound(float * outputBuffer,
	float * modulatorBuffer, unsigned int bufferLength)
{
	synthesisFunction(outputBuffer, modulatorBuffer, bufferLength);
}

void MainOscillator::synthesizeFromWavetable(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	// TODO
}

void MainOscillator::synthesizeSawtooth(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	// TODO
}

void MainOscillator::synthesizePulseWave(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	// TODO
}

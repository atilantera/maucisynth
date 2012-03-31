/*
 * MainOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "MainOscillator.h"

#ifndef NULL
#define NULL 0
#endif

// curve steepness in envelope curve
const float steepness = 4;

const int muteLength = 16;

MainOscillator::MainOscillator(OscillatorParameters & p) :
	globalParameters(p)
{

	wavetable = sineTable;
	dedicatedLfo = NULL;

	envelopePhase = OFF;
	previousEnvelopePhase = OFF;
	envelopeAmplitude = 0;
	attackTime = MinAttackTime;
	decayTime = MinDecayTime;
	sustainVolume = MaxSustainVolume;
	releaseTime = MinReleaseTime;
	peakAmplitude = 0;
	envelopePhaseTime = 0;
	pulseWidth = 0.5;
	lastSample = 0;
}

MainOscillator::~MainOscillator() {
}

// Sets a LowFrequencyOscillator whose relative frequency this
// MainOscillator changes at "note on" events when frequency type
// is RELATIVE.
void MainOscillator::setDedicatedLfo(LowFrequencyOscillator * lfo)
{
	dedicatedLfo = lfo;
}

// Sets oscillator base frequency and maximum amplitude.
// Sets envelope curve to the beginning of attack phase.
void MainOscillator::noteOn(unsigned char noteKey, unsigned char noteVelocity)
{
	if (noteKey > 127 || noteVelocity == 0) {
		return;
	}

	attackTime = globalParameters.attackTime * 0.001 * samplerate;
	decayTime = globalParameters.decayTime * 0.001 * samplerate;
	sustainVolume = globalParameters.sustainVolume;
	releaseTime = globalParameters.releaseTime * 0.001 * samplerate;

	lastSample = 0;
	setFrequency(baseFrequency[noteKey]);

	envelopePhase = ATTACK;
	envelopePhaseTime = 0;

	if (noteVelocity > 127) {
		noteVelocity = 127;
	}
	peakAmplitude = (float)noteVelocity / 127;

	if (globalParameters.lfoFrequencyType == RELATIVE && dedicatedLfo != NULL) {
		dedicatedLfo->updateRelativeFrequency(frequency);
	}
}

// Sets envelope curve to the beginning of release phase.
void MainOscillator::noteOff()
{
	envelopePhase = RELEASE;
}

// Sets envelope curve to fast mute: interpolation from last sample
// value to zero.
void MainOscillator::muteFast()
{
	envelopePhase = FAST_MUTE;
}

// Generates a sound of an oscillator.
// outputBuffer = buffer for sound output
// modulatorBuffer = sound of a low frequency oscillator
// bufferLength = length of outbutBuffer and modulatorBuffer in samples
// noteFinished is changed to true if oscillator finished its envelope curve
// (phase RELEASE ended).
void MainOscillator::generateSound(float outputBuffer[],
const float modulatorBuffer[], bool & noteFinished)
{
	if (envelopePhase == FAST_MUTE) {
		applyFastMute(outputBuffer);
		noteFinished = true;
		return;
	}

	switch (globalParameters.waveform) {
	case SINE:
		wavetable = Oscillator::sineTable;
		synthesizeFromWavetable(outputBuffer, modulatorBuffer);
		break;

	case TRIANGLE:
		synthesizeTriangleWave(outputBuffer, modulatorBuffer);
		break;

	case ABS_SINE:
		wavetable = Oscillator::absSineTable;
		synthesizeFromWavetable(outputBuffer, modulatorBuffer);
		break;

	case SAWTOOTH:
		synthesizeSawtoothWave(outputBuffer, modulatorBuffer);
		break;

	case PULSE:
		synthesizePulseWave(outputBuffer, modulatorBuffer);
		break;
	}

	if (globalParameters.lfoModulationTarget == AMPLITUDE) {
		applyAmplitudeModulation(outputBuffer, modulatorBuffer);
	}

	applyEnvelope(outputBuffer);

	noteFinished = (envelopePhase == OFF);
}

void MainOscillator::setFrequency(float f)
{
	((Oscillator *)this)->setFrequency(f);
	triangleAnglePerSample = anglePerSample;
	sawtoothAnglePerSample = anglePerSample;
}

void MainOscillator::synthesizeFromWavetable(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i]
				= wavetable[(int)(angle * WAVE_TABLE_LENGTH + 0.5)];
			angle += anglePerSample * (1 + globalParameters.lfoModulationAmount
				* modulatorBuffer[i]);
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i]
				= wavetable[(int) (angle * WAVE_TABLE_LENGTH + 0.5)];
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizeTriangleWave(float outputBuffer[],
	const float modulatorBuffer[])
{
	unsigned int i;
	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			if (angle < 0.25) {
				outputBuffer[i] = 4 * angle;
			}
			else {
				if (angle < 0.75) {
					outputBuffer[i] = 2 - 4 * angle;
				}
				else {
					outputBuffer[i] = -4 + 4 * angle;
				}
			}
			angle += triangleAnglePerSample;
			if (angle >= 1) {
				angle -= 1;
				triangleAnglePerSample = anglePerSample
					* (1 + globalParameters.lfoModulationAmount
					* modulatorBuffer[i]);
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			if (angle < 0.25) {
				outputBuffer[i] = 4 * angle;
			}
			else {
				if (angle < 0.75) {
					outputBuffer[i] = 2 - 4 * angle;
				}
				else {
					outputBuffer[i] = -4 + 4 * angle;
				}
			}
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizeSawtoothWave(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += sawtoothAnglePerSample;
			if (angle >= 1) {
				sawtoothAnglePerSample = anglePerSample * (1 +
					globalParameters.lfoModulationAmount * modulatorBuffer[i]);
				angle -= 1;
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizePulseWave(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (globalParameters.lfoModulationTarget == PULSE_WIDTH) {
		for (i = 0; i < bufferLength; i++) {
			if (angle < pulseWidth) {
				outputBuffer[i] = 1;
			}
			else {
				outputBuffer[i] = -1;
			}
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
				pulseWidth = 0.5 *
					(1 + globalParameters.lfoModulationAmount *
					 modulatorBuffer[i]);
			}
		}
	}
	else if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			if (angle < 0.5) {
				outputBuffer[i] = 1;
			}
			else {
				outputBuffer[i] = -1;
			}
			angle += anglePerSample * (1 +
				globalParameters.lfoModulationAmount * modulatorBuffer[i]);
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			if (angle < 0.5) {
				outputBuffer[i] = 1;
			}
			else {
				outputBuffer[i] = -1;
			}
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::applyAmplitudeModulation(float outputBuffer[],
const float modulatorBuffer[])
{
	float lfoAmount = globalParameters.lfoModulationAmount * 0.5;
    float lfoMidpoint = 1 - lfoAmount;

    for (unsigned int i = 0; i < bufferLength; i++) {
       outputBuffer[i] *= lfoMidpoint + lfoAmount * modulatorBuffer[i];
    }
}

void MainOscillator::applyEnvelope(float outputBuffer[])
{
	unsigned int i = 0;

	while (i < bufferLength) {
		switch (envelopePhase) {

		case OFF:
			for (; i < bufferLength; i++) {
				outputBuffer[i] = 0;
			}
			break;

		case ATTACK:
			i = applyAttack(outputBuffer, i);
			break;

		case DECAY:
			i = applyDecay(outputBuffer, i);
			break;

		case SUSTAIN:
			for (; i < bufferLength; i++) {
				outputBuffer[i] *= sustainVolume;
			}
			break;

		case RELEASE:
			i = applyRelease(outputBuffer, i);
			break;

		case FAST_MUTE:
			// This is done in generateSound().
			break;

		}
	}

	lastSample = outputBuffer[bufferLength - 1];
}

unsigned int MainOscillator::applyAttack(float outputBuffer[], unsigned int i)
{
	unsigned int samplesLeft = attackTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / attackTime;
	float timeIncrease = 1.0 / (attackTime - 1);
	float b = 1 / (1 - expf(-steepness));
	float amplitude;

	if (samplesLeft > bufferLength) {
		for (; i < bufferLength; i++) {
			amplitude = b * (1 - expf(-steepness * relativeTime));
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhaseTime += bufferLength;
	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
			amplitude = b * (1 - expf(-steepness * relativeTime));
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhase = DECAY;
		previousEnvelopePhase = ATTACK;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = amplitude;

	return i;
}

unsigned int MainOscillator::applyDecay(float outputBuffer[], unsigned int i)
{
	unsigned int samplesLeft = decayTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / decayTime;
	float timeIncrease = 1.0 / (decayTime - 1);
	float b = 1 / (expf(-steepness) - 1);
	float amplitude;

	if (samplesLeft > bufferLength) {
		envelopePhaseTime += bufferLength - i;
		for (; i < bufferLength; i++) {
			amplitude = 1 - (1 - sustainVolume) * b
				* (expf(-steepness * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
			amplitude = 1 - (1 - sustainVolume) * b
				* (expf(-steepness * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhase = SUSTAIN;
		previousEnvelopePhase = ATTACK;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = amplitude;

	return i;
}

unsigned int MainOscillator::applyRelease(float outputBuffer[], unsigned int i)
{
	if (previousEnvelopePhase != RELEASE) {
		envelopePhaseTime = 0;
		previousEnvelopePhase = RELEASE;
	}

	unsigned int samplesLeft = releaseTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / releaseTime;
	float timeIncrease = 1.0 / releaseTime;
	float b = 1 / (expf(-steepness) - 1);
	float amplitude;

	if (samplesLeft > bufferLength) {
		envelopePhaseTime += bufferLength - i;
		for (; i < bufferLength; i++) {
			amplitude = sustainVolume -
				sustainVolume * b * (expf(-steepness * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
			amplitude = sustainVolume -
				sustainVolume * b * (expf(-steepness * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhase = OFF;
		envelopePhaseTime = 0;
	}

	return i;
}

void MainOscillator::applyFastMute(float outputBuffer[])
{
	unsigned int i;
	unsigned int turnOffLength = muteLength;
	if (turnOffLength > bufferLength) {
		turnOffLength = bufferLength;
	}
	for (i = 0; i < turnOffLength; i++) {
		outputBuffer[i] = (turnOffLength - i) * lastSample / turnOffLength;
	}
	for (; i < bufferLength; i++) {
		outputBuffer[i] = 0;
	}
	envelopePhase = OFF;
}

EnvelopePhase MainOscillator::getEnvelopePhase() const
{
	return envelopePhase;
}

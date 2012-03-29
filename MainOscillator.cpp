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
	waveform = SINE;
	wavetable = sineTable;
	modulation = NONE;
	modulationAmount = 0;
	lfoFrequencyType = FIXED;
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

// Sets waveform type
void MainOscillator::setWaveform(WaveformType w)
{
	waveform = w;
	switch (waveform) {
	case SINE:
		wavetable = Oscillator::sineTable;
		break;

	case TRIANGLE:
		wavetable = Oscillator::triangleTable;
		break;

	case ABS_SINE:
		wavetable = Oscillator::absSineTable;
		break;

	case SAWTOOTH:
	case PULSE:
		break;
	}
}

// Sets a LowFrequencyOscillator whose relative frequency this
// MainOscillator changes at "note on" events when frequency type
// is RELATIVE.
void MainOscillator::setDedicatedLfo(LowFrequencyOscillator * lfo)
{
	dedicatedLfo = lfo;
}

// Sets modulation target of a LowFrequencyOscillator
void MainOscillator::setModulationTarget(LfoModulationTarget m)
{
	modulation = m;
}

// Sets modulation amount of a LowFrequencyOscillator
void MainOscillator::setModulationAmount(float a)
{
	if (0 <= a && a <= 1) {
		modulationAmount = a;
	}
}

// Sets frequency type of a LowFrequencyOscillator:
// FIXED or RELATIVE.
void MainOscillator::setLfoFrequencyType(LfoFrequencyType t)
{
	lfoFrequencyType = t;
}

// Sets length of attack phase of envelope curve in milliseconds
void MainOscillator::setAttack(unsigned int a)
{
	if (a > 5 && a < 10000) {
		attackTime = a * samplerate / 1000;
	}
}

// Sets length of decay phase of envelope curve in milliseconds
void MainOscillator::setDecay(unsigned int d)
{
	if (d > 5 && d < 10000) {
		decayTime = d * samplerate / 1000;
	}

}

// Sets relative volume (0..1) of sustain phase of envelope curve
void MainOscillator::setSustain(float s)
{
	if (0 <= s && s <= 1) {
		sustainVolume = s;
	}
}

// Sets length of release phase of envelope curve in milliseconds
void MainOscillator::setRelease(unsigned int r)
{
	if (r > 5 && r < 10000) {
		releaseTime = r * samplerate / 1000;
	}
}

// Sets oscillator base frequency and maximum amplitude.
// Sets envelope curve to the beginning of attack phase.
void MainOscillator::noteOn(unsigned char noteKey, unsigned char noteVelocity)
{
	if (noteKey > 127 || noteVelocity == 0) {
		return;
	}

	lastSample = 0;
	setFrequency(baseFrequency[noteKey]);

	envelopePhaseTime = 0;

	if (noteVelocity > 127) {
		noteVelocity = 127;
	}
	peakAmplitude = (float)noteVelocity / 127;

	if (lfoFrequencyType == RELATIVE && dedicatedLfo != NULL) {
		dedicatedLfo->setRelativeFrequency(frequency);
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

	switch (waveform) {
	case SINE:
	case TRIANGLE:
	case ABS_SINE:
		synthesizeFromWavetable(outputBuffer, modulatorBuffer);
		break;
	case SAWTOOTH:
		synthesizeSawtooth(outputBuffer, modulatorBuffer);
		break;
	case PULSE:
		synthesizePulseWave(outputBuffer, modulatorBuffer);
		break;
	}

	if (modulation == AMPLITUDE) {
		applyAmplitudeModulation(outputBuffer, modulatorBuffer);
	}

	applyEnvelope(outputBuffer);

	noteFinished = (envelopePhase == OFF);
}

void MainOscillator::synthesizeFromWavetable(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (modulation == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i]
				= wavetable[(int)(angle * WAVE_TABLE_LENGTH + 0.5)];
			angle += anglePerSample * (1 + modulationAmount
				* modulatorBuffer[i]);
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i]
				= wavetable[(int) (angle * WAVE_TABLE_LENGTH + 0.5)];
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizeSawtooth(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (modulation == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += anglePerSample * (1 + modulationAmount
				* modulatorBuffer[i]);
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
	else {
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizePulseWave(float outputBuffer[],
const float modulatorBuffer[])
{
	unsigned int i;

	if (modulation == PULSE_WIDTH) {
		for (i = 0; i < bufferLength; i++) {
			if (angle < pulseWidth) {
				outputBuffer[i] = 1;
			}
			else {
				outputBuffer[i] = -1;
			}
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
				pulseWidth = 0.5 * (1 + modulationAmount * modulatorBuffer[i]);
			}
		}
	}
	else if (modulation == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
			if (angle < 0.5) {
				outputBuffer[i] = 1;
			}
			else {
				outputBuffer[i] = -1;
			}
			angle += anglePerSample * (1 + modulationAmount
				* modulatorBuffer[i]);
			if (angle > 1) {
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
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::applyAmplitudeModulation(float outputBuffer[],
const float modulatorBuffer[])
{
	float lfoAmount = modulationAmount * 0.5;
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
	unsigned int samplesLeft = releaseTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / releaseTime;
	float timeIncrease = 1.0 / releaseTime;
	float b = 1 / (expf(-steepness) - 1);
	float amplitude;

	if (previousEnvelopePhase != RELEASE) {
		envelopePhaseTime = 0;
		previousEnvelopePhase = RELEASE;
	}

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

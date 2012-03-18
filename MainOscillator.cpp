/*
 * MainOscillator.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "MainOscillator.h"

MainOscillator::MainOscillator()
{
	waveform = SINE;
	wavetable = sineTable;
	modulation = NONE;
	modulationAmount = 0;
	envelopePhase = OFF;
	previousEnvelopePhase = OFF;
	envelopeAmplitude = 0;
	attack = 100;
	decay = 100;
	sustain = 50;
	release = 500;
	key = 0;
	peakAmplitude = 0;
	phaseTime = 0;
	pulseWidth = 0.5;
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

// Sets LFO modulation target
void MainOscillator::setModulationTarget(LfoModulationTarget m)
{
	modulation = m;
}

void MainOscillator::setModulationAmount(float a)
{
	if (0 <= a && a <= 1) {
		modulationAmount = a;
	}
}

// Sets oscillator base frequency and maximum amplitude.
// Sets envelope curve to the beginning of attack phase.
void MainOscillator::noteOn(unsigned char noteKey, unsigned char noteVelocity,
	NoteSource source)
{
	if (noteKey > 127 || noteVelocity == 0) {
		return;
	}

	lastSample = 0;
	setFrequency(baseFrequency[noteKey]);

	key = noteKey;
	phaseTime = 0;

	if (noteVelocity > 127) {
		noteVelocity = 127;
	}
	peakAmplitude = (float)noteVelocity / 127;
}

// Sets envelope curve to the beginning of release phase.
void MainOscillator::noteOff()
{
	// TODO
}

// Generates a sound of an oscillator.
// outputBuffer = buffer for sound output
// modulatorBuffer = sound of a low frequency oscillator
// bufferLength = length of outbutBuffer and modulatorBuffer in samples
// noteFinished is changed to true if oscillator finished its envelope curve
// (phase RELEASE ended).
void MainOscillator::generateSound(float * outputBuffer,
	float * modulatorBuffer, unsigned int bufferLength, bool & noteFinished)
{
	switch (waveform) {
	case SINE:
	case TRIANGLE:
	case ABS_SINE:
		synthesizeFromWavetable(outputBuffer, modulatorBuffer, bufferLength);
		break;
	case SAWTOOTH:
		synthesizeSawtooth(outputBuffer, modulatorBuffer, bufferLength);
		break;
	case PULSE:
		synthesizePulseWave(outputBuffer, modulatorBuffer, bufferLength);
		break;
	}

	if (modulation == AMPLITUDE) {
		applyAmplitudeModulation(outputBuffer, modulatorBuffer,
			bufferLength);
	}

	// TODO: applyEnvelope. use peakAmplitude with it!

	noteFinished = (envelopePhase == OFF);
}

void MainOscillator::synthesizeFromWavetable(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	float * endPtr = outputBuffer + bufferLength;
	unsigned int index;
	float lfoValue;

	if (modulation == FREQUENCY) {
		while (outputBuffer < endPtr) {
			index = (unsigned int)(angle * WAVE_TABLE_LENGTH + 0.5);
			*outputBuffer++ = wavetable[index];
			lfoValue = *modulatorBuffer++;
			angle += anglePerSample * (1 + modulationAmount * lfoValue);
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
	else {
		while (outputBuffer < endPtr) {
			index = (unsigned int)(angle * WAVE_TABLE_LENGTH + 0.5);
			*outputBuffer++ = wavetable[index];
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizeSawtooth(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	float * endPtr = outputBuffer + bufferLength;
	float lfoValue;

	if (modulation == FREQUENCY) {
		while (outputBuffer < endPtr) {
			*outputBuffer++ = angle * 2 - 1;
			lfoValue = *modulatorBuffer++;
			angle += anglePerSample * (1 + modulationAmount * lfoValue);
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
	else {
		while (outputBuffer < endPtr) {
			*outputBuffer++ = angle * 2 - 1;
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::synthesizePulseWave(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	float * endPtr = outputBuffer + bufferLength;
	float lfoValue;

	if (modulation == PULSE_WIDTH) {
		float * startPtr = outputBuffer;
		unsigned int difference;
		while (outputBuffer < endPtr) {
			if (angle < pulseWidth) {
				*outputBuffer++ = 1;
			}
			else {
				*outputBuffer++ = -1;
			}
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
				difference = outputBuffer - startPtr;
				if (difference < bufferLength) {
					lfoValue = modulatorBuffer[outputBuffer - startPtr];
					pulseWidth = 0.5 * (1 + modulationAmount * lfoValue);
				}
			}
		}
	}
	else if (modulation == FREQUENCY) {
		while (outputBuffer < endPtr) {
			if (angle < 0.5) {
				*outputBuffer++ = 1;
			}
			else {
				*outputBuffer++ = -1;
			}
			lfoValue = *modulatorBuffer++;
			angle += anglePerSample * (1 + modulationAmount * lfoValue);
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
	else {
		while (outputBuffer < endPtr) {
			if (angle < 0.5) {
				*outputBuffer++ = 1;
			}
			else {
				*outputBuffer++ = -1;
			}
			angle += anglePerSample;
			if (angle > 1) {
				angle -= 1;
			}
		}
	}
}

void MainOscillator::applyAmplitudeModulation(float * outputBuffer,
float * modulatorBuffer, unsigned int bufferLength)
{
	float lfoAmount = modulationAmount * 0.5;
    float lfoMidpoint = 1 - lfoAmount;
    float * endPtr = outputBuffer + bufferLength;
    float * lfoPtr = modulatorBuffer;
    while (outputBuffer < endPtr) {
        *outputBuffer++ *= (lfoMidpoint + lfoAmount * (*lfoPtr++));
    }
}

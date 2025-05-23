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


// Envelope curve steepness
const float STEEPNESS = 4;

const unsigned int ENVELOPE_TABLE_LENGTH = 1024;
const unsigned int MUTE_LENGTH = 16;
const unsigned int RETRIGGER_LENGTH = 16;

MainOscillator::MainOscillator(OscillatorParameters & p) :
	globalParameters(p)
{
	wavetable = sineTable;

	envelopePhase = OFF;
	previousEnvelopePhase = OFF;
	envelopeAmplitude = 0;
    previousEnvelopeAmplitude = 0;
	attackTime = MinAttackTime;
	decayTime = MinDecayTime;
	sustainVolume = MaxSustainVolume;
	releaseTime = MinReleaseTime;
	peakAmplitude = 0;
	envelopePhaseTime = 0;
	pulseWidth = 0.5;
	lastSampleValue = 0;
}

MainOscillator::~MainOscillator() {
}

// Sets oscillator base frequency and maximum amplitude.
// Sets envelope curve to the beginning of attack phase.
void MainOscillator::noteOn(unsigned char noteKey, unsigned char noteVelocity,
    bool retrigger)
{
	if (noteKey > 127 || noteVelocity == 0) {
		return;
	}

	attackTime = globalParameters.attackTime * 0.001 * samplerate;
	decayTime = globalParameters.decayTime * 0.001 * samplerate;
	sustainVolume = globalParameters.sustainVolume;
	releaseTime = globalParameters.releaseTime * 0.001 * samplerate;

	setFrequency(baseFrequency[noteKey]);
    if (retrigger == true) {
        envelopePhase = RETRIGGER;
    }
    else {
        envelopePhase = ATTACK;
        lastSampleValue = 0;
        angle = 0;
    }

	envelopePhaseTime = 0;

	if (noteVelocity > 127) {
		noteVelocity = 127;
	}
	peakAmplitude = (float)noteVelocity / 127;
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
// rangeStart - index of first sample in the buffer to generate
// rangeEnd - index of last sample + 1
// noteFinished is changed to true if oscillator finished its envelope curve
// (phase RELEASE ended).
void MainOscillator::generateSound(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd, bool & noteFinished)
{
	if (envelopePhase == FAST_MUTE) {
		applyFastMute(outputBuffer);
		noteFinished = true;
		return;
	}

	switch (globalParameters.waveform) {
	case SINE:
		wavetable = Oscillator::sineTable;
		synthesizeFromWavetable(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
		break;

	case TRIANGLE:
		synthesizeTriangleWave(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
		break;

	case ABS_SINE:
		wavetable = Oscillator::absSineTable;
		synthesizeFromWavetable(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
		break;

	case SAWTOOTH:
		synthesizeSawtoothWave(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
		break;

	case PULSE:
		synthesizePulseWave(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
		break;
	}

	if (globalParameters.lfoModulationTarget == AMPLITUDE) {
		applyAmplitudeModulation(outputBuffer, modulatorBuffer, rangeStart,
			rangeEnd);
	}

	applyEnvelope(outputBuffer, rangeStart, rangeEnd);

	noteFinished = (envelopePhase == OFF);
}

void MainOscillator::setFrequency(float f)
{
	((Oscillator *)this)->setFrequency(f);
	triangleAnglePerSample = anglePerSample;
	sawtoothAnglePerSample = anglePerSample;
}

// Synthesizes oscillation. Waveform is sine wave or rectified sine wave,
// waveform source is wavetable.
// outputBuffer - sample buffer for sound output
// modulatorBuffer - sample buffer for modulating signal (input)
// rangeStart - index of first sample in outputBuffer and modulatorBuffer
//              to process
// rangeEnd   - index of last sample in outputBuffer and modulatorBuffer
//              to process + 1
void MainOscillator::synthesizeFromWavetable(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd)
{
#if USE_OPTIMIZATIONS
	float * endPtr = outputBuffer + rangeEnd;
	outputBuffer += rangeStart;
	float tableLength = WAVE_TABLE_LENGTH;
	float wavetableIndex = angle * tableLength;
	float indexIncrease = anglePerSample * tableLength;

	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		float modAmount = globalParameters.lfoModulationAmount;
		while (outputBuffer < endPtr) {
			if (wavetableIndex >= tableLength) {
				wavetableIndex -= tableLength;
			}

			*outputBuffer++ = wavetable[(int)wavetableIndex];
			wavetableIndex += indexIncrease *
					          (1 + modAmount * (*modulatorBuffer++));
		}
		angle = wavetableIndex / tableLength;
	}
	else {
		while (outputBuffer < endPtr) {
			if (wavetableIndex >= tableLength) {
				wavetableIndex -= tableLength;
			}
			*outputBuffer = wavetable[(int)wavetableIndex];
			outputBuffer++;
			wavetableIndex += indexIncrease;
		}
		angle = wavetableIndex / tableLength;
	}
#else

	unsigned int i;
	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = rangeStart; i < rangeEnd; i++) {
			outputBuffer[i] = wavetable[(int)(angle * WAVE_TABLE_LENGTH)];
			angle += anglePerSample * (1 + globalParameters.lfoModulationAmount
				* modulatorBuffer[i]);
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}
	else {
		unsigned int index;
		for (i = rangeStart; i < rangeEnd; i++) {
			index = angle * WAVE_TABLE_LENGTH;
			outputBuffer[i] = wavetable[(int)index];
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
	}

#endif // USE_OPTIMIZATIONS
}

// Synthesizes triangle wave.
// outputBuffer - sample buffer for sound output
// modulatorBuffer - sample buffer for modulating signal (input)
// rangeStart - index of first sample in outputBuffer and modulatorBuffer
//              to process
// rangeEnd   - index of last sample in outputBuffer and modulatorBuffer
//              to process + 1
void MainOscillator::synthesizeTriangleWave(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd)
{
	unsigned int i;
	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = rangeStart; i < rangeEnd; i++) {
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
		for (i = rangeStart; i < rangeEnd; i++) {
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

// Synthesizes sawtooth wave.
// outputBuffer - sample buffer for sound output
// modulatorBuffer - sample buffer for modulating signal (input)
// rangeStart - index of first sample in outputBuffer and modulatorBuffer
//              to process
// rangeEnd   - index of last sample in outputBuffer and modulatorBuffer
//              to process + 1
void MainOscillator::synthesizeSawtoothWave(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd)
{
	unsigned int i;

	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = rangeStart; i < rangeEnd; i++) {
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
#if USE_OPTIMIZATIONS
		float * endPtr = outputBuffer + rangeEnd;
		outputBuffer += rangeStart;
		float sawtoothAngle = angle * 2 - 1;
		float sawtoothIncrease = anglePerSample * 2;
		while (outputBuffer < endPtr) {
			*outputBuffer++ = sawtoothAngle;
			sawtoothAngle += sawtoothIncrease;
			if (sawtoothAngle >= 1) {
				sawtoothAngle -= 2;
			}
		}
		angle = (sawtoothAngle + 1) * 0.5;
#else
		for (i = rangeStart; i < rangeEnd; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
#endif // USE_OPTIMIZATIONS
	}

}

// Synthesizes square/pulse wave.
// outputBuffer - sample buffer for sound output
// modulatorBuffer - sample buffer for modulating signal (input)
// rangeStart - index of first sample in outputBuffer and modulatorBuffer
//              to process
// rangeEnd   - index of last sample in outputBuffer and modulatorBuffer
//              to process + 1
void MainOscillator::synthesizePulseWave(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd)
{
	unsigned int i;

	if (globalParameters.lfoModulationTarget == PULSE_WIDTH) {
		for (i = rangeStart; i < rangeEnd; i++) {
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
		for (i = rangeStart; i < rangeEnd; i++) {
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
		for (i = rangeStart; i < rangeEnd; i++) {
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

// Applies amplitude modulation to synthesized sound.
// outputBuffer - sample buffer for sound output
// modulatorBuffer - sample buffer for modulating signal (input)
// rangeStart - index of first sample in outputBuffer and modulatorBuffer
//              to process
// rangeEnd   - index of last sample in outputBuffer and modulatorBuffer
//              to process + 1
void MainOscillator::applyAmplitudeModulation(float outputBuffer[],
const float modulatorBuffer[], unsigned int rangeStart,
unsigned int rangeEnd)
{
	float lfoAmount = globalParameters.lfoModulationAmount * 0.5;
    float lfoMidpoint = 1 - lfoAmount;

    for (unsigned int i = rangeStart; i < rangeEnd; i++) {
       outputBuffer[i] *= lfoMidpoint + lfoAmount * modulatorBuffer[i];
    }
}

// Applies amplitude envelope curve to synthesized sound.
// outputBuffer - sample buffer for sound input/output
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
void MainOscillator::applyEnvelope(float outputBuffer[],
unsigned int rangeStart, unsigned int rangeEnd)
{
	unsigned int i = rangeStart;

    while (i < rangeEnd) {
		switch (envelopePhase) {

		case ATTACK:
			i = applyAttack(outputBuffer, i, rangeEnd);
			break;

		case DECAY:
			i = applyDecay(outputBuffer, i, rangeEnd);
			break;

		case SUSTAIN:
			i = applySustain(outputBuffer, i, rangeEnd);
			break;

		case RELEASE:
			i = applyRelease(outputBuffer, i, rangeEnd);
			break;

        case RETRIGGER:
            i = applyRetrigger(outputBuffer, i, rangeEnd);
            break;

		case FAST_MUTE:
			// This is done in generateSound().
			break;

		case OFF:
			for (; i < bufferLength; i++) {
				outputBuffer[i] = 0;
			}
			break;

		}
	}
	lastSampleValue = outputBuffer[rangeEnd - 1];
}

// Applies envelope phase ATTACK to samples of outputBuffer.
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
// When the envelope phase ends, less than rangeEnd - rangeStart samples
// may have been processed.
//     Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyAttack(float outputBuffer[],
	unsigned int rangeStart, unsigned int rangeEnd)
{
	// Optimized version.
	// See doc/expcurves.txt for explanation of the optimization.
	unsigned int samplesLeftInPhase = attackTime - envelopePhaseTime;
	unsigned int rangeLength = rangeEnd - rangeStart;
	float t = -STEEPNESS * envelopePhaseTime / attackTime;
	float tIncrease = -STEEPNESS / (attackTime - 1);
	float b = peakAmplitude / (1 - expf(-STEEPNESS));

	unsigned int i = rangeStart;
	if (samplesLeftInPhase > rangeLength) {
		for (; i < rangeEnd; i++) {
			outputBuffer[i] *= b * (1 - expf(t));
			t += tIncrease;
		}
		envelopePhaseTime += rangeLength;
	}
	else {
		unsigned int endI = rangeStart + samplesLeftInPhase;
		for (; i < endI; i++) {
			outputBuffer[i] *= b * (1 - expf(t));
			t += tIncrease;
		}
		envelopePhase = DECAY;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = b * (1 - expf(t - tIncrease));
	previousEnvelopePhase = ATTACK;
	return i;
}

// Retriggers the note in means of envelope curve. The retrigger curve is
// smooth transition from last envelope curve value to zero in
// RETRIGGER_LENGTH samples. Then the envelope is switched to the beginning of
// ATTACK phase.
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
// When the envelope phase ends, less than rangeEnd - rangeStart samples
// may have been processed.
//     Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyRetrigger(float outputBuffer[],
	unsigned int rangeStart, unsigned int rangeEnd)
{
	unsigned int samplesLeftInPhase = RETRIGGER_LENGTH - envelopePhaseTime;
	unsigned int rangeLength = rangeEnd - rangeStart;
	unsigned int i;
	float angle = (0.5 + (float)envelopePhaseTime / RETRIGGER_LENGTH) * M_PI;
	float angleIncrease = M_PI / RETRIGGER_LENGTH;

	if (samplesLeftInPhase > rangeLength) {
		for (i = rangeStart; i < rangeEnd; i++) {
			outputBuffer[i] *= envelopeAmplitude * (0.5 * sinf(angle) + 0.5);
			angle += angleIncrease;
		}
		envelopePhaseTime += rangeLength;
	}
	else {
		unsigned int endI = rangeStart + samplesLeftInPhase;
		for (i = rangeStart; i < endI; i++) {
			outputBuffer[i] *= envelopeAmplitude * (0.5 * sinf(angle) + 0.5);
			angle += angleIncrease;
		}
		envelopePhase = ATTACK;
		envelopePhaseTime = 0;
	}
	previousEnvelopePhase = RETRIGGER;
	return i;
}

// Applies envelope phase DECAY to samples of outputBuffer.
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
// When the envelope phase ends, less than rangeEnd - rangeStart samples
// may have been processed.
//     Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyDecay(float outputBuffer[],
	unsigned int rangeStart, unsigned int rangeEnd)
{
	unsigned int samplesLeftInPhase = decayTime - envelopePhaseTime;
	unsigned int rangeLength = rangeEnd - rangeStart;

	// See doc/expcurves.txt for explanation of the optimizations.
	float t = -STEEPNESS * envelopePhaseTime / decayTime;
	float tIncrease = -STEEPNESS / (decayTime - 1);
	float c = (1 - sustainVolume) / (1 - expf(-STEEPNESS));
	float d = 1 - c;
	c *= peakAmplitude;
	d *= peakAmplitude;

	unsigned int i = rangeStart;
	if (samplesLeftInPhase > rangeLength) {
		for (; i < bufferLength; i++) {
			outputBuffer[i] *= d + c * expf(t);
			t += tIncrease;
		}
		envelopePhaseTime += rangeLength;
	}
	else {
		unsigned int endI = i + samplesLeftInPhase;
		for (; i < endI; i++) {
			outputBuffer[i] *= d + c * expf(t);
			t += tIncrease;
		}
		envelopePhase = SUSTAIN;
		previousEnvelopePhase = ATTACK;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = d + c * expf(t - tIncrease);
	previousEnvelopePhase = DECAY;
	return i;
}

// Applies envelope phase SUSTAIN to samples of outputBuffer.
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
// Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applySustain(float outputBuffer[],
	unsigned int rangeStart, unsigned int rangeEnd)
{
	float c = sustainVolume * peakAmplitude;
	unsigned int i;
	for (i = rangeStart; i < rangeEnd; i++) {
		outputBuffer[i] *= c;
	}
    envelopeAmplitude = c;
    return i;
}

// Applies envelope phase RELEASE to samples of outputBuffer.
// rangeStart - index of first sample in outputBuffer to process
// rangeEnd   - index of last sample in outputBuffer to process + 1
// When the envelope phase ends, less than rangeEnd - rangeStart samples
// may have been processed.
//     Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyRelease(float outputBuffer[],
	unsigned int rangeStart, unsigned int rangeEnd)
{
	if (previousEnvelopePhase != RELEASE) {
        previousEnvelopeAmplitude = envelopeAmplitude;
		envelopePhaseTime = 0;
		previousEnvelopePhase = RELEASE;
	}

	unsigned int samplesLeftInPhase = releaseTime - envelopePhaseTime;
	unsigned int rangeLength = rangeEnd - rangeStart;
	float t = -STEEPNESS * envelopePhaseTime / releaseTime;
	float tIncrease = -STEEPNESS / (releaseTime - 1);
	float b = 1 / (1 - expf(-STEEPNESS));
	float c = (1 - b) * previousEnvelopeAmplitude;
	float d = b * previousEnvelopeAmplitude;

	unsigned int i = rangeStart;
	if (samplesLeftInPhase > rangeLength) {
		envelopePhaseTime += rangeLength;
		for (; i < rangeEnd; i++) {
			outputBuffer[i] *= c + d * expf(t);
			t += tIncrease;
		}
	}
	else {
		unsigned int endI = rangeStart + samplesLeftInPhase;
		for (; i < endI; i++) {
			outputBuffer[i] *= c + d * expf(t);
			t += tIncrease;
		}
		envelopePhase = OFF;
		envelopePhaseTime = 0;
	}
    envelopeAmplitude = c + d * expf(t - tIncrease);
	return i;
}

// Does a "fast mute": does an interpolation from last sample value to zero
// in MUTE_LENGTH samples. The remaining sample values are replaced with
// zeroes.
void MainOscillator::applyFastMute(float outputBuffer[])
{
	unsigned int i;
	unsigned int turnOffLength = MUTE_LENGTH;
	if (turnOffLength > bufferLength) {
		turnOffLength = bufferLength;
	}

	float sampleValue = lastSampleValue;
	float valueDecrease = sampleValue / turnOffLength;

	for (i = 0; i < turnOffLength; i++) {
		outputBuffer[i] = sampleValue;
		sampleValue -= valueDecrease;
	}
	memset(outputBuffer + turnOffLength, 0,
			(bufferLength - turnOffLength) * sizeof(float));
	envelopePhase = OFF;
	previousEnvelopePhase = OFF;
}

EnvelopePhase MainOscillator::getEnvelopePhase() const
{
	return envelopePhase;
}

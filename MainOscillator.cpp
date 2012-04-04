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

MainOscillator::MainOscillator(OscillatorParameters & p) :
	globalParameters(p)
{
	wavetable = sineTable;
	dedicatedLfo = NULL;

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
        lastSample = 0;
        angle = 0;
    }

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
#if USE_OPTIMIZATIONS
	float * endPtr = outputBuffer + bufferLength;
	float tableLength = WAVE_TABLE_LENGTH;
	float index = angle * tableLength;
	float indexIncrease = anglePerSample * tableLength;

	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		float modAmount = globalParameters.lfoModulationAmount;
		while (outputBuffer < endPtr) {
			if (index >= tableLength) {
				index -= tableLength;
			}

			*outputBuffer++ = wavetable[(int)index];
			index += indexIncrease * (1 + modAmount * (*modulatorBuffer++));
		}
		angle = index / tableLength;
	}
	else {
		while (outputBuffer < endPtr) {
			if (index >= tableLength) {
				index -= tableLength;
			}
			*outputBuffer = wavetable[(int)index];
			outputBuffer++;
			index += indexIncrease;
		}
		angle = index / tableLength;
	}
#else

	unsigned int i;
	if (globalParameters.lfoModulationTarget == FREQUENCY) {
		for (i = 0; i < bufferLength; i++) {
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
		for (i = 0; i < bufferLength; i++) {
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
#if USE_OPTIMIZATIONS
		float * endPtr = outputBuffer + bufferLength;
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
		for (i = 0; i < bufferLength; i++) {
			outputBuffer[i] = angle * 2 - 1;
			angle += anglePerSample;
			if (angle >= 1) {
				angle -= 1;
			}
		}
#endif // USE_OPTIMIZATIONS
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

// Applies amplitude envelope curve to all sound samples in outputBuffer.
void MainOscillator::applyEnvelope(float outputBuffer[])
{
	unsigned int i = 0;
    float c;

    while (i < bufferLength) {
		switch (envelopePhase) {

		case ATTACK:
			i = applyAttack(outputBuffer, i);
			break;

		case DECAY:
			i = applyDecay(outputBuffer, i);
			break;

		case SUSTAIN:
            c = sustainVolume;
			for (; i < bufferLength; i++) {
				outputBuffer[i] *= c;
			}
            envelopeAmplitude = c;
			break;

		case RELEASE:
			i = applyRelease(outputBuffer, i);
			break;

        case RETRIGGER:
            i = applyRetrigger(outputBuffer);
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
	lastSample = outputBuffer[bufferLength - 1];
}

// Applies envelope phase ATTACK to samples of outputBuffer.
// i = index of first sample in outputBuffer where the ATTACK curve is
//     applied to.
// Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyAttack(float outputBuffer[], unsigned int i)
{
	unsigned int samplesLeft = attackTime - envelopePhaseTime;
	float t = -STEEPNESS * envelopePhaseTime / attackTime;
	float tIncrease = -STEEPNESS / (attackTime - 1);
	float k = 1 / (1 - expf(-STEEPNESS));

	if (samplesLeft > bufferLength) {
		envelopePhaseTime += bufferLength - i;
		for (; i < bufferLength; i++) {
			outputBuffer[i] *= k * (1 - expf(t));
			t += tIncrease;
		}
	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
			outputBuffer[i] *= k * (1 - expf(t));
			t += tIncrease;
		}
		envelopePhase = DECAY;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = k * (1 - expf(t - tIncrease));
	previousEnvelopePhase = ATTACK;

// Optimized version
//	if (samplesLeft > bufferLength) {
//		envelopePhaseTime += bufferLength - i;
//		for (; i < bufferLength; i++) {
//			amplitude = k * (1 - expf(t));
//			outputBuffer[i] *= amplitude;
//			t += tIncrease;
//		}
//
//	}
//	else {
//		unsigned int endI = i + samplesLeft;
//		for (; i < endI; i++) {
//			amplitude = k * (1 - expf(t));
//			outputBuffer[i] *= amplitude;
//			t += tIncrease;
//		}
//		envelopePhase = DECAY;
//		envelopePhaseTime = 0;
//	}
//	envelopeAmplitude = amplitude;
//	previousEnvelopePhase = ATTACK;


//  // Original version
//	unsigned int samplesLeft = attackTime - envelopePhaseTime;
//	float relativeTime = (float)envelopePhaseTime / attackTime;
//	float timeIncrease = 1.0 / (attackTime - 1);
//	float b = 1 / (1 - expf(-STEEPNESS));
//	float amplitude = 0;
//
//	if (samplesLeft > bufferLength) {
//		envelopePhaseTime += bufferLength - i;
//		for (; i < bufferLength; i++) {
//			amplitude = b * (1 - expf(-STEEPNESS * relativeTime));
//			outputBuffer[i] *= amplitude;
//			relativeTime += timeIncrease;
//		}
//
//	}
//	else {
//		unsigned int endI = i + samplesLeft;
//		for (; i < endI; i++) {
//			amplitude = b * (1 - expf(-STEEPNESS * relativeTime));
//			outputBuffer[i] *= amplitude;
//			relativeTime += timeIncrease;
//		}
//		envelopePhase = DECAY;
//		envelopePhaseTime = 0;
//	}
//	envelopeAmplitude = amplitude;
//	previousEnvelopePhase = ATTACK;

	return i;
}

// Retriggers the note in means of envelope curve. The retrigger curve is
// linear transition from last envelope curve value to zero in
// RETRIGGER_LENGTH samples. Then the envelope is switched to the beginning of
// ATTACK phase.
// Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyRetrigger(float outputBuffer[])
{
	unsigned int i;
	float angle = 0.5 * M_PI;
	float angleIncrease = M_PI / RETRIGGER_LENGTH;
	for (i = 0; i < RETRIGGER_LENGTH; i++) {
		outputBuffer[i] *= envelopeAmplitude * (0.5 * sinf(angle) + 0.5);
		angle += angleIncrease;
	}
	envelopePhase = ATTACK;
	previousEnvelopePhase = RETRIGGER;
	envelopePhaseTime = 0;
	return RETRIGGER_LENGTH;


}

// Applies envelope phase DECAY to samples of outputBuffer.
// i = index of first sample in outputBuffer where the DECAY curve is
//     applied to.
// Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyDecay(float outputBuffer[], unsigned int i)
{
	unsigned int samplesLeft = decayTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / decayTime;
	float timeIncrease = 1.0 / (decayTime - 1);
	float b = 1 / (expf(-STEEPNESS) - 1);
	float amplitude = 0;

	if (samplesLeft > bufferLength) {
		envelopePhaseTime += bufferLength - i;
		for (; i < bufferLength; i++) {
			amplitude = 1 - (1 - sustainVolume) * b
				* (expf(-STEEPNESS * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}

	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
			amplitude = 1 - (1 - sustainVolume) * b
				* (expf(-STEEPNESS * relativeTime) - 1);
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhase = SUSTAIN;
		previousEnvelopePhase = ATTACK;
		envelopePhaseTime = 0;
	}
	envelopeAmplitude = amplitude;
	previousEnvelopePhase = DECAY;
	return i;
}

// Applies envelope phase RELEASE to samples of outputBuffer.
// i = index of first sample in outputBuffer where the RELEASE curve is
//     applied to.
// Returns index of next sample to be processed in outputBuffer.
unsigned int MainOscillator::applyRelease(float outputBuffer[], unsigned int i)
{
	if (previousEnvelopePhase != RELEASE) {
        previousEnvelopeAmplitude = envelopeAmplitude;
		envelopePhaseTime = 0;
		previousEnvelopePhase = RELEASE;
	}

	unsigned int samplesLeft = releaseTime - envelopePhaseTime;
	float relativeTime = (float)envelopePhaseTime / releaseTime;
	float timeIncrease = 1.0 / releaseTime;
	float b = 1 / (expf(-STEEPNESS) - 1);
	float amplitude = 0;

	if (samplesLeft > bufferLength) {
		envelopePhaseTime += bufferLength - i;
		for (; i < bufferLength; i++) {
			amplitude = previousEnvelopeAmplitude *
				(1 - b * (expf(-STEEPNESS * relativeTime) - 1));
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
	}
	else {
		unsigned int endI = i + samplesLeft;
		for (; i < endI; i++) {
            amplitude = previousEnvelopeAmplitude *
                (1 - b * (expf(-STEEPNESS * relativeTime) - 1));
			outputBuffer[i] *= amplitude;
			relativeTime += timeIncrease;
		}
		envelopePhase = OFF;
		envelopePhaseTime = 0;
	}

    envelopeAmplitude = amplitude;

	return i;
}

// Does a "fast mute": does an interpolation from last sample value to zero
// in MUTE_LENGTH samples. The remaining sample values are replaced with
// zeroes.
// Returns index of next sample to be processed in outputBuffer.
void MainOscillator::applyFastMute(float outputBuffer[])
{
	unsigned int i;
	unsigned int turnOffLength = MUTE_LENGTH;
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
	previousEnvelopePhase = OFF;
}

EnvelopePhase MainOscillator::getEnvelopePhase() const
{
	return envelopePhase;
}

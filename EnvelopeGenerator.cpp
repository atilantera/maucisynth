#include "EnvelopeGenerator.h"

EnvelopeGenerator::EnvelopeGenerator(int sampleRate, int bufferLength) {
	this->sampleRate = sampleRate;
	this->bufferLength = bufferLength;
    this->envelopePhase = OFF;
    this->envelopeChangeCount = 0;    
}

void EnvelopeGenerator::generateEnvelope(float * outputBuffer) {
    if (this->envelopePhase == OFF) {
        for (int i = 0; i < this->bufferLength; i++) {
            outputBuffer[i] = 0;
        }
    }
}

void EnvelopeGenerator::addEnvelopeChange(int time, EnvelopePhase phase) {
    if (time < 0 || time >= this->bufferLength) {
        return;
    }

}

EnvelopePhase EnvelopeGenerator::getPhase() const {
    return this->envelopePhase;
}

/**
 * Sets ADSR attack phase length in samples
 */
void EnvelopeGenerator::setAttackTime(int samples) {
    if (samples >= 0) {
        this->attackTime = samples;
    }
}

/**
 * Sets ADSR decay phase length in samples
 */
void EnvelopeGenerator::setDecayTime(int samples) {
    if (samples >= 0) {
        this->decayTime = samples;
    }
}

/**
 * Sets ADSR sustain level
 */
void EnvelopeGenerator::setSustainLevel(float level) {
    if (level >= 0) {
        this->sustainLevel = level;
    }
}

/**
 * Sets ADSR release phase length in samples
 */
void EnvelopeGenerator::setReleaseTime(int samples) {
    if (samples >= 0) {
        this->releaseTime = samples;
    }
}

/**
 * Renders attack phase of envelope curve to *outputBuffer.
 * Assumes the first index to be rendered is at outputBuffer[0].
 * Renders _samples_number of samples.
*/
void EnvelopeGenerator::renderAttack(float * outputBuffer) {
}

/**
 * Renders attack phase of envelope curve to *outputBuffer.
 * Assumes the first index to be rendered is at outputBuffer[0].
 * Renders _samples_number of samples.
*/
void EnvelopeGenerator::renderDecay(float * outputBuffer) {
}

/**
 * Renders attack phase of envelope curve to *outputBuffer.
 * Assumes the first index to be rendered is at outputBuffer[0].
 * Renders _samples_number of samples.
*/
void EnvelopeGenerator::renderSustain(float * outputBuffer) {
}

/**
 * Renders attack phase of envelope curve to *outputBuffer.
 * Assumes the first index to be rendered is at outputBuffer[0].
 * Renders _samples_number of samples.
*/
void EnvelopeGenerator::renderRelease(float * outputBuffer) {
}




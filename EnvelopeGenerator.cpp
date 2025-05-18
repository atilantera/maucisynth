#include "EnvelopeGenerator.h"

EnvelopeGenerator::EnvelopeGenerator(unsigned int sampleRate,
		unsigned int bufferLength) {
	this->sampleRate = sampleRate;
	this->bufferLength = bufferLength;
    this->envelopePhase = OFF;
}

void EnvelopeGenerator::generateEnvelope(float * outputBuffer) {
    if (this->envelopePhase == OFF) {
        for (int i = 0; i < this->bufferLength; i++) {
            outputBuffer[i] = 0;
        }
    }
}

void addEnvelopeChange(unsigned int time, EnvelopePhase phase) {

}

EnvelopePhase EnvelopeGenerator::getPhase() const {
    return this->envelopePhase;
}


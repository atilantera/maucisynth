#include "EnvelopeGenerator.h"

EnvelopeGenerator::EnvelopeGenerator(unsigned int sampleRate,
		unsigned int bufferLength) {
	this->sampleRate = sampleRate;
	this->bufferLength = bufferLength;
}

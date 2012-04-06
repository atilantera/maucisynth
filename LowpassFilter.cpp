/*
 * LowpassFilter.cpp
 *
 *  Created on: 26.3.2012
 *      Author: alkim
 */

#include "LowpassFilter.h"

#ifndef NULL
#define NULL 0
#endif

const unsigned int FILTER_BUFFER_LENGTH = 4096;

LowpassFilter::LowpassFilter(float & frequency):
	parameterFrequency(frequency)
{
	filterBuffer = new float[FILTER_BUFFER_LENGTH];
	if (filterBuffer != NULL) {
		memset(filterBuffer, 0, FILTER_BUFFER_LENGTH * sizeof(float));
	}
	lastSampleIndex = 0;
	filterSum = 0;
	samplerate = 44100;
	updateFrequency();
}

LowpassFilter::~LowpassFilter() {
	delete [] filterBuffer;
}

void LowpassFilter::setSamplerate(unsigned int samplesPerSecond)
{
	if (samplesPerSecond > 0) {
		samplerate = samplesPerSecond;
		updateFrequency();
	}
}

// Performs a lowpass filter that is average of last windowLength samples.
void LowpassFilter::doFiltering(float * soundBuffer,
unsigned int bufferLength)
{
	if (ownFrequency != parameterFrequency) {
		updateFrequency();
	}

	unsigned int i;
	float onePerN = 1.0 / windowLength;
	for (i = 0; i < bufferLength; i++) {
		filterSum -= filterBuffer[lastSampleIndex];
		filterBuffer[lastSampleIndex] = soundBuffer[i];
		filterSum += filterBuffer[lastSampleIndex];
		lastSampleIndex++;
		if (lastSampleIndex == windowLength) {
			lastSampleIndex = 0;
		}
		soundBuffer[i] = filterSum * onePerN;
	}
}

void LowpassFilter::updateFrequency()
{
	ownFrequency = parameterFrequency;
	windowLength = samplerate / ownFrequency;
	if (windowLength > FILTER_BUFFER_LENGTH) {
		windowLength = FILTER_BUFFER_LENGTH;
	}
	if (windowLength < 2) {
		windowLength = 2;
	}
	if (filterBuffer != NULL) {
		memset(filterBuffer, 0, FILTER_BUFFER_LENGTH * sizeof(float));
	}
	lastSampleIndex = 0;
	filterSum = 0;
}

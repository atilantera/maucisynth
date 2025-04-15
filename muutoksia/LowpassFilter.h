/*
 * LowpassFilter.h
 *
 * Provides a simple lowpass filter.
 *
 *  Created on: 26.3.2012
 *      Author: alkim
 */

#ifndef LOWPASSFILTER_H_
#define LOWPASSFILTER_H_

#include <cstring>

class LowpassFilter {
public:
	LowpassFilter(float & frequency);
	virtual ~LowpassFilter();

	void setSamplerate(unsigned int samplesPerSecond);
	void doFiltering(float * soundBuffer, unsigned int bufferLength);

private:
	float & parameterFrequency;
	float ownFrequency;

	float * filterBuffer;
	unsigned int windowLength;
	unsigned int lastSampleIndex;
	float filterSum;
	unsigned int samplerate;

	void updateFrequency();
};

#endif /* LOWPASSFILTER_H_ */

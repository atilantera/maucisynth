/*
 * SoundBuffer.cpp
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#include "SoundBuffer.h"

SoundBuffer::SoundBuffer(int lengthInSamples) {
	if (lengthInSamples > 0) {
		samples = new float[lengthInSamples];
		endPointer = samples + lengthInSamples;
	}
	else {
		samples = 0;
		endPointer = 0;
	}
}

SoundBuffer::~SoundBuffer() {
	if (samples != 0) {
		delete [] samples;
	}
}

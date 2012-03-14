/*
 * SoundBuffer.h
 *
 *  Created on: 14.3.2012
 *      Author: alkim
 */

#ifndef SOUNDBUFFER_H_
#define SOUNDBUFFER_H_

class SoundBuffer {
public:
	SoundBuffer(int lengthInSamples);
	~SoundBuffer();

	float * samples;
	float * endPointer;
};

#endif /* SOUNDBUFFER_H_ */

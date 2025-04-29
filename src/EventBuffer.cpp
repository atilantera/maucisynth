/*
 * EventBuffer.cpp
 *
 *  Created on: 15.2.2012
 *      Author: alkim
 */

#include "EventBuffer.h"

EventBuffer::EventBuffer() {
	bufferLock = PTHREAD_MUTEX_INITIALIZER;
	bufferLength = 256;
	buffer1 = new unsigned char[bufferLength];
	buffer2 = new unsigned char[bufferLength];
	receivingBuffer = buffer1;
	bufferUsed = 0;
}

EventBuffer::~EventBuffer()
{
	delete [] buffer1;
	delete [] buffer2;
}

// Adds "Note on" message to buffer.
// key = MIDI key number (0..127)
// velocity = MIDI note velocity (0..127)
// source: see enum NoteSource in Synthesizer.h
void EventBuffer::addNoteOn(unsigned char key, unsigned char velocity,
		NoteSource source) {
	if (velocity > maxVelocity) {
		velocity = maxVelocity;
	}

	pthread_mutex_lock(&bufferLock);

#ifdef GUI_TESTING
	std::cout << "EventBuffer::addNoteOn: " << (int)key << " " <<
	(int)velocity << " bufferUsed=" << bufferUsed << std::endl;
#endif

	if (bufferUsed + 4 <= bufferLength) {
		unsigned char * ptr = receivingBuffer + bufferUsed;
		*ptr++ = 0x01;
		*ptr++ = key;
		*ptr++ = velocity;
		*ptr++ = source;
		bufferUsed += 4;
	}
	pthread_mutex_unlock(&bufferLock);
}

// Adds "Note off" message to buffer.
// key = MIDI key number (0..127
// source: see enum NoteSource in Synthesizer.h
void EventBuffer::addNoteOff(unsigned char key, NoteSource source)
{
	pthread_mutex_lock(&bufferLock);

#ifdef GUI_TESTING
	std::cout << "EventBuffer::addNoteOff: " << (int)key << " bufferUsed=" <<
		bufferUsed << std::endl;
#endif

	if (bufferUsed + 3 <= bufferLength) {
		unsigned char * ptr = receivingBuffer + bufferUsed;
		*ptr++ = 0x02;
		*ptr++ = key;
		*ptr++ = source;
		bufferUsed += 3;
	}
	pthread_mutex_unlock(&bufferLock);
}

// Adds "All notes off" message to buffer.
// source: see enum NoteSource in Synthesizer.h
// NOTE: This message overrides all other messages even if the buffer
// is full.
void EventBuffer::addAllNotesOff(NoteSource source)
{
	pthread_mutex_lock(&bufferLock);

#ifdef GUI_TESTING
	std::cout << "EventBuffer::addAllNotesOff" << " bufferUsed=" <<
			bufferUsed << std::endl;
#endif

	if (bufferUsed + 2 <= bufferLength) {
		unsigned char * ptr = receivingBuffer + bufferUsed;
		*ptr++ = 0x03;
		*ptr++ = source;
		bufferUsed += 2;
	}
	else {
		receivingBuffer[0] = 0x03;
		receivingBuffer[1] = source;
		bufferUsed = 2;
	}
	pthread_mutex_unlock(&bufferLock);
}

// Adds "Parameter change" message to buffer.
// controller: see values in enum controllerType in Synthesizer.h
// value: depends on controller type.
void EventBuffer::addParameterChange(unsigned char controller,
unsigned short value) {
	pthread_mutex_lock(&bufferLock);

#ifdef GUI_TESTING
	std::cout << "EventBuffer::addParameterChange: controller=" <<
		(int)controller << " value=" << value << " bufferUsed=" <<
		bufferUsed << std::endl;
#endif

	if (bufferUsed + 4 <= bufferLength) {
		unsigned char * ptr = receivingBuffer + bufferUsed;
		*ptr++ = 0x04;
		*ptr++ = controller;
		*ptr++ = (value & 0xFF00) >> 8;
		*ptr++ = (value & 0x00FF);
		bufferUsed += 4;
	}
	pthread_mutex_unlock(&bufferLock);
}

// Swaps roles of receiving and interpreted buffers.
// returns: pointer to buffer that should be interpreted
// parameter bufferUsed returns length of data in the buffer
unsigned char * EventBuffer::swapBuffers(unsigned int * dataLength) {
	unsigned char * nextProcessedBuffer;
	pthread_mutex_lock(&bufferLock);
	*dataLength = bufferUsed;
	bufferUsed = 0;
	nextProcessedBuffer = receivingBuffer;
	if (receivingBuffer == buffer1) {
		receivingBuffer = buffer2;
	}
	else {
		receivingBuffer = buffer1;
	}
	pthread_mutex_unlock(&bufferLock);
	return nextProcessedBuffer;
}

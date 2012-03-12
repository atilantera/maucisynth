/*
 * EventBuffer.cpp
 *
 *  Created on: 15.2.2012
 *      Author: alkim
 */

#include "EventBuffer.h"

EventBuffer::EventBuffer() {
	bufferLock = PTHREAD_MUTEX_INITIALIZER;
	buffer = new unsigned char[bufferLength];
	bufferUsed = 0;
}

EventBuffer::~EventBuffer() {
	delete [] buffer;
}

void EventBuffer::addNoteOn(unsigned char key, unsigned char velocity,
NoteSource source) {
	if (velocity > maxVelocity) {
		velocity = maxVelocity;
	}

	pthread_mutex_lock(&bufferLock);
	std::cout << "EventBuffer::addNoteOn: " << (int)key << " " <<
		(int)velocity << std::endl;

	if (bufferUsed + 4 <= bufferLength) {
		unsigned char * ptr = buffer + bufferUsed;
		*ptr++ = 0x01;
		*ptr++ = key;
		*ptr++ = velocity;
		*ptr++ = source;
		bufferUsed += 4;
	}
	pthread_mutex_unlock(&bufferLock);
}

void EventBuffer::addNoteOff(unsigned char key, NoteSource source)
{
	pthread_mutex_lock(&bufferLock);
	std::cout << "EventBuffer::addNoteOff: " << (int)key << std::endl;
	if (bufferUsed + 3 <= bufferLength) {
		unsigned char * ptr = buffer + bufferUsed;
		*ptr++ = 0x02;
		*ptr++ = key;
		*ptr++ = source;
		bufferUsed += 3;
	}
	pthread_mutex_unlock(&bufferLock);
}

void EventBuffer::addAllNotesOff(NoteSource source)
{
	pthread_mutex_lock(&bufferLock);
	std::cout << "EventBuffer::addAllNotesOff\n" << std::endl;
	if (bufferUsed + 2 <= bufferLength) {
		unsigned char * ptr = buffer + bufferUsed;
		*ptr++ = 0x03;
		*ptr++ = source;
		bufferUsed += 2;
	}
	pthread_mutex_unlock(&bufferLock);
}

void EventBuffer::addParameterChange(unsigned char controller,
unsigned short value) {
	pthread_mutex_lock(&bufferLock);
	std::cout << "EventBuffer::addParameterChange: controller=" <<
		(int)controller << "value=" << value << std::endl;

	if (bufferUsed + 4 <= bufferLength) {
		unsigned char * ptr = buffer + bufferUsed;
		*ptr++ = 0x04;
		*ptr++ = controller;
		*ptr++ = (value & 0xFF00) >> 8;
		*ptr++ = (value & 0x00FF);
		bufferUsed += 4;
	}
	pthread_mutex_unlock(&bufferLock);
}

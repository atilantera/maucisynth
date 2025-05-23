/*
 * EventBuffer.h
 * EventBuffer stores messages from GUI (class SynthGui) such as
 * key presses, key releases, GUI button presses and slider changes.
 * Class Synthesizer reads messages later.
 *
 *  Created on: 15.2.2012
 *      Author: alkim
 *
 * Implementation: two fixed length byte tables.
 * One table is for receiving messages from the GUI, and the another one
 * for Synthesizer class to process. At method swapBuffers() the byte tables
 * are swapped.
 */

#ifndef EVENTBUFFER_H_
#define EVENTBUFFER_H_

#include <pthread.h>
#include "tests/testing.h"
#include "SynthParameters.h"

#ifdef GUI_TESTING
#include <iostream>
#endif

/* Data format in EventBuffer.buffer:
 * Buffer contains adjacent messages.
 * Message data in hexadecimal notation:
 *
 * [01 nn vv ss] = Note on immediately. nn = key number (0..127)
 *                                      vv = note velocity (0..127)
 *                                      ss = source
 *
 * [02 nn s s]   = Note off immediately. nn = key number (0..127)
 *                                       ss = source
 *
 * [03 ss]       = All notes off immediately.
 *                                      ss = source
 *
 * [04 tt vv vv] = Synth controller change.
 *                      tt = type.
 *                      vv = value (16-bit unsigned int)
 */



class EventBuffer {
public:
	EventBuffer();
	~EventBuffer();

	void addNoteOn(unsigned char key, unsigned char velocity,
	NoteSource source);

	void addNoteOff(unsigned char key, NoteSource source);

	void addAllNotesOff(NoteSource source);

	void addParameterChange(unsigned char controller, unsigned short value);

	unsigned char * swapBuffers(unsigned int * dataLength);

private:
	pthread_mutex_t bufferLock;

	unsigned int bufferLength;

	// Two byte tables
	unsigned char * buffer1;
	unsigned char * buffer2;

	unsigned char * receivingBuffer;

	// How many bytes of receivingBuffer is used
	unsigned int bufferUsed;
};

#endif /* EVENTBUFFER_H_ */


































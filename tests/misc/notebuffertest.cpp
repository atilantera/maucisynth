/*
 * g++ -g -pg -o notebuffertest notebuffertest.cpp
 *
 * Gprof results:
 * Flat profile:
 * 
 * Each sample counts as 0.01 seconds.
 *   %   cumulative   self              self     total           
 *  time   seconds   seconds    calls  ms/call  ms/call  name    
 *  35.19      0.47     0.47 16000000     0.00     0.00  NoteBuffer::addNoteOn(unsigned int,
 *                                                          unsigned char, unsigned char)
 *  28.15      0.85     0.38  1000000     0.00     0.00  testDirect(NoteBuffer&)
 *  16.30      1.07     0.22  1000000     0.00     0.00  testMethodCall(NoteBuffer&)
 *  14.81      1.27     0.20  1000000     0.00     0.00  testPacked(NoteBuffer&)
 *
 * testMethodCall + addNoteOn = (0.22 + 0.47) seconds = 0.69 seconds.
 * testDirect = 0.38 seconds (1.8 times faster)
 * testPacked = 0.20 seconds (3.5 times faster than with method call
 * 
 * testMethodCall + addNoteOn: 249 ns/event
 * testDirect:                  24 ns/event
 * testPacked:                  12 ns/event
 *
 * With 44100 Hz samplerate and 4096 sample buffer length, one buffer is 93 msec long.
 * If 16 note on/off events per buffer, then it's 16 * 250 ns = 4 msec for processing
 * the events, which is 4/93 = 4 % of the total CPU time (for all processes). That's much.
 */
 
#include <iostream>

const unsigned int NOTE_BUFFER_LENGTH = 16;

class NoteBuffer {
public:
    NoteBuffer();
    inline void addNoteOn(unsigned int time, unsigned char key,
        unsigned char velocity);

	bool eventIsNoteOn[NOTE_BUFFER_LENGTH];
	unsigned int  eventTime[NOTE_BUFFER_LENGTH];
	unsigned char eventKey[NOTE_BUFFER_LENGTH];
	unsigned char eventVelocity[NOTE_BUFFER_LENGTH];
	unsigned int eventCount;
};

void testDirect(NoteBuffer & buffer)
{
    for (unsigned int i = 0; i < NOTE_BUFFER_LENGTH; i++)
    {
        buffer.eventIsNoteOn[i] = true;
        buffer.eventTime[i] = i;
        buffer.eventKey[i] = i;
        buffer.eventVelocity[i] = i;
        buffer.eventCount = i;
    }
}

void testMethodCall(NoteBuffer & buffer)
{
    buffer.eventCount = 0;
    for (unsigned int i = 0; i < NOTE_BUFFER_LENGTH; i++)
    {
        buffer.addNoteOn(i, i, i);
    }
}

void testPacked(NoteBuffer & buffer)
{
    for (unsigned int i = 0; i < NOTE_BUFFER_LENGTH; i++)
    {
        // [  31  ] [ 30-24] [23-17] [16-0]
        // isNoteOn velocity   key     time
        buffer.eventTime[i] = 0x80000000 | (i << 24) | (i << 17) | i;
    }
}

int main(int argc, char ** argv)
{
    NoteBuffer buffer;
    for (unsigned int i = 0; i < 1000000; i++) {
        testDirect(buffer);
        testPacked(buffer);
        testMethodCall(buffer);
    }
    return 0;
}

NoteBuffer::NoteBuffer()
{
    eventCount = 0;
}

void NoteBuffer::addNoteOn(unsigned int time, unsigned char key,
unsigned char velocity)
{
    if (eventCount < NOTE_BUFFER_LENGTH) {
        eventIsNoteOn[eventCount] = true;
        eventTime[eventCount] = time;
        eventKey[eventCount] = key;
        eventVelocity[eventCount] = velocity;
        eventCount++;
    }
}

    

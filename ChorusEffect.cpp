#include "ChorusEffect.h"

RingBuffer::RingBuffer(int ringBufferLength) {
    samples = new float[ringBufferLength];
    for (int i = 0; i < ringBufferLength; i++) {
        samples[i] = 0;
    }
    length = ringBufferLength;
    nextIn = 0;
    nextOut = length - 1;
}

RingBuffer::~RingBuffer() {
    delete [] samples;
}

void RingBuffer::put(float x) {
    samples[nextIn] = x;
    nextIn = (nextIn + 1) % length;
}

float RingBuffer::get() {
    float x = samples[nextOut];
    nextOut = (nextOut + 1) % length;
    return x; 
}


ChorusEffect::ChorusEffect() {
    this->samplerate = 44100;
    ringLength = 1024;
    ringBuffer = new float[ringLength];
    for (int i = 0; i < ringLength; i++) {
        ringBuffer[i] = 0;
    }
    ringBufferIndexIn = 0;

    mixAmount = 1.0;
    baseDelay = ringLength / 2 - 1;
    delay = baseDelay;
    dDelay = 1;
}

ChorusEffect::ChorusEffect(int ringBufferLength) {
    this->samplerate = 44100;
    ringLength = ringBufferLength;
    ringBuffer = new float[ringLength];
    for (int i = 0; i < ringLength; i++) {
        ringBuffer[i] = 0;
    }
    ringBufferIndexIn = 0;

    mixAmount = 1.0;
    baseDelay = ringLength / 2 - 1;
    delay = baseDelay;
    dDelay = 1;
}



ChorusEffect::~ChorusEffect() {
    delete [] ringBuffer;
}

void ChorusEffect::applyEffect(float * soundBuffer, unsigned int bufferLength) {
    for (unsigned int i = 0; i < bufferLength; i++) {
        ringBuffer[ringBufferIndexIn] = soundBuffer[i];
        ringBufferIndexIn = (ringBufferIndexIn + 1) % ringLength;
        ringBufferIndexOut = (ringBufferIndexIn + 1 - 20) % ringLength;
        soundBuffer[i] = ringBuffer[ringBufferIndexOut];
    }
}

/*
void ChorusEffect::applyEffect(float * soundBuffer, unsigned int bufferLength) {
    for (unsigned int i = 0; i < bufferLength; i++) {
        soundBuffer[i] = 0.5 * (soundBuffer[i] +
            ringBuffer[(int)ringBufferIndexOut]);

        ringBuffer[ringBufferIndexIn] = soundBuffer[i];
        ringBufferIndexIn = (ringBufferIndexIn + 1) % ringLength;
        ringBufferIndexOut = (int)(ringBufferIndexIn + 1 - delay) %
                             ringLength;
        
        delay += dDelay;
        if (delay > (ringLength - 1)) {
            dDelay = -1;
            delay += 2 * dDelay;
        }
        if (delay < 1) {
            dDelay = 1;
            delay += 2 * dDelay;
        }
    }


}
*/



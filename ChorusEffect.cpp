#include "ChorusEffect.h"

ChorusEffect::ChorusEffect() {
    this->samplerate = 44100;
    ringBufferLength = 1024;
    ringBuffer = new float[ringBufferLength];
    for (int i = 0; i < ringBufferLength; i++) {
        ringBuffer[i] = 0;
    }
    ringBufferIndexIn = 0;

    mixAmount = 1.0;
    baseDelay = ringBufferLength / 2 - 1;
    delay = baseDelay;
    dDelay = 1;
    ringBufferIndexOut = (0 - (int)delay) % ringBufferLength;
}

ChorusEffect::~ChorusEffect() {
    delete [] ringBuffer;
}

void ChorusEffect::applyEffect(float * soundBuffer, unsigned int bufferLength) {
    for (unsigned int i = 0; i < bufferLength; i++) {
        soundBuffer[i] = 0.5 * (soundBuffer[i] +
            ringBuffer[(int)ringBufferIndexOut]);

        ringBuffer[ringBufferIndexIn] = soundBuffer[i];
        ringBufferIndexIn = (ringBufferIndexIn + 1) % ringBufferLength;
        ringBufferIndexOut = (int)(ringBufferIndexIn + 1 - delay) %
                             ringBufferLength;
        
        delay += dDelay;
        if (delay > (ringBufferLength - 1)) {
            dDelay = -1;
            delay += 2 * dDelay;
        }
        if (delay < 1) {
            dDelay = 1;
            delay += 2 * dDelay;
        }
    }


}




#ifndef CHORUS_H_
#define CHORUS_H_

class RingBuffer {
public:
    RingBuffer(int ringBufferLength);
    ~RingBuffer();

    void put(float x);
    float get();

    float * samples;
    int length;     // number of samples
    int nextIn;     // index in samples
    int nextOut;    // index in samples
};

class ChorusEffect {
public:
    ChorusEffect();
    ChorusEffect(int ringBufferLength);
    ~ChorusEffect();

    void applyEffect(float * soundBuffer, unsigned int bufferLength);

private:
    float samplerate;
    float * ringBuffer;
    int ringLength;
    int ringBufferIndexIn;
    int ringBufferIndexOut;

    float mixAmount; // 0..1: amount of duplicate signal
    float baseDelay; // base delay in samples    
    float delay;     // in samples
    float dDelay;    // in samples
};

#endif

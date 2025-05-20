#ifndef CHORUS_H_
#define CHORUS_H_

class ChorusEffect {
public:
    ChorusEffect();
    ~ChorusEffect();

    void applyEffect(float * soundBuffer, unsigned int bufferLength);

private:
    float samplerate;
    float * ringBuffer;
    int ringBufferLength;
    int ringBufferIndexIn;
    float ringBufferIndexOut;

    float mixAmount; // 0..1: amount of duplicate signal
    float baseDelay; // base delay in samples    
    float delay;     // in samples
    float dDelay;    // in samples
};

#endif

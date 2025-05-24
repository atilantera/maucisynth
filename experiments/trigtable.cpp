/* An experiment: does it really pay to create a precomputed wave table
 * for the trigonometric sine function?
 *
 * Compiling:
 * g++ -lm -g -pg trigtable.cpp -o trigtable
 */

#include <iostream>
#include <cmath>

using namespace std;

const float M2PI = 2 * M_PI;

class SoundBuffer {
public:
    SoundBuffer(int bufferLength);
    ~SoundBuffer();

    float * samples;
    const int length;

    void clear();
    void print() const;
};

SoundBuffer::SoundBuffer(int bufferLength): length(bufferLength) {
    samples = new float[bufferLength];
}

SoundBuffer::~SoundBuffer() {
    delete [] samples;
}

void SoundBuffer::clear() {
    for (int i = 0; i < length; i++) {
        samples[i] = 0;
    }
}

void SoundBuffer::print() const {
    cout << "SoundBuffer::print()" << endl;
    cout << "index\tvalue" << endl;
    for (int i = 0; i < length; i++) {
        cout << i << "\t" << samples[i] << endl;
    }
}


class SineGenerator {
public:
    SineGenerator(int waveTableLength, int samplesPerSecond);

    void setFrequency(float frequency);

    void hardwareSignal(SoundBuffer & buffer);
    void tableSignal(SoundBuffer & buffer);
    void tableSignal2(SoundBuffer & buffer);
    void tableSignal3(SoundBuffer & buffer);
    void printTable() const;

private:
    float * sineTable; // sin(x) for in in range [0, 2 * pi]
    int tableLength;   // length of sineTable in floats
    int sampleRate;    // samples per second
    float t;           // angle in radians for sin(x) [-pi, pi]
    float dt;          // change of angle per sample
    float ti;          // index of sineTable
    float dti;         // change of ti per sample
};

SineGenerator::SineGenerator(int waveTableLength, int samplesPerSecond) {
    sineTable = new float[waveTableLength];
    tableLength = waveTableLength;

	// Sine wave
	float x = 0;
	float dx = 2 * M_PI / waveTableLength;
	for (int i = 0; i < waveTableLength; i++) {
		sineTable[i] = sinf(x);
		x += dx;
	}

    sampleRate = samplesPerSecond;
    t = 0;
    dt = 0;
    ti = 0;
    dti = 0;
}

/**
 * Sets the frequency of the generator in cycles per second.
 */
void SineGenerator::setFrequency(float frequency) {
    // Convert frequency to angular velocity
    float angular = frequency * 2 * M_PI;
    dt = angular / sampleRate;
    cout << "dt: " << dt << endl;

    
    ti = 0;
    // dti: how many table indices per sample
    // frequency: cycles/second
    // sampleRate: samples/second
    // tableLength: samples/cycle
    dti = frequency * tableLength / sampleRate;
}

void SineGenerator::hardwareSignal(SoundBuffer & buffer) {
    for (int i = 0; i < buffer.length; i++) {
        buffer.samples[i] = sinf(t);
        t += dt;
        
        if (t > M2PI) {
            t -= M2PI;
        }
    }
}

void SineGenerator::tableSignal(SoundBuffer & buffer) {
    float tableLengthf = tableLength;
    for (int i = 0; i < buffer.length; i++) {
        int index = (int)ti;
        if (index >= tableLength) {
            ti -= tableLength;
            if (ti < 0) {
                ti = 0;
            }
            index = (int)ti;
        }
        if (index < 0 || index >= tableLength) {
            cout << "Got index: " << index << " and ti " << ti << endl;
            return;
        } 
        buffer.samples[i] = sineTable[index];
        ti += dti;
    }
}

void SineGenerator::tableSignal2(SoundBuffer & buffer) {
    float tableLengthf = tableLength;
    for (int i = 0; i < buffer.length; i++) {
        buffer.samples[i] = sineTable[(int)ti];
        ti += dti;
        if (ti > tableLengthf) {
            ti -= tableLengthf;
        }
    }
}

void SineGenerator::tableSignal3(SoundBuffer & buffer) {
    float tableLengthf = tableLength;
    float * ptr = buffer.samples;
    float * endPtr = ptr + buffer.length;
    while (ptr < endPtr) {
        *ptr++ = sineTable[(int)ti];
        ti += dti;
        if (ti > tableLengthf) {
            ti -= tableLengthf;
        }
    }
}


void SineGenerator::printTable() const {
    cout << "SineGenerator::printTable()" << endl;
    cout << "index\tvalue" << endl;
    for (int i = 0; i < tableLength; i++) {
        cout << i << "\t" << sineTable[i] << endl;
    }
}

int main() {
    cout << "sizeof(float): " << sizeof(float) << endl;
    const int waveTableLength = 256;
    const int sampleRate = 44100;

    SineGenerator gen(waveTableLength, sampleRate);
    gen.printTable();

    gen.setFrequency(100);


    const int soundBufferLength = 32;
    SoundBuffer soundBuffer(soundBufferLength);
    gen.hardwareSignal(soundBuffer);
    cout << "Hardware signal:" << endl;
    soundBuffer.print();

    soundBuffer.clear();
    cout << "Table signal:" << endl;
    gen.tableSignal(soundBuffer);
    soundBuffer.print();

    for (int i = 0; i < 1000000; i++) {
        gen.hardwareSignal(soundBuffer);
        gen.tableSignal2(soundBuffer);
        gen.tableSignal3(soundBuffer);
    }

    return 0;
}


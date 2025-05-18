#include <iostream>
#include "../EnvelopeGenerator.h"

using namespace std;

class EnvelopeTest {
public:
	void runAllTests();
	void offByDefault();
};

void EnvelopeTest::runAllTests() {
    cout << "EnvelopeTest: Running all tests:" << endl;
	this->offByDefault();
}

void EnvelopeTest::offByDefault() {
    // The generator should be off by default, meaning that its
    // .generateEnvelope() fills the buffer with zeroes.
    cout << "--- offByDefault()" << endl;
    const int sampleRate = 20;
    const int bufferLength = 10;
	EnvelopeGenerator g = EnvelopeGenerator(sampleRate, bufferLength);
    if (g.getPhase() != OFF) {
        cout << "Phase should be EnvelopePhase.OFF by default!" << endl;
    }

    float testBuffer[10];
    for (int i = 0; i < bufferLength; i++) {
        testBuffer[i] = 0.2;
    }
    g.generateEnvelope(testBuffer);
    for (int i = 0; i < bufferLength; i++) {
        if (testBuffer[i] != 0) {
            cout << "Buffer not set to zeroes!" << endl;
            break;
        }
    }
}

int main() {
    cout << "EnvelopeTest test suite" << endl;
    EnvelopeTest testSuite;
    testSuite.runAllTests();
    return 0;
}

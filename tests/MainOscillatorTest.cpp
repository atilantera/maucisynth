#include <iostream>

#include "../MainOscillator.h"

void testFunctionPointers()
{
	MainOscillator osc;

	float outputBuffer[128];
	float modulatorBuffer[128];
	unsigned int bufferLength = 128;

	std::cout << "testFunctionPointers(): outputBuffer=" << outputBuffer <<
		" modulatorBuffer=" << modulatorBuffer << " bufferLength=" <<
		bufferLength << std::endl;

	osc.setWaveform(SINE);
	osc.generateSound(outputBuffer, modulatorBuffer, bufferLength);

	osc.setWaveform(PULSE);
	osc.generateSound(outputBuffer, modulatorBuffer, bufferLength);

	osc.setWaveform(TRIANGLE);
	osc.generateSound(outputBuffer, modulatorBuffer, bufferLength);
}

int main(int argc, char * argv[])
{
	testFunctionPointers();
	return 0;
}


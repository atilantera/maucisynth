
#include "MainOscillatorTest.h"

MainOscillatorTest::MainOscillatorTest()
{
	 bufferLength = 128;
}

bool MainOscillatorTest::initTests()
{
	testFile.open("MainOscillatorTest-output.txt");
	if (!testFile.good()) {
		std::cout << "Error: can't create an output file!" << std::endl;
		return false;
	}
	return true;
}

void MainOscillatorTest::testAll()
{
	testBasicSynthesis();
}

void MainOscillatorTest::finishTesting()
{
	if (testFile.is_open()) {
		testFile.close();
	}
}

void MainOscillatorTest::testBasicSynthesis()
{
	unsigned int i;
	float x, growth;
	samplerate = 32;
	setFrequency(2.0);

	// Sine wave 1
	synthesizeFromWavetable(outputBuffer, modulatorBuffer, 32);
	for (i = 0; i < 32; i++) {
		x = (float)i / 16 * 2 * M_PI;
		if (fabs(outputBuffer[i] - sinf(x) > 0.01)) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 1 (sine wave) failed!" << std::endl;
			break;
		}
	}

	// Triangle wave 1
	setWaveform(TRIANGLE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer, 32);
	x = 0;
	growth = 0.25;
	for (i = 0; i < 32; i++) {
		if (fabs(outputBuffer[i] - x) > 0.01) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 2 (triangle wave) failed!" << std::endl;
			break;
		}
		x += growth;
		if (x > 1) {
			x = 0.75;
			growth = -0.25;
		}
		if (x < -1) {
			x = -0.75;
			growth = 0.25;
		}
	}
	testFile << std::endl;

	// Sawtooth wave 1
	synthesizeSawtooth(outputBuffer, modulatorBuffer, 32);
	x = -1;
	growth = 0.125;
	for (i = 0; i < 32; i++) {
		if (i == 16) {
			x = -1;
		}
		if (fabs(outputBuffer[i] - x) > 0.01) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 3 (sawtooth wave) failed!" << std::endl;
			break;
		}
		x += growth;

	}

	// Pulse wave 1
	synthesizePulseWave(outputBuffer, modulatorBuffer, 32);
	x = 1;
	for (i = 0; i < 32; i++) {
		if (i == 8 || i == 24) {
			x = -1;
		}
		if (i == 16) {
			x = 1;
		}
		if (outputBuffer[i] != x) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 4 (pulse wave) failed!" << std::endl;
			break;
		}
	}

	// Abs-sine wave 1
	setWaveform(ABS_SINE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer, 32);
	for (i = 0; i < 32; i++) {
		x = (float)i / 32 * 2 * M_PI;
		x = 2 * fabs(sinf(x)) - 1;
		if (fabs(outputBuffer[i] - x > 0.03)) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 5 (abs-sine wave) failed!" << std::endl;
			break;
		}
	}
}


#include "MainOscillatorTest.h"

MainOscillatorTest::MainOscillatorTest()
{
	 testBufferLength = 128;

	 outputBuffer = new float[testBufferLength];
	 modulatorBuffer = new float[testBufferLength];

	 frequencyModulatedSine = new float[testBufferLength];
	 frequencyModulatedTriangle = new float[testBufferLength];
	 frequencyModulatedSawtooth = new float[testBufferLength];
	 frequencyModulatedPulse = new float[testBufferLength];
	 frequencyModulatedAbsSine = new float[testBufferLength];
	 pulseModulatedPulse = new float[testBufferLength];
	 amplitudeModulatedPulse = new float[testBufferLength];
}

MainOscillatorTest::~MainOscillatorTest()
{
	delete [] outputBuffer;
	delete [] modulatorBuffer;

	delete [] frequencyModulatedSine;
	delete [] frequencyModulatedTriangle;
	delete [] frequencyModulatedSawtooth;
	delete [] frequencyModulatedPulse;
	delete [] frequencyModulatedAbsSine;
	delete [] pulseModulatedPulse;
	delete [] amplitudeModulatedPulse;
}

bool MainOscillatorTest::initTests()
{
	testFile.open("MainOscillatorTest-output.txt");
	if (!testFile.good()) {
		std::cout << "Error: can't create an output file!" << std::endl;
		return false;
	}
	if (initTestData() == false) {
		return false;
	}
	return true;
}

void MainOscillatorTest::testAll()
{
	testBasicSynthesis();
	testFrequencyModulation(false);
	testAmplitudeModulation(false);
	testPulseWidthModulation(false);
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
	Oscillator::setBufferLength(32);
	setFrequency(2.0);

	// Sine wave 1
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
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
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
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

	// Sawtooth wave 1
	synthesizeSawtooth(outputBuffer, modulatorBuffer);
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
	synthesizePulseWave(outputBuffer, modulatorBuffer);
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
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
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

void MainOscillatorTest::testFrequencyModulation(bool generateOutput)
{
	unsigned int i;
	samplerate = testBufferLength;
	Oscillator::setBufferLength(testBufferLength);
	modulation = FREQUENCY;
	modulationAmount = 0.5;
	setFrequency(4.0);

	setWaveform(SINE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Frequency modulated sine wave. Wavelength = 32 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 5) {
		if (fabs(outputBuffer[i] - frequencyModulatedSine[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" sine wave failed!" << std::endl;
		}
		break;
	}

	setWaveform(TRIANGLE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Frequency modulated triangle wave. Wavelength = 32 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 5) {
		if (fabs(outputBuffer[i] - frequencyModulatedTriangle[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" triangle wave failed!" << std::endl;
		}
		break;
	}

	setWaveform(SAWTOOTH);
	synthesizeSawtooth(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Frequency modulated sawtooth wave. Wavelength = 32 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 5) {
		if (fabs(outputBuffer[i] - frequencyModulatedSawtooth[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" sawtooth wave failed!" << std::endl;
		}
		break;
	}

	setWaveform(PULSE);
	synthesizePulseWave(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Frequency modulated pulse wave. Wavelength = 32 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 5) {
		if (fabs(outputBuffer[i] - frequencyModulatedPulse[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" pulse wave failed!" << std::endl;
		}
		break;
	}

	setWaveform(ABS_SINE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Frequency modulated abs-sine wave. Wavelength = 32 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 5) {
		if (fabs(outputBuffer[i] - frequencyModulatedAbsSine[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" abs-sine wave failed!" << std::endl;
		}
		break;
	}
}

void MainOscillatorTest::testAmplitudeModulation(bool generateOutput)
{
	unsigned int i;
	samplerate = testBufferLength;
	Oscillator::setBufferLength(testBufferLength);
	modulation = AMPLITUDE;
	modulationAmount = 0.5;
	setFrequency(4.0);

	setWaveform(PULSE);
	synthesizePulseWave(outputBuffer, modulatorBuffer);
	applyAmplitudeModulation(outputBuffer, modulatorBuffer);
	for (i = 0; i < testBufferLength; i += 4) {
		if (fabs(outputBuffer[i] - amplitudeModulatedPulse[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testAmplitudeModulation "
				"failed!" << std::endl;
		}
		break;
	}
	if (generateOutput) {
		testFile << "Amplitude modulated pulse wave. Amplitude = 50%..100%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
}

void MainOscillatorTest::testPulseWidthModulation(bool generateOutput)
{
	unsigned int i;
	samplerate = testBufferLength;
	Oscillator::setBufferLength(testBufferLength);
	modulation = PULSE_WIDTH;
	modulationAmount = 0.5;
	setFrequency(4.0);

	setWaveform(PULSE);
	synthesizePulseWave(outputBuffer, modulatorBuffer);
	if (generateOutput) {
		testFile << "Pulse width modulated pulse wave. Pulse width = 16 +- 50%"
				<< std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}
	for (i = 0; i < testBufferLength; i += 4) {
		if (fabs(outputBuffer[i] - frequencyModulatedPulse[i]) > 0.001) {
			std::cout << "MainOscillatorTest::testFrequencyModulation: "
				" pulse wave failed!" << std::endl;
		}
		break;
	}
}

bool MainOscillatorTest::initTestData()
{
	unsigned int i;
	char buffer[256];
	std::ifstream dataFile("testdata.txt");
	if (!dataFile.good()) {
		std::cout << "Error: can't load file testdata.txt!" << std::endl;
		return false;
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		frequencyModulatedSine[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		frequencyModulatedTriangle[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		frequencyModulatedSawtooth[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		frequencyModulatedPulse[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		frequencyModulatedAbsSine[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		pulseModulatedPulse[i] = atof(buffer);
	}

	dataFile.getline(buffer, 256);
	for (i = 0; i < testBufferLength; i++) {
		dataFile.getline(buffer, 256);
		amplitudeModulatedPulse[i] = atof(buffer);
	}

	dataFile.close();
	float x;
	for (i = 0; i < testBufferLength; i++) {
		x = (float)i / testBufferLength * 2 * M_PI;
		modulatorBuffer[i] = sinf(x);
	}

	return true;
}

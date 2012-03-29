
#include "MainOscillatorTest.h"

MainOscillatorTest::MainOscillatorTest() : MainOscillator(parameters)
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
	testBasicSynthesis(false);
	testFrequencyModulation(false);
	testAmplitudeModulation(true);
	testPulseWidthModulation(false);
	testAdsrCurve(false);
	testFastMute(false);
}

void MainOscillatorTest::finishTesting()
{
	if (testFile.is_open()) {
		testFile.close();
	}
}

void MainOscillatorTest::testBasicSynthesis(bool generateOutput)
{
	unsigned int i;
	float x, growth;
	samplerate = 32;
	Oscillator::setBufferLength(32);
	setFrequency(2.0);

	// Sine wave 1
	setWaveform(SINE);
	synthesizeFromWavetable(outputBuffer, modulatorBuffer);
	for (i = 0; i < 32; i++) {
		x = (float)i / 16 * 2 * M_PI;
		if (fabs(outputBuffer[i] - sinf(x) > 0.01)) {
			std::cout << "MainOscillatorTest::testBasicSynthesis: "
				"Test 1 (sine wave) failed!" << std::endl;
			break;
		}
	}
	if (generateOutput) {
		testFile << "Basic sine wave. Wavelength = 16." << std::endl;
		for (i = 0; i < 32; i++) {
			testFile << outputBuffer[i] << std::endl;
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
	if (generateOutput) {
		testFile << "Basic triangle wave. Wavelength = 16." << std::endl;
		for (i = 0; i < 32; i++) {
			testFile << outputBuffer[i] << std::endl;
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
	if (generateOutput) {
		testFile << "Basic sawtooth wave. Wavelength = 16." << std::endl;
		for (i = 0; i < 32; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
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
	if (generateOutput) {
		testFile << "Basic pulse wave. Wavelength = 16." << std::endl;
		for (i = 0; i < 32; i++) {
			testFile << outputBuffer[i] << std::endl;
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
	if (generateOutput) {
		testFile << "Basic abs-sine wave. Wavelength = 16." << std::endl;
		for (i = 0; i < 32; i++) {
			testFile << outputBuffer[i] << std::endl;
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
	globalParameters.lfoModulationAmount = 0.5;
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

void MainOscillatorTest::testAdsrCurve(bool generateOutput)
{
	unsigned int i;
	samplerate = testBufferLength;
	Oscillator::setBufferLength(testBufferLength);
	modulation = NONE;

	setADSR(250, 250, 0.5, 500);

	setWaveform(PULSE);
	setFrequency(16);
	envelopePhase = ATTACK;
	previousEnvelopePhase = OFF;
	envelopePhaseTime = 0;

	bool noteFinished;
	generateSound(outputBuffer, modulatorBuffer, noteFinished);
	if (noteFinished == true) {
		std::cout << "MainOscillatorTest::testAdsrCurve: "
			"after first buffer noteFinished == true!" << std::endl;
		return;
	}
	if (envelopePhase != SUSTAIN) {
		std::cout << "MainOscillatorTest::testAdsrCurve: "
			"after first buffer envelopePhase != SUSTAIN!" << std::endl;
		return;
	}
	if (generateOutput) {
		testFile << "Pulse wave with Adsr curve. Attack = " <<
			testBufferLength / 4 << " samples, Decay = " <<
			testBufferLength / 4 << " samples, Sustain = 50%, "
			"Release = " << testBufferLength / 2 << " samples" << std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}

	envelopePhase = RELEASE;
	previousEnvelopePhase = SUSTAIN;

	generateSound(outputBuffer, modulatorBuffer, noteFinished);
	if (noteFinished == false) {
		std::cout << "MainOscillatorTest::testAdsrCurve: "
			"after second buffer noteFinished == false!" << std::endl;
		return;
	}
	if (envelopePhase != OFF) {
		std::cout << "MainOscillatorTest::testAdsrCurve: "
			"after second buffer envelopePhase != OFF!" << std::endl;
		return;
	}
	if (generateOutput) {
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}

}

void MainOscillatorTest::testFastMute(bool generateOutput)
{
	unsigned int i;
	bool noteFinished;
	samplerate = testBufferLength;
	Oscillator::setBufferLength(testBufferLength);
	lastSample = -1;
	muteFast();
	generateSound(outputBuffer, modulatorBuffer, noteFinished);

	if (generateOutput) {
		testFile << "Fast mute from value -1 to zero." << std::endl;
		for (i = 0; i < testBufferLength; i++) {
			testFile << outputBuffer[i] << std::endl;
		}
	}

	if (envelopePhase != OFF) {
		std::cout << "MainOscillatorTest::testFastMute: "
			"after generateSound() envelopePhase != OFF!" << std::endl;
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

void MainOscillatorTest::setWaveform(WaveformType w)
{
	switch (w) {
	case SINE:
		wavetable = Oscillator::sineTable;
		break;

	case TRIANGLE:
		wavetable = Oscillator::triangleTable;
		break;

	case ABS_SINE:
		wavetable = Oscillator::absSineTable;
		break;

	case SAWTOOTH:
	case PULSE:
		break;
	}
}

void MainOscillatorTest::setADSR(unsigned int a, unsigned int d, float s,
	unsigned int r)
{
	this->attackTime = a * 0.001 * samplerate;
	this->decayTime = d * 0.001 * samplerate;
	this->sustainVolume = s;
	this->releaseTime = r * 0.001 * samplerate;
}

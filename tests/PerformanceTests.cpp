/*
 * PerformanceTests.cpp
 *
 *  Created on: 31.3.2012
 *      Author: alkim
 */

#include "PerformanceTests.h"

const unsigned int tableLength = 5512;

int main(int argc, char ** argv)
{
	PerformanceTests ptests;
    ptests.runWithParameters(argc, argv);
	return 0;
}

PerformanceTests::PerformanceTests()
{
	unsigned int i;
	for (i = 0; i < POLYPHONY; i++) {
		oscillators[i] = new MainOscillator(parameters);
	}

	outputBuffer = new float[BUFFER_LENGTH];
	modulatorBuffer = new float[BUFFER_LENGTH];
	Oscillator::setBufferLength(BUFFER_LENGTH);
	Oscillator::setSamplerate(SAMPLE_RATE);
}

PerformanceTests::~PerformanceTests() {
	unsigned int i;
	for (i = 0; i < POLYPHONY; i++) {
		delete oscillators[i];
	}
	delete [] outputBuffer;
	delete [] modulatorBuffer;
}

void PerformanceTests::runWithParameters(int argc, char ** argv)
{
    if (argc < 2) {
        printUsageText();
        return;
    }

    std::string param(argv[1]);

    if (param.compare("render") == 0) {
        renderingSpeedTest();
        return;
    }

    if (param.compare("table") == 0) {
        tableVsSinTest();
        return;
    }

    std::cout << "Unknown parameter '" << param << "'." << std::endl;
    printUsageText();
}

void PerformanceTests::printUsageText()
{
    std::cout << "Usage: performancetests <parameter>" << std::endl;
    std::cout << "Choose one of following parameters:" << std::endl;
    std::cout << "    render - Rendering speed test" << std::endl;
    std::cout << "    table - Table versus sin(x) test" << std::endl;
}

// Tests rendering speed of maximum number of simultaneous notes
// with simultaneous conditions:
// 1. envelope phase is SUSTAIN
// 2. no LFO modulation is used
void PerformanceTests::renderingSpeedTest()
{
	std::cout << "Rendering speed test begins!" << std::endl;
    std::cout << "This may take a minute." << std::endl;

	parameters.attackTime = 0;
	parameters.decayTime = 0;
	parameters.sustainVolume = MaxSustainVolume;
	parameters.releaseTime = 0;


	unsigned int bufferCount = 50000;
	std::cout << "Running " << bufferCount << " buffers, each of " <<
		BUFFER_LENGTH << " samples and with " << POLYPHONY << " oscillators."
        << std::endl;

	unsigned int osc;
	for (osc = 0; osc < POLYPHONY; osc++) {
		oscillators[osc]->noteOn((unsigned char)
			(maxVelocity * osc / POLYPHONY), 100, true);
	}

	unsigned int buffer;
	bool noteFinished;

	std::cout << "Testing synthesizeFromWavetable()" << std::endl;
	parameters.waveform = ABS_SINE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizeTriangleWave()" << std::endl;
	parameters.waveform = TRIANGLE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizeSawtoothWave()" << std::endl;
	parameters.waveform = SAWTOOTH;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	std::cout << "Testing synthesizePulseWave()" << std::endl;
	parameters.waveform = PULSE;
	for (buffer = 0; buffer < bufferCount; buffer++) {
		for (osc = 0; osc < POLYPHONY; osc++) {
			oscillators[osc]->generateSound(outputBuffer, modulatorBuffer,
				noteFinished);
		}
	}

	for (osc = 0; osc < POLYPHONY; osc++) {
		oscillators[osc]->muteFast();
	}

	std::cout << "Rendering speed test ends." << std::endl;
    std::cout << "Type `gprof performancetests > performancetests-gprof.txt"
        " to see the results." << std::endl;
}

// Tests whether it is faster to each time call sinf()
// than store values of sine function to a lookup table
void PerformanceTests::tableVsSinTest()
{
	std::cout << "Table vs sin() test begins!" << std::endl;
   
    struct timespec beginTime, endTime;
    double sineTime, tableTime;
    unsigned int i;
 
    float sourceTable[tableLength];
    float destTable[tableLength];
    float angle = 0;
    float angleIncrease = 2.35;
    float * ptr = destTable;
    float * endPtr = destTable + tableLength;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &beginTime);
    while (ptr < endPtr) {
        *ptr++ = sourceTable[(int)angle];
        angle += angleIncrease;
        *ptr++ = sourceTable[(int)angle];
        angle += angleIncrease;
        *ptr++ = sourceTable[(int)angle];
        angle += angleIncrease;
        *ptr++ = sourceTable[(int)angle];
        angle += angleIncrease;
        if (angle >= tableLength) {
            angle -= tableLength;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);
    tableTime = secondDifference(endTime, beginTime);

    ptr = destTable;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &beginTime);
    while (ptr < endPtr) {
        *ptr++ = sinf(angle);
        angle += angleIncrease;
        *ptr++ = sinf(angle);
        angle += angleIncrease;
        *ptr++ = sinf(angle);
        angle += angleIncrease;
        *ptr++ = sinf(angle);
        angle += angleIncrease;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);
    sineTime = secondDifference(endTime, beginTime);

    std::cout << "Table version:  " << tableTime << " s." <<
        " (" << tableTime / tableLength * 1e9 << " ns/sample)" <<
        std::endl;

    std::cout << "sinf() version: " << sineTime <<  " s." <<
        " (" << sineTime / tableLength * 1e9 << " ns/sample)" <<
        std::endl;

    if (tableTime < sineTime) {
        std::cout << "Table is " << sineTime / tableTime << " times "
            "faster." << std::endl;
    }
    else {
        std::cout << "sinf() is " << tableTime / sineTime << " times "
            "faster." << std::endl;
    }
}

double PerformanceTests::secondDifference(struct timespec t1,
struct timespec t0)
{
    double seconds = t1.tv_sec - t0.tv_sec;
    if (t0.tv_nsec < t1.tv_nsec) {
        seconds += 0.000000001 * (t1.tv_nsec - t0.tv_nsec);
    }
    else {
        seconds -= 1;
        seconds += 0.000000001 * (1000000000 - t0.tv_nsec + t1.tv_nsec);
    }
    return seconds;
}

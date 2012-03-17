/*
 * MainOscillatorTest.h
 *
 *  Created on: 17.3.2012
 *      Author: alkim
 */

#ifndef MAINOSCILLATORTEST_H_
#define MAINOSCILLATORTEST_H_

#include <fstream>
#include "../MainOscillator.h"

class MainOscillatorTest : public MainOscillator {
public:
	MainOscillatorTest();
	~MainOscillatorTest() {}

	bool initTests();
	void testAll();
	void finishTesting();

	void testBasicSynthesis();

private:
	float outputBuffer[128];
	float modulatorBuffer[128];
	unsigned int bufferLength;

	std::ofstream testFile;
};


#endif /* MAINOSCILLATORTEST_H_ */

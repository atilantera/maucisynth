/*
 * NormalRandom.h
 *
 * Pseudorandom numbers from normal distribution using Box-Muller
 * transform.
 *
 *  Created on: 30.6.2012
 *      Author: alkim
 */

#ifndef NORMALRANDOM_H_
#define NORMALRANDOM_H_

#include <cmath>
#include <cstdlib>
#include <ctime>

class NormalRandom {
public:
	NormalRandom(float expectedValue, float variance);
	~NormalRandom();

	void setShape(float expectedValue, float variance);
	void setRangeLimiter(float min, float max);

	float getNext();
	float getLimited();

private:
	float mu;       // Expected value
	float sigma;    // Standard deviation

	float minValue; // Minimum value of limited range
	float maxValue; // Maximum value of limited range
	bool useRangeLimiting;

	float savedValue; // Saved random value
	bool useSavedValue;
};

#endif /* NORMALRANDOM_H_ */

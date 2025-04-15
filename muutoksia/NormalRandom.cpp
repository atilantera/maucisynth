/*
 * NormalRandom.cpp
 *
 *  Created on: 30.6.2012
 *      Author: alkim
 */

#include "NormalRandom.h"

NormalRandom::NormalRandom(float expectedValue, float variance)
{
	srand(time(NULL));
	mu = expectedValue;

	if (variance > 0) {
		sigma = sqrt(variance);
	}
	else {
		sigma = 1;
	}

	minValue = expectedValue - 3 * sigma;
	maxValue = expectedValue + 3 * sigma;
	useRangeLimiting = false;

	useSavedValue = false;

}

void NormalRandom::setShape(float expectedValue, float variance)
{
	mu = expectedValue;

	if (variance > 0) {
		sigma = sqrt(variance);
	}
	else {
		sigma = 1;
	}
}

void NormalRandom::setRangeLimiter(float min, float max)
{
	if (min < max) {
		minValue = min;
		maxValue = max;
	}
}

/*
 * Box-Muller transform: how to generate pseudorandom numbers from
 * the normal distribution.
 *
 * u and v are random values from uniform distribution whose
 * range is ]0,1]. (Zero is excluded.)
 *
 * After u and v are generated, a andb are random values from the standard
 * normal distribution (expected value = 0, variance = 1):
 *
 * a = sqrt(-2 * log(u)) * cos(2 * pi * v)
 * b = sqrt(-2 * log(u)) * sin(2 * pi * v)
 *
 * http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
 *
 * On 32-bit systems, where RAND_MAX = 2^31 - 1 = 2147483647,
 * it is guaranteed that minimum random value for u,
 * 1 / 2147483647 ~= 4.66 * 10^-10, will not underflow to zero
 * (which would cause an error when calculating log(u)).
 *
 * Producing pseudorandom numbers from normal distribution with parameters
 * (expected value = m, standard deviation = s):
 *
 * z = (x - m) / s   <=>   x = z * s + m
 *
 * z = normalized value (from Box-Muller transform)
 * x = denormalized value
 */

// Returns next random value
float NormalRandom::getNext()
{
	if (useSavedValue) {
		useSavedValue = false;
		return savedValue;
	}

    float u, v, R, Cos, Sin, a, b;
    int r;

    do {
        r = rand();
    } while (r == 0);
    u = (float)r / RAND_MAX;

    do {
        r = rand();
    } while (r == 0);
    v = (float)r / RAND_MAX;

    R = sqrt(-2 * logf(u));
    sincosf(v * 6.283185308, &Sin, &Cos);

    a = R * Cos * sigma + mu;
    b = R * Sin * sigma + mu;

    savedValue = b;
    return a;
}

// Returns next random value from limited range
float NormalRandom::getLimited()
{
	float value;
	do {
		value = getNext();
	} while (value < minValue || value > maxValue);
	return value;
}

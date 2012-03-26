/*
 * LowpassFilter.cpp
 *
 *  Created on: 26.3.2012
 *      Author: alkim
 */

#include "LowpassFilter.h"

LowpassFilter::LowpassFilter() {
	frequency = 1;

}

LowpassFilter::~LowpassFilter() {

}

void LowpassFilter::setFrequency(float f)
{
	frequency = f;
}

/*
 * ValueSet.cpp
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#include "ValueSet.h"

#ifndef NULL
#define NULL 0
#endif

// Constructor. maxSize = how many values this set can store at maximum.
ValueSet::ValueSet(unsigned char maxSize)
{
	valueTable = NULL;
	valueCount = 0;
	reservedSize = 0;
	if (maxSize > 0) {
		valueTable = new unsigned char[maxSize];
		if (valueTable == NULL) {
			reservedSize = 0;
		}
		else {
			reservedSize = maxSize;
		}
	}
}

ValueSet::~ValueSet() {
	delete [] valueTable;
}

// Returns maximum number of values this set can hold
unsigned char ValueSet::getReservedSize() const
{
	return reservedSize;
}

// Return number of values this set currently contains
unsigned char ValueSet::getSize() const
{
	return valueCount;
}

// Adds new value to the set
void ValueSet::putValue(unsigned char value)
{
	if (valueCount < reservedSize) {
		valueTable[valueCount++] = value;
	}
}

// Removes some value from the set
unsigned char ValueSet::getValue()
{
	if (valueCount > 0) {
		return valueTable[--valueCount];
	}
	else {
		return 0;
	}
}

// Empties the list from all values stored
void ValueSet::clear()
{
	valueCount = 0;
}

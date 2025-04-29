/*
 * KeyValueSet.cpp
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#include "KeyValueSet.h"

#ifndef NULL
#define NULL 0
#endif

// Constructor. maxSize = how many values this set can store at maximum.
// maxKeyValue = maximum value of a key. Keys with higher value than that
// will be discarded by method put().
// maxPairCount = how many key-value pairs this set can store.
KeyValueSet::KeyValueSet(unsigned char maxPairCount,
unsigned char maxKeyValue)
{
	keyValueTable = NULL;
	iteratedValues = NULL;
	valueCount = 0;
	reservedSize = 0;
	maxKey = maxKeyValue;
	iteratorIndex = 0;

	keyValueTable = new unsigned short[maxKey + 1];
	iteratedValues = new unsigned char[maxPairCount];
	if (keyValueTable == NULL || iteratedValues == NULL) {
		reservedSize = 0;
	}
	else {
		reservedSize = maxPairCount;
		for (unsigned int i = 0; i <= maxKey; i++) {
			keyValueTable[i] = UNUSED_KEY;
		}
	}
}

KeyValueSet::~KeyValueSet() {
	delete [] keyValueTable;
	delete [] iteratedValues;
}

// Returns maximum number of key-value pairs this set can hold
unsigned char KeyValueSet::getReservedSize() const
{
	return reservedSize;
}

// Inserts new key-value pair
void KeyValueSet::put(unsigned char key, unsigned char value)
{
	if (key <= maxKey && valueCount < reservedSize &&
		keyValueTable[key] == UNUSED_KEY)
	{
		keyValueTable[key] = value;
		iteratedValues[valueCount] = value;
		valueCount++;
	}
}

// Replaces a key-value pair
void KeyValueSet::replace(unsigned char key, unsigned char value)
{
	if (key <= maxKey && keyValueTable[key] != UNUSED_KEY) {
		unsigned char oldValue = keyValueTable[key];
		keyValueTable[key] = value;
		for (unsigned int i = 0; i < valueCount; i++) {
			if (iteratedValues[i] == oldValue) {
				iteratedValues[i] = value;
				break;
			}
		}
	}
}

// Returns value of specified key or UNUSED_KEY if value assigned
// to that key was not found.
unsigned short KeyValueSet::get(unsigned char key) const
{
	if (key <= maxKey) {
		return keyValueTable[key];
	}
	else {
		return UNUSED_KEY;
	}
}

// Removes a key-value pair
void KeyValueSet::remove(unsigned char key)
{
	if (key <= maxKey) {
		unsigned short value = keyValueTable[key];
		if (value == UNUSED_KEY) {
			return;
		}
		keyValueTable[key] = UNUSED_KEY;
		for (unsigned int i = 0; i < valueCount; i++) {
			if (iteratedValues[i] == value) {
				iteratedValues[i] = iteratedValues[valueCount - 1];
				break;
			}
		}
		valueCount--;
	}
}

// Initializes iteration for methods getNext() and popNext().
void KeyValueSet::beginIteration()
{
	iteratorIndex = 0;
}

bool KeyValueSet::iterationFinished()
{
	if (iteratorIndex < valueCount) {
		return false;
	}
	return true;
}

// Returns the next value that is stored in the set
unsigned short KeyValueSet::getNext()
{
	if (iteratorIndex < valueCount) {
		return iteratedValues[iteratorIndex++];
	}
	return ITERATION_FINISHED;
}

// Returns and removes the next value that is stored in the set
unsigned short KeyValueSet::popNext()
{
	if (iteratorIndex < valueCount) {
		unsigned short value = iteratedValues[iteratorIndex];
		for (unsigned int i = 0; i <= maxKey; i++) {
			if (keyValueTable[i] == value) {
				keyValueTable[i] = UNUSED_KEY;
				iteratedValues[iteratorIndex] = iteratedValues[--valueCount];
				return value;
			}
		}
	}
	return ITERATION_FINISHED;
}


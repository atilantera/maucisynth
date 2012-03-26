/*
 * ValueSetTest.cpp
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#include <assert.h>
#include "ValueSetTest.h"

const unsigned char testSize = 8;

ValueSetTest::ValueSetTest() {
	valueSet = new ValueSet(testSize);
	keyValueSet = new KeyValueSet(testSize, 127);
}

ValueSetTest::~ValueSetTest()
{
	delete valueSet;
	delete keyValueSet;
}

void ValueSetTest::testAll()
{
	testValueSet();
	testKeyValueSet();
}

void ValueSetTest::testValueSet()
{
	assert(valueSet->getReservedSize() == testSize);
	assert(valueSet->getSize() == 0);

	valueSet->putValue(2);
	valueSet->putValue(5);
	valueSet->putValue(255);
	assert(valueSet->getSize() == 3);

	unsigned char value;
	value = valueSet->getValue();
	assert(value == 255);
	assert(valueSet->getSize() == 2);

	valueSet->clear();
	assert(valueSet->getSize() == 0);
	assert(valueSet->getReservedSize() == testSize);

	value = valueSet->getValue();
	assert(value == 0);

	unsigned int i;
	for (i = 0; i < testSize; i++) {
		valueSet->putValue(53 + i);
	}
	valueSet->putValue(14);
	valueSet->putValue(15);
	assert(valueSet->getSize() == testSize);
	value = valueSet->getValue();
	assert(value == testSize - 1 + 53);

}

void ValueSetTest::testKeyValueSet()
{
	assert(keyValueSet->getReservedSize() == testSize);

	// Simple insert
	keyValueSet->put(60, 5);
	keyValueSet->put(67, 4);

	assert(keyValueSet->get(60) == 5);
	assert(keyValueSet->get(67) == 4);
	assert(keyValueSet->get(61) == UNUSED_KEY);

	keyValueSet->put(60, 15);
	assert(keyValueSet->get(60) == 5);

	// Simple replace
	keyValueSet->replace(60, 15);
	assert(keyValueSet->get(60) == 15);

	// Simple iteration
	keyValueSet->beginIteration();
	assert(keyValueSet->getNext() == 15);
	assert(keyValueSet->getNext() == 4);
	assert(keyValueSet->getNext() == ITERATION_FINISHED);

	// Simple removal
	keyValueSet->remove(60);
	assert(keyValueSet->get(60) == UNUSED_KEY);

	// Full insert
	keyValueSet->put(11, 21);
	keyValueSet->put(12, 22);
	keyValueSet->put(13, 23);
	keyValueSet->put(14, 24);
	keyValueSet->put(15, 25);
	keyValueSet->put(16, 26);
	keyValueSet->put(17, 27);

	keyValueSet->put(17, 28);
	keyValueSet->put(18, 28);

	// Full iteration
	keyValueSet->beginIteration();
	assert(keyValueSet->getNext() == 4);
	assert(keyValueSet->getNext() == 21);
	assert(keyValueSet->getNext() == 22);
	assert(keyValueSet->getNext() == 23);
	assert(keyValueSet->getNext() == 24);
	assert(keyValueSet->getNext() == 25);
	assert(keyValueSet->getNext() == 26);
	assert(keyValueSet->getNext() == 27);
	assert(keyValueSet->iterationFinished() == true);

	// Random removal
	keyValueSet->remove(15);
	keyValueSet->replace(12, 30);
	keyValueSet->remove(16);
	keyValueSet->beginIteration();
	assert(keyValueSet->getNext() == 4);
	assert(keyValueSet->getNext() == 21);
	assert(keyValueSet->getNext() == 30);
	assert(keyValueSet->getNext() == 23);
	assert(keyValueSet->getNext() == 24);
	assert(keyValueSet->getNext() == 27);
	assert(keyValueSet->iterationFinished() == true);

	// Removal by iteration
	keyValueSet->beginIteration();

	// table: [4] 21 30 23 24 27
	assert(keyValueSet->popNext() == 4);

	// table: [27] 21 30 23 24
	assert(keyValueSet->popNext() == 27);

	// table: [24] 21 30 23
	assert(keyValueSet->popNext() == 24);

	// table: [23] 21 30
	assert(keyValueSet->popNext() == 23);

	// table: [30] 21
	assert(keyValueSet->popNext() == 30);

	// table: [21]
	assert(keyValueSet->popNext() == 21);

	// table: []
	assert(keyValueSet->iterationFinished() == true);

	keyValueSet->beginIteration();
	assert(keyValueSet->iterationFinished() == true);
	assert(keyValueSet->popNext() == ITERATION_FINISHED);
	assert(keyValueSet->iterationFinished() == true);
}

/*
 * ValueSetTest.h
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#ifndef VALUESETTEST_H_
#define VALUESETTEST_H_

#include <iostream>
#include "../ValueSet.h"
#include "../KeyValueSet.h"

class ValueSetTest {
public:
	ValueSetTest();
	~ValueSetTest();

	void testAll();
	void testValueSet();
	void testKeyValueSet();

private:
	ValueSet * valueSet;
	KeyValueSet * keyValueSet;
};

#endif /* VALUESETTEST_H_ */

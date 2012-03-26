/*
 * ValueSett.h
 *
 * Stores fixed amount of values (in unsorted order).
 * This is used for storing information about which oscillators currently
 * free.
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#ifndef VALUESET_H_
#define VALUESET_H_

class ValueSet {
public:
	ValueSet(unsigned char maxSize);
	~ValueSet();

	unsigned char getReservedSize() const;
	unsigned char getSize() const;
	void putValue(unsigned char value);
	unsigned char getValue();
	void clear();

protected:
	unsigned char * valueTable;
	unsigned char valueCount;
	unsigned char reservedSize;
};

#endif /* VALUESET_H_ */

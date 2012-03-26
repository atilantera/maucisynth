/*
 * KeyValueSet.h
 *
 * Set of key-value pairs. Searching by key.
 * Stores information about what oscillator group is playing a certain note.
 *
 * NOTE: This KeyValueSet is based on two assumptions:
 * 1. One value is assigned to only one key, i.e. there aren't two different
 * keys that have the same value.
 * 2. Every key is unique, i.e. the set can't store duplicate keys.
 *
 *
 *
 *  Created on: 25.3.2012
 *      Author: alkim
 */

#ifndef KEYVALUESET_H_
#define KEYVALUESET_H_

const unsigned short UNUSED_KEY = 256;
const unsigned short ITERATION_FINISHED = 256;

class KeyValueSet {
public:
	KeyValueSet(unsigned char maxPairCount, unsigned char maxKeyValue);
	~KeyValueSet();

	unsigned char getReservedSize() const;

	void put(unsigned char key, unsigned char value);
	void replace(unsigned char key, unsigned char value);
	unsigned short get(unsigned char key) const;
	void remove(unsigned char key);

	void beginIteration();
	bool iterationFinished();
	unsigned short getNext();
	unsigned short popNext();

protected:
	// Possible key values are 0..maxKey. Keys are indices of valueTable.
	// valueTable contains values assigned to keys.
	unsigned short * keyValueTable;
	unsigned int valueCount;
	unsigned int reservedSize;
	unsigned int maxKey;

	unsigned char * iteratedValues;
	unsigned int iteratorIndex;
};

#endif /* KEYVALUESET_H_ */

/*
 * LowpassFilter.h
 *
 *  Created on: 26.3.2012
 *      Author: alkim
 */

#ifndef LOWPASSFILTER_H_
#define LOWPASSFILTER_H_

class LowpassFilter {
public:
	LowpassFilter();
	void setFrequency(float f);
	virtual ~LowpassFilter();

private:
	float frequency;
};

#endif /* LOWPASSFILTER_H_ */

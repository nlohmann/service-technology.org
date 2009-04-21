/*
 * bounds.h
 *
 *  Created on: 21.04.2009
 *      Author: Jan
 */

#ifndef BOUNDS_H_
#define BOUNDS_H_

#include <iostream>

#define BOUND int
#define UNBOUNDED -1


template <class T>
class Bounds {
public:
	BOUND min;
	BOUND max;
	T key;

	Bounds() { min = UNBOUNDED; max = UNBOUNDED;}
	Bounds(T _key) { key = _key; min = UNBOUNDED; max = UNBOUNDED;}

	bool isExact() { return min == max; }

};

BOUND getBoundFromLPSolveString(std::string);

#endif /* BOUNDS_H_ */

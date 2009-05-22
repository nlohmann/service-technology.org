/*
 * bounds.h
 *
 *  Created on: 21.04.2009
 *      Author: Jan
 */

#ifndef BOUNDS_H_
#define BOUNDS_H_

#include <string>

#define LINDA_BOUND int
#define LINDA_UNBOUNDED -1

template <class T>
class Bounds {
public:
	LINDA_BOUND min;
	LINDA_BOUND max;
	T key;

	Bounds() { min = LINDA_UNBOUNDED; max = LINDA_UNBOUNDED;}
	Bounds(T _key) { key = _key; min = LINDA_UNBOUNDED; max = LINDA_UNBOUNDED;}

	bool isExact() { return min == max; }

};

LINDA_BOUND getBoundFromLPSolveString(std::string);

#endif /* BOUNDS_H_ */

/*
 * messageprofile.h
 *
 *  Created on: 27.04.2009
 *      Author: Jan
 */

#ifndef MESSAGEPROFILE_H_
#define MESSAGEPROFILE_H_

#include "helpers.h"
#include "bounds.h"

#define EVENT pnapi::Place*

class L0MessageProfile {

public:
	unsigned int nrOfEvents;
	Bounds<EVENT> * bounds;

	L0MessageProfile(unsigned int _nrOfEvents) {
		nrOfEvents = _nrOfEvents;
		bounds = new Bounds<EVENT>[nrOfEvents];
	}

	void out() {

		for (int i = 0; i < nrOfEvents; ++i) {
			Bounds<EVENT>& b = bounds[i];
			std::cout << b.min << " <= " << b.key->getName() << " <= " << b.max << std::endl;
		}

	}

};


#endif /* MESSAGEPROFILE_H_ */

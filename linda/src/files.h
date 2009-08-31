/*
 * files.h
 *
 *  Created on: 30.07.2009
 *      Author: Jan
 */

#ifndef FILES_H_
#define FILES_H_

#include "helpers.h"
#include <fstream>
#include <vector>
#include "stateEquation.h"


class ProfileFile {
public:
	std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >* systems;
	pnapi::PetriNet* net;
	ProfileFile(std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >* s, pnapi::PetriNet* n) {
		systems = s;
		net = n;
	}
	void output(std::ofstream& file,bool termsAsGiven);

};


#endif /* FILES_H_ */

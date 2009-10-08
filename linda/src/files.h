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

/*!
 * An output file containing a number of terms, final markings, and lower/upper bounds for each final marking and term.
 */
class ProfileFile {
public:
	std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >* systems;//!< The LP systems that are to be considered.
	pnapi::PetriNet* net; //!< The underlying open net.

	/*!
	 * Basic constructor, assigning the fields, does not create the file yet.
	 */
	ProfileFile(std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >* s, pnapi::PetriNet* n) {
		systems = s;
		net = n;
	}

	/*!
	 * Creates the file and its contents.
	 */
	void output(std::ofstream& file,bool termsAsGiven);

};


#endif /* FILES_H_ */

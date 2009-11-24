#ifndef FILES_H_
#define FILES_H_

#include "helpers.h"
#include <fstream>
#include <vector>
#include "stateEquation.h"

/// An output file containing a number of terms, final markings, and lower/upper bounds for each final marking and term.
class ProfileFile {
public:
	ExtendedStateEquation** systems;//!< The LP systems that are to be considered.
	pnapi::PetriNet* net; //!< The underlying open net.
	uint8_t nr; //!< Number of systems.

	/// Basic constructor, assigning the fields, does not create the file yet.
	ProfileFile(ExtendedStateEquation** s, pnapi::PetriNet* n, uint8_t nrOfSystems) : systems(s), net(n), nr(nrOfSystems) {}

	/// Creates the file and its contents.
	void output(std::ostream& file);

};


#endif /* FILES_H_ */

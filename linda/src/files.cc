/*
 * files.cc
 *
 *  Created on: 30.07.2009
 *      Author: Jan
 */

#include "files.h"

void ProfileFile::output(std::ofstream& file,bool termsAsGiven) {

	file << "FINALMARKINGS";

	int counter = 1;

	for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems->begin();
	systemsIt != systems->end();
	++systemsIt) {
		file << "\n    F" << counter << ": " << (*systemsIt).first->toString();
		++counter;
	}

	counter = 1;

	file << "\n\nTERMS";

	for (std::map<EventTerm*,EventTermBound*>::iterator termsIt = (*systems->begin()).second->calculated.begin(); termsIt != (*systems->begin()).second->calculated.end();
	++termsIt) {
		file << "\n    T" << counter << ": ";
		if (termsAsGiven) {
			file << (*termsIt).first->toString();
		} else {
			file << EventTerm::toPrettyString((*termsIt).first);
		}
		file << ";";
		++counter;

	}

	file << "\n\nBOUNDS";
	int tCounter = 1;
	int fCounter = 1;

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt2 = systems->begin();
		systemsIt2 != systems->end();
		++systemsIt2) {


				for (std::map<EventTerm*,EventTermBound*>::iterator termsIt = (*systemsIt2).second->calculated.begin(); termsIt != (*systemsIt2).second->calculated.end();
					++termsIt) {
					file << "\n    F" << fCounter << ",T" << tCounter << ": ";
					file << (*termsIt).second->getLowerBoundString() << "," <<(*termsIt).second->getUpperBoundString() << ";";

					++tCounter;
				}


		tCounter = 1;
		++fCounter;
	}



}

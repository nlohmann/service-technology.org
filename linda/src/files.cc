#include "files.h"

void ProfileFile::output(std::ostream& file) {

	file << "PLACE";
	file << std::endl << "  INTERNAL" << std::endl << "    ";

	bool first = true;

	for (std::set<pnapi::Place *>::iterator it =
			net->getInternalPlaces().begin(); it
			!= net->getInternalPlaces().end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << (*it)->getName();

	}

	file << ";";

	file << std::endl << "  INPUT";

	first = true;

	for (std::set<pnapi::Place *>::iterator it = net->getInputPlaces().begin(); it
			!= net->getInputPlaces().end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << std::endl << "    ";

		file << (*it)->getName();

	}

	file << ";";
	file << std::endl << "  OUTPUT";

	first = true;

	for (std::set<pnapi::Place *>::iterator it = net->getOutputPlaces().begin(); it
			!= net->getOutputPlaces().end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << std::endl << "    ";

		file << (*it)->getName();

	}

	file << ";";

	file << std::endl << std::endl << "FINALMARKINGS";

	for (int i = 0; i < nr; ++i) {
		file << std::endl << "    F" << i+1 << ": "
				<< systems[i]->omega->toString();
	}

	int counter = 1;

	file << std::endl << std::endl << "TERMS";

	for (std::vector<int*>::iterator termsIt =
			systems[0]->calculatedEventTerms.begin(); termsIt
			!= systems[0]->calculatedEventTerms.end(); ++termsIt) {
		file << std::endl << "    T" << counter << ": ";
		for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
			if ((*termsIt)[i] == 0) {
				continue;
			}

			if ((*termsIt)[i] > 0) {
				file << "+";
			}
			file << (*termsIt)[i] << "*" << LindaHelpers::EVENT_STRINGS[i] << " ";
		}
		file << ";";
		++counter;

	}

	file << std::endl << std::endl << "BOUNDS";
	int tCounter = 1;
	int fCounter = 1;

	for (int i = 0; i < nr; ++i) {

		for (std::vector<EventTermBound*>::iterator termsIt =
				systems[i]->calculatedBounds.begin(); termsIt
				!= systems[i]->calculatedBounds.end(); ++termsIt) {

			if ((*termsIt)->upperBounded || (*termsIt)->lowerBounded) {

			file << std::endl << "    F" << fCounter << ",T" << tCounter
					<< ": ";
			file << (*termsIt)->getLowerBoundString() << ","
					<< (*termsIt)->getUpperBoundString() << ";";

			}

			++tCounter;
		}

		tCounter = 1;
		++fCounter;
	}

	file << std::endl;

}

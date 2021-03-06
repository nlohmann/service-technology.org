#include "staticAnalysis.h"

FlowMatrix::FlowMatrix(pnapi::PetriNet* net) {


	terms = 0;
	this->net = net;
	root = 0;
	last = 0;
	width = net->getPlaces().size() + LindaHelpers::NR_OF_EVENTS;
	int index = 0;
	maxNameLen = 0;
/*
	for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
		int nameLen = LindaHelpers::EVENT_NAMES[i].length();
		if (maxNameLen < nameLen) {
			maxNameLen = nameLen;
		}
	}

	for (std::set<pnapi::Transition*>::iterator transIt =
		net->getTransitions().begin(); transIt
		!= net->getTransitions().end(); ++transIt) {

		int* currentRow = new int[width];

		int count = 0;

		for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
			int inPre = 0;
			int inPost = 0;
			if (p->getPreset().find((*transIt)) != p->getPreset().end()) {
				inPre = 1;
			}
			if (p->getPostset().find((*transIt)) != p->getPostset().end()) {
				inPost = 1;
			}
			currentRow[count] = inPre + inPost;
			++count;
		}

		for (std::set<pnapi::Place*>::iterator placeIt =
			net->getInternalPlaces().begin(); placeIt
			!= net->getInternalPlaces().end(); ++placeIt) {
			pnapi::Place* p = (*placeIt);
			int inPre = 0;
			int inPost = 0;
			if (p->getPreset().find((*transIt)) != p->getPreset().end()) {
				inPre = 1;
			}
			if (p->getPostset().find((*transIt)) != p->getPostset().end()) {
				inPost = 1;
			}
			currentRow[count] = inPre - inPost;
			++count;
		}

		ListElement<int*>* currentElement = new ListElement<int*> (currentRow,
				last, 0);

		if (root == 0) {
			root = currentElement;
		} else {
			last->next = currentElement;
		}
		last = currentElement;
		++index;

	}
	*/
}

void FlowMatrix::computeTInvariants() {

/*

	int firstPlace = net->getInterfacePlaces().size();
	int lastPlace = firstPlace + net->getInternalPlaces().size() - 1;

	for (int i = firstPlace; i <= lastPlace; ++i) {


		ListElement<int*>* newRows = 0;
		ListElement<int*>* firstNew = 0;

		ListElement<int*>* current = root;
		while (current != 0) {
			ListElement<int*>* currentComp = current->next;

			int currNr = current->element[i];

			while (currentComp != 0) {
				int currCompNr = currentComp->element[i];
				if (currNr < 0 && currCompNr > 0 || currNr > 0 && currCompNr
						< 0) {
					int* newRow = new int[width];

					int fac = LindaHelpers::abs(currNr);
					int facComp = LindaHelpers::abs(currCompNr);

					int euclidA = fac;
					int euclidB = facComp;

					while (euclidB != 0) {
						int h = euclidA % euclidB;
						euclidA = euclidB;
						euclidB = h;
					}

					int ggt = euclidA;

					int kgv = LindaHelpers::abs(fac*facComp)/ggt;

					assert(kgv/fac - facComp == 0);
					assert(kgv/facComp - fac == 0);

					for (int init = 0; init < width; ++init) {
						newRow[init] =
							kgv/fac * current->element[init] + kgv/facComp
							* currentComp->element[init];
					}
					ListElement<int*>* toInsert = new ListElement<int*> (
							newRow, newRows, 0);

					if (newRows == 0) {
						firstNew = toInsert;
						newRows = toInsert;

					} else {
						newRows->next = toInsert;
					}

					newRows = toInsert;


				}

				currentComp = currentComp->next;

			}

			current = current->next;
		}

		ListElement<int*>* current2 = root;
		while (current2 != 0) {

			if (current2->element[i] != 0) {
				if (current2 == root) {
					root = current2->next;
					if (current2->next != 0) {
						current2->prev = 0;
					} else {
						last = 0;
					}
					delete[] current2->element;
					delete current2;
					current2 = root;
				} else {
					ListElement<int*>* current3 = current2->next;
					current2->prev->next = current2->next;
					if (current2->next != 0) {
						current2->next->prev = current2->prev;
					} else {
						last = current2->prev;
					}
					delete[] current2->element;
					delete current2;
					current2 = current3;
				}
			} else {

				current2 = current2->next;
			}
		}

		if (newRows != 0) {
			if (root == 0) {
				root = firstNew;
				last = newRows;
			} else {
				firstNew->prev = last;
				last->next = firstNew;
				last = newRows;
			}
		}

	}

	width = firstPlace;
*/
}

void FlowMatrix::createTerms(EventTermBound*** level0) {
/*
	ListElement<int*>* currentInList = 0;
	ListElement<int*>* current = root;
	while (current != 0) {
		for (int i = 0; i < width - 1; ++i) {
			if (current->element[i] > 0) {
				int e1 = i;
				int f1 = current->element[i];
				for (int j = i + 1; j < width; ++j) {
					if (current->element[j] > 0) {

						if (level0 == 0 || !((level0[0][i]->upperBounded && !level0[0][j]->upperBounded) || (level0[0][j]->upperBounded && !level0[0][i]->upperBounded)) ) {

							int e2 = j;
							int f2 = -1* current->element[j];

							ListElement<int*>* search = terms;
							bool alreadyStored = false;

							while (search != 0) {



								int stored1 = abs(search->element[e1]);
								int stored2 = abs(search->element[e2]);

								if (stored1 != 0 && stored2 != 0) {

								bool eq = (stored1 == f1 && stored2 == abs(f2)) || (stored2 == f1 && stored1 == abs(f2));

								if (eq) {
									alreadyStored = true;
								}

								else {

									bool multiple = (stored1 % f1 == 0 && stored1/f1 == stored2/abs(f2)) || (f1 % stored1 == 0 && f1/stored1 == abs(f2)/stored2);

									if (multiple) {
										alreadyStored = true;
									}
								}

								}
	
								search = search->next;

							}

							if (!alreadyStored) {

								int* term = new int[LindaHelpers::NR_OF_EVENTS];
								for (int counter = 0; counter < LindaHelpers::NR_OF_EVENTS; ++counter) {
									term[counter] = 0;
								}

								term[e1] = f1;
								term[e2] = f2;

								ListElement<int*>* newElem = new ListElement<
								int*> (term, 0, 0);

								if (terms == 0) {
									terms = newElem;
									currentInList = newElem;
								} else {
									currentInList->next = newElem;
									newElem->prev = currentInList;
									currentInList = newElem;
								}

							}
						}
					}
				}
			}
		}
		current = current->next;
	}
	*/
}

void FlowMatrix::output() {
/*
	for (std::set<pnapi::Place*>::iterator eventIt =
		net->getInterfacePlaces().begin(); eventIt
		!= net->getInterfacePlaces().end(); ++eventIt) {
		std::cerr
		<< LindaHelpers::fillup((*eventIt)->getName(), maxNameLen + 2);
	}

	std::cerr << std::endl;
	ListElement<int*>* current = root;
	while (current != 0) {
		for (int i = 0; i < width; ++i) {
			std::cerr << LindaHelpers::fillup(LindaHelpers::intToStr(
					current->element[i]), maxNameLen + 2);
		}
		std::cerr << std::endl;
		current = current->next;
	}
	*/
}

FlowMatrix::~FlowMatrix() {
	/*{
		ListElement<int*>* current = root;

		while (current != 0) {
			delete[] current->element;
			ListElement<int*>* temp = current;
			current = current->next;
			delete temp;
		}
	}

*/
}

#ifndef STATICANALYSIS_H_
#define STATICANALYSIS_H_

#include "helpers.h"
#include "eventTerm.h"

class FlowMatrix {
public:
	FlowMatrix(pnapi::PetriNet* net);
	~FlowMatrix();

	ListElement<int*>* root;
	ListElement<int*>* last;
	pnapi::PetriNet* net;

	int width;
	int maxNameLen;

	void createTerms();

	ListElement<EventTerm*>* terms;


	void output();

	void computeTInvariants();

};



#endif /* STATICANALYSIS_H_ */

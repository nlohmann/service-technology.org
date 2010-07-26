#ifndef LINDAAGENT_H_
#define LINDAAGENT_H_

#include <config.h>
#include <typeinfo>
#include <pnapi/pnapi.h>
#include "helpers.h"
#include "eventTerm.h"
#include "stateEquation.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

class LindaAgent {
public:
	/*! \brief Initialize the Linda Agent with an open net and a collection of final markings.
	 */

	static bool initialize(pnapi::PetriNet* net, bool keepHistory=false);

	/// add all final markings from the final condition
	static void addFinalMarkingsFromFinalCondition(uint8_t bound);

	static std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> translateFinalCondition(pnapi::PetriNet* net);

	
	/// Get a bit table, 1 = xor dependency.
	static uint8_t* getXORTable(uint8_t firstOfInterest, uint8_t lastOfInterest);
	static uint8_t* getXORTable(uint8_t* eventsOfInterest, uint8_t count);

	/// Terminate the Linda Agent, clean up everything.
	static void finalize();

	
	static std::string getFinalMarkingString(int i);

	static unsigned int getNumberOfSystems () {
		return mFinals->size();
	}

	static std::vector<BinaryTree<pnapi::Place*,std::pair<int,int> >*>* getFinalMarkings() {
		return mFinals;
	}

	static BinaryTree<pnapi::Place*,std::pair<int,int> >* getFinalMarking(int x) {
		if (x < mFinals->size()) {
			return (*mFinals)[x];
		}
		return 0;
	}
	static ExtendedStateEquation* getSystem(int x) {
		if (x < mSystems->size()) {
			return (*mSystems)[x];
		}
		return 0;
	}

	static ExtendedStateEquation** getSystemsArray() {
		ExtendedStateEquation** result = new ExtendedStateEquation*[mSystems->size()];
		for (int i = 0; i < mSystems->size(); ++i) {
			result[i] = (*mSystems)[i];
		}
		return result;
	}

private:
	static pnapi::PetriNet* mNet;
	static std::vector<BinaryTree<pnapi::Place*,std::pair<int,int> >*>* mFinals;
	static std::vector<ExtendedStateEquation*>* mSystems;
	static uint8_t firstID;
	static uint8_t lastID;
	static bool keepHistory;
};

#endif

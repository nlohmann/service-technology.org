#ifndef LINDAAGENT_H_
#define LINDAAGENT_H_

#include <pnapi/pnapi.h>
#include "helpers.h"
#include "eventTerm.h"
#include "setsOfFinalMarkings.h"
#include "stateEquation.h"



class LindaAgent {
public:
	/*! \brief Initialize the Linda Agent with an open net and a collection of final markings.
	 */
	static bool initialize(pnapi::PetriNet* net, std::map<std::string, uint8_t>& name2id, std::map<uint8_t, std::string>& id2name, bool keepHistory=false);

	static bool initialize(pnapi::PetriNet* net, bool keepHistory=false);

	/// add a final marking and build an lp system
	static bool addFinalMarking(std::map<const pnapi::Place*,unsigned int>* finalMarking);

	/// add a final marking and build an lp system
	static bool addFinalMarking(PartialMarking* finalMarking);

	/// add all final markings from the final condition
	static bool addFinalMarkingsFromFinalCondition(uint8_t bound);

	/// Get a bit table, 1 = xor dependency.
	static uint8_t* getXORTable(uint8_t firstOfInterest, uint8_t lastOfInterest);
	static uint8_t* getXORTable(uint8_t* eventsOfInterest, uint8_t count);

	/// Terminate the Linda Agent, clean up everything.
	static void finalize();

	static unsigned int getNumberOfSystems () {
		return mFinals->size();
	}

	static std::vector<PartialMarking*>* getFinalMarkings() {
		return mFinals;
	}

	static PartialMarking* getFinalMarking(int x) {
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
	static std::vector<PartialMarking*>* mFinals;
	static std::vector<ExtendedStateEquation*>* mSystems;
	static uint8_t firstID;
	static uint8_t lastID;
	static bool keepHistory;
};

#endif

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


	/// add a final marking and build an lp system
	static uint8_t* getXORTable(uint8_t firstOfInterest, uint8_t lastOfInterest);
	static uint8_t* getXORTable(uint8_t* eventsOfInterest, uint8_t count);

	/// Terminate the Linda Agent, clean up everything.
	static void finalize();

private:
	static pnapi::PetriNet* mNet;
	static std::vector<PartialMarking*>* mFinals;
	static std::vector<ExtendedStateEquation*>* mSystems;
	static uint8_t firstID;
	static uint8_t lastID;
	static bool keepHistory;
};

#endif

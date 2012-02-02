#ifndef TARA_H
#define TARA_H

#include <deque>
#include <pnapi/pnapi.h>
#include "MaxCost.h"

class Tara {
public:
    static unsigned int highestTransitionCosts;
    static unsigned int sumOfLocalMaxCosts;

    static unsigned int initialState;

    static unsigned int cost(pnapi::Transition*);

	//the partial CostFunction maps some transition to a natural number
	//this function can be completed by returning zero as default
	static std::map<pnapi::Transition* ,unsigned int> partialCostFunction;


    ///the input net
    static pnapi::PetriNet* net;

    // The actual inner graph, realized by a deque (quick insertion, quick access)
    static std::deque<innerState *> graph;
};

#endif

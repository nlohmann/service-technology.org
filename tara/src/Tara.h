#ifndef TARA_H
#define TARA_H

#include <deque>
#include <pnapi/pnapi.h>
#include <string>
#include <iostream>
#include <fstream>

#include "MaxCost.h"
#include "cmdline.h"
#include "verbose.h"
#include "config.h"
#include "config-log.h"
#include "Output.h"

class Tara {
public:
    /// costs of the most expensive transition
    static unsigned int highestTransitionCosts;

    static unsigned int sumOfLocalMaxCosts;

    /// the initial state of the taraGraph, parsed from Lola
    static unsigned int initialState;

    /**
     * @brief returns the cost of a given transition,
     * returns 0 if no costs for this transition are known
     */
    static unsigned int cost(pnapi::Transition*);

    /**
     * @brief the partial CostFunction maps some transition to a natural number
     * this function can be completed by returning zero as default
     */
    static std::map<pnapi::Transition* ,unsigned int> partialCostFunction;

    ///the input net
    static pnapi::PetriNet* net;

    /// The actual inner graph, realized by a deque (quick insertion, quick access)
    static std::deque<innerState *> graph;

    ///cmd-line options, see .ggo file for more information
    static gengetopt_args_info args_info;

    /// evaluate the command line parameters
    static void evaluateParameters(int argc, char** argv);

};

#endif

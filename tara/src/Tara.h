#ifndef TARA_H
#define TARA_H

#include <deque>
#include <string>
#include <iostream>
#include <fstream>

#include <pnapi/pnapi.h>
#include <lp_solve/lp_lib.h>

#include "Modification.h"
#include "MaxCost.h"
#include "cmdline.h"
#include "verbose.h"
#include "config.h"
#include "config-log.h"
#include "Output.h"
#include "Parser.h"

class Tara {
public:
    /// costs of the most expensive transition
    static unsigned int highestTransitionCosts;

    static unsigned int sumOfLocalMaxCosts;

    /// the initial state of the taraGraph, parsed from Lola
    static unsigned int initialState;

    // minimal costs (gna task#7709)
    static unsigned int minCosts;

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

    static bool isReset(pnapi::Transition*);
    static std::map<pnapi::Transition* ,bool> resetMap;

    ///the input net
    static pnapi::PetriNet* net;

    ///the input net
    static Modification* modification;

    /// The actual inner graph, realized by a deque (quick insertion, quick access)
    static std::deque<innerState *> graph;

    ///cmd-line options, see .ggo file for more information
    static gengetopt_args_info args_info;

    /// evaluate the command line parameters
    static void evaluateParameters(int argc, char** argv);

    static Output tempFile;

    /// The system used if the lp-heuristic is used
    static lprec* lp; 

    /// The number of final states in the inner graph
    static int nrOfEdges;

    /// The number of edges in the inner graph
    static int nrOfFinals;
    
    /// Constructs the linear program from the parsed graph
    static void constructLP();

    /// Solves the linear program
    static int solveLP();

    /// Delete the linear program
    static void deleteLP();

    /// Print the linear program
    static void printLP();

};

#endif

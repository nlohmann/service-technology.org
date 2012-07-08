/*!
\file Net.h
\author Karsten
\status approved 27.01.2012
\brief Global data for net structure

All data that describe the net structure can be found here. Additional
information for places only, or for transition only, is deffered to files
Place.* and Transition.*
*/

#pragma once
#include <Core/Dimensions.h>

/*!
\brief collection of information related to nodes

All data that describe the net structure can be found here. Additional
information for places only, or for transition only, is deffered to files
Place.* and Transition.*
*/
struct Net
{
    /// number of nodes: Card[PL] = places, Card[TR] = transitions
    static index_t Card[2];

    /// For each node, the number of nodes in pre- resp. post set.
    /// CardArcs[PL][PRE][17] is the number of transitions that produce on place 17.
    static index_t* CardArcs[2][2];

    /// For each node, the indices of nodes in pre- reps. post set.
    /// Arc[PL][POST][17][4] is the number of the 5th transition that consumes tokens from place 17.
    static index_t** Arc[2][2];

    /// for each node, the multiplicities of arcs in pre- resp. post set.
    static mult_t** Mult[2][2];

    /// names of the nodes
    /// Name[TR][15] is the name of transition 15.
    static char** Name[2];

    /// free all allocated memory
    static void deleteNodes();

    /// calculates progress measure for all transitions (used by sweep line method)
    static void setProgressMeasure();

    /// sorts a concrete pair of arcs and multiplicities (used as helper method for sortAllArcs)
    static void sortArcs(index_t* arcs, mult_t* mults, const index_t from, const index_t to);

    /// sorts all arcs
    static void sortAllArcs();

    /// swaps two places (does not care about ordering)
    static void swapPlaces(index_t, index_t);

    /// checks whether all arcs are properly (especially to both directions)
    static bool DEBUG__checkConsistency();

    /// checks whether all arcs are ordered properly
    static bool DEBUG__checkArcOrdering();

    /// print the net (using printf)
    static void print();

    /// assumes that raw net is read and places, transitions and the edges in-between are set properly. Computes additional net information used to speed up the simulation.
    static void preprocess();

private:
    // calculates DeltaT and DeltaHash for each transition
    static void preprocess_organizeDeltas();

    /// calculates the set of conflicting transitions for each transition
    static void preprocess_organizeConflictingTransitions();

    // moves all elements in the range [first1,last1), that are also in [first2,last2), to result.
    // returns the number of elements moved.
    static index_t set_moveall (index_t* first1, index_t* last1, index_t* first2, index_t* last2, index_t* result);

    /// calculates all significant places and changes order of places(!)
    static void setSignificantPlaces();
};

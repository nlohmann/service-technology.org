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

#include "Dimensions.h"

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

    /// Aufräumen der Knoten - Service für valgrind
    static void deleteNodes();

    static void print(); // for debugging
};

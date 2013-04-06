/*!
\file NetState.h
\author Christian K and Gregor B
\status unknown
*/

#pragma once
#include "Core/Dimensions.h"


/*!
\brief container object which includes all informations about the marking of a petri net

Basically this is a struct containing all information available and necessary about a marking.
This includes the marking itself, its hash, the enabled transitions and several further information,
to be able to efficiently calculate the NetState of all successor markings.

\author Christian K and Gregor B
 */
class NetState
{

public:
    NetState(): membersInitialized(false) {}
    ~NetState();

    //--------- COPIED FOR MARKING.h
    /// current  marking
    capacity_t* Current;

    /// hash value of initial marking
    hash_t HashCurrent;


    //--------- COPIED FROM TRANSITION.h
    /// Activation status
    bool* Enabled;

    /// number of enabled transitions
    index_t CardEnabled;

    /// If transition i is disabled, this is its position in its scapegpat's Disabled list
    index_t* PositionScapegoat;


    //--------- COPIED FROFM NET.h
    /// For each node, the indices of nodes in pre- reps. post set.
    /// Arc[PL][POST][17][4] is the number of the 5th transition that consumes tokens from place 17.
    index_t** Arc[2][2];

    /// for each node, the multiplicities of arcs in pre- resp. post set.
    mult_t** Mult[2][2];


    //--------- COPIED PLACE.h
    /// Number of transitions for which this place is the distinguished insufficiently marked Pre-place (scapegoat)
    index_t* CardDisabled;

    /// These are the transitions for which this place is the scapegoat.
    index_t** Disabled;

    /// create a NetState object from the global variables set by the parsing process
    static NetState* createNetStateFromInitial();
    /// copy constructor
    NetState(const NetState &ns);
    // copy operator, be aware that this changes the given ns (this is necessary to ensure proper deallocation of memory)
    NetState &operator=(const NetState &ns);

    // swaps all internal members with the given NetState
    void swap(NetState &ns);

private:
    /// marker variable, whether we have to delete some of the arrays ourself
    bool membersInitialized;
    /// delete all members
    void deleteAllMembers();
};

/*!
\file NetState.h
\author Christian K and Gregor B
\status unknown
\brief

*/

#pragma once
#include "Core/Dimensions.h"


class NetState
{

    public:
        NetState(): need_to_delete_members_on_delete(false) {}
        ~NetState();

        /////// COPIED FOR MARKING.h
        /// current  marking
        capacity_t* Current;

        /// hash value of initial marking
        hash_t HashCurrent;


        ////// COPIED FROM TRANSITION.h
        /// Activation status
        bool* Enabled;

        /// number of enabled transitions
        index_t CardEnabled;

        /// If transition i is disabled, this is its position in its scapegpat's Disabled list
        index_t* PositionScapegoat;


        //////// COPIED FROFM NET.h
        /// For each node, the indices of nodes in pre- reps. post set.
        /// Arc[PL][POST][17][4] is the number of the 5th transition that consumes tokens from place 17.
        index_t** Arc[2][2];

        /// for each node, the multiplicities of arcs in pre- resp. post set.
        mult_t** Mult[2][2];


        /////// COPIED PLACE.h
        /// Number of transitions for which this place is the distinguished insufficiently marked Pre-place (scapegoat)
        index_t* CardDisabled;

        /// These are the transitions for which this place is the scapegoat.
        index_t** Disabled;

        static NetState* createNetStateFromInitial();
        static NetState* createNetStateFromCurrent(NetState &ns);
        void copyNetState(NetState &ns);

    private:
        bool need_to_delete_members_on_delete;
        void deleteAllMembers();
};

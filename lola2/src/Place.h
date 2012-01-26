/*!

\file Place.h
\author Karsten
\status new
\brief Global data for place specific information

All data that describe attributes of places can be found here. General information for a place
in its role as a node, ar contained in Node.*
*/

#pragma once

#include <stdint.h>
// Dimensions.h has a type index_type that has at least max(nr of places,nr of transitions) elements
#include "Dimensions.h"

/*!

\brief collection of information related to places

*/

struct Place
{
    public:
        /// number of signifcant places; significant places aare such that comparing markings on the
        /// significant places suffices for checking equality
        static index_type CardSignificant;

        /// The hash factor for a place. We compute hash values for places as
        /// sum_{p in P} m(p)*hash(p)
        static unsigned int* Hash;

        /// The maximum number of tokens that, according to input specification, can ever
        /// be prsent on this place. This is only used for calculating a dense representation of a
        /// marking, not as a blocker for transition activation.
        static capacity_type* Capacity;

        /// The number of bits needed for representing the numbers 0 .. Capacity[i]
        /// Used for handling dense representations of markings
        static uint8_t*   CardBits;

        /// Number of transitions for which this place is the distinguushed insufficiently marked
        /// Pre-place (scapegoat)
        static index_type* CardDisabled;

        /// These are the transitions for which this place is the scapegoat
        /// The arrays Disabled[i] are allocated for the maximum number of such transitions
        /// which is card(p\bullet)
        static index_type** Disabled;
};


/// Aufräumen der Knoten - Service für valgrind
extern void deletePlaces();

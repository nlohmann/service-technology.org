/*!
\file Marking.h
\author Karsten
\status new
\brief Global data for marking specific information

All data that describe attributes of markings can be found here.
*/

#pragma once

#include "Dimensions.h"

/*!
\brief collection of information related to markings
*/
struct Marking
{
    public:
        /// initial marking
        static capacity_type* Initial;

        /// hash value of initial marking
        static unsigned int HashInitial;

        /// current  marking
        static capacity_type* Current;

        /// hash value of initial marking
        static unsigned int HashCurrent;

        /// target marking
        static capacity_type* Target;

        /// hash value of target marking
        static unsigned int HashTarget;
};

/// Aufräumen der Knoten - Service für valgrind
extern void deleteMarkings();

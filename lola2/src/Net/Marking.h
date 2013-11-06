/*!
 \file Marking.h
 \author Karsten
 \status approved 27.01.2012

 \brief Global data for marking specific information

 All data that describe attributes of markings can be found here.
 */

#pragma once
#include <Core/Dimensions.h>

/*!
 \brief collection of information related to markings
 */
struct Marking
{
public:
    /// initial marking
    static capacity_t *Initial;

    /// hash value of initial marking
    static hash_t HashInitial;

    /// current  marking
    static capacity_t *Current;

    /// hash value of initial marking
    static hash_t HashCurrent;

    /// target marking
    static capacity_t *Target;

    /// hash value of target marking
    static hash_t HashTarget;

    /// init for the behavioral part of the net
    static void init();

    /// Aufräumen der Knoten - Service für valgrind
    static void deleteMarkings();

    static void DEBUG__printMarking(capacity_t *marking);
};

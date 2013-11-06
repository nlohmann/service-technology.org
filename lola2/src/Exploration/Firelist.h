/*!
\file Firelist.h
\author Karsten
\status new
\brief class for firelist generation. Default is firelist consisting of all enabled transitions.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Net/NetState.h>

class SimpleProperty;

/*!
\brief Class for firelist generation. Default is firelist consisting of all enabled transitions.

Base class for all firelists. By default a firelist contains all enabled transition.
A firelist may contain not all enabled transtitions, but must not contain transition, which are not enabled.

\author Karsten
*/
class Firelist
{
public:
    /*!
    \brief return value contains number of elements in fire list, argument is reference parameter for actual list
    \param ns the net-state for which the firelist should be determined.
    */
    virtual index_t getFirelist(NetState &ns, index_t **);
    virtual ~Firelist() {}

    /*!
     \brief create a new firelist for the given simple property

     This function will create a new firelist of the same type as the current one based on the given simple property.
     in some cases this property will be ignored, but if this is a stubborn-state firelist the property will be used.

     This function is essential for the parallel exploration of the state space.
     Each thread will have its own firelist with the current simple property.
     */
    virtual Firelist *createNewFireList(SimpleProperty *property);
};

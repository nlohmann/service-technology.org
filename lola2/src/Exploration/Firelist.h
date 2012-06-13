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

class Firelist
{
    public:
        /// return value contains number of elements in fire list, argument is reference
        /// parameter for actual list
        virtual index_t getFirelist(NetState* ns, index_t**);
        virtual ~Firelist() {}
};

class FireListCreator
{
    public:
        virtual Firelist* createFireList(SimpleProperty* sp);
};

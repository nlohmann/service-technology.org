/*!
\file Firelist.cc
\author Karsten
\status new

\brief Class for firelist generation. Default is firelist consisting of all enabled transitions.
*/

#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Exploration/Firelist.h>


index_t Firelist::getFirelist(NetState &ns, index_t** result)
{
    assert(ns.CardEnabled <= Net::Card[TR]);

    *result = new index_t[ns.CardEnabled];
    index_t i = 0;
    for (index_t t = 0; t < Net::Card[TR]; ++t)
    {
        if (ns.Enabled[t])
        {
            assert(i < ns.CardEnabled);
            (*result)[i++] = t;
        }
    }
    return ns.CardEnabled;
}


Firelist* FireListCreator::createFireList(SimpleProperty*)
{
    return new Firelist();
}

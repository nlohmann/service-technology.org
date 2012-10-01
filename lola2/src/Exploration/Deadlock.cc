/*!
\file Deadlock.cc
\author Karsten
\status approved 18.04.2012

\brief property to find deadlocks
*/

#include <cstring>
#include <cstdio>
#include <Net/Transition.h>
#include <Exploration/Deadlock.h>


bool Deadlock::initProperty(NetState &ns)
{
    return !ns.CardEnabled;
}

bool Deadlock::checkProperty(NetState &ns, index_t t)
{
    return !ns.CardEnabled;
}

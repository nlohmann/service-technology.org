/*!
\author Gregor
\file SIStore.h
\status new
*/

#pragma once
#include <config.h>
#include <pthread.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "Net/NetState.h"

class SIStore
{
public:
    SIStore(int numberOfThreads): number_of_threads(numberOfThreads) {};
    virtual ~SIStore() {};

    /// check whether current marking is stored
    virtual bool search(NetState* ns, int threadNumber) = 0;
    virtual bool insert(NetState* ns, int threadNumber) = 0;
protected:
    int number_of_threads;
};

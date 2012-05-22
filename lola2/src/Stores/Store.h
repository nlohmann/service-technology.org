/*!
\author Niels
\file Store.h
\status new
*/

#pragma once
#include <config.h>
#include <pthread.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif


class State;

/*!
\example

State *s = NULL;

if (searchAndInsert(&s))
{
    // already there - work with *s;
}
else
{
    // *s has just been created
}
*/
class Store
{
    private:
        /// a thread that runs the frequent reports
        pthread_t reporter_thread;

        /// the reporter function
        void* reporter_internal(void);

        /// a helper function to start the reporter thread
        static void* reporter_helper(void* context);

    public:
        /// the number of stored markings
        uint64_t markings;

        /// the number of calles to searchAndInsert()
        uint64_t calls;

        Store();

        virtual ~Store();

        /// check whether current marking is stored
         virtual bool searchAndInsert() = 0;

        /// check whether current marking is stored
        /// threaded version, this have not to be implemented by each store
        virtual bool searchAndInsert(int thread);

        /// check whether current marking is stored and return state
        virtual bool searchAndInsert(State** s) = 0;
};

#pragma once

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
    public:
        /// the number of stored markings
        uint64_t markings;

        /// the number of calles to searchAndInsert()
        uint64_t calls;

        Store() : markings(0), calls(0) {}

        virtual ~Store() = 0;

        /// check whether current marking is stored
        virtual bool searchAndInsert() = 0;

        /// check whether current marking is stored and return state
        virtual bool searchAndInsert(State **s) = 0;
};

// class BinStore : public Store;
// class BDDStore : public Store;
// class HashStore : public Store;
// class NaiveStore : public Store;

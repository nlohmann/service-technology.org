#ifndef HASHREGIONS_H_
#define HASHREGIONS_H_

#include "Region.h"

const int HASH_SIZE=2000;

//template<>
struct hashRegion
{
    size_t operator()(Region *a) const {

        size_t h  = a->cache_key() % HASH_SIZE;
//	 cout << "              key : " << h << endl;
        return h;
    }
};


class KeysReg
{
public :

    bool operator() (Region *a, Region *b) const
    {
        return *a == *b;
    }

};


struct hashSS
{
    size_t operator()(SS a) const {

        size_t h  = a.getNodeSize() % HASH_SIZE;
//	 cout << "           bdd key: " << h << endl;
        return h;
    }
};

class KeysSS
{
public :

    bool operator() (const SS a, const SS b) const
    {
        return a == b;
    }

};


#endif /*HASHREGIONS_H_*/

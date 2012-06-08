/*!
\file BinStore2.cc
\author Karsten, Christian Koch
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#include <cstdlib>
#include <cstdio>
#include <config.h>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/BinStore2.h>

class State;


BinStore2::BinStore2()
{
    branch = (Decision**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    firstvector = (vectordata_t**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    rwlocks =(pthread_rwlock_t*) calloc(sizeof(pthread_rwlock_t), SIZEOF_MARKINGTABLE);

    pthread_mutex_init(&inc_mutex, NULL);
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    	pthread_rwlock_init(rwlocks+i, NULL);

    // initialize bit masks
    //   capacity_t tmp1=1;
    //    for(int i=1; i<PLACE_WIDTH; i++) {
    //        tmp1 <<= 1;
    //        place_bitmask[i] = tmp1-1;
    //    }
    //   place_bitmask[PLACE_WIDTH] = (tmp1-1)+tmp1;

    //    vectordata_t tmp2=1;
    //    for(int i=1; i<VECTOR_WIDTH; i++) {
    //        tmp2 <<= 1;
    //        vector_bitmask[i] = tmp2-1;
    //    }
    //    vector_bitmask[VECTOR_WIDTH] = (tmp2-1)+tmp2;
}

BinStore2::~BinStore2()
{
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        if (branch[i])
        {
            delete branch[i];
        }
        if (firstvector[i])
        {
            free(firstvector[i]);
        }

    }
    free(branch);
    free(firstvector);
}

/// create a new branch in the decision tree at depth b.
BinStore2::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL), state(0)
{
}

BinStore2::Decision::~Decision()
{
    free(vector);
    if (nextnew)
    {
        delete nextnew;
    }
    if (nextold)
    {
        delete nextold;
    }

}

/// search for a state in the binStore and insert it, if it is not there
/// Do not care about states
bool BinStore2::searchAndInsert(NetState* ns)
{
	pthread_mutex_lock(&inc_mutex);
    ++calls;
	pthread_mutex_unlock(&inc_mutex);
    // the general assumption is that we read marking, vectors etc. left to right, with
    // low indices left, high indices right,
    // msb left and lsb right.

    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the vector we are currently investigating
    vectordata_t* currentvector;

    /// the place where the new vector goes to
    vectordata_t** newvector;

    /// the place we are currently dealing with
    index_t place_index = 0;

    /// the bits of the place's marking we have NOT dealt with so far
    bitindex_t place_bitstogo = Place::CardBits[0]; // indicates start with msb

    /// the index in the vector we are currently dealing with
    index_t vector_index = 0;

    /// the bits of the current vector's data we have NOT dealt with so far
    bitindex_t vector_bitstogo = VECTOR_WIDTH;

    /// the number of bits processed until reaching the current branch
    bitindex_t position = 0;


    // first searching, so lock the search lock
    pthread_rwlock_wrlock(rwlocks + ns->HashCurrent);

    // Is hash bucket empty? If so, assign to currentvector
    if (!(currentvector = (firstvector[ns->HashCurrent])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = firstvector + ns->HashCurrent;
    }
    else
    {
        // Here, hash bucket is not empty.
        anchor = branch + ns->HashCurrent;

        while (true)
        {
            bitindex_t comparebits;
            while (true)
            {
                if (place_bitstogo < vector_bitstogo)
                {
                    if ((capacity_t(ns->Current[place_index] << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - place_bitstogo))
                            //                    if ((Marking::Current[place_index] & place_bitmask[place_bitstogo])
                            ==
                            (vectordata_t(currentvector[vector_index] << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - place_bitstogo)))
                    {
                        vector_bitstogo -= place_bitstogo;
                        place_index++;
                        if (place_index < Place::CardSignificant)
                        {
                            place_bitstogo = Place::CardBits[place_index];
                        }
                        else
                        {
                        	pthread_rwlock_unlock(rwlocks + ns->HashCurrent);
                            return true;
                        }
                    }
                    else
                    {
                        comparebits = place_bitstogo >> 1;
                        break;
                    }
                }
                else if (place_bitstogo > vector_bitstogo)
                {
                    if ((capacity_t(ns->Current[place_index] << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - vector_bitstogo))
                            ==
                            (vectordata_t(currentvector[vector_index] << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - vector_bitstogo)))
                        //                            (currentvector[vector_index] & vector_bitmask[vector_bitstogo]))
                    {
                        place_bitstogo -= vector_bitstogo;
                        vector_index++, vector_bitstogo = VECTOR_WIDTH;
                    }
                    else
                    {
                        comparebits = vector_bitstogo >> 1;
                        break;
                    }
                }
                else
                {
                    if ((capacity_t(ns->Current[place_index] << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - place_bitstogo))
                            //                    if ((Marking::Current[place_index] & place_bitmask[place_bitstogo])
                            ==
                            (vectordata_t(currentvector[vector_index] << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - vector_bitstogo)))
                        //                            (currentvector[vector_index] & vector_bitmask[vector_bitstogo]))
                    {
                        vector_index++, vector_bitstogo = VECTOR_WIDTH;
                        place_index++;
                        if (place_index < Place::CardSignificant)
                        {
                            place_bitstogo = Place::CardBits[place_index];
                        }
                        else
                        {
                        	pthread_rwlock_unlock(rwlocks + ns->HashCurrent);
                            return true;
                        }
                    }
                    else
                    {
                        comparebits = place_bitstogo >> 1;
                        break;
                    }
                }
            }
            while (comparebits)
            {
                if ((capacity_t(ns->Current[place_index] << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - comparebits))
                        ==
                        (vectordata_t(currentvector[vector_index] << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - comparebits)))
                {
                    vector_bitstogo -= comparebits;
                    place_bitstogo -= comparebits;
                    if (comparebits > vector_bitstogo)
                    {
                        comparebits = vector_bitstogo;
                    }
                    if (comparebits > place_bitstogo)
                    {
                        comparebits = place_bitstogo;
                    }
                }
                else
                {
                    comparebits >>= 1;
                }
            }
            while ((*anchor) && (position + vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo)) > (*anchor)->bit)
            {
                anchor = &((*anchor)->nextold);
            }
            if ((*anchor) && (*anchor)->bit == (position + vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo)))
            {
                currentvector = (* anchor) -> vector;
                anchor = &((* anchor) -> nextnew);

                position += (vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo)) + 1;
                vector_index = 0;
                vector_bitstogo = VECTOR_WIDTH;

                place_bitstogo--;
                if (!place_bitstogo)
                {
                    place_index++;
                    if (place_index < Place::CardSignificant)
                    {
                        place_bitstogo = Place::CardBits[place_index];
                    }
                    else
                    {
                    	pthread_rwlock_unlock(rwlocks + ns->HashCurrent);
                        return true;
                    }
                }
            }
            else
            {
                break;
            }
        }

        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(position + vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo));
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
        position += (vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo)) + 1;
        vector_index = 0;
        vector_bitstogo = VECTOR_WIDTH;

        place_bitstogo--;
        if (!place_bitstogo)
        {
            place_index++;
            if (place_index < Place::CardSignificant)
            {
                place_bitstogo = Place::CardBits[place_index];
            }
            else
            {
                * newvector = NULL;
                pthread_mutex_lock(&inc_mutex);
                ++markings;
                pthread_mutex_unlock(&inc_mutex);
            	pthread_rwlock_unlock(rwlocks + ns->HashCurrent);
               return false;
            }
        }
    }

    // compress current marking into bitvector
    // we assume that place_index, placebit_index, position have the correct
    // initial values
    *newvector = (vectordata_t*) calloc(((Place::SizeOfBitVector - (position + vector_index * VECTOR_WIDTH + (VECTOR_WIDTH - vector_bitstogo))) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH, sizeof(vectordata_t));


    while (true)
    {
        bitindex_t insertbits = place_bitstogo < vector_bitstogo ? place_bitstogo : vector_bitstogo;
        (*newvector)[vector_index] |= vectordata_t((capacity_t(ns->Current[place_index] << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - insertbits)) << (vector_bitstogo - insertbits));

        if (vector_bitstogo == insertbits)
        {
            vector_index++, vector_bitstogo = VECTOR_WIDTH;
        }
        else
        {
            vector_bitstogo -= insertbits;
        }
        if (place_bitstogo == insertbits)
        {
            place_index++;
            if (place_index < Place::CardSignificant)
            {
                place_bitstogo = Place::CardBits[place_index];
            }
            else
            {
            	pthread_mutex_lock(&inc_mutex);
                ++markings;
                pthread_mutex_unlock(&inc_mutex);
            	pthread_rwlock_unlock(rwlocks + ns->HashCurrent);
                return false;
            }
        }
        else
        {
            place_bitstogo -= insertbits;
        }
    }
}

// LCOV_EXCL_START
bool BinStore2::searchAndInsert(NetState* ns,State** result)
{
    assert(false);
}
// LCOV_EXCL_STOP

/** may not work anymore due to variable vector data size

void BinStore2::pbs(unsigned int b, unsigned int p, vectordata_t* f, void* v)
{

    int i;
    Decision* d = (Decision*) v;
    if (d)
    {
        pbs(d -> bit + 1, 0, d -> vector, d -> nextnew);
    }
    for (i = 0; i < b; i++)
    {
        printf(" ");
    }
    for (; i <= (d ? d->bit : Place::SizeOfBitVector - 1); i++)
    {
        if (f[(i - b + p) / 8] & (1 << (7 - (i - b + p) % 8)))
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    printf("\n");

    if (d)
    {
        pbs(d->bit + 1, p + d->bit - b + 1, f , d -> nextold);
    }
}
void BinStore2::printBinStore()
{
    for (unsigned int i = 0; i < Place::CardSignificant; i++)
    {
        printf("%s:%d ", Net::Name[PL][i], Marking::Current[i]);
    }
    printf("\n\n");

    for (unsigned int i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        if (firstvector[i])
        {
            printf("\n hash value %u\n", i);
            BinStore2::pbs(0, 0, firstvector[i], branch[i]);
            printf("\n");
        }
    }

}*/

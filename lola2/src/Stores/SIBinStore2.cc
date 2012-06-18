/*!
 \file SIBinStore2.cc
 \author Karsten, Christian Koch, Gregor B.
 \status new
 \brief implement Store as a decision tree with bitvectors.
 */

#include <cstdlib>
#include <cstdio>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/SIBinStore2.h>

class State;

SIBinStore2::SIBinStore2(int threadnum) :
    SIStore(threadnum)
{
    branch = (Decision**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    firstvector = (vectordata_t**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    rwlocks = (pthread_rwlock_t*) calloc(sizeof(pthread_rwlock_t),
                                         SIZEOF_MARKINGTABLE);
    // init the rwlocks
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        pthread_rwlock_init(rwlocks + i, NULL);
    }

    g_anchor = (Decision***) calloc(threadnum, sizeof(Decision**));
    g_currentvector = (vectordata_t**) calloc(threadnum, sizeof(vectordata_t*));
    g_newvector = (vectordata_t***) calloc(threadnum, sizeof(vectordata_t**));
    g_place_index = (index_t*) calloc(threadnum, sizeof(index_t));
    g_place_bitstogo = (bitindex_t*) calloc(threadnum, sizeof(bitindex_t));
    g_vector_index = (index_t*) calloc(threadnum, sizeof(index_t));
    g_vector_bitstogo = (bitindex_t*) calloc(threadnum, sizeof(bitindex_t));
    g_position = (bitindex_t*) calloc(threadnum, sizeof(bitindex_t));
    g_prepareInsertion = (bool*) calloc(threadnum, sizeof(bool));
    g_validInsertInformation = (bool*) calloc(threadnum, sizeof(bool));
    g_currentHashs = (hash_t*) calloc(threadnum, sizeof(hash_t));

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

SIBinStore2::~SIBinStore2()
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
SIBinStore2::Decision::Decision(bitindex_t b) :
    bit(b), nextold(NULL), nextnew(NULL)//, state(0)
{
}

SIBinStore2::Decision::~Decision()
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

bool SIBinStore2::search(NetState &ns, int threadNumber)
{
    return search(ns, threadNumber, true);
}

/// search for a state in the binStore and insert it, if it is not there
/// Do not care about states
bool SIBinStore2::search(NetState &ns, int threadNumber,
                         bool lock_rw_mutex)
{

    if (lock_rw_mutex)
    {
        pthread_rwlock_rdlock(rwlocks + ns.HashCurrent);
    }

    Decision** anchor;
    vectordata_t* currentvector;
    vectordata_t** newvector;
    index_t place_index;
    bitindex_t place_bitstogo;
    index_t vector_index;
    bitindex_t vector_bitstogo;
    bitindex_t position;
    bool prepareInsertion;
    // return value
    uint64_t returnValue;

    //++calls;;
    // the general assumption is that we read marking, vectors etc. left to right, with
    // low indices left, high indices right,
    // msb left and lsb right.

    place_index = 0;
    place_bitstogo = Place::CardBits[0]; // indicates start with msb
    vector_index = 0;
    vector_bitstogo = VECTOR_WIDTH;
    position = 0;

    prepareInsertion = false;

    // Is hash bucket empty? If so, assign to currentvector
    if (!(currentvector = (firstvector[ns.HashCurrent])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = firstvector + ns.HashCurrent;
    }
    else
    {
        // Here, hash bucket is not empty.
        anchor = branch + ns.HashCurrent;

        while (true)
        {
            bitindex_t comparebits;
            while (true)
            {
                if (place_bitstogo < vector_bitstogo)
                {
                    if ((capacity_t(
                                ns.Current[place_index]
                                << (PLACE_WIDTH - place_bitstogo))
                            >> (PLACE_WIDTH - place_bitstogo))
                            //                    if ((ns.Current[place_index] & place_bitmask[place_bitstogo])
                            == (vectordata_t(
                                    currentvector[vector_index]
                                    << (VECTOR_WIDTH - vector_bitstogo))
                                >> (VECTOR_WIDTH - place_bitstogo)))
                    {
                        vector_bitstogo -= place_bitstogo;
                        place_index++;
                        if (place_index < Place::CardSignificant)
                        {
                            place_bitstogo = Place::CardBits[place_index];
                        }
                        else
                        {
                            if (lock_rw_mutex)
                                pthread_rwlock_unlock(
                                    rwlocks + ns.HashCurrent);
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
                    if ((capacity_t(
                                ns.Current[place_index]
                                << (PLACE_WIDTH - place_bitstogo))
                            >> (place_bitstogo - vector_bitstogo))
                            == (vectordata_t(
                                    currentvector[vector_index]
                                    << (VECTOR_WIDTH - vector_bitstogo))
                                >> (VECTOR_WIDTH - vector_bitstogo)))
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
                    if ((capacity_t(
                                ns.Current[place_index]
                                << (PLACE_WIDTH - place_bitstogo))
                            >> (PLACE_WIDTH - place_bitstogo))
                            //                    if ((ns.Current[place_index] & place_bitmask[place_bitstogo])
                            == (vectordata_t(
                                    currentvector[vector_index]
                                    << (VECTOR_WIDTH - vector_bitstogo))
                                >> (VECTOR_WIDTH - vector_bitstogo)))
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
                            if (lock_rw_mutex)
                                pthread_rwlock_unlock(
                                    rwlocks + ns.HashCurrent);
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
                if ((capacity_t(
                            ns.Current[place_index]
                            << (PLACE_WIDTH - place_bitstogo))
                        >> (PLACE_WIDTH - comparebits))
                        == (vectordata_t(
                                currentvector[vector_index]
                                << (VECTOR_WIDTH - vector_bitstogo))
                            >> (VECTOR_WIDTH - comparebits)))
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
            while ((*anchor)
                    && (position + vector_index * VECTOR_WIDTH
                        + (VECTOR_WIDTH - vector_bitstogo)) > (*anchor)->bit)
            {
                anchor = &((*anchor)->nextold);
            }
            if ((*anchor)
                    && (*anchor)->bit
                    == (position + vector_index * VECTOR_WIDTH
                        + (VECTOR_WIDTH - vector_bitstogo)))
            {
                currentvector = (*anchor)->vector;
                anchor = &((*anchor)->nextnew);

                position += (vector_index * VECTOR_WIDTH
                             + (VECTOR_WIDTH - vector_bitstogo)) + 1;
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
                        if (lock_rw_mutex)
                        {
                            pthread_rwlock_unlock(rwlocks + ns.HashCurrent);
                        }
                        return true;
                    }
                }
            }
            else
            {
                break;
            }
        }

        prepareInsertion = true;
    }

    g_anchor[threadNumber] = anchor;
    g_currentvector[threadNumber] = currentvector;
    g_newvector[threadNumber] = newvector;
    g_place_index[threadNumber] = place_index;
    g_place_bitstogo[threadNumber] = place_bitstogo;
    g_vector_index[threadNumber] = vector_index;
    g_vector_bitstogo[threadNumber] = vector_bitstogo;
    g_position[threadNumber] = position;
    g_prepareInsertion[threadNumber] = prepareInsertion;
    g_validInsertInformation[threadNumber] = true;
    g_currentHashs[threadNumber] = ns.HashCurrent;

    if (lock_rw_mutex)
    {
        pthread_rwlock_unlock(rwlocks + ns.HashCurrent);
    }
    return false;
}

bool SIBinStore2::insert(NetState &ns, int threadNumber)
{

    pthread_rwlock_wrlock(rwlocks + ns.HashCurrent);

    // if the currently stored informations are not valid we need a new search run to get them
    if (!g_validInsertInformation[threadNumber])
    {
        uint64_t searchResult = search(ns, threadNumber, false);
        // maybe the value is not acutally in the store
        if (searchResult == 0)
        {
            //printf("INSERT, but is in\n");
            pthread_rwlock_unlock(rwlocks + ns.HashCurrent);
            return true;
        }
    }


    Decision** anchor;
    vectordata_t* currentvector;
    vectordata_t** newvector;
    index_t place_index;
    bitindex_t place_bitstogo;
    index_t vector_index;
    bitindex_t vector_bitstogo;
    bitindex_t position;
    bool prepareInsertion;

    anchor = g_anchor[threadNumber];
    currentvector = g_currentvector[threadNumber];
    newvector = g_newvector[threadNumber];
    place_index = g_place_index[threadNumber];
    place_bitstogo = g_place_bitstogo[threadNumber];
    vector_index = g_vector_index[threadNumber];
    vector_bitstogo = g_vector_bitstogo[threadNumber];
    position = g_position[threadNumber];
    prepareInsertion = g_prepareInsertion[threadNumber];

    for (int i = 0; i < number_of_threads; i++)
        if (g_currentHashs[i] == g_currentHashs[threadNumber])
        {
            g_validInsertInformation[i] = false;
        }

    if (prepareInsertion)
    {

        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(
            position + vector_index * VECTOR_WIDTH
            + (VECTOR_WIDTH - vector_bitstogo));
        newdecision->nextold = *anchor;
        *anchor = newdecision;
        newdecision->nextnew = NULL;
        newvector = &(newdecision->vector);
        // the mismatching bit itself is not represented in the new vector
        position += (vector_index * VECTOR_WIDTH
                     + (VECTOR_WIDTH - vector_bitstogo)) + 1;
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
                *newvector = NULL;
                //++markings;
                pthread_rwlock_unlock(rwlocks + ns.HashCurrent);
                return false;
            }
        }
    }

    // compress current marking into bitvector
    // we assume that place_index, placebit_index, position have the correct
    // initial values
    *newvector = (vectordata_t*) calloc(
                     ((Place::SizeOfBitVector
                       - (position + vector_index * VECTOR_WIDTH
                          + (VECTOR_WIDTH - vector_bitstogo)))
                      + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH, sizeof(vectordata_t));

    while (true)
    {
        bitindex_t insertbits =
            place_bitstogo < vector_bitstogo ?
            place_bitstogo : vector_bitstogo;
        (*newvector)[vector_index] |= vectordata_t(
                                          (capacity_t(
                                               ns.Current[place_index]
                                               << (PLACE_WIDTH - place_bitstogo))
                                           >> (PLACE_WIDTH - insertbits))
                                          << (vector_bitstogo - insertbits));

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
                //++markings;
                pthread_rwlock_unlock(rwlocks + ns.HashCurrent);
                return false;
            }
        }
        else
        {
            place_bitstogo -= insertbits;
        }
    }
}

//bool SIBinStore2::searchAndInsert(State** result) {}

/** may not work anymore due to variable vector data size

 void SIBinStore2::pbs(unsigned int b, unsigned int p, vectordata_t* f, void* v)
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
 void SIBinStore2::printBinStore()
 {
 for (unsigned int i = 0; i < Place::CardSignificant; i++)
 {
 printf("%s:%d ", Net::Name[PL][i], ns.Current[i]);
 }
 printf("\n\n");

 for (unsigned int i = 0; i < SIZEOF_MARKINGTABLE; i++)
 {
 if (firstvector[i])
 {
 printf("\n hash value %u\n", i);
 SIBinStore2::pbs(0, 0, firstvector[i], branch[i]);
 printf("\n");
 }
 }

 }*/


// LCOV_EXCL_START
SIBinStore2* createSIBinStore2(int threads)
{
    return new SIBinStore2(threads);
}
// LCOV_EXCL_STOP

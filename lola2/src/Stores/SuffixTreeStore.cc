/*!
\file SuffixTreeStore.cc
\author Christian Koch
\status new
\brief abstract Store using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/SuffixTreeStore.h>

class State;


SuffixTreeStore::SuffixTreeStore()
{
    pthread_mutex_init(&inc_mutex, NULL);

    branch = (Decision**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    firstvector = (vectordata_t**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    rwlocks =(pthread_rwlock_t*) calloc(sizeof(pthread_rwlock_t), SIZEOF_MARKINGTABLE);

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_init(rwlocks+i, NULL);
}

SuffixTreeStore::~SuffixTreeStore()
{
    pthread_mutex_destroy(&inc_mutex);
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

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_destroy(rwlocks+i);
    free(rwlocks);
}

/// search for a state in the suffix tree and insert it, if it is not present
bool SuffixTreeStore::searchAndInsert(NetState* ns, void** s)
{
    pthread_mutex_lock(&inc_mutex);
    ++calls;
    pthread_mutex_unlock(&inc_mutex);

    bitindex_t bitlen;
    input_t* in = getInput(ns, bitlen);

    bool ret = searchAndInsert(in,bitlen,ns->HashCurrent);
    if(!ret) {
        pthread_mutex_lock(&inc_mutex);
        ++markings;
        pthread_mutex_unlock(&inc_mutex);
    }
    return ret;
}


/// create a new branch in the decision tree at depth b.
SuffixTreeStore::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL)
{
}

SuffixTreeStore::Decision::~Decision()
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

/// search for an input vector in the suffix tree and insert it, if it is not present
bool SuffixTreeStore::searchAndInsert(input_t* in, bitindex_t bitlen, hash_t hash)
{
    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the place where the new vector goes to
    vectordata_t** newvector;

    /// the input word we are currently dealing with
    index_t input_index = 0;

    /// the last input word to consider.
    index_t max_input_index = (bitlen - 1) / INPUT_WIDTH;

    /// the number of significant bits in the last input word (aligned at msb)
    index_t max_input_numbits = bitlen%INPUT_WIDTH;
    if(!max_input_numbits) max_input_numbits = INPUT_WIDTH;

    /// the bits of the current input word we have NOT dealt with so far
    bitindex_t input_bitstogo = INPUT_WIDTH; // indicates start with msb

    // pointer to the current input word
    input_t* pInput = in;


    /// the vector word we are currently investigating
    vectordata_t* pVector;

    /// the bits of the current vector word we have NOT dealt with so far
    bitindex_t vector_bitstogo = VECTOR_WIDTH; // indicates start with msb

    /// the number of bits processed since reaching the current branch
    bitindex_t position = 0;

    // lock the current bucket to ensure thread safety
    pthread_rwlock_wrlock(rwlocks + hash);

    // Is hash bucket empty? If so, assign to currentvector
    if (!(pVector = (firstvector[hash])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = firstvector + hash;
    }
    else
    {
        // Here, hash bucket is not empty.
        anchor = branch + hash;

        while (true) // just entered new suffix tree vector
        {
            // number of bits to compare at once, searching for differences. Used only to locate the exact bit position of the difference
            bitindex_t comparebits;

            // maximum number of vector words to consider. The actual vector (pVector) can be smaller, but in this case a difference is found before reaching the end anyway (see getInput rule 2).
            index_t vectorlen = ((bitlen - position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;

// try to use more efficient implementation relying on input and vector using the same data type
#if VECTOR_WIDTH == INPUT_WIDTH
            // test for good alignment
            if(input_bitstogo == vector_bitstogo) {
                // good alignment, can use memcmp
                if(!memcmp(pInput,pVector,vectorlen*sizeof(input_t))) {
                    // match found, we're done
                    pthread_rwlock_unlock(rwlocks + hash);
                    return true;
                }
                // difference found, skip to first differing word.
                while(*pInput == *pVector) // reaching end of input is impossible
                {
                    position+=VECTOR_WIDTH, pVector++;
                    input_index++, ++pInput;
                }
                comparebits = VECTOR_WIDTH >> 1; // initialize binary search for differing bit
            }
            else // bad alignment; input_bitstogo < vector_bitstogo, since vector_bitstogo is always VECTOR_WIDTH at this point
            {
                while(true) { // vector_bitstogo == VECTOR_WIDTH == INPUT_WIDTH
                    // compare remaining input bits with msb bits of current vector
                    if ((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)))
                            ==
                            (vectordata_t((*pVector >> (VECTOR_WIDTH - input_bitstogo)) << (VECTOR_WIDTH - input_bitstogo))))
                    {
                        // they're equal, input word done. Test for EOI
                        if(++input_index <= max_input_index) {
                            // compare msb of next input word with the remaining bits of the current vector word
                            if ((input_t(*(++pInput) >> input_bitstogo) << input_bitstogo)
                                    ==
                                    (vectordata_t(*pVector << input_bitstogo)))
                            {
                                // they're equal, vector word done. Test for EOV and repeat with next vector word.
                                if(--vectorlen) {
                                    position += VECTOR_WIDTH, pVector++;
                                }
                                else {
                                    pthread_rwlock_unlock(rwlocks + hash);
                                    return true;
                                }
                            }
                            else
                            {
                                // difference found. Update bitstogo variables and setup binary search for differing bit
                                vector_bitstogo -= input_bitstogo;
                                input_bitstogo = INPUT_WIDTH;
                                comparebits = vector_bitstogo >> 1;
                                break;
                            }
                        } else {
                            pthread_rwlock_unlock(rwlocks + hash);
                            return true;
                        }
                    }
                    else
                    {
                        // difference found. Setup binary search for differing bit
                        comparebits = input_bitstogo >> 1;
                        break;
                    }
                }
            }

// otherwise use default implementation
#else
            while (true)
            {
                if (input_bitstogo < vector_bitstogo)
                {
                    if ((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)) >> (INPUT_WIDTH - input_bitstogo))
                            ==
                            (vectordata_t(*pVector << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - input_bitstogo)))
                    {
                        if(++input_index > max_input_index) {
                            pthread_rwlock_unlock(rwlocks + hash);
                            return true;
                        }
                        ++pInput;
                        vector_bitstogo -= input_bitstogo;
                        input_bitstogo = INPUT_WIDTH;
                    }
                    else
                    {
                        comparebits = input_bitstogo >> 1;
                        break;
                    }
                }
                else if (input_bitstogo > vector_bitstogo)
                {
                    if ((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)) >> (INPUT_WIDTH - vector_bitstogo))
                            ==
                            (vectordata_t(*pVector << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - vector_bitstogo)))
                    {
                        if(!(--vectorlen)) {
                            pthread_rwlock_unlock(rwlocks + hash);
                            return true;
                        }
                        input_bitstogo -= vector_bitstogo;
                        position+=VECTOR_WIDTH, pVector++, vector_bitstogo = VECTOR_WIDTH;
                    }
                    else
                    {
                        comparebits = vector_bitstogo >> 1;
                        break;
                    }
                }
                else
                {
                    if ((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)) >> (INPUT_WIDTH - input_bitstogo))
                            ==
                            (vectordata_t(*pVector << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - vector_bitstogo)))
                    {
                        if(!(--vectorlen)) {
                            pthread_rwlock_unlock(rwlocks + hash);
                            return true;
                        }
                        position+=VECTOR_WIDTH, pVector++, vector_bitstogo = VECTOR_WIDTH;
                        input_index++, ++pInput;
                        input_bitstogo = INPUT_WIDTH;
                    }
                    else
                    {
                        comparebits = input_bitstogo >> 1;
                        break;
                    }
                }
            }
#endif
            // difference was found in current input and vector words. locate the first differing bit using binary search.
            while (comparebits)
            {
                // test if next <comparebits> bits of input and vector are equal
                if ((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)) >> (INPUT_WIDTH - comparebits))
                        ==
                        (vectordata_t(*pVector << (VECTOR_WIDTH - vector_bitstogo)) >> (VECTOR_WIDTH - comparebits)))
                {
                    // they're equal, move forward
                    vector_bitstogo -= comparebits;
                    input_bitstogo -= comparebits;
                    if (comparebits > input_bitstogo)
                    {
                        comparebits = input_bitstogo >> 1;
                    }
                    if (comparebits > vector_bitstogo)
                    {
                        comparebits = vector_bitstogo >> 1;
                    }
                }
                else
                {
                    // they differ, repeat using halved comparebits
                    comparebits >>= 1;
                }
            }
            // we're now exactly at the bit that differs. Search for suiting branch
            // skipping all early branches
            while ((*anchor) && (position + (VECTOR_WIDTH - vector_bitstogo)) > (*anchor)->bit)
            {
                anchor = &((*anchor)->nextold);
            }
            // test whether there already is a branch at the differing bit
            if ((*anchor) && (*anchor)->bit == (position + (VECTOR_WIDTH - vector_bitstogo)))
            {
                // Indeed, there is. Switch to that branch and repeat with new suffix tree vector
                pVector = (* anchor) -> vector;
                anchor = &((* anchor) -> nextnew);

                position += (VECTOR_WIDTH - vector_bitstogo) + 1;
                vector_bitstogo = VECTOR_WIDTH;

                // skip the differing bit. We don't need to store it since its value is determined by the old vector and the branch position.
                input_bitstogo--;
                if(input_index == max_input_index && input_bitstogo + max_input_numbits <= INPUT_WIDTH) {
                    pthread_rwlock_unlock(rwlocks + hash);
                    return true;
                }
                if (!input_bitstogo)
                {
                    input_index++, ++pInput, input_bitstogo = INPUT_WIDTH;
                }
            }
            else
            {
                // there isn't. Place to insert new branch is found.
                break;
            }
        }

        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(position + (VECTOR_WIDTH - vector_bitstogo));
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
        position += (VECTOR_WIDTH - vector_bitstogo) + 1;
        vector_bitstogo = VECTOR_WIDTH;

        input_bitstogo--;
        if(input_index == max_input_index && input_bitstogo + max_input_numbits == INPUT_WIDTH) {
            pthread_rwlock_unlock(rwlocks + hash);
            return false;
        }
        if (!input_bitstogo)
        {
            input_index++, ++pInput, input_bitstogo = INPUT_WIDTH;
        }
    }

    assert(bitlen > position);
    // vector_bitstogo is always VECTOR_WIDTH here
    int newvectorlen = ((bitlen - position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;
    *newvector = (vectordata_t*) calloc(newvectorlen, sizeof(vectordata_t));
    pVector = *newvector;

    // again more efficient implementation
#if INPUT_WIDTH == VECTOR_WIDTH
    // test for good alignment
    if(input_bitstogo == INPUT_WIDTH)
    {
        // good alignment, use memcpy
        memcpy(pVector,pInput,newvectorlen*sizeof(vectordata_t));
        pthread_rwlock_unlock(rwlocks + hash);
        return false;
    } else {
        // bad alignment, copy contents manually
        while(newvectorlen--) {
            *pVector |= input_t(*pInput << (INPUT_WIDTH - input_bitstogo));
            pInput++;
            if(++input_index > max_input_index)
            	break;
            *pVector |= input_t(*pInput >> input_bitstogo);
            pVector++;
        }
        pthread_rwlock_unlock(rwlocks + hash);
        return false;
    }
// default implementation
#else
    while (newvectorlen && input_index <= max_input_index)
    {
#if INPUT_WIDTH >= VECTOR_WIDTH
        *pVector |= vectordata_t((input_t(*pInput << (INPUT_WIDTH - input_bitstogo)) >> (INPUT_WIDTH - vector_bitstogo)));
#else
        *pVector |= vectordata_t(vectordata_t(*pInput) << (VECTOR_WIDTH - input_bitstogo)) >> (VECTOR_WIDTH - vector_bitstogo);
#endif
        if(input_bitstogo < vector_bitstogo) {
            vector_bitstogo -= input_bitstogo;
            input_index++, pInput++;
            input_bitstogo = INPUT_WIDTH;
        } else if(input_bitstogo > vector_bitstogo) {
            input_bitstogo -= vector_bitstogo;
            ++pVector, --newvectorlen;
            vector_bitstogo = VECTOR_WIDTH;
        } else {
            input_index++, pInput++;
            input_bitstogo = INPUT_WIDTH;
            ++pVector, --newvectorlen;
            vector_bitstogo = VECTOR_WIDTH;
        }

    }
#endif
}

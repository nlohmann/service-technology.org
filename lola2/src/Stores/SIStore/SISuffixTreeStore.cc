/*!
\file SuffixTreeStore.cc
\author Christian Koch & Gregor Behnke
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
This Version has been transformed into an search and insert store.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/SIStore/SISuffixTreeStore.h>




SISuffixTreeStore::SISuffixTreeStore(int numberOfThreads) : SIStore(numberOfThreads)
{
    branch = (Decision**) calloc(SIZEOF_MARKINGTABLE,SIZEOF_VOIDP);
    firstvector = (vectordata_t**) calloc(SIZEOF_MARKINGTABLE, SIZEOF_VOIDP);
    rwlocks =(pthread_rwlock_t*) calloc( SIZEOF_MARKINGTABLE,sizeof(pthread_rwlock_t));

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_init(rwlocks+i, NULL);

    search_states = (Search_State*) calloc(numberOfThreads,sizeof(Search_State));
    last_search_not_found_thread = (index_t*) calloc(SIZEOF_MARKINGTABLE,sizeof(Search_State));
}

SISuffixTreeStore::~SISuffixTreeStore()
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

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_destroy(rwlocks+i);
    free(rwlocks);
    free(last_search_not_found_thread);
    free(search_states);
}

/// create a new branch in the decision tree at depth b.
SISuffixTreeStore::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL)
{
}

SISuffixTreeStore::Decision::~Decision()
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


bool SISuffixTreeStore::search(const vectordata_t* in, bitindex_t bitlen, hash_t hash, uint32_t threadIndex)
{
	return search(in, bitlen, hash, threadIndex,true);
}


/// search for an input vector in the suffix tree and insert it, if it is not present
bool SISuffixTreeStore::search(const vectordata_t* in, bitindex_t bitlen, hash_t hash, uint32_t threadIndex, bool lock)
{
	Search_State curr_search_state;

    curr_search_state.input_index = 0;
    curr_search_state.max_input_index = (bitlen - 1) / VECTOR_WIDTH;
    curr_search_state.max_input_numbits = bitlen%VECTOR_WIDTH;
    if(!curr_search_state.max_input_numbits) curr_search_state.max_input_numbits = VECTOR_WIDTH;
    curr_search_state.input_bitstogo = VECTOR_WIDTH; // indicates start with msb
    curr_search_state.pInput = in;
    curr_search_state.vector_bitstogo = VECTOR_WIDTH; // indicates start with msb
    curr_search_state.position = 0;
    curr_search_state.need_to_prepare_insertion = false;

    // lock the current bucket to ensure thread safety
    if (lock)pthread_rwlock_wrlock(rwlocks + hash);

    // Is hash bucket empty? If so, assign to currentvector
    if (!(curr_search_state.pVector = (firstvector[hash])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
    	curr_search_state.newvector = firstvector + hash;
    }
    else
    {
        // Here, hash bucket is not empty.
    	curr_search_state.anchor = branch + hash;

        while (true) // just entered new suffix tree vector
        {
            // number of bits to compare at once, searching for differences. Used only to locate the exact bit position of the difference
            bitindex_t comparebits;

            // maximum number of vector words to consider. The actual vector (pVector) can be smaller, but in this case a difference is found before reaching the end anyway (see getInput rule 2).
            index_t vectorlen = ((bitlen - curr_search_state.position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;


            // test for good alignment
            if(curr_search_state.input_bitstogo == curr_search_state.vector_bitstogo) {
                // good alignment, can use memcmp
                if(!memcmp(curr_search_state.pInput,curr_search_state.pVector,vectorlen*sizeof(vectordata_t))) {
                    // match found, we're done
                	if (lock) pthread_rwlock_unlock(rwlocks + hash);
                    return true;
                }
                // difference found, skip to first differing word.
                while(*curr_search_state.pInput == *curr_search_state.pVector) // reaching end of input is impossible
                {
                	curr_search_state.position+=VECTOR_WIDTH, curr_search_state.pVector++;
                	curr_search_state.input_index++, ++curr_search_state.pInput;
                }
                comparebits = VECTOR_WIDTH >> 1; // initialize binary search for differing bit
            }
            else // bad alignment; input_bitstogo < vector_bitstogo, since vector_bitstogo is always VECTOR_WIDTH at this point
            {
                while(true) { // vector_bitstogo == VECTOR_WIDTH == VECTOR_WIDTH
                    // compare remaining input bits with msb bits of current vector
                    if ((vectordata_t(*curr_search_state.pInput << (VECTOR_WIDTH - curr_search_state.input_bitstogo)))
                            ==
                            (vectordata_t((*curr_search_state.pVector >> (VECTOR_WIDTH - curr_search_state.input_bitstogo)) << (VECTOR_WIDTH - curr_search_state.input_bitstogo))))
                    {
                        // they're equal, input word done. Test for EOI
                        if(++curr_search_state.input_index <= curr_search_state.max_input_index) {
                            // compare msb of next input word with the remaining bits of the current vector word
                            if ((vectordata_t(*(++curr_search_state.pInput) >> curr_search_state.input_bitstogo) << curr_search_state.input_bitstogo)
                                    ==
                                    (vectordata_t(*curr_search_state.pVector << curr_search_state.input_bitstogo)))
                            {
                                // they're equal, vector word done. Test for EOV and repeat with next vector word.
                                if(--vectorlen) {
                                	curr_search_state.position += VECTOR_WIDTH, curr_search_state.pVector++;
                                }
                                else {
                                	if (lock) pthread_rwlock_unlock(rwlocks + hash);
                                    return true;
                                }
                            }
                            else
                            {
                                // difference found. Update bitstogo variables and setup binary search for differing bit
                            	curr_search_state.vector_bitstogo -= curr_search_state.input_bitstogo;
                                curr_search_state.input_bitstogo = VECTOR_WIDTH;
                                comparebits = curr_search_state.vector_bitstogo >> 1;
                                break;
                            }
                        } else {
                        	if (lock) pthread_rwlock_unlock(rwlocks + hash);
                            return true;
                        }
                    }
                    else
                    {
                        // difference found. Setup binary search for differing bit
                        comparebits = curr_search_state.input_bitstogo >> 1;
                        break;
                    }
                }
            }

            // difference was found in current input and vector words. locate the first differing bit using binary search.
            while (comparebits)
            {
                // test if next <comparebits> bits of input and vector are equal
                if ((vectordata_t(*curr_search_state.pInput << (VECTOR_WIDTH - curr_search_state.input_bitstogo)) >> (VECTOR_WIDTH - comparebits))
                        ==
                        (vectordata_t(*curr_search_state.pVector << (VECTOR_WIDTH - curr_search_state.vector_bitstogo)) >> (VECTOR_WIDTH - comparebits)))
                {
                    // they're equal, move forward
                	curr_search_state.vector_bitstogo -= comparebits;
                	curr_search_state.input_bitstogo -= comparebits;
                    if (comparebits > curr_search_state.input_bitstogo)
                    {
                        comparebits = curr_search_state.input_bitstogo >> 1;
                    }
                    if (comparebits > curr_search_state.vector_bitstogo)
                    {
                        comparebits = curr_search_state.vector_bitstogo >> 1;
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
            while ((*curr_search_state.anchor) && (curr_search_state.position + (VECTOR_WIDTH - curr_search_state.vector_bitstogo)) > (*curr_search_state.anchor)->bit)
            {
            	curr_search_state.anchor = &((*curr_search_state.anchor)->nextold);
            }
            // test whether there already is a branch at the differing bit
            if ((*curr_search_state.anchor) && (*curr_search_state.anchor)->bit == (curr_search_state.position + (VECTOR_WIDTH - curr_search_state.vector_bitstogo)))
            {
                // Indeed, there is. Switch to that branch and repeat with new suffix tree vector
            	curr_search_state.pVector = (* curr_search_state.anchor) -> vector;
                curr_search_state.anchor = &((* curr_search_state.anchor) -> nextnew);

                curr_search_state.position += (VECTOR_WIDTH - curr_search_state.vector_bitstogo) + 1;
                curr_search_state.vector_bitstogo = VECTOR_WIDTH;

                // skip the differing bit. We don't need to store it since its value is determined by the old vector and the branch position.
                curr_search_state.input_bitstogo--;
                if(curr_search_state.input_index == curr_search_state.max_input_index && curr_search_state.input_bitstogo + curr_search_state.max_input_numbits <= VECTOR_WIDTH) {
                	if (lock) pthread_rwlock_unlock(rwlocks + hash);
                    return true;
                }
                if (!curr_search_state.input_bitstogo)
                {
                    curr_search_state.input_index++, ++curr_search_state.pInput, curr_search_state.input_bitstogo = VECTOR_WIDTH;
                }
            }
            else
            {
                // there isn't. Place to insert new branch is found.
                break;
            }
        }

        curr_search_state.need_to_prepare_insertion = true;
    }

    // write back the current search state
    search_states[threadIndex] = curr_search_state;
    last_search_not_found_thread[hash] = threadIndex;
    if (lock) pthread_rwlock_unlock(rwlocks + hash);
}


bool SISuffixTreeStore::insert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex)
{
    // lock the current bucket to ensure thread safety
    pthread_rwlock_wrlock(rwlocks + hash);

    // if this thread was the last to search in this bucket it can insert safely
    // if not we need to re-do the search, but without locks, because we hold them already
    if (last_search_not_found_thread[hash] != threadIndex)
    	// we may find the marking now!
    	if (search(in, bitlen, hash, threadIndex,false))
    	{
    		pthread_rwlock_unlock(rwlocks + hash);
    		return true;
    	}

	Search_State curr_search_state = search_states[threadIndex];

	if (curr_search_state.need_to_prepare_insertion){
        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(curr_search_state.position + (VECTOR_WIDTH - curr_search_state.vector_bitstogo));
        newdecision -> nextold = * curr_search_state.anchor;
        * curr_search_state.anchor = newdecision;
        newdecision -> nextnew = NULL;
        curr_search_state.newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
        curr_search_state.position += (VECTOR_WIDTH - curr_search_state.vector_bitstogo) + 1;
        curr_search_state.vector_bitstogo = VECTOR_WIDTH;

        curr_search_state.input_bitstogo--;
        if(curr_search_state.input_index == curr_search_state.max_input_index && curr_search_state.input_bitstogo + curr_search_state.max_input_numbits == VECTOR_WIDTH) {
        	pthread_rwlock_unlock(rwlocks + hash);
            return false;
        }
        if (!curr_search_state.input_bitstogo)
        {
        	curr_search_state.input_index++, ++curr_search_state.pInput, curr_search_state.input_bitstogo = VECTOR_WIDTH;
        }
	}


	assert(bitlen > curr_search_state.position);
    // vector_bitstogo is always VECTOR_WIDTH here
    int newvectorlen = ((bitlen - curr_search_state.position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;
    *curr_search_state.newvector = (vectordata_t*) calloc(newvectorlen, sizeof(vectordata_t));
    curr_search_state.pVector = *curr_search_state.newvector;


    // test for good alignment
    if(curr_search_state.input_bitstogo == VECTOR_WIDTH)
    {
        // good alignment, use memcpy
        memcpy(curr_search_state.pVector,curr_search_state.pInput,newvectorlen*sizeof(vectordata_t));
    } else {
        // bad alignment, copy contents manually
        while(newvectorlen--) {
            *curr_search_state.pVector |= vectordata_t(*curr_search_state.pInput << (VECTOR_WIDTH - curr_search_state.input_bitstogo));
            curr_search_state.pInput++;
            if(++curr_search_state.input_index > curr_search_state.max_input_index)
            	break;
            *curr_search_state.pVector |= vectordata_t(*curr_search_state.pInput >> curr_search_state.input_bitstogo);
            curr_search_state.pVector++;
        }
    }

    pthread_rwlock_unlock(rwlocks + hash);
    return false;

}

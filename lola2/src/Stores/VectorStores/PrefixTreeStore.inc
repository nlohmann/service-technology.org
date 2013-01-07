/*!
\file PrefixTreeStore.inc
\author Christian Koch
\status new
\brief VectorStore implementation using binary prefix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#include <Stores/VectorStores/PrefixTreeStore.h>


template <typename T>
PrefixTreeStore<T>::PrefixTreeStore() : popVectorCache(NULL)
{
    branch = NULL;
    firstvector = NULL;

    pthread_rwlock_init(&rwlock, NULL);
}

template <typename T>
inline size_t PrefixTreeStore<T>::getPayloadSize()
{
    return sizeof(T);
}

template <typename T>
PrefixTreeStore<T>::~PrefixTreeStore()
{
    if (branch)
    {
        delete branch;
    }
    if (firstvector)
    {
        free(firstvector);
    }

	if(popVectorCache)
        free(popVectorCache);

    pthread_rwlock_destroy(&rwlock);
}

/// create a new branch in the decision tree at depth b.
template <typename T>
PrefixTreeStore<T>::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL)
{
}

template <typename T>
PrefixTreeStore<T>::Decision::~Decision()
{
    vector = NULL;
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
template <typename T>
bool PrefixTreeStore<T>::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex)
{
    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the place where the new vector goes to
    vectordata_t** newvector;

    /// the input word we are currently dealing with
    index_t input_index = 0;

    /// the last input word to consider.
    index_t max_input_index = (bitlen - 1) / VECTOR_WIDTH;

    /// the number of significant bits in the last input word (aligned at msb)
    index_t max_input_numbits = bitlen%VECTOR_WIDTH;
    if(!max_input_numbits) max_input_numbits = VECTOR_WIDTH;

    /// the bits of the current input word we have NOT dealt with so far
    bitindex_t input_bitstogo = VECTOR_WIDTH; // indicates start with msb

    // pointer to the current input word
    const vectordata_t* pInput = in;


    /// the starting address of the current vector. In each vector, the payload is loacted before the marking data.
    vectordata_t* pPayload;

    /// the vector word we are currently investigating
    vectordata_t* pVector;

    /// the bits of the current vector word we have NOT dealt with so far
    bitindex_t vector_bitstogo = VECTOR_WIDTH; // indicates start with msb

    /// the number of bits processed since reaching the current branch
    bitindex_t position = 0;
    
    size_t payloadSize = getPayloadSize();

    // lock the current bucket to ensure thread safety
    pthread_rwlock_wrlock(&rwlock);
    
    // Is hash bucket empty? If so, assign to currentvector
    if (!(pPayload = firstvector))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = &firstvector;
    }
    else
    {
    	// skip payload. Cast to char* is done to ensure skipping exaclty the right amount of bytes.
    	pVector = (vectordata_t*)(((char*)pPayload) + payloadSize);
    
        // Here, hash bucket is not empty.
        anchor = &branch;

        while (true) // just entered new suffix tree vector
        {
            // number of bits to compare at once, searching for differences. Used only to locate the exact bit position of the difference
            bitindex_t comparebits;

            // maximum number of vector words to consider. The actual vector (pVector) can be smaller, but in this case a difference is found before reaching the end anyway (see getInput rule 2).
            index_t vectorlen = ((bitlen - position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;


            // test for good alignment
            if(input_bitstogo == vector_bitstogo) {
                // good alignment, can use memcmp
                if(!memcmp(pInput,pVector,vectorlen*sizeof(vectordata_t))) {
                    // match found, we're done
                    pthread_rwlock_unlock(&rwlock);
                    
                    if(payload)
                        *payload = (T*) pPayload;
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
                while(true) { // vector_bitstogo == VECTOR_WIDTH == VECTOR_WIDTH
                    // compare remaining input bits with msb bits of current vector
                    if ((vectordata_t(*pInput << (VECTOR_WIDTH - input_bitstogo)))
                            ==
                            (vectordata_t((*pVector >> (VECTOR_WIDTH - input_bitstogo)) << (VECTOR_WIDTH - input_bitstogo))))
                    {
                        // they're equal, input word done. Test for EOI
                        if(++input_index <= max_input_index) {
                            // compare msb of next input word with the remaining bits of the current vector word
                            if ((vectordata_t(*(++pInput) >> input_bitstogo) << input_bitstogo)
                                    ==
                                    (vectordata_t(*pVector << input_bitstogo)))
                            {
                                // they're equal, vector word done. Test for EOV and repeat with next vector word.
                                if(--vectorlen) {
                                    position += VECTOR_WIDTH, pVector++;
                                }
                                else {
                                    pthread_rwlock_unlock(&rwlock);
                                    if(payload)
                                        *payload = (T*) pPayload;
                                    return true;
                                }
                            }
                            else
                            {
                                // difference found. Update bitstogo variables and setup binary search for differing bit
                                vector_bitstogo -= input_bitstogo;
                                input_bitstogo = VECTOR_WIDTH;
                                comparebits = vector_bitstogo >> 1;
                                break;
                            }
                        } else {
                            pthread_rwlock_unlock(&rwlock);
                            if(payload)
                                *payload = (T*) pPayload;
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

            // difference was found in current input and vector words. locate the first differing bit using binary search.
            while (comparebits)
            {
                // test if next <comparebits> bits of input and vector are equal
                if ((vectordata_t(*pInput << (VECTOR_WIDTH - input_bitstogo)) >> (VECTOR_WIDTH - comparebits))
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
                pPayload = (* anchor) -> vector;
    	        pVector = (vectordata_t*)(((char*)pPayload) + payloadSize);
                anchor = &((* anchor) -> nextnew);

                position += (VECTOR_WIDTH - vector_bitstogo) + 1;
                vector_bitstogo = VECTOR_WIDTH;

                // skip the differing bit. We don't need to store it since its value is determined by the old vector and the branch position.
                input_bitstogo--;
                if(input_index == max_input_index && input_bitstogo + max_input_numbits <= VECTOR_WIDTH) {
                    pthread_rwlock_unlock(&rwlock);
                    if(payload)
                        *payload = (T*) pPayload;
                    return true;
                }
                if (!input_bitstogo)
                {
                    input_index++, ++pInput, input_bitstogo = VECTOR_WIDTH;
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
        if(input_index == max_input_index && input_bitstogo + max_input_numbits == VECTOR_WIDTH) {
            pthread_rwlock_unlock(&rwlock);
            if(payload)
                *payload = (T*) pPayload;
            return false;
        }
        if (!input_bitstogo)
        {
            input_index++, ++pInput, input_bitstogo = VECTOR_WIDTH;
        }
    }

    assert(bitlen >= position);
    // vector_bitstogo is always VECTOR_WIDTH here
    int newvectorlen = ((bitlen - position) + (VECTOR_WIDTH - 1)) / VECTOR_WIDTH;
    *newvector = (vectordata_t*) calloc(payloadSize + newvectorlen*sizeof(vectordata_t), 1);
    pPayload = *newvector;
    pVector = (vectordata_t*)(((char*)pPayload) + payloadSize);


    // test for good alignment
    if(input_bitstogo == VECTOR_WIDTH)
    {
        // good alignment, use memcpy
        memcpy(pVector,pInput,newvectorlen*sizeof(vectordata_t));
        pthread_rwlock_unlock(&rwlock);
        if(payload)
            *payload = (T*) pPayload;
        return false;
    } else {
        // bad alignment, copy contents manually
        while(newvectorlen--) {
            *pVector |= vectordata_t(*pInput << (VECTOR_WIDTH - input_bitstogo));
            pInput++;
            if(++input_index > max_input_index)
                break;
            *pVector |= vectordata_t(*pInput >> input_bitstogo);
            pVector++;
        }
        pthread_rwlock_unlock(&rwlock);
        if(payload)
            *payload = (T*) pPayload;
        return false;
    }
}

/*!
 * \brief  gets and removes a vector from the store
 */
template <typename T>
bool PrefixTreeStore<T>::popVector(vectordata_t * & out)
{
  /// size of resulting vector
  index_t vectorlen = SIZEOF_CAPACITY_T * Net::Card[PL];
  // create intermediate vector
  if(!popVectorCache)
  {
    popVectorCache = (vectordata_t *) malloc(vectorlen);
  }
  
  // vector currently viewed
  vectordata_t * pPayload = NULL;
  vectordata_t * pVector = NULL;
  // anchor to a decision node
  Decision * anchor = NULL;
  Decision ** pAnchor = NULL;
  
  if(!(pPayload = firstvector))
  {
      return false;
  }
  // skip payload data
  pVector = (vectordata_t*)(((char*)pPayload) + getPayloadSize());

  // copy "first" vector to return it later
  memcpy(popVectorCache, pVector, vectorlen);
  out = popVectorCache;
  
  if(!(anchor = branch))
  {
    // no further markings in this bucket
    free(pPayload);
    firstvector = NULL;
    return true;
  }
    
  // we start with the root of the decision tree
  pAnchor = &anchor;
  // and search for the decision node differing as last from the "first" vector
  while(anchor->nextold)
   {
      pAnchor = &(anchor->nextold);
      anchor = anchor-> nextold;
    }
    
    // alignment of both vectors (i.e. relative position of first bit of anchor->vector)
    bitindex_t shift = (anchor->bit + 1) % VECTOR_WIDTH;
    // remaining words to copy
    bitindex_t remaining = (vectorlen*8 - anchor->bit - 1) / VECTOR_WIDTH;
    
    // go to position of first differing bit
    pVector += (anchor->bit / VECTOR_WIDTH);
    // toggle bit 
    *pVector ^= 1 << (VECTOR_WIDTH - (anchor->bit % VECTOR_WIDTH) - 1);
    
    // vector to be copied from
    vectordata_t * source = anchor->vector;
    
    // test for good alignment
    if(shift == 0)
    {
      // good alignment, use memcpy
      memcpy(++pVector, source, remaining * SIZEOF_VECTORDATA_T);
    }
    else
    {
      // bad alignment, copy manually
      // initially clear recent word
      *pVector = ((*pVector >> (VECTOR_WIDTH-shift)) << (VECTOR_WIDTH-shift));
      
      while(true)
      {
        // copy lower half
        *(pVector++) |= *source >> shift;
        if(remaining-- == 0)
        {
          // finished copying
          break;
        }
        // copy upper half
        *pVector = *(source++) << (VECTOR_WIDTH-shift);
      }
    }
    
    // remove node
    *pAnchor = anchor->nextnew;
    anchor->nextnew = NULL; // prevent it from being deleted when calling the destructor
    delete anchor;
    
    // return result
    return true;
}

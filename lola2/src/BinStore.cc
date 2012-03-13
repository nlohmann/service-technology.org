/*!
\file BinStore.cc
\author Karsten
\status new
\brief implement Store as a decision tree with bitvectors..
*/

#include <cstdlib>
#include <cstdio>

#include "BinStore.h"
#include "Marking.h"
#include "Net.h"
#include "Place.h"

class State;


BinStore::BinStore()
{
	branch = (Decision **) calloc(SIZEOF_VOIDP,SIZEOF_MARKINGTABLE);
	firstvector = (unsigned char **) calloc(SIZEOF_VOIDP,SIZEOF_MARKINGTABLE);
}

BinStore::~BinStore()
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
BinStore::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL), state(0)
{
}

BinStore::Decision::~Decision()
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

bool BinStore::searchAndInsert()
{
	++calls;;
    // the general assumption is that we read marking, vectors etc. left to right, with 
    // low indices left, high indices right, 
    // msb left and lsb right.

    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the vector we are currently investigating
    unsigned char* currentvector;

    /// the place where the new vector goes to
    unsigned char** newvector;

    /// the place we are currently dealing with
    index_t place_index = 0;

    /// the bit of the place's marking we are currently dealing with
    bitindex_t placebit_index = Place::CardBits[0] - 1; // indicates start with msb

    /// the byte in the vector we are currently dealing with
    bitindex_t vector_byte = 0;

    ///  the current bit if bits are counted through all places
    /// This means, position is the current global depth in the decision tree
    bitindex_t position = 0;

    // Is hash bucket empty? If so, assign to currentvector
    if (!(currentvector = (firstvector[Marking::HashCurrent])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = firstvector + Marking::HashCurrent;
    }
    else
    {
        // Here, hash bucket is not empty.
        anchor = branch + Marking::HashCurrent;


	while(true) // for various currentvectors do...
	{
        
freshvector:
		while(true) // dive down in a single currentvector to first mismatch. No mismatch = state found
		{
			// The body of this loop exists 8 times, once for each bit in vector byte.
/*** incarnation for bit 7 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 7) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 6 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 6) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 5 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 5) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/**** incarnation for bit 4 ********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 4) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 3 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 3) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 2 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 2) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 1 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte] >> 1) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/*** incarnation for bit 0 *********/
		     // comparison of one bit between current marking and current vector
		    if (((Marking::Current[place_index] >> placebit_index) & 1 ) != ((currentvector[vector_byte]) & 1))
		    {
			// This is the mismatch we were looking for
		        // --> evaluate the branching tree:
			// Mismatch between two branches: state not found
			// Mismatch at some branch: switch to other currentvector
			while (true)
			{
			    if(!*anchor) 
			    {
				goto insert; // no further branch
			    }
			    // mismatch exactly at branch?
			    if (position == (*anchor)->bit)
			    {
				// switch to other currentvector 
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);
				// Indices in new vector start at msb in byte 0
				vector_byte = 0;
				++position;  // the mismatching bit is not represented in new vector
					     // as it must be the opposite of the mismatching one in old vector

				// forward one bit in current marking, as mismatching one is implicitly
				// matched by the switch to new vector
				if (placebit_index == 0)
				{
				    place_index++;
				    if (place_index >= Place::CardSignificant)
				    {
					// no mismatch --> state found
					return true;
				    }
				    placebit_index = Place::CardBits[place_index] - 1;
				}
				else
				{
					--placebit_index;
				}
				goto freshvector; //goto next iteration of "for various currentvectors do"
			       // since we need a new mismatch in new vector

			    }
			    // mismatch beyond branch
			    if (position > (*anchor)->bit)
			    {
				// dive down in nextold list for next branch descending from current vector
				anchor = &((*anchor)->nextold);
			    }
			    else
			    {
				    // arriving here, the mismatch is before next branch off current vector
				    assert(position < (*anchor)->bit);
				    goto insert;
			    }
			}
		    }

		    // increment global bit counter
		    ++position;

		    // increment index in currentvector
			++vector_byte;
		     //increment index in currentmarking
		    if (placebit_index == 0)
		    {
			place_index++;
			if (place_index >= Place::CardSignificant)
			{
			    // no mismatch --> state found
			    return true;
			}
			placebit_index = Place::CardBits[place_index] - 1;
		    }
		    else
	            {
			--placebit_index;
	            }
/************/
		}
	}
insert:
        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(position);
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
        ++position;
        if (placebit_index == 0)
        {
            place_index++;
            if (place_index >= Place::CardSignificant)
            {
                // new vector needs only 0 bits.
		// Handle this manually as effect of malloc(0) is implementation dependent 
		* newvector = NULL;
		    ++markings;
		return false;
            }
            placebit_index = Place::CardBits[place_index] - 1;
        }
	else
	{
		--placebit_index;
	}
    }

    // compress current marking into bitvector
    // we assume that place_index, placebit_index, position have the correct
    // initial values
    *newvector = (unsigned char *) calloc (((Place::SizeOfBitVector - position) + 7) / 8, sizeof(unsigned char));
    vector_byte = 0;
    while (true) 
    {
	// the body of this loop exists in 8 incarnations, once for each bit of vector byte
/*** incarnation for bit 7 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			(*newvector)[vector_byte] |= 128;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 6 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 64;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 5 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 32;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 4 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 16;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 3 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 8;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 2 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 4;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 1 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 2;
		}
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
/*** incarnation for bit 0 ********/
		if (Marking::Current[place_index] & (1 << placebit_index))
		{
		    assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);
			//\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
			(*newvector)[vector_byte] |= 1;
		}
		++vector_byte ;
		if (placebit_index == 0) 
                {
			++place_index;
			if(place_index >= Place::CardSignificant) break;
			placebit_index = Place::CardBits[place_index] - 1;
		}
		else
		{
			--placebit_index;
		}
    }

    ++markings;
    return false;
}

bool BinStore::searchAndInsert(State** result) {}


void BinStore::pbs(unsigned int b, unsigned int p, unsigned char * f, void * v)
{
	
		int i;
		Decision * d = (Decision *) v;
		if( d )	
		{
			pbs(d -> bit + 1, 0, d -> vector, d -> nextnew);
		}
		for(i = 0; i < b; i++) printf(" ");
		for(; i <= (d? d->bit : Place::SizeOfBitVector -1); i++)
		{
			if(f[(i-b + p)/8] & (1 << (7-(i-b+p)%8))) printf("1"); else printf("0");
		}
		printf("\n");
		
		if( d )
		{
			pbs(d->bit + 1, p + d->bit -b +1, f , d -> nextold);
		}
}
void BinStore::printBinStore()
{
	for(unsigned int i = 0; i < Place::CardSignificant; i++) printf("%s:%d ",Net::Name[PL][i],Marking::Current[i]);
	printf("\n\n");
	
	for(unsigned int i = 0; i < SIZEOF_MARKINGTABLE;i++)
	{
		printf("\n hash value %u\n", i);
		if(firstvector[i]) BinStore::pbs(0,0,firstvector[i],branch[i]);
		printf("\n");
	}
}

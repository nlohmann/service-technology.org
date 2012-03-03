/*!
\file BinStore.cc
\author Karsten
\status new
\brief implement Store as a decision tree with bitvectors..
*/

#include <cstdlib>

#include "BinStore.h"
#include "Marking.h"
#include "Net.h"
#include "Place.h"

class State;

BinStore::BinStore()
{
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        branch[i] = NULL;
        firstvector[i] = NULL;
    }
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
            delete firstvector[i];
        }

    }
}

/// create a new branch in the decision tree at depth b.
BinStore::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL), state(0)
{
}

BinStore::Decision::~Decision()
{
    delete [] vector;
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
    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the vector we are currently investigating
    unsigned char* currentvector;

    /// the the place where the new vector goes to
    unsigned char** newvector;

    /// the place we are currently ealing with
    index_t place_index = 0;

    /// the bit of the place's marking we are currently dealing with
    bitindex_t placebit_index = 0;

    /// the byte in the vector we are currently dealing with
    bitindex_t vector_byte = 0;

    /// the bit within current byte of vector we are currently dealing with
    unsigned short int vector_bit = 0;

    ///  the current bit if bits are counted through all places
    bitindex_t position = 0;

    /// the part of the marking that is not yet transformed to bits
    capacity_t remainder = 0;

    // Is hash bucket empty?
    if (!(firstvector[Marking::HashCurrent]))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        place_index = 0;
        placebit_index = 0;
        remainder = Marking::Current[0];
        position = 0;
        newvector = firstvector + Marking::HashCurrent;
    }
    else
    {
        // Here, hash bucket is not empty.
        currentvector = firstvector[Marking::HashCurrent];
        place_index = 0;
        placebit_index = Place::CardBits[0] - 1;
        anchor = branch + Marking::HashCurrent;

        // dive down in currentvector to first mismatch. No mismatch = state found

        do
        {
            if ((remainder & (1 << placebit_index)) != (currentvector[vector_byte] & (1 << vector_bit)))
            {
                break;
            }
            ++position;
            if (++vector_bit >= 8)
            {
                vector_bit = 0;
                ++vector_byte;
            }
            if (placebit_index-- == 0)
            {
                place_index++;
                if (place_index >= Place::CardSignificant)
                {
                    // no mismatch --> state found
                    return true;
                }
                placebit_index = Place::CardBits[place_index] - 1;
            }
        }
        while (true);
        // here, we have the first mismatch to currentvector --> evaluate the branching tree:
        // between two branches: state not found
        // at some branch: switch to other currentvector
        while (*anchor)
        {
            if (position == (*anchor)->bit)
            {
                // switch to other currentvector
                anchor = &((* anchor) -> nextnew);
                currentvector = (* anchor) -> vector;
                vector_byte = 0;
                vector_bit = 0;
                ++position;
                if (placebit_index-- == 0)
                {
                    place_index++;
                    if (place_index >= Place::CardSignificant)
                    {
                        // no mismatch --> state found
                        return true;
                    }
                    placebit_index = Place::CardBits[place_index] - 1;
                }
                continue;

            }
            if (position > (*anchor)->bit)
            {
                anchor = &((*anchor)->nextold);
                continue;
            }
            assert(position < (*anchor)->bit);
            break;
        }
        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(position);
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
        ++position;
        if (placebit_index-- == 0)
        {
            place_index++;
            if (place_index >= Place::CardSignificant)
            {
                // no mismatch --> state found
                return true;
            }
            placebit_index = Place::CardBits[place_index] - 1;
        }
    }

    // compress current marking into bitvector
    // we assume that place_index, placebit_index, and remainer have the correct
    // initial values
    *newvector = new unsigned char [((Place::SizeOfBitVector - position) + 7) / 8];
    vector_byte = 0;
    vector_bit = 0;
    while (place_index < Net::Card[PL])
    {
        placebit_index = Place::CardBits[place_index] - 1;
        do
        {
            if (vector_bit == 0)
            {
                *newvector[vector_byte] = 0;
            }
            if (remainder / (1 << placebit_index))
            {
                //\todo conversion to ‘unsigned char’ from ‘int’ may alter its value
                *newvector[vector_byte] |= 1 << vector_bit++;
                remainder -= 1 << placebit_index;
                if (vector_bit >= 8)
                {
                    vector_bit = 0;
                    vector_byte ++;
                }
            }
        }
        while (placebit_index-- != 0);
        ++place_index;
        if (place_index >= Net::Card[PL])
        {
            break;
        }
        remainder = Marking::Current[place_index];
    }
    return false;
}

bool BinStore::searchAndInsert(State** result) {}

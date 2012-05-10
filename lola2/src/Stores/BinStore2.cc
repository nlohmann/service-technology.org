/*!
\file BinStore2.cc
\author Karsten, Christian Koch
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#include <cstdlib>
#include <cstdio>
#include "Net/Marking.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Stores/BinStore2.h"

class State;



BinStore2::BinStore2()
{
    branch = (Decision**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    firstvector = (uchar**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
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


bitindex_t max(bitindex_t a, bitindex_t b) {
	return a>b?a:b;
}

/// search for a state in the binStore and insert it, if it is not there
/// Do not care about states

const int PLACE_WIDTH = sizeof(capacity_t) * 8;

int collcounter = 0;
bool BinStore2::searchAndInsert()
{
    ++calls;;
    // the general assumption is that we read marking, vectors etc. left to right, with
    // low indices left, high indices right,
    // msb left and lsb right.

    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// the vector we are currently investigating
    uchar* currentvector;

    /// the place where the new vector goes to
    uchar** newvector;

    /// the place we are currently dealing with
    index_t place_index = 0;

//    index_t place_byteoffest = (PLACE_WIDTH - Place::CardBits[0]) / 8;
    /// the bit of the place's marking we are currently dealing with
    bitindex_t place_bitoffset = (PLACE_WIDTH - Place::CardBits[0]); // indicates start with msb

    /// the byte in the vector we are currently dealing with
    index_t vector_byte = 0;
    uchar vector_bitoffset = 0;

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

        while(true) {
			bitindex_t comparebits = 8-max(place_bitoffset&7,vector_bitoffset);
			bool founddiff = false;
			while(comparebits) {
				if((capacity_t(Marking::Current[place_index] << place_bitoffset) >> (PLACE_WIDTH-comparebits))
						!=
						(uchar(currentvector[vector_byte] << vector_bitoffset) >> (8-comparebits))) {
					founddiff = true;
					comparebits >>= 1;
				} else {
					position += comparebits;
					vector_bitoffset += comparebits;
					if(vector_bitoffset >= 8)
						vector_byte++, vector_bitoffset=0;
					place_bitoffset += comparebits;
					if(place_bitoffset >= PLACE_WIDTH) {
						place_index++;
						if(place_index >= Place::CardSignificant)
							return true;
						place_bitoffset=(PLACE_WIDTH - Place::CardBits[place_index]);
					}
					if(!founddiff)
						comparebits = 8-max(place_bitoffset&7,vector_bitoffset);
				}
			}
			while((*anchor) && position > (*anchor)->bit)
				anchor = &((*anchor)->nextold);
			if((*anchor) && (*anchor)->bit == position) {
				currentvector = (* anchor) -> vector;
				anchor = &((* anchor) -> nextnew);

				vector_byte = 0;
				vector_bitoffset = 0;
				++position;

				place_bitoffset++;
				if(place_bitoffset >= PLACE_WIDTH) {
					place_index++;
					if(place_index >= Place::CardSignificant)
						return true;
					place_bitoffset=(PLACE_WIDTH - Place::CardBits[place_index]);
				}
			} else
				break;
        }

        // state not found --> prepare for insertion
        Decision* newdecision = new Decision(position);
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        // the mismatching bit itself is not represented in the new vector
		vector_byte = 0;
		vector_bitoffset = 0;
		++position;

		place_bitoffset++;
		if(place_bitoffset >= PLACE_WIDTH) {
			place_index++;
			if(place_index >= Place::CardSignificant) {
                * newvector = NULL;
                ++markings;
                return false;
			}
			place_bitoffset=(PLACE_WIDTH - Place::CardBits[place_index]);
		}
    }

    // compress current marking into bitvector
    // we assume that place_index, placebit_index, position have the correct
    // initial values
    *newvector = (uchar*) calloc(((Place::SizeOfBitVector - position) + 7) / 8, sizeof(uchar));

    while(true) {
		bitindex_t insertbits = 8-max(place_bitoffset&7,vector_bitoffset);
		(*newvector)[vector_byte] |= uchar((capacity_t(Marking::Current[place_index] << place_bitoffset) >> (PLACE_WIDTH-insertbits)) << (8-insertbits-vector_bitoffset));

		vector_bitoffset += insertbits;
		if(vector_bitoffset >= 8)
			vector_byte++, vector_bitoffset=0;
		place_bitoffset += insertbits;
		if(place_bitoffset >= PLACE_WIDTH) {
			place_index++;
			if(place_index >= Place::CardSignificant) {
				++markings;
				return false;
			}
			place_bitoffset=(PLACE_WIDTH - Place::CardBits[place_index]);
		}
    }
}

bool BinStore2::searchAndInsert(State** result) {}


void BinStore2::pbs(unsigned int b, unsigned int p, unsigned char* f, void* v)
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
}

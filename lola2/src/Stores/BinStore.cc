/*!
\file BinStore.cc
\author Karsten
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#include <cstdlib>
#include <cstdio>
#include <config.h>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/BinStore.h>
class State;


BinStore::BinStore()
{
    branch = (Decision**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    firstvector = (unsigned char**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
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
BinStore::Decision::Decision(bitindex_t b) : bit(b) , nextold(NULL), nextnew(NULL)//, state(0)
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

/// create a decision node
BinStore::Decision* BinStore::createDecision(bitindex_t b)
{
    return new Decision(b);
}


/// search for a state in the binStore and insert it, if it is not there
/// Do not care about states

bool BinStore::searchAndInsert(NetState &ns, void** result)
{
    ++calls;;
    // the general assumption is that we read marking, vectors etc. left to right, with
    // low indices left, high indices right,
    // msb left and lsb right.

    /// If a new decision record is inserted, * anchor must point to it
    Decision** anchor;

    /// pointer to last decision taken (stores result if record found)
    Decision* lastDecision = NULL;

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
    if (!(currentvector = (firstvector[ns.HashCurrent])))
    {
        // Indeed, hash bucket is empty --> just insert vector, no branch yet.
        newvector = firstvector + ns.HashCurrent;
        getAndCreateFirstPayload(ns.HashCurrent, result);
    }
    else
    {
        // Here, hash bucket is not empty.
        anchor = branch + ns.HashCurrent;


        while (true) // for various currentvectors do...
        {

freshvector:
            while (true) // dive down in a single currentvector to first mismatch. No mismatch = state found
            {
                // The body of this loop exists 8 times, once for each bit in vector byte.
#ifdef BINSTORE_LOOP_BODY
#error BINSTORE_LOOP_BODY already defined
#else
#define BINSTORE_LOOP_BODY(N, I)\
    /* comparison of one bit between current marking and current vector*/\
    if (((ns.Current[place_index] >> placebit_index) & 1) != ((currentvector[vector_byte] N) & 1))\
    {\
        /* This is the mismatch we were looking for*/\
        /* --> evaluate the branching tree:*/\
        /* Mismatch between two branches: state not found*/\
        /* Mismatch at some branch: switch to other currentvector*/\
        while (true)\
        {\
            if (!*anchor)\
            {\
                goto insert; /* no further branch*/\
            }\
            /* mismatch exactly at branch?*/\
            if (position == (*anchor)->bit)\
            {\
                /* switch to other currentvector*/\
                currentvector = (lastDecision = * anchor) -> vector;\
                anchor = &((* anchor) -> nextnew);\
                /* Indices in new vector start at msb in byte 0*/\
                vector_byte = 0;\
                ++position;  /* the mismatching bit is not represented in new vector*/\
                /* as it must be the opposite of the mismatching one in old vector*/\
                \
                /* forward one bit in current marking, as mismatching one is implicitly*/\
                /* matched by the switch to new vector*/\
                if (placebit_index == 0)\
                {\
                    place_index++;\
                    if (place_index >= Place::CardSignificant)\
                    {\
                        /* no mismatch --> state found*/\
                        /* we currently set this pointer so no test for NULL necessary */\
                        lastDecision->getPayload(result);\
                        return true;\
                    }\
                    placebit_index = Place::CardBits[place_index] - 1;\
                }\
                else\
                {\
                    --placebit_index;\
                }\
                goto freshvector; /*goto next iteration of "for various currentvectors do"*/\
                /* since we need a new mismatch in new vector*/\
                \
            }\
            /* mismatch beyond branch*/\
            if (position > (*anchor)->bit)\
            {\
                /* dive down in nextold list for next branch descending from current vector*/\
                anchor = &((*anchor)->nextold);\
                /* if anchor is not NULL it will hold the result for a possible match*/\
                lastDecision = anchor ? *anchor : NULL;\
            }\
            else\
            {\
                /* arriving here, the mismatch is before next branch off current vector*/\
                assert(position < (*anchor)->bit);\
                goto insert;\
            }\
        }\
    }\
    \
    /* increment global bit counter*/\
    ++position;\
    \
    /* increment index in currentvector*/\
    I\
    /*increment index in currentmarking*/\
    if (placebit_index == 0)\
    {\
        place_index++;\
        if (place_index >= Place::CardSignificant)\
        {\
            /* no mismatch --> state found*/\
            if(lastDecision)\
            {\
                lastDecision->getPayload(result);\
            }\
            else\
            {\
                getFirstPayload(ns.HashCurrent, result);\
            }\
            return true;\
        }\
        placebit_index = Place::CardBits[place_index] - 1;\
    }\
    else\
    {\
        --placebit_index;\
    }
#endif
                BINSTORE_LOOP_BODY( >> 7,);
                BINSTORE_LOOP_BODY( >> 6,);
                BINSTORE_LOOP_BODY( >> 5,);
                BINSTORE_LOOP_BODY( >> 4,);
                BINSTORE_LOOP_BODY( >> 3,);
                BINSTORE_LOOP_BODY( >> 2,);
                BINSTORE_LOOP_BODY( >> 1,);
                BINSTORE_LOOP_BODY(, ++vector_byte;);
            }
        }
insert:
        // state not found --> prepare for insertion
        Decision* newdecision = createDecision(position);
        newdecision -> nextold = * anchor;
        * anchor = newdecision;
        newdecision -> nextnew = NULL;
        newvector = &(newdecision -> vector);
        newdecision->getPayload(result); // return result
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
    *newvector = (unsigned char*) calloc(((Place::SizeOfBitVector - position) + 7) / 8, sizeof(unsigned char));
    vector_byte = 0;
    while (true)
    {
        // the body of this loop exists in 8 incarnations, once for each bit of vector byte
#ifdef BINSTORE_LOOP_BODY_2
#error BINSTORE_LOOP_BODY_2 already defined
#else
#define BINSTORE_LOOP_BODY_2(N, I)\
    if (ns.Current[place_index] & (1 << placebit_index))\
    {\
        assert(vector_byte < ((Place::SizeOfBitVector - position) + 7) / 8);\
        (*newvector)[vector_byte] |= N;\
    }\
    /* increment vector byte */\
    I\
    if (placebit_index == 0)\
    {\
        ++place_index;\
        if (place_index >= Place::CardSignificant)\
        {\
            break;\
        }\
        placebit_index = Place::CardBits[place_index] - 1;\
    }\
    else\
    {\
        --placebit_index;\
    }
#endif
        BINSTORE_LOOP_BODY_2(128,); /*** incarnation for bit 7 ********/
        \
        BINSTORE_LOOP_BODY_2(64,);
        BINSTORE_LOOP_BODY_2(32,);
        BINSTORE_LOOP_BODY_2(16,);
        BINSTORE_LOOP_BODY_2(8,);
        BINSTORE_LOOP_BODY_2(4,);
        BINSTORE_LOOP_BODY_2(2,);
        BINSTORE_LOOP_BODY_2(1, ++vector_byte;); /*** incarnation for bit 0 ********/
        \
    }

    ++markings;
    return false;
}

// debugging only
// LCOV_EXCL_START
void BinStore::pbs(unsigned int b, unsigned int p, unsigned char* f, void* v)
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
void BinStore::printBinStore()
{
    for (unsigned int i = 0; i < Place::CardSignificant; i++)
    {
        printf("%s:%d ", Net::Name[PL][i], Marking::Current[i]);
    }
    printf("\n\n");

    for (unsigned int i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        printf("\n hash value %u\n", i);
        if (firstvector[i])
        {
            BinStore::pbs(0, 0, firstvector[i], branch[i]);
        }
        printf("\n");
    }
}
// LCOV_EXCL_STOP

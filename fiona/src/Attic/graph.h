#ifndef GRAPH_H
#define GRAPH_H

#include "mynew.h"
#include "dimensions.h"
#include "owfnTransition.h"
#include "owfn.h"
#include "owfnPlace.h"
#include "main.h"
#include "debug.h"
#include <iostream>

#include "state.h"
#include "binDecision.h"
#include "searchTrace.h"

using namespace std;

// depository and search structures for reachability graph

extern unsigned int NonEmptyHash;

void statistics(unsigned int, unsigned int, unsigned int);

inline unsigned int bitstoint(unsigned char * bytepos, int bitpos, int nrbits) {
	// transform the sequence of nrbits bits in a char array starting at byte bytepos, bit bitpos
 	// into an integer

	unsigned int result;

	result = 0;

	if(nrbits <= 8 - bitpos) {
		// all bits in same byte
		result = (*bytepos >> 8 - (bitpos + nrbits)) % (1 << nrbits);
		return result;
	}
	result = (*bytepos % (1 << ((8 - bitpos))));
	nrbits-= 8 - bitpos;
	bytepos++;
	while(nrbits > 8) {
		nrbits -= 8;
		result = (result << 8) + * bytepos;
		bytepos++;
	}
	if(nrbits) {
		result = (result << nrbits) + ( * bytepos >> (8 - nrbits));
	}
	return result;
}

inline void trace() {
	// bin_p = index, bin_pb = 0!
	Trace[bin_p].bn_byte =  bin_byte; 
	Trace[bin_p].bn_t = bin_t; 
	Trace[bin_p].bn_v = bin_v; 
	Trace[bin_p].bn_s = bin_s; 
	Trace[bin_p].bn_dir = bin_dir; 
	Trace[bin_p].bn_b = bin_b; 
	Trace[bin_p].frmdec = fromdec;
    Trace[bin_p].vectrdec = vectordec;
	Trace[bin_p].tdec = todec;
}

	
#endif

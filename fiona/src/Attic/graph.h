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

using namespace std;

// depository and search structures for reachability graph

class State;
class oWFN;

// State objects are nodes in the state graph.


class State {
public:
  State();
  ~State();
  static unsigned int card;         // number of states
  unsigned int CardFireList;        // number of transitions to be fired
  owfnTransition ** firelist;       // Transitions to be fired
  owfnTransition ** quasiFirelist;  // Transitions that are quasi enabled
  unsigned int current;             // Nr of last already fired element of firelist
//  unsigned int * myMarking;         // current marking at that state
  binDecision * my_entry;           // last branch in binary decision tree that represents marking
  unsigned int placeHashValue;      // hashValue of that state (necessary for binDecision)
  State ** succ;                    // successor states in graph
  State * parent;                   // state responsible for first generation
  stateType type;                   // type of state (Deadlock, Final, Transient)
  void decode(oWFN *);      		// decode state into given marking vector
  void decodeShowOnly(oWFN *);		// decode state into given marking vector just for showing the marking!!!

  // Provides user defined operator new. Needed to trace all new operations on
  // this class.
#undef new
  NEW_OPERATOR(State)
#define new NEW_NEW
};


inline State::State() :
				type(TRANS),
				current(0),
				CardFireList(0),
				placeHashValue(0) {
	card++;
}

// binDecision records form a binary decision tree


class binDecision {
public:
	int bitnr;
	binDecision *  nextold;
	binDecision *  nextnew;
	unsigned char * vector; 	// actual bit vector
    binDecision * prev; 		// backlink to previous decision
    State * state;   			// link to state record for this state
	binDecision(int, long int);
	~binDecision();

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(binDecision)
#define new NEW_NEW
};

State * binSearch();


inline State::~State() {
  if(firelist) delete [] firelist;
  if(quasiFirelist) delete [] quasiFirelist;
  if(succ) delete [] succ;
}


extern unsigned int NonEmptyHash;

void statistics(unsigned int, unsigned int, unsigned int);


inline int logzwo(int m) {
	int k;

	k = 0;
	while(m) {
		k++;
		m = m >> 1;
	}
	return k;
}


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


inline void inttobits(unsigned char * bytepos, int bitpos, int nrbits, unsigned int value) {
	// store value as a bit sequence of a char array starting at byte bytepos, bit bitpos and consisting of nrbits bits

	unsigned char comp;
	if(nrbits <= 8 - bitpos) {
		// value fits completely into first byte
		* bytepos |= value << (8 - (bitpos + nrbits));
		return;
	}
	* bytepos |= value >> (nrbits + bitpos - 8);
	nrbits -= 8 - bitpos;
	value = value % (1 << nrbits);
	bytepos++;
	while(nrbits > 8) {
		nrbits -= 8;
		*bytepos = value >> nrbits;
		bytepos++;
		value = value % (1 << nrbits) ;
	}
	if(nrbits) {
		*bytepos = value << 8 - nrbits;
	}
	return;
}
	
	
inline binDecision::binDecision(int b, long int BitVectorSize) {
	int i;
	bitnr = b;
	nextold = nextnew = (binDecision *) 0;
	vector = new unsigned char [ (BitVectorSize - (bitnr + 2) ) / 8 + 2];
	for(i=0;i<(BitVectorSize - (bitnr + 2) ) / 8 + 1;i++) {
		vector[i] = 0;
	}
}


inline binDecision::~binDecision() {
	delete[] vector;
	delete state;
}


class SearchTrace {
public:
	unsigned char bn_byte;	// byte to be matched against tree vector; constructed from MARKINGVECTOR
	int bn_t;				// index in tree vector
	unsigned char * bn_v;	// current tree vector
	int bn_s;				// nr of bits pending in byte from previous iteration
	int bn_dir;				// did we go "old" or "new" in last decision?
	int bn_b;				// bit nr at start of byte
	binDecision * frmdec, * tdec;
	binDecision * vectrdec;
};


extern SearchTrace * Trace;
extern unsigned int pos;

//	extern binDecision ** binHashTable;
extern int bin_p; // (=place); index in CurrentMarking
extern int bin_pb; // next bit of place to be processed;
extern unsigned char bin_byte; // byte to be matched against tree vector; constructed from CurrentMarking
extern int bin_t; // index in tree vector
extern unsigned char * bin_v; // current tree vector
extern int bin_s; // nr of bits pending in byte from previous iteration
extern int bin_d; // difference position
extern int bin_dir; // did we go "old" or "new" in last decision?
extern int bin_b; // bit nr at start of byte
extern binDecision * fromdec, * todec;
extern binDecision * vectordec;
        

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

      
inline State * binSearch(binDecision * Bucket, oWFN * PN) {
	
//	cout << "search for marking " << PN->printMarking(PN->CurrentMarking) << endl;
	
	bin_dir = 0;
	// search the state given in CurrentMarking in the binHashTable
	if(!( fromdec = Bucket)) {
//		cout << "\t not found!" << endl;
		return (State *) 0;
	}
	
	bin_p = 0;						// current place
	bin_pb = 0;						// current bit in current place
	bin_s = 0;						// 
	bin_t = 0;						//
	bin_b = 0;						//
	bin_byte = 0;					// working byte to compare
	bin_v = fromdec -> vector;		// vector to compare
	todec = fromdec -> nextold;		// next decision in bintree
	vectordec = fromdec;			// bindecision of v
	
	while(1) {

		// - fill byte starting at bit s
		while(bin_s < 8 && bin_p < PN->getPlaceCnt()) {
			if(8 - bin_s < PN->Places[bin_p]->nrbits - bin_pb) {
				inttobits(&bin_byte, bin_s, 8 - bin_s, (PN->CurrentMarking[bin_p] % (1 << (PN->Places[bin_p] -> nrbits - bin_pb)))>> (PN->Places[bin_p]->nrbits + bin_s - (8 + bin_pb)));
				bin_pb += 8 - bin_s;
				bin_s = 8;
				break;
			} else {
				inttobits(&bin_byte, bin_s, PN->Places[bin_p] -> nrbits - bin_pb, PN->CurrentMarking[bin_p] % (1 << (PN->Places[bin_p] -> nrbits - bin_pb)));
				bin_s += PN->Places[bin_p] -> nrbits - bin_pb;
				bin_p++;
				bin_pb = 0;
			}
		}
		// - compare byte with bin_v[bin_t] and find first difference
		bin_d = 8 - logzwo( bin_v[bin_t] ^ bin_byte);
		// - conclude
		// case 0: length of vector reached without difference = state found
		if(bin_b + bin_d >= PN->BitVectorSize) {
//			cout << "\t state found at address: " << vectordec -> state << endl;
			return vectordec -> state;
		}
checkagain:     if(todec) {
			if(bin_b + bin_d < todec -> bitnr) {
				// case 1: no difference or difference before "to" branch = state not found
				if(bin_d < 8) {
//					cout << "\t not found!" << endl;
					return (State *) 0;
				} else {
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
			if(bin_b + bin_d == todec -> bitnr) {
				// case 2: no difference or difference equals "to" branch = continue with nextnew
				if(bin_d<8) {
					bin_byte = bin_byte << (bin_d+1);
					bin_s = 7 - bin_d;
					bin_v = todec -> vector;
					bin_t = 0;
					bin_b = todec -> bitnr + 1;
					vectordec = 
						fromdec = todec;
					todec = fromdec -> nextnew;
					bin_dir = 1;
					continue;
				} else {
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
			
			// case 3: difference beyond "to" branch = continue with nextold
			fromdec = todec;
			todec = fromdec -> nextold;
			bin_dir = 0;
			goto checkagain;
		} else {
			// case 4: no difference or state not found
			if(bin_d < 8) {
//				cout << "\t not found!" << endl;
				return (State *) 0;
			} else {
				bin_s = 0;
				bin_t++;
				bin_b += 8;
				bin_byte = 0;
				continue;
			}
		}
	}
	
//	cout << "\t state found at address: ???" << endl;
}


inline State * binInsert(binDecision ** Bucket, oWFN * PN) {
	// relies on a previous bin_search with all values bin_* set properly!

	trace(TRACE_5, "binInsert(binDecision ** Bucket, oWFN * PN): start\n");

//	cout << "binInsert: inserting " << PN->printMarking(PN->CurrentMarking) << endl;

	binDecision * newd;
	int vby, vbi;

	if(!*Bucket) {
		// empty hash bucket --> create first entry

	    NonEmptyHash++;
		*Bucket = new binDecision(-1, PN->BitVectorSize);
		// 3. remaining places
		vby = vbi = 0;
		for(bin_p=0; bin_p < PN->getPlaceCnt();bin_p ++) {
			inttobits((*Bucket)->vector + vby, vbi, PN->Places[bin_p] -> nrbits, PN->CurrentMarking[bin_p]);
			vby += (vbi + PN->Places[bin_p] -> nrbits) / 8;
			vbi =  (vbi + PN->Places[bin_p] -> nrbits) % 8;
		}

		(*Bucket)-> prev = (binDecision *) 0;
		(*Bucket)->state = new State;
		(*Bucket)->state -> my_entry = * Bucket;
		
//		cout << "\t at address: " << (*Bucket)->state << endl;
		
		return (*Bucket)->state;
	}
	
	newd = new binDecision(bin_b + bin_d, PN->BitVectorSize);
	if(bin_dir) {
		newd -> nextold = fromdec -> nextnew;
		fromdec -> nextnew = newd;
	} else {
		newd -> nextold = fromdec -> nextold;
		fromdec -> nextold = newd;
	}
	
	newd -> prev = vectordec;
	
	// fill vector
	// 1. remaining   bin_byte
	if(bin_d < 7) {
		newd->vector[0] = bin_byte << (bin_d+1);
		vby = 0;
		vbi = 7 - bin_d;
	} else {
		vby = 0;
		vbi = 0;
	}
		
	// 2. remaining bit of current place
	if(bin_pb) {
		inttobits(newd->vector, vbi, PN->Places[bin_p] -> nrbits - bin_pb, PN->CurrentMarking[bin_p] % (1 << (PN->Places[bin_p] -> nrbits - bin_pb)));
		vby += (vbi + PN->Places[bin_p] -> nrbits - bin_pb) / 8;
		vbi =  (vbi + PN->Places[bin_p] -> nrbits - bin_pb) % 8;
		bin_p++;
	}

	// 3. remaining places
	for(; bin_p < PN->getPlaceCnt();bin_p ++) {
		inttobits(newd->vector + vby, vbi, PN->Places[bin_p] -> nrbits, PN->CurrentMarking[bin_p]);
		vby += (vbi + PN->Places[bin_p] -> nrbits) / 8;
		vbi =  (vbi + PN->Places[bin_p] -> nrbits) % 8;
	}

	newd->state = new State;
	newd->state->my_entry = newd;
        
//	cout << "\t at address: " << newd->state << endl;
	return newd -> state;
	
}


inline void binDelete(binDecision ** Bucket, long int BitVectorSize) {
	// deletes the top level state in HashTable[h]

	// procedure:
	// - top level state has nextold, but no nextnew (no branch at -1)
	// - descend to end of nextold list (that state has largest common prefix with top level state)
	// - copy remainder of that state into top level state vector
	// - append nextnew of that state to end of nextold list (its nextold is 0)
	// - free that state's vector and decision record

	binDecision ** oldlist;

	if((*Bucket) -> nextold) {
		int vbyte, obyte;
		int upper, lower; 
		unsigned char * v, * o;
		v = (*Bucket) -> vector;
		
		for(oldlist = &((*Bucket) -> nextold); (*oldlist) -> nextold ; oldlist = & (  (*oldlist) -> nextold));
		vbyte = (*oldlist) -> bitnr / 8;
		upper = (*oldlist) -> bitnr % 8;
		o = (*oldlist) -> vector;
		// copy state vector
		// 1. reverse branch bit
		v[vbyte] = v[vbyte] ^ (1 << (7 - upper++));
		
		if(upper == 8) { upper = 0; vbyte++;}
		// 2. copy remaining vector
		if(upper) {
			
			lower = 8 - upper;
			obyte = 0;
			unsigned char vuppermask = ( (1 << upper) - 1) << lower ;
			unsigned char vlowermask = (1 << lower) - 1;
			unsigned char ouppermask = ( (1 << lower) - 1) << upper ;
			unsigned char olowermask = (1 << upper) - 1;
			
			do {
				/* 1. upper part of o --> lower part of v */
				v[vbyte] = (v[vbyte]  & vuppermask) | ((o[obyte] & ouppermask) >> upper);
				vbyte++;
				if( 8 * vbyte >= BitVectorSize) break;
				/* 2. lower part of o --> upper part of v */
				v[vbyte] = (v[vbyte]  & vlowermask) | ((o[obyte++] & olowermask) << lower);
				if( (*oldlist)->bitnr + 8 * obyte >= BitVectorSize) break;
			} while(1);
		} else {
			for(obyte=0; (*oldlist)->bitnr + 8 * obyte < BitVectorSize;obyte++) {
				v[vbyte++] = o[obyte];
			}
		}
		// 3. delete vector and set pointer in decision records
		delete [] (*oldlist) -> vector;
		(*oldlist) = (*oldlist) -> nextnew;
	} else {
		// top level state is last state in list
		delete [] (*Bucket) -> vector;
		*Bucket = (binDecision *) 0;
	}
}


inline void binDeleteAll(binDecision * d) {
	if(!d) return;
	binDeleteAll(d -> nextold);
	binDeleteAll(d -> nextnew);
	//delete [] d -> vector;
	delete d;
}
	

inline void binDeleteAll(oWFN * PN, int h) {
	// delete all states in hash bucket h
	if(!PN->binHashTable[h]) return;
	binDeleteAll(PN->binHashTable[h] -> nextold);
	delete [] PN->binHashTable[h] -> vector;
	PN->binHashTable[h] = (binDecision *) 0;
}


inline State * binSearch(oWFN * PN) {

	trace(TRACE_5, "binSearch(oWFN * PN)\n");
	return binSearch(PN->binHashTable[PN->getPlaceHashValue()], PN);
}


inline State * binInsert(oWFN * PN) {

	trace(TRACE_5, "binInsert(oWFN * PN)\n");
	return binInsert(PN->binHashTable + (PN->getPlaceHashValue()), PN);

}

	
#endif

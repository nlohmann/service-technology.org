/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    state.cc
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "state.h"
#include "enums.h"
#include "owfnTransition.h"
#include "owfn.h"
#include "binDecision.h"


//! \fn State::State()
//! \brief constructor
// inline rausgenommen!
State::State() :
				CardFireList(0),
				firelist(NULL),
				quasiFirelist(NULL),
				current(0),
				myMarking(NULL),
				my_entry(NULL),
				placeHashValue(0),
				succ(NULL),
				parent(NULL),
				type(TRANS)
                {
	card++;
}

//! \fn State::~State()
//! \brief destructor
// inline rausgenommen!
State::~State() {
    if(firelist) {
		delete [] firelist;
    }
    if(quasiFirelist) {
		delete [] quasiFirelist;
    }
    if(succ) {
		delete [] succ;
    }
    if (myMarking) {
    	delete [] myMarking;
    }
}

//! \fn void State::decode(oWFN * PN)
//! \param PN the corresponding open workflow net
//! \brief decodes State in bintree and writes the corresponding marking into currentMarking
void State::decode(oWFN * PN) {
	
	trace(TRACE_5, "void State::decode(int * v, oWFN * PN):start\n");

	numberOfDecodes++;
	
//	for (int i = 0; i < PN->getPlaceCnt(); i++) {
//		cout << PN->Places[i]->name << " (" << PN->Places[i]->nrbits << ")" << endl;
//	}

    binDecision * currentbindec;                // the considered part of bintree
    currentbindec = my_entry;

    unsigned char * currentvector;              // the corresponding bitvector
    currentvector = my_entry -> vector;

    int vectorsfirstbit;                        // offset where first bit of currentvector starts
    vectorsfirstbit = my_entry -> bitnr + 1;    // bit of decision not represented in vector

    int currentplacenr;                         // iterates through all places of the oWFN
    currentplacenr = PN -> getPlaceCnt() - 1;   // starting with the last one

    int pfirst;                                 // first bit of current place in bintree
    pfirst = PN->Places[currentplacenr]->startbit;

    int plast;                                  // last  bit of place in bintree
    plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

    int currentbyte;                            // current byte in currentvector of bintree
    currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8 ;
        
    
    int cfirst;                                 // first bit of current byte in currentvector of bintree
    cfirst = vectorsfirstbit + 8*currentbyte;

    int clast;                                  // last  bit of current byte in currentvector of bintree
    clast = PN->BitVectorSize - 1;

    long int cutplace;                          // the value decoded for the current place
    cutplace = currentvector[currentbyte];

    // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
    cutplace = cutplace >> (7-(clast -cfirst));
    cutplace = cutplace << (7-(clast -cfirst));

	while(1) {
        if(cfirst < pfirst) {
            // vorn abschneiden = verunden mit 00011111 wenn differenz 3
            cutplace &= (1 << (cfirst + 8 - pfirst)) - 1;
        }

		// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts ausrichten
		cutplace = cutplace >> (7 - (clast - cfirst));

        if(clast > plast) {
            // noch hinten abschneiden
            cutplace = cutplace >> (clast - plast);
        }


		// eintragen
		if(plast > clast) {
            // always preceded by the else-branch at prior currentbyte
			PN->CurrentMarking[currentplacenr] += cutplace << (plast - clast); 		
		} else {
			PN->CurrentMarking[currentplacenr] = cutplace;
		}

//		// after decoding the new marking for a place update the final condition
//		if (PN->FinalCondition) {
//		    for(int j=0; j < PN->Places[currentplacenr]->cardprop; j++) {
//				if (PN->Places[currentplacenr]->proposition != NULL) {
//				    PN->Places[currentplacenr]->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
//				}
//		    }
//		}
		
		// erster Fall: neuer Platz und neues Byte
		if(cfirst == pfirst) {
			if(currentplacenr == 0) {
				// all places decoded - finishing
				PN->placeHashValue = placeHashValue;
				PN->initializeTransitions();

				trace(TRACE_5, "void State::decode(int * v, oWFN * PN):end\n");
				return;
			}
			
			// take new place
			currentplacenr--;

			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

			if(currentbyte == 0) {
				// go to predecessing vector in bintree
				currentbindec = currentbindec -> prev;
				currentvector = currentbindec -> vector;
				vectorsfirstbit = currentbindec -> bitnr + 1;
				currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
				clast = cfirst - 1;
				cfirst = vectorsfirstbit + 8 * currentbyte;
				
//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(bindecision changed)" << endl;
				
				cutplace =	currentvector[currentbyte];
				// bit of decision not represented in vector -> so XOR with 1
				cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
				// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
				cutplace = cutplace >> (7-(clast -cfirst));
				cutplace = cutplace << (7-(clast -cfirst));
			} else {
				// go to predecessing byte in current vector
				currentbyte--;
				cfirst -= 8;
				clast = cfirst + 7;

//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(in same bindecision)" << endl;

				cutplace = currentvector[currentbyte];
			}
			
			continue;
		}
		
		// zweiter Fall: neuer Platz, gleiches Byte
		if(cfirst < pfirst) {
			currentplacenr--;
			
			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

//			cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in same byte " << currentbyte << endl;
			
			cutplace = currentvector[currentbyte];
			
			continue;
		}
		
		// hier implizit pfirst < cfirst, also
		// dritter Fall: gleicher Platz in neuem Byte
		if(currentbyte == 0) {
			// new vector
			currentbindec = currentbindec -> prev;
			currentvector = currentbindec -> vector;
			vectorsfirstbit = currentbindec -> bitnr + 1;
			currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
			clast = cfirst - 1;
			cfirst = vectorsfirstbit + 8 * currentbyte;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(bindecision changed)" << endl;

			cutplace =	currentvector[currentbyte];
			// bit of decision not represented in vector -> so XOR with 1
			cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
			// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
			cutplace = cutplace >> (7-(clast -cfirst));	
			cutplace = cutplace << (7-(clast -cfirst));	
		} else {
			currentbyte--;
			cfirst -= 8;
			clast = cfirst + 7;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(in same bindecision)" << endl;

			cutplace = currentvector[currentbyte];
		}
	}

	cerr << "this line is not reachable" << endl;
	cerr << "if this happens please contact the authors" << endl;	
	cerr << "\tvoid State::decode(oWFN * PN) in graph.cc" << endl;
}

//! \fn void State::decodeShowOnly(oWFN * PN)
//! \param PN the corresponding open workflow net
//! \brief decodes State in bintree and returns the corresponding marking
void State::decodeShowOnly(oWFN * PN) {
	
	trace(TRACE_5, "void State::decodeShowOnly(oWFN * PN) : start\n");
		
//	for (int i = 0; i < PN->getPlaceCnt(); i++) {
//		cout << PN->Places[i]->name << " (" << PN->Places[i]->nrbits << ")" << endl;
//	}

	numberOfDecodes++;

    binDecision * currentbindec;                // the considered part of bintree
    currentbindec = my_entry;

    unsigned char * currentvector;              // the corresponding bitvector
    currentvector = my_entry -> vector;

    int vectorsfirstbit;                        // offset where first bit of currentvector starts
    vectorsfirstbit = my_entry -> bitnr + 1;    // bit of decision not represented in vector

    int currentplacenr;                         // iterates through all places of the oWFN
    currentplacenr = PN -> getPlaceCnt() - 1;   // starting with the last one

    int pfirst;                                 // first bit of current place in bintree
    pfirst = PN->Places[currentplacenr]->startbit;

    int plast;                                  // last  bit of place in bintree
    plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

    int currentbyte;                            // current byte in currentvector of bintree
    currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8 ;
        
    
    int cfirst;                                 // first bit of current byte in currentvector of bintree
    cfirst = vectorsfirstbit + 8*currentbyte;

    int clast;                                  // last  bit of current byte in currentvector of bintree
    clast = PN->BitVectorSize - 1;

    long int cutplace;                          // the value decoded for the current place
    cutplace = currentvector[currentbyte];

    // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
    cutplace = cutplace >> (7-(clast -cfirst));
    cutplace = cutplace << (7-(clast -cfirst));

	while(1) {		
        if(cfirst < pfirst) {
            // vorn abschneiden = verunden mit 00011111 wenn differenz 3
            cutplace &= (1 << (cfirst + 8 - pfirst)) - 1;
        }

		// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts ausrichten
		cutplace = cutplace >> (7 - (clast - cfirst));

        if(clast > plast) {
            // noch hinten abschneiden
            cutplace = cutplace >> (clast - plast);
        }


		// eintragen
		if(plast > clast) {
            // always preceded by the else-branch at prior currentbyte
			PN->CurrentMarking[currentplacenr] += cutplace << (plast - clast); 		
		} else {
			PN->CurrentMarking[currentplacenr] = cutplace;
		}

//		// after decoding the new marking for a place update the final condition
//		if (PN->FinalCondition) {
//		    for(int j=0; j < PN->Places[currentplacenr] -> cardprop; j++) {
//				if (PN->Places[currentplacenr]->proposition != NULL) {
//				    PN->Places[currentplacenr]->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
//				}
//		    }
//		}


		// erster Fall: neuer Platz und neues Byte
		if(cfirst == pfirst) {
			if(currentplacenr == 0) {
				// all places decoded - finishing
				trace(TRACE_5, "void State::decodeShowOnly(oWFN * PN) : end\n");
				
				return;
			}
			
			// take new place
			currentplacenr--;

			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

			if(currentbyte == 0) {
				// go to predecessing vector in bintree
				currentbindec = currentbindec -> prev;
				currentvector = currentbindec -> vector;
				vectorsfirstbit = currentbindec -> bitnr + 1;
				currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
				clast = cfirst - 1;
				cfirst = vectorsfirstbit + 8 * currentbyte;
				
//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(bindecision changed)" << endl;
				
				cutplace =	currentvector[currentbyte];
				// bit of decision not represented in vector -> so XOR with 1
				cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
				// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
				cutplace = cutplace >> (7-(clast -cfirst));
				cutplace = cutplace << (7-(clast -cfirst));
			} else {
				// go to predecessing byte in current vector
				currentbyte--;
				cfirst -= 8;
				clast = cfirst + 7;

//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(in same bindecision)" << endl;

				cutplace = currentvector[currentbyte];
			}
			
			continue;
		}
		
		// zweiter Fall: neuer Platz, gleiches Byte
		if(cfirst < pfirst) {
			currentplacenr--;
			
			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

//			cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in same byte " << currentbyte << endl;
			
			cutplace = currentvector[currentbyte];
			
			continue;
		}
		
		// hier implizit pfirst < cfirst, also
		// dritter Fall: gleicher Platz in neuem Byte
		if(currentbyte == 0) {
			// new vector
			currentbindec = currentbindec -> prev;
			currentvector = currentbindec -> vector;
			vectorsfirstbit = currentbindec -> bitnr + 1;
			currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
			clast = cfirst - 1;
			cfirst = vectorsfirstbit + 8 * currentbyte;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(bindecision changed)" << endl;

			cutplace =	currentvector[currentbyte];
			// bit of decision not represented in vector -> so XOR with 1
			cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
			// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
			cutplace = cutplace >> (7-(clast -cfirst));	
			cutplace = cutplace << (7-(clast -cfirst));	
		} else {
			currentbyte--;
			cfirst -= 8;
			clast = cfirst + 7;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(in same bindecision)" << endl;

			cutplace = currentvector[currentbyte];
		}
	}

	cerr << "this line is not reachable" << endl;
	cerr << "if this happens please contact the authors" << endl;	
	cerr << "\tvoid State::decode(oWFN * PN) in graph.cc" << endl;
	
}

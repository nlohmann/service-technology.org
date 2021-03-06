/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    state.cc
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "state.h"
#include "options.h"
#include "binDecision.h"
#include "debug.h"
#include "owfnTransition.h"
#include <cassert>


//! \brief constructor
//! \fn State::State()
State::State() :
    		cardFireList(0), firelist(NULL), 
            quasiFirelist(NULL), current(0), my_entry(NULL), placeHashValue(0),
            succ(NULL), parent(NULL), type(TRANS), nexttar(NULL), prevtar(NULL) {
    
	state_count++;
    state_count_stored_in_binDec++;

    dfs = 0;
    tlowlink = 0;
    tarlevel = 0;
    repTSCC = false;
}


//! \brief destructor
//! \fn State::~State()
State::~State() {
	
    if (firelist) {
        delete [] firelist;
    }
    if (quasiFirelist) {
        delete [] quasiFirelist;
    }
    if (succ) {
        delete [] succ;
    }
    
    state_count_stored_in_binDec--;
}


/// \brief Decodes State in bintree and writes the corresponding marking into
///        the CurrentMarking of the given 'PN'.
/// \param PN The corresponding open net.
void State::decodeShowOnly(oWFN* PN) {

    TRACE(TRACE_5, "void State::decodeShowOnly(oWFN * PN) : start\n");

    numberOfDecodes++;

    //	for (int i = 0; i < PN->getPlaceCount(); i++) {
    //		cout << PN->getPlace(i)->name << " (" << PN->getPlace(i)->nrbits << ")" << endl;
    //	}

    binDecision * currentbindec; // the considered part of bintree
    currentbindec = my_entry;

    unsigned char * currentvector; // the corresponding bitvector
    currentvector = my_entry -> vector;

    int vectorsfirstbit; // offset where first bit of currentvector starts
    vectorsfirstbit = my_entry -> bitnr + 1; // bit of decision not represented in vector

    int currentplacenr; // iterates through all places of the oWFN
    currentplacenr = PN -> getPlaceCount() - 1; // starting with the last one

    int pfirst; // first bit of current place in bintree
    pfirst = PN->getPlace(currentplacenr)->startbit;

    int plast; // last  bit of place in bintree
    plast = pfirst + PN->getPlace(currentplacenr)->nrbits- 1;

    int currentbyte; // current byte in currentvector of bintree
    currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8;

    int cfirst; // first bit of current byte in currentvector of bintree
    cfirst = vectorsfirstbit + 8*currentbyte;

    int clast; // last  bit of current byte in currentvector of bintree
    clast = PN->BitVectorSize - 1;

    long int cutplace; // the value decoded for the current place
    cutplace = currentvector[currentbyte];

    // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
    cutplace = cutplace >> (7-(clast -cfirst));
    cutplace = cutplace << (7-(clast -cfirst));

    while (true) {
        if (cfirst < pfirst) {
            // vorn abschneiden = verunden mit 00011111 wenn differenz 3
            cutplace &= (1 << (cfirst + 8- pfirst)) - 1;
        }

        // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts ausrichten
        cutplace = cutplace >> (7 - (clast - cfirst));

        if (clast > plast) {
            // noch hinten abschneiden
            cutplace = cutplace >> (clast - plast);
        }

        // eintragen
        if (plast > clast) {
            // always preceded by the else-branch at prior currentbyte
            PN->CurrentMarking[currentplacenr] += cutplace << (plast - clast);
        } else {
            PN->CurrentMarking[currentplacenr] = cutplace;
        }

        //		// after decoding the new marking for a place update the final condition
        //		if (PN->FinalCondition) {
        //		    for(int j=0; j < PN->getPlace(currentplacenr)->cardprop; j++) {
        //				if (PN->getPlace(currentplacenr)->proposition != NULL) {
        //				    PN->getPlace(currentplacenr)->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
        //				}
        //		    }
        //		}

        // erster Fall: neuer Platz und neues Byte
        if (cfirst == pfirst) {
            if (currentplacenr == 0) {
                // all places decoded - finishing
                TRACE(TRACE_5, "void State::decodeShowOnly(oWFN * PN) : end\n");
                return;
            }

            // take new place
            currentplacenr--;

            pfirst = PN->getPlace(currentplacenr)->startbit;
            plast = pfirst + PN->getPlace(currentplacenr)->nrbits- 1;

            if (currentbyte == 0) {
                // go to predecessing vector in bintree
                currentbindec = currentbindec -> prev;
                currentvector = currentbindec -> vector;
                vectorsfirstbit = currentbindec -> bitnr + 1;
                currentbyte = (cfirst - 1- vectorsfirstbit) / 8;
                clast = cfirst - 1;
                cfirst = vectorsfirstbit + 8 * currentbyte;

                // cout << "\t\tnew place " << PN->getPlace(currentplacenr)->name << " in new byte " << currentbyte << endl;
                // cout << "\t\t\t(bindecision changed)" << endl;

                cutplace = currentvector[currentbyte];
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

                // cout << "\t\tnew place " << PN->getPlace(currentplacenr)->name << " in new byte " << currentbyte << endl;
                // cout << "\t\t\t(in same bindecision)" << endl;

                cutplace = currentvector[currentbyte];
            }

            continue;
        }

        // zweiter Fall: neuer Platz, gleiches Byte
        if (cfirst < pfirst) {
            currentplacenr--;

            pfirst = PN->getPlace(currentplacenr)->startbit;
            plast = pfirst + PN->getPlace(currentplacenr)->nrbits- 1;

            // cout << "\t\tnew place " << PN->getPlace(currentplacenr)->name << " in same byte " << currentbyte << endl;

            cutplace = currentvector[currentbyte];

            continue;
        }

        // hier implizit pfirst < cfirst, also
        // dritter Fall: gleicher Platz in neuem Byte
        if (currentbyte == 0) {
            // new vector
            currentbindec = currentbindec -> prev;
            currentvector = currentbindec -> vector;
            vectorsfirstbit = currentbindec -> bitnr + 1;
            currentbyte = (cfirst - 1- vectorsfirstbit) / 8;
            clast = cfirst - 1;
            cfirst = vectorsfirstbit + 8 * currentbyte;

            // cout << "\t\tsame place " << PN->getPlace(currentplacenr)->name << " in new byte " << currentbyte << endl;
            // cout << "\t\t\t(bindecision changed)" << endl;

            cutplace = currentvector[currentbyte];
            // bit of decision not represented in vector -> so XOR with 1
            cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
            // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
            cutplace = cutplace >> (7-(clast -cfirst));
            cutplace = cutplace << (7-(clast -cfirst));
        } else {
            currentbyte--;
            cfirst -= 8;
            clast = cfirst + 7;

            // cout << "\t\tsame place " << PN->getPlace(currentplacenr)->name << " in new byte " << currentbyte << endl;
            // cout << "\t\t\t(in same bindecision)" << endl;

            cutplace = currentvector[currentbyte];
        }
    }

    assert(false);
    cerr << "this line is not reachable"<< endl;
    cerr << "if this happens please contact the authors"<< endl;
    cerr << "\tvoid State::decodeShowOnly(oWFN * PN) in graph.cc"<< endl;
}

//! \brief Same as decodeShowOnly(), but initializes all transition of the given 'PN'.
//! \param PN given oWFN
void State::decode(oWFN * PN) {
    TRACE(TRACE_5, "void State::decode(int * v, oWFN * PN):start\n");
    decodeShowOnly(PN);
    PN->placeHashValue = placeHashValue;
    PN->checkEnablednessOfAllTransitions();
    TRACE(TRACE_5, "void State::decode(int * v, oWFN * PN):end\n");
}


bool State::hasEnabledTransitionWithTauLabelForMatching() const {
    //cerr << "    State::hasEnabledTransitionWithTauLabelForMatching() start" << endl;
    for (size_t itrans = 0; itrans != cardFireList; ++itrans) {
        //cerr << "        check transition " << firelist[itrans]->getName() << "(" << firelist[itrans] << ")" << endl;
        if (!firelist[itrans]->hasNonTauLabelForMatching()) {
            //cerr << "        transition " << firelist[itrans]->getName() << "(" << firelist[itrans] << ") has TAU label" << endl;
            //cerr << "    State::hasEnabledTransitionWithTauLabelForMatching() end" << endl;
            return true;
        }
    }

    //cerr << "    State::hasEnabledTransitionWithTauLabelForMatching() end" << endl;
    return false;
}



//! \brief   returns exact type of state (Final, iDL, eDL, TR)
//! \return  The exact type of the state. Unlike the "state" member variable
//!          this function distinguishes between internal and external deadlocks.
//!          A marking is a deadlock iff it does not quasi-enable a transition.
//!          A deadlock is internal if no output place of the net is marked. A
//!          deadlock where an output place is marked is an external deadlock.
//! \note    This function will never return the value "DEADLOCK".
//! \post    CurrentMarking is overwritten by marking of the state under
//!          consideration if the state is a deadlock (see below).
stateType State::exactType() {
    switch (type) {
        case DEADLOCK: {
            bool internal_deadlock = true;
            
            if (!PN->quasiEnabledTransitions.isEmpty()) {
                internal_deadlock = false;
            } else {
                decode(PN); // overwrites PN->CurrentMarking !
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->CurrentMarking[PN->getPlaceIndex(PN->getOutputPlace(i))] > 0) {
                        internal_deadlock = false;
                        continue;
                    }
                }
            }
            
            if (internal_deadlock) {
                return I_DEADLOCK;
            } else {
                return E_DEADLOCK;
            }
        }
        
        // the other types do not need to be distinguished
        default: {
            return type;
        }
    }
}


/*!
 * \brief is state transient in autonomous setting
 *
 * The function traverses all quasi-enabled and all enabled transitions of the
 * state. If both the current port and one other port has a transition
 * (quasi-) enabled, then this state can not be seen as transient in the
 * autonomous setting, because otherwise ports would pass the responsibility
 * to resolve deadlocks to each other.
 *
 * \note This code is not entirely tested. In particular, it might be
 *       sufficient to only check quasi-enabled transitions. Likewise, an early
 *       abortion might be possible if a "real" transient transition was found.
 *
 * \return false iff the state can be seen as transient in autonomous setting,
 *         or the option "autonomous" is not set
 *
 * \author Niels Lohmann <niels.lohmann@uni-rostock.de>
 */
bool State::isNotAutonomouslyTransient() const {
    if (!parameters[P_AUTONOMOUS])
        return false;
    
    assert(cardFireList > 0);
    
    bool foundMy = false;
    bool foundOther = false;
    bool foundTransient = false;
    
    // traverse and check quasi-enabled transitions
    size_t i = 0;
    while (quasiFirelist && quasiFirelist[i]) {
        foundMy = foundMy || quasiFirelist[i]->isConnectedToMyPort;
        foundOther = foundOther || quasiFirelist[i]->isConnectedToOtherPort;

        i++;
    }
    
    // traverse the enabled transitions
    for (size_t t = 0; t != cardFireList; t++) {
        foundMy = foundMy || firelist[t]->isConnectedToMyPort;
        foundOther = foundOther || firelist[t]->isConnectedToOtherPort;
        
        if (!firelist[t]->isConnectedToMyPort && !firelist[t]->isConnectedToOtherPort)
            foundTransient = true;        
    }

    // if my port and another port has a transition that might fire, then this
    // marking can not be seen as transient in the autonomous setting
    if (foundOther && foundMy && !foundTransient)
        return true;
    
    return false;
}

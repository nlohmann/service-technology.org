/*!
\file StructuralReduction.cc
\author Andreas
\status new


*/
#include <cstdlib>
#include <cstring>

#include <iostream>
using std::cout;
using std::endl;

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "StructuralReduction.h"
#include "LinearAlgebra.h"
#include "Net.h"
#include "Transition.h"
#include "Place.h"

void setSignificantPlaces()
{
    // save number of places
    const index_t cardPL = Net::Card[PL];

    // request memory for one full row
    index_t* newVar = (index_t*) calloc(cardPL, SIZEOF_INDEX_T);
    int64_t* newCoef = (int64_t*) calloc(cardPL, sizeof(int64_t));

    // create new matrix
    Matrix m(cardPL);

    // load rows into matrix
    // for each transition t
    for (index_t t = 0; t < Net::Card[TR]; ++t) {
        // index in new row
        index_t newSize = 0;        
        // for each place p in the preset of t
        for (index_t p = 0; p < Net::CardArcs[TR][PRE][t]; ++p) {
            // store place index and the it's multiplicity (from p to t)
            newVar[newSize] = Net::Arc[TR][PRE][t][p];
            // positive numbers
            assert(Net::Mult[TR][PRE][t][p] != 0);
            newCoef[newSize] = Net::Mult[TR][PRE][t][p];
            // increase newSize
            ++newSize;
        }       
        // for each place p in the postset of t
        for (index_t p = 0; p < Net::CardArcs[TR][POST][t]; ++p) {
            const index_t pID = Net::Arc[TR][POST][t][p];                          
            assert(Net::Mult[TR][POST][t][p] != 0);
            
            // check whether the p is already in the new row
            // enumerate newVar till p is hit or not inside
            index_t possiblePosition = 0;
            for (; possiblePosition < newSize; ++possiblePosition) {
                if (newVar[possiblePosition] >= pID) {
                    break;                
                }
            }
            // distinguish which case is true (hit or not in)
            if (newVar[possiblePosition] == pID) {
                // p is already inside the new row, so subtract current multiplicity
                newCoef[possiblePosition] -= Net::Mult[TR][POST][t][p];
                // new coefficient may be 0 now
                if (newCoef[possiblePosition] == 0) {
                    // erase possiblePosition entry (possiblePosition) in both array
                    memmove(&newVar[possiblePosition], &newVar[possiblePosition + 1], (newSize - possiblePosition) * SIZEOF_INDEX_T);
                    memmove(&newCoef[possiblePosition], &newCoef[possiblePosition + 1], (newSize - possiblePosition) * sizeof(int64_t));
                    // assumption: decreasing 0 will lead to maxInt but
                    //              upcoming increase will result in 0 again
                    ----newSize;
                }
            }
            else {
                // p is not in new row, so add it
                // may be it is necessary to insert in between existing entrys
                memmove(&newVar[possiblePosition + 1], &newVar[possiblePosition], (newSize - possiblePosition) * SIZEOF_INDEX_T);
                memmove(&newCoef[possiblePosition + 1], &newCoef[possiblePosition], (newSize - possiblePosition) * sizeof(int64_t)); 
                // store place index
                newVar[possiblePosition] = pID;
                // store the multiplicity (from t to p)
                // negative numbers
                newCoef[possiblePosition] = -Net::Mult[TR][POST][t][p];
            }
            // increase newSize
            ++newSize;
        }        
        // save current arrays as new row
        m.addRow(newSize, newVar, newCoef);
        
        // clear used memory
        memset(newVar, 0, newSize * SIZEOF_INDEX_T);    // necessary?
        memset(newCoef, 0, newSize * sizeof(int64_t));
    }
    
    free(newVar);
    free(newCoef);
    
    // reduce matrix
    m.reduce();
    
    // gather significant places
    ///\todo reorder sigificant places
    //Place::CardSignificant = 0;
    for (index_t p = 0; p < cardPL; ++p) {
        if (m.isSignificant(p)) {
    //        cout << p << " is significant" << endl;
    //        Place::CardSignificant++;
        }
    }
    //cout << "significant places " << Place::CardSignificant << endl;
}

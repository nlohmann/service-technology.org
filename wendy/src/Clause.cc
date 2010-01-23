/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <iostream>
#include <cstdlib>
#include <sstream>


#include "Clause.h"
#include "StoredKnowledge.h"


/******************
 * STATIC MEMBERS *
 ******************/

Clause::_stats Clause::stats;

uint8_t Clause::bytes = 0;

// a clause holding just literal false
Clause* Clause::falseClause = reinterpret_cast<Clause*>(1);


/******************
 * STATIC METHODS *
 ******************/

Clause::_stats::_stats()
        : cumulativeSizeAllClauses(0), maximalSizeOfClause(0) {}


void Clause::initialize() {
    bytes = ((Label::last_sync - 1) / 8) +1;
}


/***************
 * CONSTRUCTOR *
 ***************/

/*
  \brief all values are initialized with 0
  \note we assume sizeof(uint8_t) == 1
*/
Clause::Clause()
  : storage((uint8_t*)calloc(bytes, 1)), finalKnowledges(NULL), numberOfFinalKnowledges(0), more_than_one_literal(0), decodedLabels(NULL) {

    assert(bytes > 0);
}


/*!
  destructor
*/
Clause::~Clause() {
    free(storage);
    delete[] decodedLabels;
    if (finalKnowledges != NULL) {

        for (unsigned int i = 0; i < numberOfFinalKnowledges; ++i) {
            delete finalKnowledges[i];
        }

        free(finalKnowledges);
    }
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
    \param _finalKnowledge the knowledge that is final and has to be remembered in this clause
*/
void Clause::addFinalKnowledge(const StoredKnowledge* _finalKnowledge) {
    if (finalKnowledges == NULL) {
        if (not (finalKnowledges = (FinalKnowledge**) calloc(1, sizeof(FinalKnowledge*)))) {
            return ;
        }
    } else {

        // make sure that the given knowledge has not been added to the clause so far
        for (unsigned int i = 0; i < numberOfFinalKnowledges; ++i) {
            if (finalKnowledges[i]->knowledge == _finalKnowledge) {
                return ;
            }
        }

        if (not (finalKnowledges = (FinalKnowledge**) realloc(finalKnowledges, (numberOfFinalKnowledges + 1) * sizeof(FinalKnowledge*)))) {
            return ;
        }
    }

    FinalKnowledge * finalKnowledge = new FinalKnowledge();
    finalKnowledge->knowledge = _finalKnowledge;

    // store the given knowledge
    finalKnowledges[numberOfFinalKnowledges] = finalKnowledge;

    ++numberOfFinalKnowledges;

    // we just stored a literal ;-)
    more_than_one_literal = numberOfFinalKnowledges > 1;
}


/*!
  set the given label to 1 in the storage
  \param l label
*/
void Clause::labelPossible(const Label_ID& l) {
    assert(storage != NULL);
    assert(SENDING(l) or RECEIVING(l) or SYNC(l));

    const uint8_t myByte = (l-1) / 8;
    const uint8_t myBit  = (l-1) % 8;
    storage[myByte] += (1 << myBit);
}


/*!
  overloaded bitwise comparison and the attribute final is passed as well
*/
void Clause::operator|=(const Clause& other) {
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] |= other.storage[i];
    }

    // add final knowledge of other if it is not part of the clause already
    for (unsigned int i = 0; i < other.numberOfFinalKnowledges; ++i) {
        // add final knowledge if it is not stored yet (done in that method)
        addFinalKnowledge(other.finalKnowledges[i]->knowledge);
    }
}


/*!
 decode the bit array into a char array (function is called more than once)
*/
void Clause::decode() {
    assert(bytes > 0);

    // only reserve memory and encode on first decoding
    if (decodedLabels == NULL) {

        Label_ID counter = 0;

        decodedLabels = new char[Label::last_sync];

        // decode and store values
        for (Label_ID l = 0; l < Label::last_sync; ++l) {
            const uint8_t myByte = (l) / 8;
            const uint8_t myBit  = (l) % 8;
            decodedLabels[l] = (storage[myByte] & (1 << myBit)) >> myBit;

            // count all literals of this clause
            if (decodedLabels[l]) {
                ++counter;
            }
        }

        // check if clause contains more than one literal
        more_than_one_literal = counter + numberOfFinalKnowledges > 1;

        // do statistics
        if (stats.maximalSizeOfClause < counter) {
            stats.maximalSizeOfClause = counter;
        }

        stats.cumulativeSizeAllClauses += counter;
    }
}


/*!
  creates a string out of the set of strings representing the annotation of the set of knowledges
  \param dot the string shall be used in the dot output or not
  \param file a stream to which this clause goes to
*/
void Clause::printToStream(const bool & dot, std::ostream& file) {
    // create the annotation of the current set of knowledges
    std::string stringOr = (dot) ? " &or; " : " + ";

    decode();

    if (decodedLabels != NULL) {
        bool first = true;

        if (more_than_one_literal) {
            file << "(";
        }

        // final cannot be the only literal in the clause, as otherwise this clause would not exist!
        if (numberOfFinalKnowledges > 0) {

            for (unsigned int i = 0; i < numberOfFinalKnowledges; ++i) {

                if (not first) {
                    file << stringOr;
                }

                file << reinterpret_cast<size_t>((StoredKnowledge* ) finalKnowledges[i]->knowledge);
                first = false;
            }

        }

        // get clause which contains !, ? or # events
        for (Label_ID i = 0; i < Label::last_sync; ++i) {

            if (decodedLabels[i] == 0) {
                continue;
            }
            if (not first) {
                file << stringOr;
            }

            file << Label::id2name[i+1];

            first = false;
        }

        if (more_than_one_literal) {
            file << ")";
        }
    }

    // required for diagnose mode in which a non-final node might have no
    // successors
//    if (formula.empty()) {
//      //  file << "false";
//    }
}

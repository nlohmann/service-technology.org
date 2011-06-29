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


#include "CompositeMarking.h"
#include "LivelockOperatingGuideline.h"
#include "Label.h"
#include "Clause.h"
#include "util.h"


/********************
 * CompositeMarking *
 ********************/

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
*/
CompositeMarking::CompositeMarking(const StoredKnowledge* _storedKnowledge,
                                   const InnerMarking_ID _innerMarking_ID,
                                   InterfaceMarking* _interface) : dfs(0), lowlink(0), storedKnowledge(_storedKnowledge), interface(_interface), innerMarking_ID(_innerMarking_ID)

{

}


/*!
  destructor deletes the interface only, because before constructing a composite marking the interface has been created anew
*/
CompositeMarking::~CompositeMarking() {
    // an interface stored in a composite marking has always been created anew just to get stored in here
    delete interface;
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

/*!
  constructs a string that contains the annotation of the composite marking with respect to the given set of knowledges
  \param booleanClause a pointer to the Boolean clause of the composite marking
  \param emptyClause if true, the clause of this composite marking is empty
*/
void CompositeMarking::getMyFormula(Clause* booleanClause, bool& emptyClause) {

    // receiving event
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        // receiving event resolves deadlock
        if (interface->marked(l) and
                storedKnowledge->successors[l - 1] != NULL and storedKnowledge->successors[l - 1] != StoredKnowledge::empty and
                storedKnowledge->successors[l - 1]->is_sane) {

            emptyClause = false;

            booleanClause->addTransitionLiteral(storedKnowledge, l);
        }
    }

    // synchronous communication
    for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
        // synchronous communication resolves deadlock
        if (InnerMarking::synchs[l].find(innerMarking_ID) != InnerMarking::synchs[l].end() and
                storedKnowledge->successors[l - 1] != NULL and storedKnowledge->successors[l - 1] != StoredKnowledge::empty and
                storedKnowledge->successors[l - 1]->is_sane) {

            emptyClause = false;

            booleanClause->addTransitionLiteral(storedKnowledge, l);
        }
    }

    // collect outgoing !-edges
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (storedKnowledge->successors[l - 1] != NULL and storedKnowledge->successors[l - 1]->is_sane) {

            emptyClause = false;

            booleanClause->addTransitionLiteral(storedKnowledge, l);
        }
    }

    // this marking is final
    if (interface->unmarked() and InnerMarking::inner_markings[innerMarking_ID]->is_final) {
        booleanClause->addFinalKnowledge(storedKnowledge);
        emptyClause = false;
    }
}


bool CompositeMarking::operator== (const CompositeMarking& other) const {

    if (storedKnowledge != other.storedKnowledge or innerMarking_ID != other.innerMarking_ID or* interface != *(other.interface)) {
        return false;
    }

    return true;
}

std::ostream& operator<< (std::ostream& o, const CompositeMarking& m) {
    o << "[";
    o << reinterpret_cast<size_t>(m.storedKnowledge);
    o << ", ";
    o << "[" << m.innerMarking_ID << ", " << *m.interface << "]";
    return o << "]";
}

/***************************
 * CompositeMarkingsHandler *
 ***************************/

/******************
 * STATIC MEMBERS *
 ******************/

CompositeMarking** CompositeMarkingsHandler::visitedCompositeMarkings = NULL;
std::vector<CompositeMarking* > CompositeMarkingsHandler::tarjanStack;
unsigned int CompositeMarkingsHandler::numberElements = 0;
unsigned int CompositeMarkingsHandler::maxSize = 0;
unsigned int CompositeMarkingsHandler::minBookmark = 0;
std::vector<Clause* > CompositeMarkingsHandler::conjunctionOfDisjunctionsBoolean;


/********************
 * STATIC FUNCTIONS *
 ********************/

/*!
  checks if the given marking has been visited already, if so return a pointer to the marking otherwise return the new marking
  Note: in case the marking has been found, you may need to delete the given marking afterwards
  \param _storedKnowledge the knowledge of the new marking
  \param _innerMarking_ID the inner marking of the new marking
  \param _interface the interface of the new marking
  \param foundMarking [in,out] indicates whether the "given" marking has been visited already

  \return pointer to the new marking or the marking that has been visited already but equals to the one given

  \todo Niels thinks the for loop can be simplified
*/
CompositeMarking* CompositeMarkingsHandler::isVisited(const StoredKnowledge* _storedKnowledge,
                                                      const InnerMarking_ID _innerMarking_ID,
                                                      InterfaceMarking* _interface,
                                                      bool& foundMarking) {

    // create new marking
    CompositeMarking* newMarking = new CompositeMarking(_storedKnowledge, _innerMarking_ID, _interface);
    foundMarking = false;

    // we have not yet seen any marking, so return the newly created one
    if (numberElements == 0) {
        return newMarking;
    }

    CompositeMarking* storedMarking;

    // iterate through all visited markings
    for (unsigned int i = 0; i < numberElements; ++i) {
        storedMarking = visitedCompositeMarkings[i];

        assert(storedMarking != NULL);

        // we have seen the "new" marking already, so return the old one and delete the new one
        if (*newMarking == *storedMarking) {

            foundMarking = true;

            delete newMarking;

            return storedMarking;
        }
    }

    // return the newly created marking
    return newMarking;
}


/*!
  store the given marking in the set of visited markings and set the Tarjan values of the marking to the current number of elements stored
  \param marking composite marking to be stored
*/
void CompositeMarkingsHandler::visitMarking(CompositeMarking* marking) {

    assert(visitedCompositeMarkings != NULL);
    assert(numberElements < maxSize);

    visitedCompositeMarkings[numberElements++] = marking;

    // set Tarjan values
    marking->dfs = marking->lowlink = numberElements;
}


/*!
  adds the given string clause to the conjunction of disjunctions associated with the current set of knowledges
  \param booleanClause a string containing a disjunction of events
*/
void CompositeMarkingsHandler::addClause(Clause* booleanClause) {

    // add whole clause to the conjunction of clauses
    conjunctionOfDisjunctionsBoolean.push_back(booleanClause);

    if (booleanClause == Clause::falseClause) {
        Clause::stats.cumulativeSizeAllClauses++;
    }

    if (booleanClause == Clause::trueClause) {
        Clause::stats.cumulativeSizeAllClauses++;
    }

    if (booleanClause != Clause::falseClause && booleanClause != Clause::trueClause) {
        Clause::stats.cumulativeSizeAllClauses += booleanClause->numberOfFinalKnowledges;
        Clause::stats.cumulativeSizeAllClauses += booleanClause->numberOfTransitionLiterals;

        unsigned int countLiteralsOfClause = 0;
        countLiteralsOfClause += booleanClause->numberOfFinalKnowledges;
        countLiteralsOfClause += booleanClause->numberOfTransitionLiterals;

        if (countLiteralsOfClause > Clause::stats.maximalSizeOfClause) {
            Clause::stats.maximalSizeOfClause = countLiteralsOfClause;
        }
    }
}


/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/


/*!
  initialize all values that are essential in the composite marking handler;
  using the given set of knowledges a disjunction of all possible sending events is constructed
  \note make sure function finalize() has been called before
  \param setOfStoredKnowledges the currently considered set of knowledges used for calculating the current set of sending events
*/
void CompositeMarkingsHandler::initialize(const std::set<StoredKnowledge* > & setOfStoredKnowledges) {

    // construct a valid disjunction of sending events
    FOREACH(iter, setOfStoredKnowledges) {
        maxSize += (*iter)->sizeAllMarkings;
    }

    visitedCompositeMarkings = new CompositeMarking*[maxSize];
}


/*!
  clean up all values
*/
void CompositeMarkingsHandler::finalize() {

    // delete all stored composite markings one by one
    for (unsigned int i = 0; i < numberElements; ++i) {
        delete visitedCompositeMarkings[i];
    }

    delete [] visitedCompositeMarkings;

    numberElements = 0;

    tarjanStack.clear();
    maxSize = 0;
    minBookmark = 0;
    conjunctionOfDisjunctionsBoolean.clear();
}

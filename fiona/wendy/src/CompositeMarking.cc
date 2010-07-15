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
  \todo check if this constructor is always called before CompositeMarkingsHandler::getMarking() -- if yes, combine them
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
  \param knowledgeSet set of knowledges
  \param setOfEdges the currently considered set of edges within the current SCS (set of knowledges)
  \param booleanClause a pointer to the Boolean clause of the composite marking
  \param emptyClause if true, the clause of this composite marking is empty
*/
void CompositeMarking::getMyFormula(const std::set<StoredKnowledge*> & knowledgeSet,
                                    SetOfEdges& setOfEdges,
                                    Clause* booleanClause,
                                    bool& emptyClause) {

    // edges that belong to the stored knowledge
    std::set<Label_ID> edges;

    // if the setOfEdges is empty, we ignore it ;-)
    bool useEdges = false;

    // otherwise, we store the set of edges belonging to the stored knowledge
    if (not setOfEdges.empty()) {
        edges = (*setOfEdges.find(storedKnowledge)).second;
        useEdges = true;
    }

    // receiving event
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        // receiving event resolves deadlock
        if (interface->marked(l) and
                (knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end() or
                 (useEdges and edges.find(l - 1) == edges.end())) and
                storedKnowledge->successors[l-1] != NULL and storedKnowledge->successors[l-1] != StoredKnowledge::empty and
                storedKnowledge->successors[l-1]->is_sane) {

            booleanClause->labelPossible(l);
            emptyClause = false;
        }
    }

    // synchronous communication
    for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
        // synchronous communication resolves deadlock
        if (InnerMarking::synchs[l].find(innerMarking_ID) != InnerMarking::synchs[l].end() and
                (knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end() or
                 (useEdges and edges.find(l - 1) == edges.end())) and
                storedKnowledge->successors[l-1] != NULL and storedKnowledge->successors[l-1] != StoredKnowledge::empty and
                storedKnowledge->successors[l-1]->is_sane) {

            booleanClause->labelPossible(l);
            emptyClause = false;
        }
    }

    // collect outgoing !-edges
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if ((knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end()  or
                (useEdges and edges.find(l - 1) == edges.end())) and
                storedKnowledge->successors[l-1] != NULL and
                storedKnowledge->successors[l-1]->is_sane) {

            booleanClause->labelPossible(l);
            emptyClause = false;
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
  checks if the given marking has been visited already, if so return a pointer to the marking otherwise return NULL
  Note: in case the marking has been found, you may need to delete the given marking afterwards
  \param marking composite marking to be looked for

  \todo Niels thinks the for loop can be simplified
*/
CompositeMarking* CompositeMarkingsHandler::getMarking(const CompositeMarking* marking) {
    if (numberElements == 0) {
        return NULL;
    }

    CompositeMarking* storedMarking;

    for (unsigned int i = 0; i < numberElements; ++i) {
        storedMarking = visitedCompositeMarkings[i];

        assert(storedMarking != NULL);

        if (*marking == *storedMarking) {
            return storedMarking;
        }
    }

    return NULL;
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

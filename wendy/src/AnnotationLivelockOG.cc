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


#include <algorithm>
#include <vector>
#include "AnnotationLivelockOG.h"
#include "util.h"


/******************************************
 * AnnotationElement                      *
 ******************************************/

/******************
 * STATIC MEMBERS *
 ******************/

AnnotationElement::_stats AnnotationElement::stats;

/******************
 * STATIC METHODS *
 ******************/

AnnotationElement::_stats::_stats()
    : cumulativeNumberOfClauses(0), maximalNumberOfClauses(0), numberOfTrueAnnotations(0) {}


/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
  \param _setOfKnowledges the set of knowledges this annotation belongs to
*/
AnnotationElement::AnnotationElement(const std::set<StoredKnowledge* > & _setOfKnowledges) :

    // create C-Array to store the set of knowledges
    setOfKnowledges(new StoredKnowledge*[_setOfKnowledges.size() + 1]),
    successor(NULL) {

    // copy values into the C arrays
    std::copy(_setOfKnowledges.begin(), _setOfKnowledges.end(), setOfKnowledges);

    // that way we do not need to remember the size of the array
    setOfKnowledges[_setOfKnowledges.size()] = NULL;
}

/*!
  destructor
*/
AnnotationElement::~AnnotationElement() {
    delete[] setOfKnowledges;
}


/**********************************************
 * AnnotationElementTrue                      *
 **********************************************/


/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
  \param _setOfKnowledges the set of knowledges this annotation belongs to
*/
AnnotationElementTrue::AnnotationElementTrue(const std::set<StoredKnowledge* > & _setOfKnowledges) :
               AnnotationElement(_setOfKnowledges)

{
    ++stats.cumulativeNumberOfClauses;
    ++stats.numberOfTrueAnnotations;
}


/*!
  destructor
*/
AnnotationElementTrue::~AnnotationElementTrue() {
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
  creates a string out of the set of strings representing the annotation of the set of knowledges
  \param dot the string shall be used in the dot output or not
  \param file a stream to which this annotation goes to
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead
*/
void AnnotationElementTrue::myAnnotationToStream(const bool& dot,
                                             std::ostream& file,
                                             std::map<const StoredKnowledge*, unsigned int>& nodeMapping) const {
    file << "true";
}

/*************************************
 * AnnotationElementTransitionLabel  *
 *************************************/

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
  \param _setOfKnowledges the set of knowledges this annotation belongs to
  \param _annotationBoolean disjunction of clauses representing the annotation
*/
AnnotationElementTransitionLabel::AnnotationElementTransitionLabel(const std::set<StoredKnowledge* > & _setOfKnowledges,
                                                                   const std::vector<Clause* > & _annotationBoolean) :

    AnnotationElement(_setOfKnowledges),
    // create C-Array to store the set of clauses
    annotationBool(new Clause*[_annotationBoolean.size() + 1]) {

    std::copy(_annotationBoolean.begin(), _annotationBoolean.end(), annotationBool);

    annotationBool[_annotationBoolean.size()] = NULL;

    // do some statistics
    if (stats.maximalNumberOfClauses < _annotationBoolean.size()) {
        stats.maximalNumberOfClauses = _annotationBoolean.size();
    }

    stats.cumulativeNumberOfClauses += _annotationBoolean.size();
}


/*!
  destructor
*/
AnnotationElementTransitionLabel::~AnnotationElementTransitionLabel() {

    // explicitly delete every (real) clause
    for (unsigned int i = 0; annotationBool[i] != NULL; ++i) {
        if (annotationBool[i] != Clause::falseClause && annotationBool[i] != Clause::trueClause) {
            delete annotationBool[i];
        }
    }

    delete[] annotationBool;
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
  creates a string out of the set of strings representing the annotation of the set of knowledges
  \param dot the string shall be used in the dot output or not
  \param file a stream to which this annotation goes to
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead
*/
void AnnotationElementTransitionLabel::myAnnotationToStream(const bool& dot,
                                             std::ostream& file,
                                             std::map<const StoredKnowledge*, unsigned int>& nodeMapping) const {

    // create the annotation of the current set of knowledges
    std::string stringAnd = (dot) ? " &and; " : " * ";

    for (unsigned int i = 0; annotationBool[i] != NULL; ++i) {

        if (i != 0) {
            file << stringAnd;
        }

        if (annotationBool[i] == Clause::falseClause) {
            file << "false";
        } else if (annotationBool[i] == Clause::trueClause) {
            file << "true";
        } else {
            // the clause itself takes care of "(" and ")"
            annotationBool[i]->printToStream(dot, file, nodeMapping);
        }
    }
}


/******************************************
 * AnnotationLivelockOG                   *
 ******************************************/

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
*/
AnnotationLivelockOG::AnnotationLivelockOG() : rootElement(NULL), lastElement(NULL), currentPointer(NULL) {
    currentPointer = rootElement;
}

/*!
  destructor
*/
AnnotationLivelockOG::~AnnotationLivelockOG() {

    // definitely start with the root element
    initIterator();

    // delete all elements one by one
    AnnotationElement* temp;

    while ((temp = pop()) != NULL) {
        delete temp;
    }
}


/********************
 * MEMBER FUNCTIONS *
 ********************/


/*!
  inserts an annotation to the queue
  \param setOfKnowledges set of knowledges the annotation belongs to
  \param annotationBoolean set of label ids representing an annotation

*/
void AnnotationLivelockOG::push(const std::set<StoredKnowledge* > & setOfKnowledges,
                                const std::vector<Clause* > & annotationBoolean) {

    if (rootElement == NULL) {
        Clause::initialize();
    }

    bool trueAnnotation = true;

    // does the current annotation consist of true clauses only
    FOREACH (literal, annotationBoolean) {
        if (*literal != Clause::trueClause) {
            trueAnnotation = false;
            break;
        }
    }

    AnnotationElement* annotationElement;

    // annotationBoolean is deleted in calling function
    if (trueAnnotation) {
        annotationElement = new AnnotationElementTrue(setOfKnowledges);
    } else {
        annotationElement = new AnnotationElementTransitionLabel(setOfKnowledges, annotationBoolean);
    }

    // the queue is empty
    if (rootElement == NULL) {
        rootElement = lastElement = currentPointer = annotationElement;
    } else {
        lastElement->successor = annotationElement;
        lastElement = annotationElement;
    }
}


/*!
 returns the first element of the queue
*/
AnnotationElement* AnnotationLivelockOG::pop() {
    if (currentPointer != NULL) {
        AnnotationElement* temp = currentPointer;
        currentPointer = temp->successor;

        return temp;
    }

    return NULL;
}


/*!
  resets the iterator currentPointer to point to the beginning of the queue again
*/
void AnnotationLivelockOG::initIterator() {
    currentPointer = rootElement;
}




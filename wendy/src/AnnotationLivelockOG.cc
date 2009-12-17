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


#include "AnnotationLivelockOG.h"


/******************************************
 * AnnotationElement                      *
 ******************************************/

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
*/
AnnotationElement::AnnotationElement(const std::set<StoredKnowledge* > & _setOfKnowledges, const std::vector<std::set<Label_ID> > & _annotation) :
                                        setOfKnowledges(_setOfKnowledges), annotation(_annotation), successor(NULL)
{

}


/*!
  destructor
*/
AnnotationElement::~AnnotationElement() {

}


/*!
  creates a string out of the set of strings representing the annotation of the set of knowledges
  \param dot the string shall be used in the dot output or not
*/
std::string AnnotationElement::myAnnotation(const bool & dot) {
    // create the annotation of the current set of knowledges
    std::string formula;

    if (not annotation.empty()) {
        // traverse the conjunctions to access the disjunctions
        for (std::vector<std::set<Label_ID> >::iterator it = annotation.begin();
                it != annotation.end(); ++it) {

            if (it != annotation.begin()) {
                formula += (dot) ? " &and; " : " * ";
            }
            if (it->size() > 1) {
                formula += "(";
            }
            // get clause which contains !, ? or # events
            for (std::set<Label_ID>::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
                if (it2 != it->begin()) {
                    formula += (dot) ? " &or; " : " + ";
                }
                if (*it2 == 0) {
                    formula += "final";
                } else {
                    formula += Label::id2name[*it2];
                }
            }
            if (it->size() > 1) {
                formula += ")";
            }
        }
    }

    // required for diagnose mode in which a non-final node might have no
    // successors
    if (formula.empty()) {
        formula = "false";
    }

    return formula;
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
    AnnotationElement * temp;

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
  \param annotation set of label ids representing an annotation
*/
void AnnotationLivelockOG::push(const std::set<StoredKnowledge* > & setOfKnowledges, const std::vector<std::set<Label_ID> > & annotation) {
    // create new annotation element
    AnnotationElement * annotationElement = new AnnotationElement(setOfKnowledges, annotation);

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
AnnotationElement * AnnotationLivelockOG::pop() {
    if (currentPointer != NULL) {
        AnnotationElement * temp = currentPointer;
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




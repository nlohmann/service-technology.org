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
#include "cmdline.h"


extern gengetopt_args_info args_info;

/************************
 * Class FinalKnowledge *
 ************************/

/*!
     constructor
     \param _knowledge final knowledge to be stored
*/
FinalKnowledge::FinalKnowledge(const StoredKnowledge* _knowledge) : knowledge(_knowledge) {

}


/*!
     destructor
*/
FinalKnowledge::~FinalKnowledge() {

}

/***************************
 * Class TransitionLiteral *
 ***************************/

/*!
     constructor
     \param _knowledge final knowledge to be stored
*/
TransitionLiteral::TransitionLiteral(const StoredKnowledge* _knowledge, const Label_ID& _label) : knowledge(_knowledge), label(_label) {

}


/*!
     destructor
*/
TransitionLiteral::~TransitionLiteral() {

}

/******************
 * Class Clause   *
 ******************/


/******************
 * STATIC MEMBERS *
 ******************/

Clause::_stats Clause::stats;

// a clause holding just literal false
Clause* Clause::falseClause = reinterpret_cast<Clause*>(1);

// a clause holding just literal true
Clause* Clause::trueClause = reinterpret_cast<Clause*>(2);

/******************
 * STATIC METHODS *
 ******************/

Clause::_stats::_stats()
    : cumulativeSizeAllClauses(0), maximalSizeOfClause(0) {}


void Clause::initialize() {
}


/***************************
 * CONSTRUCTOR, DESTRUCTOR *
 ***************************/

/*
  \brief all values are initialized with 0
*/
Clause::Clause()
    : finalKnowledges(NULL), transitionLiterals(NULL), numberOfFinalKnowledges(0), numberOfTransitionLiterals(0), more_than_one_literal(0) {

}


/*!
  destructor
*/
Clause::~Clause() {
    if (finalKnowledges != NULL) {

        for (unsigned int i = 0; i < numberOfFinalKnowledges; ++i) {
            delete finalKnowledges[i];
        }

        free(finalKnowledges);
    }

    if (transitionLiterals != NULL) {

        for (unsigned int i = 0; i < numberOfTransitionLiterals; ++i) {
            delete transitionLiterals[i];
        }

        free(transitionLiterals);
    }
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
    adds a knowledge as a final state literal to this clause
    \param _finalKnowledge the knowledge that is final and has to be remembered in this clause
*/
void Clause::addFinalKnowledge(const StoredKnowledge* _finalKnowledge) {

    if (finalKnowledges == NULL) {
        if (not(finalKnowledges = (FinalKnowledge**) calloc(1, sizeof(FinalKnowledge*)))) {
            return ;
        }
    } else {

        // make sure that the given knowledge has not been added to the clause so far
        for (unsigned int i = 0; i < numberOfFinalKnowledges; ++i) {
            if (finalKnowledges[i]->knowledge == _finalKnowledge) {
                return ;
            }
        }

        if (not(finalKnowledges = (FinalKnowledge**) realloc(finalKnowledges, (numberOfFinalKnowledges + 1) * sizeof(FinalKnowledge*)))) {
            return ;
        }
    }

    FinalKnowledge* finalKnowledge = new FinalKnowledge(_finalKnowledge);

    // store the given knowledge
    finalKnowledges[numberOfFinalKnowledges] = finalKnowledge;

    ++numberOfFinalKnowledges;

    // we just stored a literal ;-)
    more_than_one_literal = numberOfFinalKnowledges > 1;
}

/*!
    adds a transition literal (a knowledge and the label of the transition) to this clause
    \param _knowledge the knowledge belonging to the transition literal
    \param l the label of the transition literal
*/
void Clause::addTransitionLiteral(const StoredKnowledge* _knowledge, const Label_ID& l) {

    if (transitionLiterals == NULL) {
        if (not(transitionLiterals = (TransitionLiteral**) calloc(1, sizeof(TransitionLiteral*)))) {
            return ;
        }
    } else {

        // make sure that the given transition literal has not been added to the clause so far
        for (unsigned int i = 0; i < numberOfTransitionLiterals; ++i) {
            if (transitionLiterals[i]->knowledge == _knowledge && transitionLiterals[i]->label == l) {
                return ;
            }
        }

        if (not(transitionLiterals = (TransitionLiteral**) realloc(transitionLiterals, (numberOfTransitionLiterals + 1) * sizeof(TransitionLiteral*)))) {
            return ;
        }
    }

    TransitionLiteral* transitionLiteral = new TransitionLiteral(_knowledge, l);

    // store the given knowledge
    transitionLiterals[numberOfTransitionLiterals] = transitionLiteral;

    ++numberOfTransitionLiterals;

    // we just stored a literal ;-)
    more_than_one_literal = numberOfTransitionLiterals > 1;
}


/*!
  overloaded bitwise comparison and the attribute final is passed as well
*/
void Clause::operator|=(const Clause& other) {
    // add final knowledge of other if it is not part of the clause already
    for (unsigned int i = 0; i < other.numberOfFinalKnowledges; ++i) {
        // add final knowledge if it is not stored yet (done in that method)
        addFinalKnowledge(other.finalKnowledges[i]->knowledge);
    }

    // add transition literal of other if it is not part of the clause already
    for (unsigned int i = 0; i < other.numberOfTransitionLiterals; ++i) {
        // add transition literal if it is not stored yet (done in that method)
        addTransitionLiteral(other.transitionLiterals[i]->knowledge, other.transitionLiterals[i]->label);
    }
}


/*!
  creates a string out of the set of strings representing the annotation of the set of knowledges
  \param dot the string shall be used in the dot output or not
  \param file a stream to which this clause goes to
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead
*/
void Clause::printToStream(const bool& dot, std::ostream& file, std::map<const StoredKnowledge*, unsigned int>& nodeMapping) {
    // create the annotation of the current set of knowledges
    std::string stringOr = (dot) ? " &or; " : " + ";

    if (numberOfFinalKnowledges > 0 || numberOfTransitionLiterals > 0) {
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

                if (not args_info.showInternalNodeNames_flag) {
                    if (nodeMapping.find(finalKnowledges[i]->knowledge) == nodeMapping.end()) {
                        nodeMapping[finalKnowledges[i]->knowledge] = nodeMapping.size();
                    }
                    file << nodeMapping[finalKnowledges[i]->knowledge];
                } else {
                    file << reinterpret_cast<size_t>(finalKnowledges[i]->knowledge);
                }

                file << ".final";

                first = false;
            }
        }

        if (numberOfTransitionLiterals > 0) {
            for (unsigned int i = 0; i < numberOfTransitionLiterals; ++i) {
                if (not first) {
                    file << stringOr;
                }

                if (not args_info.showInternalNodeNames_flag) {
                    if (nodeMapping.find(transitionLiterals[i]->knowledge) == nodeMapping.end()) {
                        nodeMapping[transitionLiterals[i]->knowledge] = nodeMapping.size();
                    }
                    file << nodeMapping[transitionLiterals[i]->knowledge];
                } else {
                    file << reinterpret_cast<size_t>(transitionLiterals[i]->knowledge);
                }

                file << "." << Label::id2name[transitionLiterals[i]->label];
                first = false;
            }
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

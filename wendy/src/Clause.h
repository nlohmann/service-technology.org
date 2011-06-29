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

#pragma once

#include <set>
#include <string>
#include "Label.h"


class StoredKnowledge;

/*!
 sort of dummy class to hold a const pointer, which we need in Clause to
 store a pointer to a final knowledge
*/
class FinalKnowledge {
    public: /* member functios */

        /// constructor
        FinalKnowledge(const StoredKnowledge* _knowledge);

        /// destructor
        ~FinalKnowledge();

    public: /* member attributes */
        const StoredKnowledge* knowledge;
};

/*!
 class representing a transition literal
 it holds a const pointer and a label of the outgoing transition
*/
class TransitionLiteral {
    public: /* member functios */

        /// constructor
        TransitionLiteral(const StoredKnowledge* _knowledge, const Label_ID& label);

        /// destructor
        ~TransitionLiteral();

    public: /* member attributes */
        const StoredKnowledge* knowledge;

        /// label of the transition literal
        const Label_ID label;
};


/*!
 Livelock Operating Guideline only

 a clause of the annotation of the livelock operating guideline
 clauses (final), (true) and (false) are represented by a special pointer
*/
class Clause {

        friend class CompositeMarking;
        friend class LivelockOperatingGuideline;

    public: /* static functions */

        /// initializes the class Clause
        static void initialize();

    public: /* static attributes */

        /// struct combining the statistics on the class Clause
        static struct _stats {
            /// constructor
            _stats();

            ///
            unsigned int cumulativeSizeAllClauses;

            /// maximal size a clause has reached
            unsigned int maximalSizeOfClause;
        } stats;

        /// the clause holding literal false only (just a placeholder, no object!)
        static Clause* falseClause;

        /// the clause holding literal true only (just a placeholder, no object!)
        static Clause* trueClause;

    public: /* public attributes */

        /// how many final knowledges are part of the current clause
        unsigned int numberOfFinalKnowledges;

        /// how many transition literals are part of the current clause
        unsigned int numberOfTransitionLiterals;

    public: /* member functions */

        /// constructor
        Clause();

        /// destructor
        ~Clause();

        /// overloaded bit-wise OR operator
        void operator|=(const Clause&);

        /// returns a string representation of this clause
        void printToStream(const bool& dot, std::ostream& file, std::map<const StoredKnowledge*, unsigned int>& nodeMapping);

    private: /* member functions */

        /// add final knowledge to the clause
        void addFinalKnowledge(const StoredKnowledge* finalKnowledge);

        /// add a transition literal to the clause
        void addTransitionLiteral(const StoredKnowledge* finalKnowledge, const Label_ID& l);

    private: /* member attributes */

        /// store a pointer to each final knowledge belonging to the current clause
        FinalKnowledge** finalKnowledges;

        /// store a pointer to each transition literal belonging to the current clause
        TransitionLiteral** transitionLiterals;

        /// this clause stores more than one literal
        unsigned more_than_one_literal : 1;
};

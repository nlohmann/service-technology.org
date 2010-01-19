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


#ifndef CLAUSE_H_
#define CLAUSE_H_

#include <set>
#include <string>
#include "Label.h"


/*!
 Livelock Operating Guideline only

 a clause of the annotation of the livelock operating guideline
 clauses (final) and (false) are represented by a special pointer
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
            public:
                /// constructor
                _stats();

                ///
                unsigned int cumulativeSizeAllClauses;

                /// maximal size a clause has reached
                unsigned int maximalSizeOfClause;
        } stats;

        /// the clause holding literal final only (just a placeholder, no object!)
        static Clause* finalClause;

        /// the clause holding literal false only (just a placeholder, no object!)
        static Clause* falseClause;

    private: /* static attributes */
        /// the number of bytes needed
        static uint8_t bytes;

    public: /* member functions */

        /// constructor
        Clause();

        /// destructor
        ~Clause();

        /// overloaded bit-wise OR operator
        void operator|=(const Clause&);

        /// returns a string representation of this clause
        std::string getString(const bool & dot);

    private: /* member functions */

        /// set given label to 1 (is part of the clause)
        void labelPossible(const Label_ID& l);

        /// returns array of all events (possible or not)
        inline void decode();

    private: /* member attributes */

        /// store the bits representing each label of the interface
        uint8_t * storage;

        /// reserve one bit to represent whether this clause contains literal final
        unsigned contains_final : 1;

        /// this clause stores more than one literal
        unsigned more_than_one_literal : 1;

        /// array of all events (possible or not); built by decode()
        char* decodedLabels;

};


#endif /* CLAUSE_H_ */

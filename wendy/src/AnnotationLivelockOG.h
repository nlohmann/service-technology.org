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

#ifndef ANNOTATIONLIVELOCKOG_H_

#define ANNOTATIONLIVELOCKOG_H_

#include <set>
#include <string>
#include "StoredKnowledge.h"
#include "Clause.h"

/*!
  Livelock Operating Guideline only

  a set of knowledges is annotated with a set of label IDs which will be transformed into a real string later on
*/
class AnnotationElement {

    friend class AnnotationLivelockOG;
    friend class LivelockOperatingGuideline;

    public: /* static attributes */

        /// struct combining the statistics on the class AnnotationElement
        static struct _stats {
            public:
                /// constructor
                _stats();

                ///
                unsigned int cumulativeNumberOfClauses;

                /// maximal number of clauses per annotation element
                unsigned int maximalNumberOfClauses;
        } stats;

    public: /* member functions */

        /// constructor
        AnnotationElement(const std::set<StoredKnowledge* > & _setOfKnowledges,
                          const std::vector<Clause* > & _annotationBoolean);

        /// destructor
        ~AnnotationElement();

        /// returns a string containing the annotation of this element
        std::string myAnnotation(const bool & dot) const;

    private: /* member attributes */
        /// pointer to the set of knowledges
        StoredKnowledge** setOfKnowledges;

        /// vector of label ids representing the annotation
        Clause ** annotationBool;

        /// succeeding element
        AnnotationElement * successor;

};


/*!
  Livelock Operating Guideline only

  a queue containing the annotation of the livelock operating guideline,
  hence the annotation of each strongly connected set of knowledges of the LL-OG
*/
class AnnotationLivelockOG {

    public: /* member functions */

        /// constructor
        AnnotationLivelockOG();

        ///destructor
        ~AnnotationLivelockOG();

        /// inserts an annotation to the queue
        void push(const std::set<StoredKnowledge* > & setOfKnowledges,
                  const std::vector<Clause* > & annotationBoolean);

        /// returns the first element of the queue
        AnnotationElement * pop();

        /// sets current pointer to the root element again
        void initIterator();

    private: /* member attributes */

        /// pointer to the first element of the queue
        AnnotationElement * rootElement;

        /// pointer to the last element of the queue
        AnnotationElement * lastElement;

        /// current pointer
        AnnotationElement * currentPointer;
};


#endif /* ANNOTATIONLIVELOCKOG_H_ */

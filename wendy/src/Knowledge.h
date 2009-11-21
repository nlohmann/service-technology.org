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


#ifndef _KNOWLEDGE_H
#define _KNOWLEDGE_H

#include <vector>
#include <map>
#include "Label.h"
#include "InnerMarking.h"
#include "FullMarkingQueue.h"

typedef std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > Bubble;


/*!
 \brief knowledge (data structure for building knowledges)

 This class is used to build knowledges. Its main focus is runtime
 optimization. A later translation into StoredKnowledge objects removes any
 unneccessary information, yielding a compact representation.
*/
class Knowledge {
    friend class StoredKnowledge;

    public: /* member functions */
        /// construct knowledge from (initial) inner marking
        explicit Knowledge(InnerMarking_ID);

        /// destructor
        ~Knowledge();

    private: /* member functions */
        /// construct knowledge from a given knowledge and a label
        Knowledge(const Knowledge*, const Label_ID&);

        /// whether the knowledge contains a waitstate resolvable by l
        bool resolvableWaitstate(const Label_ID&) const;

        /// whether the knowledge's waitstates can be resolved by receiving
        bool receivingHelps() const;

        /// calculate those receiving events that are essential to resolve each and every waitstate
        void sequentializeReceivingEvents();

        /// reduction rule: seguentialize receiving
        bool considerReceivingEvent(const Label_ID&) const;

        /// reduction rule: smart sending event
        bool considerSendingEvent(const Label_ID&) const;

        /// initialze member attributes
        inline void initialize();

        /// calculate the closure of this knowledge
        inline void closure();

        /// inner marking is really waitstate in the context of the current knowledge
        inline bool isWaitstateInCurrentKnowledge(const InnerMarking_ID& inner, const InterfaceMarking* interface) const;

    public: /* attributes */
        /// whether this knowledge is sane
        unsigned is_sane : 1;

    private: /* attributes */
        /// reduction rule: smart send events; array of sending events that are either possible or not
        char* posSendEventsDecoded;

        /// \brief the number of markings stored in the bubble
        innermarkingcount_t size;

        /// primary data structure
        Bubble bubble;

        /// a queue of markings to be processed by closuse()
        FullMarkingQueue todo;

        /// reduction rule: smart send events; pointer to possible sending events data structure
        PossibleSendEvents* posSendEvents;

        /// reduction rule: sequentialize receiving events; remember only those receiving events
        ///                 which are essential to resolve each and every waitstate
        std::vector<bool> consideredReceivingEvents;
};

#endif

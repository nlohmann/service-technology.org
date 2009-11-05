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

#include <queue>
#include <vector>
#include <map>
#include "Label.h"
#include "InnerMarking.h"
#include "FullMarking.h"


/*!
 \brief knowledge (data structure for building knowledges)

 This class is used to build knowledges. Its main focus is runtime
 optimization. A later translation into StoredKnowledge objects removes any
 unneccessary information, yielding a compact representation.

 \todo It seems that many of the functions that organized reduction
       techniques traverse all markings the knowledge bubble. Hence,
       efficiency could be imporoved by making the respective check while
       building the bubble to avoid repeated traversion. Then, it might be
       cheaper to precompute the results and store them as members than
       actually making a function call.

 \todo In general, std::map<Label_ID, bool> could be replaced by
       std::vector<Label_ID>. No overhead and constant access!
 */
class Knowledge {
    public: /* member functions */
        /// construct knowledge from (initial) inner marking
        explicit Knowledge(InnerMarking_ID);

        /// construct knowledge from a given knowledge and a label
        Knowledge(const Knowledge&, const Label_ID&);

        /// destructor
        ~Knowledge();

        /// whether the knowledge contains a waitstate resolvable by l
        bool resolvableWaitstate(const Label_ID&) const;

        /// whether the knowledge's waitstates can be resolved by receiving
        bool receivingHelps() const;

        /// calculate those receiving events that are essential to resolve each and every waitstate
        void sequentializeReceivingEvents();

        /// \todo Daniela, please comment me
        bool considerReceivingEvent(const Label_ID&) const;

        /// \todo Daniela, please comment me
        bool considerSendingEvent(const Label_ID&) const;

    public: /* attributes */
        /// whether this knowledge is sane
        unsigned is_sane : 1;

        /// \brief the number of markings stored in the bubble
        /// \todo Do we need to have unsigned ints here? Maybe short is enough.
        unsigned int size;

        /// primary data structure
        std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > bubble;

    private: /* member functions */
        /// initialze member attributes
        inline void initialize();

        /// calculate the closure of this knowledge
        inline void closure(std::queue<FullMarking>&);

        /// inner marking is really waitstate in the context of the current knowledge
        inline bool isWaitstateInCurrentKnowledge(const InnerMarking_ID& inner, const InterfaceMarking* interface);

    private: /* attributes */
        /// reduction rule: smart send events; pointer to possible sending events data structure
        PossibleSendEvents* posSendEvents;

        /// reduction rule: smart send events; array of sending events that are either possible or not
        char* posSendEventsDecoded;

        /// reduction rule: sequentialize receiving events; remember only those receiving events
        ///                 which are essential to resolve each and every waitstate
        std::map<Label_ID, bool> consideredReceivingEvents;
};

#endif

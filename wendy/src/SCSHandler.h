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


#ifndef _SCSHANDLER_H
#define _SCSHANDLER_H

#include <map>
#include <set>
#include <list>
#include <vector>

#include "Label.h"

class StoredKnowledge;

/// \todo make me a member of the new LL-StoredKnowledge ???
typedef std::map<const StoredKnowledge*, std::set<Label_ID> > SetOfEdges;


/*!
 class storing an outgoing edge
 */
class Edge {
    public:

        /// constructor
        Edge(const StoredKnowledge* _knowledge, const Label_ID& _edge);

        /// knowledge which has the outgoing edge stored here
        const StoredKnowledge* knowledge;

        /// label of the outgoing edge
        const Label_ID edge;
};


/*!
 given an SCS, this class handles the generation of subsystems of the given SCS
 */
class SCSHandler {
    private:  /* static functions */

        /// count the number of bits that are set
        static unsigned int bitCount(unsigned int n);

    public: /* static attributes */

        /// number of bad subsystems detected with mandatory edges
        static unsigned int countBadSubsystems;

    private:  /* member attributes */
        /// flattened list of edges
        Edge** edges;

        /// (known) mandatory edges for keeping the SCS connected
        unsigned int* mandatoryEdges;

        /// number of all edges
        unsigned int numberOfAllEdges;

        /// minimum number of edges of a subsystem
        unsigned int minNumberOfEdges;

        /// current sub set
        unsigned int currentBitMask;

        /// number of mandatory edges
        unsigned int numberOfMandatoryEdges;

    private:  /* member functions */

        /// does current subsystem contain all mandatory edges
        inline bool containsMandatoryEdges() const;

    public:  /* member functions */

        /// constructor
        SCSHandler();

        /// destructor
        ~SCSHandler();

        /// always call before getting the subsystems
        void initializeSubsystemCalculation();

        /// does there exist a next subsystem
        bool nextSubsystem() const;

        /// returns the next subsystem
        SetOfEdges getNextSubsystem();

        /// initializes the handler
        void initialize(SetOfEdges& SCS, unsigned int& _numberOfAllEdges);

};


#endif

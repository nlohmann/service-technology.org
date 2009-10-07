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


#ifndef _STOREDKNOWLEDGE_H
#define _STOREDKNOWLEDGE_H

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "Knowledge.h"


/*!
 \brief knowledge (data structure for storing knowledges)
 */
class StoredKnowledge {
	/// the Diagnosis needs to traverse the hash tree
	friend class Diagnosis;

    public: /* static functions */
        /// destroy all objects of this class
        static void finalize();

        /// generate the successor of a knowledge bubble given a label
        static void process(const Knowledge&, StoredKnowledge* const, const Label_ID&);

        /// recursively calculate knowledge bubbles
        static void processRecursively(const Knowledge&, StoredKnowledge* const);

        /// print a dot representation
        static void output_dot(std::ostream&);

        /// print the knowledges as OG
        static void output_og(std::ostream&);

        /// print the knowledges as Fiona OG
        static void output_ogold(std::ostream&);

        /// print information for instance migration
        static void output_migration(std::ostream&);

    public: /* static attributes */
        /// struct combining the statistics on the class StoredKnowledge
        static struct _stats {
            public:
                /// constructor
                _stats();

                /// the number of hash collisions (distinct knowledges with the same hash value)
                unsigned int hashCollisions;

                /// the number of edges stored overall
                unsigned int storedEdges;

                /// the maximal number of interface markings per inner marking
                unsigned int maxInterfaceMarkings;

                /// the number of nodes that were built, but immediately detected insane
                unsigned int builtInsaneNodes;

                /// maximal size of a hash bucket (1 means no collisions)
                size_t maxBucketSize;

                /// the number of knowledges stored in the hash tree
                unsigned int storedKnowledges;

                /// maximum number of scc components
                unsigned int maxSCCSize;

                /// total number of SCCs
                unsigned int numberOfNonTrivialSCCs;

                /// number of trivial SCCs
                unsigned int numberOfTrivialSCCs;
        } stats;

        /// buckets of knowledges, indexed by hash values
        static std::map<hash_t, std::vector<StoredKnowledge*> > hashTree;

        /// the root knowledge
        static StoredKnowledge* root;

        /// nodes that are reachable from the initial node
        static std::set<StoredKnowledge*> seen;

    private: /* static attributes */
        /// maps dfs (first) and lowlink (second) number to a stored knowledge which is still on the Tarjan stack
        static std::map<const StoredKnowledge*, std::pair<unsigned int, unsigned int> > tarjanMapping;

        /// the empty knowledge (just a placeholder, no object!)
        static StoredKnowledge* empty;

        /// whether the empty node is reachable from the initial node
        static bool emptyNodeReachable;

        /// nodes that should be deleted
        static std::set<StoredKnowledge*> deletedNodes;

        /// stack of StoredKnowledge for Tarjan's algorithm (LIVELOCK FREEDOM)
        static std::vector<StoredKnowledge*> tarjanStack;

        /// needed for TSCC detection within Tarjan's algorithm
        static unsigned int bookmarkTSCC;

    private: /* static functions */
        /// returns the header for output files
        static void fileHeader(std::ostream&);

        /// evaluate each member of the given (T)SCC and propagate the property of being insane accordingly
        static void analyzeSCCOfKnowledges(std::set<StoredKnowledge*>&);

    public: /* member functions */
        /// constructs an object from a Knowledge object
        explicit StoredKnowledge(Knowledge const&);

        /// destructor
        ~StoredKnowledge();

        /// stores this object in the hash tree and returns a pointer to the result
        StoredKnowledge* store();

        /// traverse knowledges
        void traverse();

    private: /* member functions */
        /// adds a successor knowledge
        inline void addSuccessor(const Label_ID&, StoredKnowledge* const);

        /// return whether this node fulfills its annotation
        bool sat(const bool = false) const;

        /// return the hash value of this object
        inline hash_t hash() const;

        /// move all transient markings to the end of the array and adjust size of the markings array
        void rearrangeKnowledgeBubble();

        /// adjust lowlink values of the stored knowledge
        inline void adjustLowlinkValue(const StoredKnowledge* const, const bool) const;

        /// evaluates the current knowledge bubble with respect to (T)SCCs
        inline void evaluateKnowledge();

        /// print knowledge
        void print(std::ostream&) const;

        /// return a string representation of the knowledge's formula
        std::string formula() const;

        /// return a two-bit representation of the knowledge's formula
        std::string bits() const;

    public: /* member attributes */
        /// whether this bubble contains a final marking
        unsigned is_final : 1;

        /// whether from this bubble a final bubble is reachable (LIVELOCK FREEDOM)
        unsigned is_final_reachable : 1;

        /// whether this bubble is sane
        unsigned is_sane : 1;

        /// whether this bubble is still on the Tarjan stack
        unsigned is_on_tarjan_stack : 1;

    private: /* member attributes */
        /// \brief the number of markings stored in this knowledge
        /// \todo Do we need to have unsigned ints here? Maybe short is enough.
        unsigned int sizeDeadlockMarkings;

        /// \brief the number of markings stored in this knowledge
        /// \todo Do we need to have unsigned ints here? Maybe short is enough.
        unsigned int sizeAllMarkings;

        /// an array of inner markings (length is sizeAllMarkings)
        InnerMarking_ID* inner;

        /// \brief an array of interface markings (length is sizeAllMarkings)
        /// \todo Do we need an array to pointers here? Maybe explicit objects are enough.
        InterfaceMarking** interface;

        /// the successors of this knowledge (length is fixed by the labels)
        StoredKnowledge** successors;
};

#endif

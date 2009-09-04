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
#include "Knowledge.h"
#include "cmdline.h"


/*!
 \brief knowledge (data structure for storing knowledges)
 */
class StoredKnowledge {

    public: /* static functions */

        /// generate the successor of a knowledge bubble given a label
        static void process(Knowledge*, StoredKnowledge*, const Label_ID&);

        /// recursively calculate knowledge bubbles
        static void processRecursively(Knowledge*, StoredKnowledge*);

        /// print a dot representation
        static void dot(std::ostream&);

        /// print the knowledges as OG
        static void output(std::ostream&);

        /// print the knowledges as Fiona OG
        static void output_old(std::ostream&);

        /// print information for instance migration
        static void migration(std::ostream&);

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

        /// report every given knowledges
        static unsigned int reportFrequency;

        /// the root knowledge
        static StoredKnowledge *root;

        /// nodes that are reachable from the initial node
        static std::set<StoredKnowledge*> seen;

        /// maps dfs (first) and lowlink (second) number to a stored knowledge which is still on the Tarjan stack
        static std::map<StoredKnowledge*, std::pair<unsigned int, unsigned int> > tarjanMapping;

    private: /* static attributes */

        /// the empty knowledge
        static StoredKnowledge *empty;

        /// whether the empty node is reachable from the initial node
        static bool emptyNodeReachable;

        /// nodes that should be deleted
        static std::set<StoredKnowledge*> deletedNodes;

        /// LIVELOCK FREEDOM
        /// stack of StoredKnowledge for Tarjan's algorithm
        static std::vector<StoredKnowledge *> tarjanStack;

        /// needed for TSCC detection within Tarjan's algorithm
        static unsigned int bookmarkTSCC;

        /// remember predecessor of a stored knowledge in case we evaluate a (T)SCC
        static std::map<StoredKnowledge* , std::set<StoredKnowledge*> > tempPredecessors;

    private: /* static functions */

        /// adjust lowlink values of the stored knowledge
        static void adjustLowlinkValue(StoredKnowledge*, StoredKnowledge*, bool);

        /// create the predecessor relation of all knowledges contained in the given set
        static void createPredecessorRelation(std::set<StoredKnowledge *> &);

        /// evaluate each member of the given set of knowledges and propagate the property of being insane accordingly
        static void evaluateKnowledgeSet(std::set<StoredKnowledge *> &);

        /// evaluates the current strongly connected components and adjusts the is_final_reachable value
        static void evaluateCurrentSCC(StoredKnowledge*);

    public: /* member functions */

        /// constructs an object from a Knowledge object
        StoredKnowledge(const Knowledge* const);

        /// destructor
        ~StoredKnowledge();

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const StoredKnowledge&);

        /// stores this object in the hash tree and returns a pointer to the result
        StoredKnowledge *store();

        /// traverse knowledges
        void traverse();

    private: /* member functions */

        /// adds a successor knowledge
        void addSuccessor(const Label_ID&, StoredKnowledge* const);

        /// return whether this node fulfills its annotation
        bool sat(const bool = false) const;

        /// return the hash value of this object
        hash_t hash() const;

        /// move all transient markings to the end of the array and adjust size of the markings array
        void rearrangeKnowledgeBubble();

        /// print knowledge
        void print(std::ostream&) const;

        /// return a string representation of the knowledge's formula
        std::string formula() const;

        /// return a two-bit representation of the knowledge's formula
        std::string bits() const;

    public: /* member attributes */

        /// whether this bubble contains a final marking
        unsigned is_final : 1;

        /// LIVELOCK FREEDOM
        /// whether from this bubble a final bubble is reachable
        unsigned is_final_reachable : 1;

        /// whether this bubble is sane
        unsigned is_sane : 1;

        /// whether this bubble is still on the Tarjan stack
        unsigned is_on_tarjan_stack : 1;

    private: /* member attributes */

        /// the number of markings stored in this knowledge
        unsigned int sizeDeadlockMarkings;

        /// the number of markings stored in this knowledge
        unsigned int sizeAllMarkings;

        /// an array of inner markings (length is size)
        InnerMarking_ID *inner;

        /// an array of interface markings (length is size)
        InterfaceMarking **interface;

        /// the successors of this knowledge (length is fixed by the labels)
        StoredKnowledge **successors;
};

#endif

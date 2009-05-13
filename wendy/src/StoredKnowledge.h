/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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

#include <fstream>
#include "Knowledge.h"
#include "cmdline.h"


/*!
 \brief knowledge (data structure for storing knowledges)
 */
class StoredKnowledge {
    public: /* static functions */

        /// recursively calculate knowledge bubbles
        static void calcRecursive(const Knowledge* const, StoredKnowledge*);

        /// traverse the graph and add predecessors
        static unsigned int addPredecessors();

        /// detect red nodes
        static unsigned int removeInsaneNodes();

        /// print a dot representation
        static void dot(std::ofstream&);

        /// print the knowledges as Fiona OG
        static void output(std::ofstream&);

    public: /* static attributes */

        /// buckets of knowledges, indexed by hash values
        static std::map<hash_t, std::vector<StoredKnowledge*> > hashTree;

        /// the number of hash collisions (distinct knowledges with the same hash value)
        static unsigned int hashCollisions;

        /// the number of knowledges stored in the hash tree
        static unsigned int storedKnowledges;

        /// report every given knowledges
        static unsigned int reportFrequency;

        /// maximal size of a hash bucket (1 means no collisions)
        static size_t maxBucketSize;

        /// number of iterations needed to removed insane nodes
        static unsigned int iterations;

        /// the root knowledge
        static StoredKnowledge *root;

        /// the empty knowledge
        static StoredKnowledge *empty;

    private: /* static attributes */

        /// number of markings stored
        static int entries_count;

        /// nodes that should be deleted
        static std::set<StoredKnowledge*> deletedNodes;

        /// nodes that are reachable from the initial node
        static std::set<StoredKnowledge*> seen;

        /// the number of all markings (deadlocks and transient markings)
        static std::map<StoredKnowledge*, unsigned int> allMarkings;

    public: /* member functions */

        /// constructs an object from a Knowledge object
        StoredKnowledge(const Knowledge* const);

        /// destructor
        ~StoredKnowledge();

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const StoredKnowledge&);

        /// stores this object in the hash tree and returns a pointer to the result
        StoredKnowledge *store();

    private: /* member functions */

        /// adds a successor knowledge
        void addSuccessor(Label_ID, StoredKnowledge* const);

        /// return whether this node fulfills its annotation
        bool sat() const;

        /// return the hash value of this object
        hash_t hash() const;

        /// adds a predecessor knowledge
        void addPredecessor(StoredKnowledge* const);

        /// return a string representation of the knowledge's formula
        std::string formula() const;

        /// return a two-bit representation of the knowledge's formula
        std::string bits() const;

        /// traverse knowledges
        void traverse();

    public: /* member attributes */

        /// whether this bubble contains a final marking
        unsigned is_final : 1;

        /// whether this bubble is sane
        unsigned is_sane : 1;

    private: /* member attributes */

        /// the number of markings stored in this knowledge
        unsigned int size;

        /// an array of inner markings (length is size)
        InnerMarking_ID *inner;

        /// an array of interface markings (length is size)
        InterfaceMarking **interface;


        /// the successors of this knowledge (length is fixed by the labels)
        StoredKnowledge **successors;


        /// the number of predecessors
        unsigned int inDegree;

        /// a counter to stored the next position of a predecessor
        unsigned int predecessorCounter;

        /// an array of predecessors (length is "inDegree")
        StoredKnowledge** predecessors;
};

#endif

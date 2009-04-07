#ifndef _STOREDKNOWLEDGE_H
#define _STOREDKNOWLEDGE_H

#include "Knowledge.h"
#include "InnerMarking.h"
#include "Label.h"

typedef uint16_t hash_t;

/*!
 \brief knowledge (data structure for storing knowledges)
 */
class StoredKnowledge {
    public: /* static functions */
    
        /// traverse the graph and add predecessors
        static unsigned int addPredecessors();
    
    public: /* static attributes */

        /// buckets of knowledges, indexed by hash values
        static std::map<hash_t, std::vector<StoredKnowledge*> > hashTree;
        
        /// the number of hash collisions (distinct knowledges with the same hash value)
        static unsigned int hashCollisions;
        
        /// the number of knowledges stored in the hash tree
        static unsigned int storedKnowledges;
        
        /// maximal size of a hash bucket (1 means no collisions)
        static size_t maxBucketSize;

        /// number of currently stored objects
        static int memory_count;

        /// maximal number of concurrently stored objects
        static int memory_max;

        /// number of markings stored
        static int entries_count;

    public: /* member functions */

        /// constructs an object from a Knowledge object
        StoredKnowledge(Knowledge*);
        
        /// destructor
        ~StoredKnowledge();

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const StoredKnowledge&);

        /// stores this object in the hash tree and returns a pointer to the result
        StoredKnowledge *store();

        /// adds a successor knowledge
        void addSuccessor(Label_ID, StoredKnowledge*);
        
    private: /* member functions */

        /// return the hash value of this object
        hash_t hash() const;
        
        /// return whether this node fulfills its annotation
        bool sat() const;

        /// adds a predecessor knowledge
        void addPredecessor(StoredKnowledge* k);

    private: /* member attributes */

        /// whether this bubble contains a final marking
        unsigned is_final : 1;

        /// an array of inner markings
        InnerMarking_ID *inner;

        /// an array of interface markings
        InterfaceMarking **interface;

        /// the number of markings stored in this knowledge
        unsigned int size;

        /// the successors of this knowledge
        StoredKnowledge **successors;

        /// the number of predecessors
        unsigned int inDegree;

        /// a counter to stored the next position of a predecessor
        unsigned int predecessorCounter;
        
        /// an array of predecessors (length is "inDegree")
        StoredKnowledge** predecessors;        
};

#endif

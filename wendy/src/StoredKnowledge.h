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
        
        /// detect red nodes
        static unsigned int removeInsaneNodes();
    
        /// print a dot representation
        static void dot();
    
    public: /* static attributes */

        /// buckets of knowledges, indexed by hash values
        static std::map<hash_t, std::vector<StoredKnowledge*> > hashTree;
        
        /// the number of hash collisions (distinct knowledges with the same hash value)
        static unsigned int hashCollisions;
        
        /// the number of knowledges stored in the hash tree
        static unsigned int storedKnowledges;
        
        /// maximal size of a hash bucket (1 means no collisions)
        static size_t maxBucketSize;

        /// number of markings stored
        static int entries_count;
        
        /// number of iterations needed to removed insane nodes
        static unsigned int iterations;

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
        
        /// return whether this node fulfills its annotation
        bool sat();

    private: /* member functions */

        /// return the hash value of this object
        hash_t hash() const;
        
        /// adds a predecessor knowledge
        void addPredecessor(StoredKnowledge* k);

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

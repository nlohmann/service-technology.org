#include "StoredKnowledge.h"
#include "config.h"

using std::cerr;
using std::endl;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<hash_t, std::vector<StoredKnowledge*> > StoredKnowledge::hashTree;
unsigned int StoredKnowledge::hashCollisions = 0;
unsigned int StoredKnowledge::storedKnowledges = 0;
size_t StoredKnowledge::maxBucketSize = 1; // sic!

int StoredKnowledge::memory_count = 0;
int StoredKnowledge::memory_max = 0;
int StoredKnowledge::entries_count = 0;


/***************
 * CONSTRUCTOR *
 ***************/

StoredKnowledge::StoredKnowledge(Knowledge *K) : inner(NULL), interface(NULL), successors(NULL), predecessors(NULL), inDegree(0), predecessorCounter(0), is_final(0) {
    ++memory_count;
    memory_max = std::max(memory_count, memory_max);
    
    assert(K);
    
    // reserve the necessary memory for the successors
    successors = new StoredKnowledge*[Label::events];
    assert(successors);
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        successors[l-1] = NULL;
    }

    // get the number of markings to store
    size = K->size;
    
    // if this is the empty node, we are done    
    if (size == 0) {
        return;
    }
    
    // reserve the necessary memory for the internal markings
    inner = new InnerMarking_ID[size];
    assert(inner);

    // reserve the necessary memory for the interface markings
    interface = new InterfaceMarking*[size];
    assert(interface);


    // copy data structure to C-style arrays
    unsigned int count = 0;

    for (std::map<InnerMarking_ID, std::vector<InterfaceMarking*> >::const_iterator pos = K->bubble.begin(); pos != K->bubble.end(); ++pos) {
        for (unsigned int i = 0; i < pos->second.size(); ++i, ++count) {
            // copy the inner marking
            inner[count] = pos->first;
            // copy the interface marking
            interface[count] = new InterfaceMarking(*(pos->second[i]));
            assert(interface[count]);
        }
    }
    
    entries_count += count;
    
    // we must not forget a marking
    assert(size == count);
}

StoredKnowledge::~StoredKnowledge() {
    // delete the interface markings
    for (unsigned int i = 0; i < size; ++i) {
        delete interface[i];
    }
    delete[] interface;
    
    delete[] inner;

    --memory_count;
    entries_count -= this->size;
}

/*************
 * OPERATORS *
 *************/


std::ostream& operator<< (std::ostream &o, const StoredKnowledge &m) {
    o << m.hash() << ":\t";
    
    if (m.size == 0) {
        return o << "[]";
    }

    // traverse the bubble
    for (unsigned int i = 0; i < m.size; ++i) {
        o << "[m" << (unsigned int)m.inner[i] << ", " << *(m.interface[i]) << "] ";
    }

    return o;
}



/********************
 * MEMBER FUNCTIONS *
 ********************/

StoredKnowledge *StoredKnowledge::store() {
    hash_t myHash = hash();
    
    std::map<hash_t, std::vector<StoredKnowledge*> >::iterator el = hashTree.find(myHash);
    if (el != hashTree.end()) {
        for (unsigned int i = 0; i < el->second.size(); ++i) {
            assert(el->second[i]);
            
            // compare the sizes
            if (size == el->second[i]->size) {
                if (size == 0) {
                    // only the empty node has the size 0 -- no collision here
                    return el->second[i];
                }
                
                // if still true after the loop, we found previously stored knowledge
                bool found = true;
                
                // compare the inner and interface markings
                for (unsigned int j = 0; (j < size && found); ++j) {
                    if (inner[j] != el->second[i]->inner[j] || *interface[j] != *el->second[i]->interface[j]) {
                        found = false;
                    }
                }

                if (found) {
                    return el->second[i];
                }
            }
        }
        
        // this object was not found in the bucket -- this is a collision
        ++hashCollisions;

        // update maximal bucket size (we add 1 as we will store this object later)
        maxBucketSize = std::max(maxBucketSize, hashTree[myHash].size()+1);        
    }

    // we need store this object
    hashTree[myHash].push_back(this);
    ++storedKnowledges;

    return this;
}

void StoredKnowledge::addSuccessor(Label_ID label, StoredKnowledge *knowledge) {
    assert(knowledge);

    // tau does not make sense here
    assert(!SILENT(label));

    // never add a successor twice
    assert(successors[label-1] == NULL);

    // we will never store label 0 (tau) -- hence decrease the label
    successors[label-1] = knowledge;
    
    // increase the successor's indegree
    ++knowledge->inDegree;
}

hash_t StoredKnowledge::hash() const {
    // the empty knowledge has the hash value 0
    if (size == 0) {
        return 0;
    }
    
    hash_t result = 1;

    for (unsigned int i = 0; i < size; ++i) {
        result += ((1 << i) * (inner[i]) + interface[i]->hash());
    }
    
    // make sure that we don't accientially use 0 for a nonempty knowledge
    if (result == 0) {
        ++result;
    }
    
    return result;
}


void StoredKnowledge::addPredecessor(StoredKnowledge* k) {
    assert(k);
    assert(inDegree > 0);
    
    // when called for the first time, get some memory
    if (predecessors == NULL) {
        predecessors = new StoredKnowledge*[inDegree];
        assert(predecessors);
        // set pointers to NULL to have a defined state
        for (unsigned int i = 0; i < inDegree; ++i) {
            predecessors[i] = NULL;
        }
    }
    
    // use the first free position and store this knowledge
    assert(predecessorCounter < inDegree);
    assert(predecessors[predecessorCounter] == NULL);
    predecessors[predecessorCounter++] = k;
}


/*!
 \todo do not process the empty node as it has an ridiculously many
       predecessors
 */
unsigned int StoredKnowledge::addPredecessors() {
    unsigned int result = 0;
    
    // traverse the hash buckets
    for (std::map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        // traverse the entries
        for (size_t i = 0; i < it->second.size(); ++i) {
            assert(it->second[i]);

//            cerr << *it->second[i] << endl;

            // check the stored markings and remove all transient states
            for (unsigned j = 0; j < it->second[i]->size; ++j) {
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_final && it->second[i]->interface[j]->empty()) {
                    // this knowledge contains a final marking
                    it->second[i]->is_final = 1;
                    it->second[i]->interface[j] = NULL;
                }
                
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_waitstate) {
                    // check if DL is resolved by interface marking
                    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                        if (it->second[i]->interface[j] != NULL &&
                            it->second[i]->interface[j]->get(l) > 0 &&
                            InnerMarking::receivers[it->second[i]->inner[j]].find(l) != InnerMarking::receivers[it->second[i]->inner[j]].end()) {
                            // this is a transient marking -- set interface marking to NULL
                            it->second[i]->interface[j] = NULL;
                        }
                    }
                }
            }
            
            /* now we know wheter this knowledge contains a final marking and
               that every marking with a non-NULL interface marking is a
               deadlock */

//            cerr << it->second[i]->sat() << endl;

            // for each successor, register the predecessor
            for (Label_ID l = Label::first_receive; l <= Label::last_send; ++l) {
                if (it->second[i]->successors[l-1] != NULL) {
                    it->second[i]->successors[l-1]->addPredecessor(it->second[i]);
                    ++result;
                }
            }
        }
    }

    // from now one, we don't need the hash tree any more
    StoredKnowledge::hashTree.clear();
    
    return result;
}


bool StoredKnowledge::sat() const {
    // if we find a sending successor, this node is OK
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL) {
//            cerr << "found a send successor: " << Label::id2name[l] << endl;
            return true;
        }
    }
    
    // now each deadlock must have ?-sucessors
    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            // we found a deadlock -- check whether for each marked output
            // place exists a respective receiving edge
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                if (interface[i]->get(l) > 0 && successors[l-1] == NULL) {
                    // found a marked place without successor
                    return false;
                }
            }
        }
    }
    
    return true;
}

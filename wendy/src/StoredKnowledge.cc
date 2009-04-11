#include <map>
#include <set>
#include <iostream>
#include "StoredKnowledge.h"
#include "config.h"

using std::map;
using std::cerr;
using std::cout;
using std::endl;
using std::set;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<hash_t, std::vector<StoredKnowledge*> > StoredKnowledge::hashTree;
unsigned int StoredKnowledge::hashCollisions = 0;
unsigned int StoredKnowledge::storedKnowledges = 0;
size_t StoredKnowledge::maxBucketSize = 1; // sic!

int StoredKnowledge::entries_count = 0;
unsigned int StoredKnowledge::iterations = 0;



/***************
 * CONSTRUCTOR *
 ***************/

StoredKnowledge::StoredKnowledge(Knowledge *K) :
    inner(NULL), interface(NULL), successors(NULL), predecessors(NULL),
    inDegree(0), predecessorCounter(0), is_final(0), is_sane(1)
{

    assert(K);
    
    // reserve the necessary memory for the successors
    successors = new StoredKnowledge*[Label::events];
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

    // reserve the necessary memory for the interface markings
    interface = new InterfaceMarking*[size];


    // copy data structure to C-style arrays
    unsigned int count = 0;

    for (std::map<InnerMarking_ID, std::vector<InterfaceMarking*> >::const_iterator pos = K->bubble.begin(); pos != K->bubble.end(); ++pos) {
        for (unsigned int i = 0; i < pos->second.size(); ++i, ++count) {
            // copy the inner marking
            inner[count] = pos->first;
            // copy the interface marking
            interface[count] = new InterfaceMarking(*(pos->second[i]));
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
        result = 1;
    }
    
    return result;
}


void StoredKnowledge::addPredecessor(StoredKnowledge* k) {
    assert(k);
    assert(inDegree > 0);
    
    // when called for the first time, get some memory
    if (predecessors == NULL) {
        predecessors = new StoredKnowledge*[inDegree];
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
 
 \todo must delete hash tree (needed by removeInsaneNodes())
 
 \todo delete interface markings before setting them to NULL?
 */
unsigned int StoredKnowledge::addPredecessors() {
    unsigned int result = 0;
    
    // traverse the hash buckets
    for (std::map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        // traverse the entries
        for (size_t i = 0; i < it->second.size(); ++i) {
            assert(it->second[i]);

            // check the stored markings and remove all transient states
            for (unsigned j = 0; j < it->second[i]->size; ++j) {
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_final &&
                    it->second[i]->interface[j]->empty()) {
                    // this knowledge contains a final marking
                    it->second[i]->is_final = 1;
                    it->second[i]->interface[j] = NULL;
                    // delete the interface marking?
                }
                
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_waitstate) {
                    // check if DL is resolved by interface marking
                    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                        if (it->second[i]->interface[j] != NULL &&
                            it->second[i]->interface[j]->get(l) > 0 &&
                            InnerMarking::receivers[it->second[i]->inner[j]].find(l) != InnerMarking::receivers[it->second[i]->inner[j]].end()) {
                            // this is a transient marking -- set interface marking to NULL
                            it->second[i]->interface[j] = NULL;
                            // delete the interface marking?
                        }
                    }
                }
            }
            
            /* now we know wheter this knowledge contains a final marking and
               that every marking with a non-NULL interface marking is a
               deadlock */

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
    // StoredKnowledge::hashTree.clear();
    
    return result;
}


bool StoredKnowledge::sat() {
//    cerr << this << ": checking sat ";
    
    // if we find a sending successor, this node is OK
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL) {
//            cerr << "OK (send)" << endl;
            return true;
        }
    }
    
    // now each deadlock (a marking with NULL interface?) must have ?-sucessors
    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            bool hasPendingMessage = false;
            
            // we found a deadlock -- check whether for each marked output
            // place exists a respective receiving edge
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                if (interface[i]->get(l) > 0) {
                    hasPendingMessage = true;
                    if (successors[l-1] == NULL) {
                        // found a marked place without successor
//                        cerr << "not OK (missing receive)" << endl;
                        is_sane = 0;
                        return false;
                    }
                }
            }
            
            if (!hasPendingMessage) {
                for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                    if (interface[i]->get(l) > 0 && (InnerMarking::inner_markings[inner[i]]->is_waitstate || InnerMarking::inner_markings[inner[i]]->is_final)) {
//                        cerr << "not OK (covered deadlock) -- [m" << inner[i] << "," << *interface[i] << "] " << InnerMarking::inner_markings[inner[i]]->is_waitstate << endl;
                        is_sane = 0;
                        return false;
                    }
                }
            }
        }
    }
    
    // if we reach this line, each deadlock was resolved
//    cerr << "OK (else)" << endl;
    return true;
}

set<StoredKnowledge*> deletedNodes; // should be member

unsigned int StoredKnowledge::removeInsaneNodes() {    
    unsigned int result = 0;
    
    /// a set of nodes that need to be removed
    std::set<StoredKnowledge*> insaneNodes;
    
    /// a set of nodes that need to be considered
    std::set<StoredKnowledge*> affectedNodes;
    
    // initially, traverse all nodes
    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            if (!it->second[i]->sat())
                insaneNodes.insert(it->second[i]);
        }
    }
    
    // iteratively removed all insane nodes and check their predecessors
    bool done = false;
    while (!done) {
        ++iterations;
        while (!insaneNodes.empty()) {
            StoredKnowledge *todo = (*insaneNodes.begin());
            insaneNodes.erase(insaneNodes.begin());
            assert(todo);

            for (unsigned int i = 0; i < todo->inDegree; ++i) {
                assert (todo->predecessors[i]);
                affectedNodes.insert(todo->predecessors[i]);
                
                bool found = false;
                for (Label_ID l = Label::first_receive; l <= Label::last_send; ++l) {
                    if (todo->predecessors[i]->successors[l-1] == todo) {
                        todo->predecessors[i]->successors[l-1] = NULL;
                        found = true;
                    }
                }
//                assert(found);
            }
            
            ++result;
            deletedNodes.insert(todo);
//            delete todo;
//            todo = NULL;
        }

        for (set<StoredKnowledge*>::iterator it = affectedNodes.begin(); it != affectedNodes.end(); ++it) {
            if (!(*it)->sat()) {
                if (deletedNodes.find(*it) == deletedNodes.end()) {
                    insaneNodes.insert(*it);
                }
            }
        }
        
        done = insaneNodes.empty();
    }
    
    return result;
}


void StoredKnowledge::dot() {
    cout << "digraph G {" << endl;
    
    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            if (it->second[i]->is_sane) {
            cout << "\"" << it->second[i] << "\" [label=\"" << *it->second[i] << "\"]" << endl;
            for (Label_ID l = Label::first_receive; l <= Label::last_send; ++l) {
                if (it->second[i]->successors[l-1] != NULL) {
                    cout << "\"" << it->second[i] << "\" -> \"" << it->second[i]->successors[l-1] << "\" [label=\"" << Label::id2name[l] << "\"]" << endl;
                }
            }}
        }
    }
    
    cout << "}" << endl;
}

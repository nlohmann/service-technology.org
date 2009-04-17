#include <set>
#include <iostream>
#include <cassert>
#include "config.h"
#include "StoredKnowledge.h"
#include "Label.h"

using std::map;
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
StoredKnowledge *StoredKnowledge::root = NULL;
int StoredKnowledge::entries_count = 0;
unsigned int StoredKnowledge::iterations = 0;
unsigned int StoredKnowledge::reportFrequency = 10000;
std::set<StoredKnowledge*> StoredKnowledge::deletedNodes;
std::set<StoredKnowledge*> StoredKnowledge::seen;



/***************
 * CONSTRUCTOR *
 ***************/

StoredKnowledge::StoredKnowledge(Knowledge *K) :
    inner(NULL), interface(NULL), successors(NULL), predecessors(NULL),
    inDegree(0), predecessorCounter(0), is_final(0), is_sane(1)
{
    // make sure we copy from an existing object
    assert(K);

    // reserve the necessary memory for the successors (fixed)
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

    // traverse the bubbles and copy the markings into the C arrays
    for (std::map<InnerMarking_ID, std::vector<InterfaceMarking*> >::const_iterator pos = K->bubble.begin(); pos != K->bubble.end(); ++pos) {
        for (size_t i = 0; i < pos->second.size(); ++i, ++count) {
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


/*!
 \note This destructor is only called during the building of the knowledges.
       That said, neither successors nor predecessors need to be deleted by
       this destructor.
 */
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

/*!
 \return a pointer to a knowledge stored in the hash tree -- it is either
         "this" if the knowledge was not found in the hash tree or a pointer
         to a previously stored knowledge. In the latter case, the calling
         function can detect the duplicate
 */
StoredKnowledge *StoredKnowledge::store() {
    // get the element's hash value
    hash_t myHash = hash();
    
    // check if we find a bucket with that hash value
    std::map<hash_t, std::vector<StoredKnowledge*> >::iterator el = hashTree.find(myHash);
    if (el != hashTree.end()) {
        // we found an element with the same hash -- is it a collision?
        for (size_t i = 0; i < el->second.size(); ++i) {
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
                    if (inner[j] != el->second[i]->inner[j] ||
                        *interface[j] != *el->second[i]->interface[j]) {
                        found = false;
                    }
                }

                // check if we found a previously stored knowledge
                if (found) {
                    // we found a previously stored element with the same
                    // markings -> return a pointer to this element
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

    // we return a pointer to the this object since it was newly stored
    return this;
}


void StoredKnowledge::addSuccessor(Label_ID label, StoredKnowledge *knowledge) {
    // make sure the knowledge to store exists
    assert(knowledge);

    // tau does not make sense here
    assert(!SILENT(label));

    // never add a successor twice
    assert(successors[label-1] == NULL);

    // we will never store label 0 (tau) -- hence decrease the label
    successors[label-1] = knowledge;

    // increase the successor's indegree (needed for later predecessor relation)
    ++(knowledge->inDegree);
}


inline hash_t StoredKnowledge::hash() const {
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
                // case 1: a final marking
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_final &&
                    it->second[i]->interface[j]->empty()) {
                    // this knowledge contains a final marking
                    it->second[i]->is_final = 1;
                    it->second[i]->interface[j] = NULL;
                }

                // case 2: a resolved waitstate
                if (InnerMarking::inner_markings[it->second[i]->inner[j]]->is_waitstate) {
                    // check if DL is resolved by interface marking
                    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                        if (it->second[i]->interface[j] != NULL &&
                            it->second[i]->interface[j]->get(l) > 0 &&
                            InnerMarking::receivers[l].find(it->second[i]->inner[j]) != InnerMarking::receivers[l].end()) {
                            // the DL is resolved -- set interface marking to NULL
                            it->second[i]->interface[j] = NULL;
                        }
                    }
                }

                // case 3: a transient marking
                if (!InnerMarking::inner_markings[it->second[i]->inner[j]]->is_waitstate &&
                    !InnerMarking::inner_markings[it->second[i]->inner[j]]->is_final) {
                        // this is a transient marking -- set interface marking to NULL
                        it->second[i]->interface[j] = NULL;                        
                }
            }

            /* now we know wheter this knowledge contains a final marking and
               that every marking with a non-NULL interface marking is a
               deadlock that needs to be resolved */

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


/*!
 \return whether each deadlock in the knowledge is resolved

 \pre the interface markings of each transient or final marking has to be set
      to NULL
 \post sets value is_sane to 0 in case false is returned
*/
bool StoredKnowledge::sat() {
    // if we find a sending successor, this node is OK
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL) {
            return true;
        }
    }

    // now each deadlock (a marking with NULL interface) must have ?-sucessors
    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            bool resolved = false;

            // we found a deadlock -- check whether for at least one marked
            // output place exists a respective receiving edge
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                if (interface[i]->get(l) > 0 && successors[l-1] != NULL) {
                    resolved = true;
                    break;
                }
            }

            // the deadlock is not resolved
            if (!resolved) {
                is_sane = 0;
                return false;
            }
        }
    }
    
    // if we reach this line, each deadlock was resolved
    return true;
}


/*!
 \todo Do I really need three sets?
*/
unsigned int StoredKnowledge::removeInsaneNodes() {    
    unsigned int result = 0;

    /// a set of nodes that need to be removed
    std::set<StoredKnowledge*> insaneNodes;

    /// a set of nodes that need to be considered
    std::set<StoredKnowledge*> affectedNodes;

    // initially, traverse all nodes
    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            if (!it->second[i]->sat()) {
                insaneNodes.insert(it->second[i]);
            }
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


    // traverse all nodes reachable from the root
    root->traverse();

    return result;
}


void StoredKnowledge::dot(std::ofstream &file, bool showTrue = false, enum_formula formulaStyle = formula_arg_dnf) {
    file << "digraph G {" << endl;
    file << " node [fontname=\"Helvetica\" fontsize=10]" << endl;
    file << " edge [fontname=\"Helvetica\" fontsize=10]" << endl;

    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            if (it->second[i]->is_sane &&
                (seen.find(it->second[i]) != seen.end()) &&
                (showTrue || it->second[i]->size > 0)) {

                string formula;
                switch (formulaStyle) {
                    case(formula_arg_dnf): formula = it->second[i]->formula(); break;
                    case(formula_arg_2bits): formula = it->second[i]->twoBitFormula(); break;
                    case(formula_arg_3bits): formula = it->second[i]->twoBitFormula(); break; // not implemented yet
                }

                file << "\"" << it->second[i] << "\" [label=\"" << formula << "\"]" << endl;
//                cout << "\"" << it->second[i] << "\" [label=\"" << it->second[i] << "\\n" << *it->second[i] << "\"]" << endl;
                for (Label_ID l = Label::first_receive; l <= Label::last_send; ++l) {
                    if (it->second[i]->successors[l-1] != NULL &&
                        (seen.find(it->second[i]->successors[l-1]) != seen.end()) &&
                        (showTrue || it->second[i]->successors[l-1]->size > 0)) {
                        file << "\"" << it->second[i] << "\" -> \"" << it->second[i]->successors[l-1] << "\" [label=\"" << Label::id2name[l] << "\"]" << endl;
                    }
                }
            }
        }
    }

    file << "}" << endl;
}


/*!
 \note possible optimization: don't create a copy for the last label but use
       the object itself
 */
void StoredKnowledge::calcRecursive(Knowledge *K, StoredKnowledge *SK) {
    assert(K);
    assert(SK);
    static unsigned int calls = 0;
    static unsigned int edges = 0;

    if (++calls % reportFrequency == 0) {
        fprintf(stderr, "%8d knowledges, %8d edges\n",
            StoredKnowledge::storedKnowledges, edges);
    }

    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        Knowledge *K_new = new Knowledge(K, l);

        // only process knowledges within the message bounds
        if (K_new->is_sane) {
            // create a compact version of the knowledge bubble
            StoredKnowledge *SK_new = new StoredKnowledge(K_new);

            // add it to the knowledge tree
            StoredKnowledge *SK_store = SK_new->store();
            assert(SK_store);

            // store an edge from the parent to this node
            SK->addSuccessor(l, SK_store);
            ++edges;

            // if the node was new, check its successors
            if (SK_store == SK_new) {
                calcRecursive(K_new, SK_store);
            } else {
                delete SK_new;
            }
        }
        // we saw K_new's successors
        delete K_new;
    }
}


/*!
 Some effort has been put to only add parentheses unless absolutely necessary.

 \return a string representation of the formula
 
 \note This function is also used for an operating guidelines output for
       Fiona.

 \todo Check whether the "true" output is correct.
 */
string StoredKnowledge::formula() const {
    // check if there is a deadlock at all
    bool containsDeadlock = false;
    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            containsDeadlock = true;
        }
    }

    // if there is no deadlock and the knowledge is not final, return true
    if (!containsDeadlock && !is_final) {
        return "true";
    }

    set<string> sendDisjunction;
    set<set<string> > receiveDisjunctions;
    string result;

    // collect !-edges
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL) {
            sendDisjunction.insert(Label::id2name[l]);
        }
    }

    // collect ?-edges for the deadlocks
    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            set<string> temp;
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                if (interface[i]->get(l) > 0 && successors[l-1] != NULL) {
                    temp.insert(Label::id2name[l]);
                }
            }
            if (!temp.empty()) {
                receiveDisjunctions.insert(temp);
            }
        }
    }

    // create a disjunction of the !-edges
    if (!sendDisjunction.empty()) {
        for (set<string>::iterator it = sendDisjunction.begin(); it != sendDisjunction.end(); ++it) {
            if (it != sendDisjunction.begin()) {
                result += " + ";
            }
            result += *it;
        }
    }

    // create a disjunction of conjunction of ?-edges for each deadlock
    if (!receiveDisjunctions.empty()) {
        if (result != "") {
            result += " + ";
        }
        if (!sendDisjunction.empty()) {
            result += "(";
        }
        for (set<set<string> >::iterator it = receiveDisjunctions.begin(); it != receiveDisjunctions.end(); ++it) {
            if (it != receiveDisjunctions.begin() && result != "") {
                result += " * ";
            }
            if (it->size() > 1) {
                result += "(";
            }
            for (set<string>::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
                if (it2 != it->begin()) {
                    result += " + ";
                }
                result += *it2;
            }
            if (it->size() > 1) {
                result += ")";
            }
        }
        if (!sendDisjunction.empty()) {
            result += ")";
        }
    }

    // add the final literal
    if (is_final) {
        if (result == "") {
            result = "final";
        } else {
            result += " + final";
        }
    }

    return result;
}


/*!
 \return "S" if formula can only be satisfied by sending events; "F" if
         formula contains the "final" literal
 */
string StoredKnowledge::twoBitFormula() const {
    string result;

    if (is_final) {
        result += "F";
    }

    for (unsigned int i = 0; i < size; ++i) {
        if (interface[i] != NULL) {
            bool resolved = false;

            // we found a deadlock -- check whether for at least one marked
            // output place exists a respective receiving edge
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                if (interface[i]->get(l) > 0 && successors[l-1] != NULL) {
                    resolved = true;
                    break;
                }
            }

            // the deadlock is not resolved
            if (!resolved) {
                return "S" + result;
            }
        }
    }

    return result;
}


void StoredKnowledge::traverse() {
    if (seen.insert(this).second) {
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            if (successors[l-1] != NULL && successors[l-1]->is_sane) {
                successors[l-1]->traverse();
            }
        }
    }
}

void StoredKnowledge::OGoutput(std::ofstream &file) {
    file << "INTERFACE" << endl;
    file << "  INPUT" << endl;
    bool first = true;
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        if (!first) {
            file << "," << endl;
        }
        first = first && false;
        file << "    " << Label::id2name[l].substr(1,Label::id2name[l].size());
    }
    file << ";" << endl;

    file << "  OUTPUT" << endl;
    first = true;
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (!first) {
            file << "," << endl;
        }
        first = first && false;
        file << "    " << Label::id2name[l].substr(1,Label::id2name[l].size());
    }
    file << ";" << endl << endl;

    file << "NODES" << endl;
    for (set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        if (it != seen.begin()) {
            file << "," << endl;
        }

        file << "  " << reinterpret_cast<unsigned int>(*it) << " : (" << (*it)->formula() << ") : blue";
        if ((*it)->is_final) {
            file << " : finalnode";
        }
    }
    file << ";" << endl << endl;

    file << "INITIALNODE" << endl << "  " << reinterpret_cast<unsigned int>(root) << ";" << endl << endl;

    file << "TRANSITIONS" << endl;
    first = true;
    for (set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            if ((*it)->successors[l-1] != NULL) {
                if (!first) {
                    file << "," << endl;
                }
                first = first && false;
                file << "  " << reinterpret_cast<unsigned int>(*it) << " -> " << reinterpret_cast<unsigned int>((*it)->successors[l-1]) << " : " << Label::id2name[l];
            }
        }
    }
    file << ";" << endl << endl;
}

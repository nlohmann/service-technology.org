#include "Knowledge.h"
#include "config.h"

using std::map;
using std::vector;
using std::endl;
using std::cerr;
using std::pair;


/******************
 * STATIC MEMBERS *
 ******************/

int Knowledge::memory_count = 0;
int Knowledge::memory_max = 0;


/***************
 * CONSTRUCTOR *
 ***************/

Knowledge::Knowledge(InnerMarking_ID m) : is_sane(true), size(1) {   
    ++memory_count;
    memory_max = std::max(memory_count, memory_max);
     
    // add this marking to the bubble and the todo queue
    bubble[m].push_back(new InterfaceMarking());
    todo.push(FullMarking(m));
    
    // calculate the closure
    closure();
}

/*!
 \note no action in this constructor can introduce a duplicate
 
 \note bubble[m].empty() for all inner markings m does not yield
       bubble.empty() to hold. Instead, we have to take care ourselves that
       the vector associated to an inner marking is erase once its empty. In
       the function, we used the vector unreachable to collect the inner
       markings without interface markings.
*/
Knowledge::Knowledge(Knowledge *parent, Label_ID label) : is_sane(true), size(0) {
    ++memory_count;
    memory_max = std::max(memory_count, memory_max);
    
    // tau does not make sense here
    assert(label);
    
    assert(parent);

    // CASE 1: we receive -- decrement interface markings
    if (RECEIVING(label)) {
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            for (unsigned int i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and decrement it
                bool result = true;
                InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]), label, false, result);
                assert(interface);
                
                // analyze the result of the copying
                if (result) {
                    // store this interface marking
                    bubble[pos->first].push_back(interface);
                    ++size;
                } else {
                    // decrement failed -- remove this interface marking
                    delete interface;
                }
            }
        }
    }
    
    // CASE 2: we send -- increment interface markings and calculate closure
    if (SENDING(label)) {
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            bool receiver = (InnerMarking::receivers.find(pos->first) != InnerMarking::receivers.end());
            // muss nicht eigentlich "[label]" hinter receivers???
            
            for (unsigned int i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and increment it
                bool result = true;
                InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]), label, true, result);
                assert(interface);
                
                // analyze the result of the copying
                if (result) {
                    // store this interface marking
                    bubble[pos->first].push_back(interface);
                    ++size;
                    
                    // success -- eventually, this marking became transient
                    if (receiver) {
                        todo.push(FullMarking(pos->first, *interface));
                    }
                } else {
                    // increment failed -- message bound violation
                    delete interface;
                    is_sane = false;
                    return;
                }
            }
        }
        
        // calculate the closure
        closure();
    }
    
    
    // CASE 3: synchronous step
    if (SYNC(label)) {
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            if (InnerMarking::sync[label].find(pos->first) != InnerMarking::sync[label].end()) {
                for (unsigned int i = 0; i < pos->second.size(); ++i) {
                    InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]));
                    assert(interface);
//                    cerr << *interface << " m" << pos->first << endl;
//                    cerr << *interface << " m" << InnerMarking::inner_markings[pos->first]->successors[i] << endl;
                    bubble[InnerMarking::inner_markings[pos->first]->successors[i]].push_back(interface);
                    todo.push(FullMarking(InnerMarking::inner_markings[pos->first]->successors[i], *interface));
                }
            }
        }

//        cerr << "SYNC STEP " << Label::id2name[label] << " " << (unsigned)label << " " << todo.size() << endl;

        // calculate the closure
        closure();
    }
}

Knowledge::~Knowledge() {
    // delete the stored interface markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {
        for (unsigned int i = 0; i < pos->second.size(); ++i) {
            delete pos->second[i];
        }
    }    
    --memory_count;
}

/*************
 * OPERATORS *
 *************/

std::ostream& operator<< (std::ostream &o, const Knowledge &m) {
    if (!m.is_sane) {
        return o << "INSANE KNOWLEDGE";
    }
    
    if (m.bubble.empty()) {
        return o << "[]";
    }
    
    // traverse the bubble
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = m.bubble.begin(); pos != m.bubble.end(); ++pos) {
        // traverse the interface markings
        for (unsigned int i = 0; i < pos->second.size(); ++i) {
            o << "[m" << (unsigned int)pos->first << ", " << *pos->second[i] << "] ";
        }
    }
    return o;
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

/*!
 \note no action in this constructor can introduce a duplicate
 
 \todo do I need to copy the queue item to "current"?
 
 \todo sort bubble
*/
void Knowledge::closure() {
    // to collect markings that were/are already considered
    set<FullMarking> considered;

    while(!todo.empty()) {
        FullMarking current = todo.front();
        todo.pop();
        
        // if this marking was already taken out of the todo queue, skip it this time
        if (!considered.insert(current).second) {
            continue;
        }
        
        // process successors of the current marking
        InnerMarking *m = InnerMarking::inner_markings[current.inner];
        assert(m);
        for (unsigned int i = 0; i < m->out_degree; ++i) {
            
            // in any case, create a successor candidate -- it will be valid for transient transitions anyway
            FullMarking candidate(m->successors[i], current.interface);
            
//            cerr << (unsigned)m->out_degree << " vs " << i << endl;
            
            if (SYNC(m->labels[i])) {
                continue;
            }
            
            // check if successor is a deadlock
            if (InnerMarking::inner_markings[m->successors[i]]->is_deadlock) {
                is_sane = false;
                return;                
            }
            
            // we receive -> the net sends
            if (RECEIVING(m->labels[i])) {
                if (!candidate.interface.inc(m->labels[i])) {
                    is_sane = false;
                    return;
                }
            }
            
            // the successor candidate is reachable
            bool candidateReachable = true;
            
            // we send -> the net receives
            if (SENDING(m->labels[i])) {
                if (!candidate.interface.dec(m->labels[i])) {
                    candidateReachable = false;
                }
            }
            
            // if we found a valid successor candidate, check if it is already stored
            if (!candidateReachable) {
                continue;
            } else {
                bool candidateFound = false;
                for (unsigned int i = 0; i < bubble[candidate.inner].size(); ++i) {
                    if (*(bubble[candidate.inner][i]) == candidate.interface) {
                        candidateFound = true;
                        break;
                    }
                }
                if (!candidateFound) {
                    InterfaceMarking *copy = new InterfaceMarking(candidate.interface);
                    bubble[candidate.inner].push_back(copy);
                    ++size;
                    todo.push(candidate);
                }
            }
        }
    }
    /* sort bubble! */
}

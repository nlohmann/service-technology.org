/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Requirement.h"
#include <list>
#include <pnapi/pnapi.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

/* test, whether an Requirement accepts a Word of Transitions */
//bool RequirementCheck::accept_word(std::deque<pnapi::Transition*> transitionWord) {
//    state* currentState;
//    currentState=this->r->initState;
//   
//    /* iterate through the word */
//    for(std::deque<pnapi::Transition*>::iterator i=transitionWord.begin();i!=transitionWord.end();i++) {
//
//        /* do we have an error state, than we can stop here */
//        if(currentState->error) return false;
//
//        /* restriction to alphabet of requirement */
//        if(this->r->alphabet.find(*i)==this->r->alphabet.end())
//            continue;
//
//        /* successors is hash table, so this is O(1) */
//        currentState=currentState->successors[*i];
//    }
//    /* do we have a final state ? */
//    if(currentState->final) return true;
//    else return false;
//}

/**
The RequirementCheck is a state of a Requirement-DFA and can be advanced by a Transition
Return: false, if we have no successor for the current Transition. In this case we have an error state.
        true, otherwise
*/
bool RequirementCheck::advance(pnapi::Transition* t) {

    /* restriction to alphabet of requirement */
    if(this->requirement->alphabet.find(t)==this->requirement->alphabet.end()) {
       /* printf("Wird uebersprungen: %s \n", t->getName().c_str());*/
        /* double the current state in the stateStack, to be consistent */
        this->stateStack.push_back(this->stateStack.back());
        return this->satisfiable;
    }
    else  {
        //get the successor of currentState in Req-DFA
        std::tr1::unordered_map<pnapi::Transition*, state*>::const_iterator s =this->currentState->successors.find(t);
        // if theres no successor, we have an error state
        if(s==this->currentState->successors.end()){
            return false;
        }
        this->stateStack.push_back(s->second);
        this->currentState=s->second;
        /* printf("Wurde gepusht: %s\n", t->getName().c_str()); */
        return true;
    }
}


/** TESTING STUFF */

/* error State, does not accept, has no successors */
/* This is a temp test function */
/* The Requirement DFA needs to be created like this */
RequirementCheck test_rc() {
    extern pnapi::PetriNet* net;
    pnapi::Transition* t1=net->findTransition("t1");
    std::tr1::unordered_map<pnapi::Transition*,state*>* s;
    s=new std::tr1::unordered_map<pnapi::Transition*,state*>();
    state* errorState=new state(false,true,std::tr1::unordered_map<pnapi::Transition*,state*>());
    (*s)[t1]=errorState;
    state* s1=new state(false,false,*s);
    std::tr1::unordered_set<pnapi::Transition*>* alph=new std::tr1::unordered_set<pnapi::Transition*>();
    alph->insert(t1);
    alph->insert(net->findTransition("t2"));
    Requirement* req=new Requirement(*alph,s1); 
    return RequirementCheck(req);
}

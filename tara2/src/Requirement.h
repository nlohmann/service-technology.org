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

#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <pnapi/pnapi.h>
#include <map>
#include <set>
#include <list>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
/* TODO: Is TR1 okay to use ?? */
#include <stdio.h>

// This represents the DFA, each state has a map to its successors
struct state {
    state(bool e, bool f, std::tr1::unordered_map<pnapi::Transition*, state*> s):
        error(e), final(f), successors(s) { }
    
    bool error; /*true, if we have no more way to a final state */
    bool final; /* true, final state */
    std::tr1::unordered_map<pnapi::Transition*,state*> successors; /* list of all successor states */
};

/* a Requirement is Tupel of an alphabet and a regular language */
class Requirement {
    public: /** simple constructor */
        Requirement( std::tr1::unordered_set<pnapi::Transition*> a, state* i):
            alphabet(a), initState(i)  { }
        const std::tr1::unordered_set<pnapi::Transition*> alphabet;
        const state* initState; /*lang, represented by the DFA-init-state */
};

/**
A RequirementCheck is a Tupel of a Requirement and a state of the DFA of the Requirement-Lang
The DFA can be advanced and also be abated
The passed states are saved in a stack
*/
class RequirementCheck {
    public:
        /** Construct a RequirementCheck based on a Requirement */
        RequirementCheck(Requirement* r):
            satisfiable(true), currentState(r->initState), requirement(r), stateStack(std::deque<state*>()) { }
        /* restrict, if needed, the using the delta function of DFA and change the state. Put state to state-Stack */
        bool advance(pnapi::Transition* t); /* return false, if is unsatisfiable */
        /* pop the statestack and go back to last state  */
        inline void abate() { this->stateStack.pop_back(); this->satisfiable=true; }
        /* returns true, if we are not in error state  */
        inline bool is_satisfiable() { return this->satisfiable; }
        /* check if we are in a final state */
        inline bool accepts() { return this->currentState->final; }
        /** UNUSED AT THE MOMENT */
        /* check, if a complete word of transitions gets accepted by the Requirement Checker  */
        bool accepts_word(std::deque<pnapi::Transition*> transitionWord);
    private:
        bool satisfiable;
        const state* currentState;
        Requirement* requirement;
        std::deque<state*> stateStack;
};


/* only for testin only for testing */
RequirementCheck test_rc();
#endif

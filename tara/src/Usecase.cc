
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

#include <pnapi/pnapi.h>
#include <map>
#include <set>

#include "Usecase.h"
#include "verbose.h"

// #define UC_DEBUG(x) x
#define UC_DEBUG(x) ;

// TODO: use UC_FASTER as default
#define UC_FASTER true


void Usecase::init() {
    pnapi::Condition* c = &this->net->getFinalCondition();

    if(!UC_FASTER) {
        *c = c->getFormula() && ((*invoice == 0) || (*finish == 0)) ;
        net->createArc(*credit, *pay_for_invoice);
        net->createArc(*invoice, *pay_for_invoice);
    }

    setToValue(i);
    // std::cout << pnapi::io::owfn << *net;
}

/**
 * apply a usecase
 */
Usecase::Usecase(
        pnapi::PetriNet* orig,
        pnapi::PetriNet* usecase,
        std::map<pnapi::Transition*,unsigned int>* costfunction,
        unsigned int initialI = 0
    ) {
    UC_DEBUG(status("applying usecase");)

    
    // DEBUG
    // std::cout << pnapi::io::owfn << *usecase;
    // std::cout << pnapi::io::owfn << *orig;

    /*-----------------------------------------*\
    | 1. add IN_ORIG place and IN_USECASE place |
    \*-----------------------------------------*/

    pnapi::Place* in_usecase = &orig->createPlace("in_usecase");
    // create loops to in_usecase place
    std::set<pnapi::Transition*> allUsecaseTransitions = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUsecaseTransitions.begin(); it != allUsecaseTransitions.end(); ++it) {
        pnapi::Transition* t = orig->findTransition( "uc_" + (*it)->getName());
        usecase->createArc(*(*it), *in_usecase, 1);
        usecase->createArc(*in_usecase, *(*it), 1);
    }

    UC_DEBUG(status("created in_usecase place with arcs");)

    // create in_orig place
    pnapi::Place* in_orig = &orig->createPlace("in_orig", 1);
    // add loops to in_orig place
    std::set<pnapi::Transition*> allOrigTransitions = orig->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allOrigTransitions.begin(); it != allOrigTransitions.end(); ++it) {
        orig->createArc(*(*it), *in_orig, 1);
        orig->createArc(*in_orig, *(*it), 1);
    }

    /*--------------------------*\
    | 2. merge usecase and orig  |
    \*--------------------------*/


    // copy those places which are NOT in orig net
    std::set<pnapi::Place*> allUcPlaces = usecase->getPlaces();
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        if(not orig->findPlace( (*it)->getName())) {
            // create prefix
            orig->createPlace((*it)->getName(), (*it)->getTokenCount());
        }
    }

    // copy all transitions from usecase to orig (and prefix)
    std::set<pnapi::Transition*> allUcTrans = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUcTrans.begin(); it != allUcTrans.end(); ++it) {
        // create prefix
        pnapi::Transition* newTrans = &orig->createTransition("uc_" + (*it)->getName());
        //copy labels
        std::map<pnapi::Label*,unsigned int> allLabels = (*it)->getLabels();
        for(std::map<pnapi::Label*,unsigned int>::iterator lIt = allLabels.begin(); lIt != allLabels.end(); ++lIt) {
            pnapi::Label * label = orig->getInterface().findLabel(lIt->first->getName());
            newTrans->addLabel(*label, lIt->second);
        }
    }

    // copy all arcs
    std::set<pnapi::Arc*> allUcArcs = usecase->getArcs();
    for(std::set<pnapi::Arc*>::iterator it = allUcArcs.begin(); it != allUcArcs.end(); ++it) {
        pnapi::Node* src;
        pnapi::Node* target;

        // find target
        if(orig->containsNode("uc_" + (*it)->getSourceNode().getName())) {
            src = orig->findNode("uc_" + (*it)->getSourceNode().getName());
        } else {
            src = orig->findNode((*it)->getSourceNode().getName());
        }

        // find source
        if(orig->containsNode("uc_" + (*it)->getTargetNode().getName())) {
            target = orig->findNode("uc_" + (*it)->getTargetNode().getName());
        } else {
            target = orig->findNode((*it)->getTargetNode().getName());
        }

        orig->createArc(*src, *target, (*it)->getWeight());
    }

    UC_DEBUG(status("merged usecase and orig net");)

    // Add INVOICE place for costs
    invoice = & orig->createPlace("invoice");
    credit = & orig->createPlace("credit", 0);
    // this two places will be updated later

    // Add invoice archs
    // for all elements in costfunction
    std::map<pnapi::Transition*, unsigned int> newCostfunction;
    for(std::map<pnapi::Transition*,unsigned int>::iterator it = costfunction->begin(); it !=  costfunction->end(); ++it) {
        // transition in usecase
        pnapi::Transition* t = orig->findTransition("uc_" + it->first->getName());

        // put cost tokens on invoice
        if(t and it->second > 0) {
            usecase->createArc(*t, *invoice, it->second);
            newCostfunction[t] = it->second;
        }
        it->second = 0;
    }
    (*costfunction) = newCostfunction;


    //  TODO: strict
    //  Compute #p (max value) for each place
    //  -> Lola
   /*----------------------------*\
   | 3. Create Complement places |
   \*----------------------------*/
    //status("before creating complement places");

   /*
#define SOME_BIG_INT 999
   // a map for remebering the complement
   std::map<pnapi::Place*, pnapi::Place*> complement;

   std::set<pnapi::Place*> allPlaces = usecase->getPlaces();
   for(std::set<pnapi::Place*>::iterator it = allPlaces.begin(); it != allPlaces.end(); ++it) {

       // create Complement
       std::string compName = "~" + (*it)->getName();
       pnapi::Place* curCompl = &usecase->createPlace(compName);
    
       int i=0;
       // copy archs from preset
       std::set<pnapi::Node*> preSet = (*it)->getPreset();
       for(std::set<pnapi::Node*>::iterator preIt = preSet.begin(); preIt != preSet.end(); ++preIt) {
           pnapi::Arc* curArc = usecase->findArc(**preIt, **it);
           usecase->createArc(*curCompl, *(*preIt), curArc->getWeight());
           ++i; // count new arcs
       }
       std::set<pnapi::Node*> postSet = (*it)->getPostset();
       for(std::set<pnapi::Node*>::iterator postIt = postSet.begin(); postIt != postSet.end(); ++postIt) {
           pnapi::Arc* curArc = usecase->findArc(*(*it), *(*postIt));
           usecase->createArc(*(*postIt), *curCompl, curArc->getWeight());
           ++i; // count new arcs
       }

       //status("Created complement place (%s)  for: %s with %i arcs", curCompl->getName().c_str() , (*it)->getName().c_str(), i);

       // remember in complement map
       complement[*it] = curCompl;

       // set the initial marking
       curCompl->setTokenCount(SOME_BIG_INT - (*it)->getTokenCount());
       
   }*/
    

    /*----------------------*\
    | 4. jump in / jump back |
    \*----------------------*/

    // add unconditional jump back
    pnapi::Transition* uncond_jump_back = &orig->createTransition("unconditional_jump_back");
    pnapi::Place* uc_in_usecase = orig->findPlace("in_usecase");
    orig->createArc(*uc_in_usecase, *uncond_jump_back, 1);
    orig->createArc(*uncond_jump_back, *in_orig, 1);

    // add conditional jump in
    pnapi::Place* pre_usecase = &orig->createPlace("pre_usecase", 1);
    pnapi::Transition* cond_jump_in = &orig->createTransition("cond_jump_in");
    orig->createArc(*pre_usecase, *cond_jump_in);
    orig->createArc(*in_orig, *cond_jump_in);
    orig->createArc(*cond_jump_in, *uc_in_usecase);

    UC_DEBUG(status("added conditional jump in");)

    // jump_in: set the pre-condition
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        // find corresponding place in orig
        pnapi::Place* ucPlace = orig->findPlace((*it)->getName());
        if(!ucPlace) continue;
        // inital marking (in usecase) for the current Place
        int tokenCount = (*it)->getTokenCount();
        if(tokenCount > 0) {
            orig->createArc(*ucPlace, *cond_jump_in, tokenCount);
            orig->createArc(*cond_jump_in, *ucPlace, tokenCount);
        }
        // TODO: if strict, add complement place arc
    }

    UC_DEBUG(status("added conditional jump in conditions");)

    // add conditional jump back
    // this place signals that the usecase was complete finished
    finish = &orig->createPlace("finish");

    pnapi::Condition* finalCond = &usecase->getFinalCondition();

    // here, we transform final condition to a transition-precondition
    // jump_back-transitions: set the pre-condition
    const pnapi::formula::Formula* ucFormula = &finalCond->getFormula();
    pnapi::formula::Disjunction* formula;
    const pnapi::formula::Formula* fTrue = new pnapi::formula::FormulaTrue();
    const pnapi::formula::Formula* fFalse = new pnapi::formula::FormulaFalse();

    // create dnf
    ucFormula->dnf();
    ucFormula->removeNegation();
    UC_DEBUG(status("formula type is: %d", ucFormula->getType());)

    // make sure we have full dnf
    switch(ucFormula->getType()) {
        case pnapi::formula::Formula::F_DISJUNCTION:
            // we have a full DNF
            formula = dynamic_cast<pnapi::formula::Disjunction*> (ucFormula->clone());
            break;
        case pnapi::formula::Formula::F_CONJUNCTION:
            // complete to full DNF
            formula = new pnapi::formula::Disjunction((*ucFormula) , (*fFalse));
            break;
        default: // everything else should be literal
            formula = new pnapi::formula::Disjunction( (*ucFormula) && (*fTrue) , *fFalse );
            break;
    }

    UC_DEBUG(status("new formula type is: %d", formula->getType());)
    const std::set<const pnapi::formula::Formula*> clauseSet = formula->getChildren();
    unsigned int jb_counter = 0;

    // for all clauses in disjunction
    for(std::set<const pnapi::formula::Formula*>::iterator clauseIt = clauseSet.begin(); clauseSet.end() != clauseIt; ++clauseIt) {

        // assume: every claus is conjunction
        if( (*clauseIt)->getType() != pnapi::formula::Formula::F_CONJUNCTION) {
            message("somewhere something went wrong. Please report bug.");
            abort();
        }

        // create Transition for each clause
        ++jb_counter;
        std::stringstream newName;
        newName << "jump_back_" << jb_counter;
        pnapi::Transition* newTrans = &orig->createTransition(newName.str().c_str());

        orig->createArc(*uc_in_usecase, *newTrans);
        orig->createArc(*newTrans, *finish);
        orig->createArc(*newTrans, *in_orig);

        // for each literal create arc
        const pnapi::formula::Conjunction* clause = dynamic_cast<pnapi::formula::Conjunction*> ((*clauseIt)->clone());
        const std::set<const pnapi::formula::Formula*> literals = clause->getChildren();
        for(std::set<const pnapi::formula::Formula*>::iterator litIt = literals.begin(); litIt != literals.end(); ++litIt) {
            // each literals is a proposition
            const pnapi::formula::Proposition* proposition = dynamic_cast<const pnapi::formula::Proposition*> (*litIt);
            pnapi::Place* place = orig->findPlace(proposition->getPlace().getName());
            unsigned int tokens = proposition->getTokens();
            switch( (*litIt)->getType()) {
                case pnapi::formula::Formula::F_FALSE:
                    // AND FALSE <=> trans aint gonna firin
                    orig->deleteTransition(*newTrans);
                    break;
                case pnapi::formula::Formula::F_TRUE:
                    // AND TRUE <=> do nothing
                    break;
                // EQUAL literal
                case pnapi::formula::Formula::F_EQUAL:
                    {
                        char m[] = "in usecase final condition: %s = %d not possible in non-strict mode, interpreted as %s > %d";
                        message(m, place->getName().c_str(), tokens, place->getName().c_str(), tokens);
                    }
                // GREATER literal
                case pnapi::formula::Formula::F_GREATER_EQUAL:
                    if(tokens > 0) {
                        orig->createArc(*place, *newTrans, tokens);
                        orig->createArc(*newTrans, *place, tokens);
                    }
                    break;
                // GREATER EQUAL literal
                case pnapi::formula::Formula::F_GREATER:
                    orig->createArc(*place, *newTrans, tokens + 1);
                    orig->createArc(*newTrans, *place, tokens + 1);
                    break;
                // LESS literal
                case pnapi::formula::Formula::F_LESS:
                    {
                        char m[] = "in usecase final condition: %s < %d not possible in non-strict mode, interpreted as 'always true'";
                        message(m, place->getName().c_str(), tokens);
                    }
                    break;
                // LESS EQUAL literal
                case pnapi::formula::Formula::F_LESS_EQUAL:
                    {
                        char m[] = "in usecase final condition: %s <= %d not possible in non-strict mode, interpreted as 'always true'";
                        message(m, place->getName().c_str(), tokens);
                    }
                    break;
                default:
                    message("such things not allowed in non-strict mode. Pleae report bug.");
            }
        }
    }

    UC_DEBUG(status("added conditional jump back with conditions");)

    // add PAY_FOR_INVOICE trans
    pay_for_invoice = &orig->createTransition("pay_for_invoice");
    orig->createArc(*pay_for_invoice, *finish);
    orig->createArc(*finish, *pay_for_invoice); 
    // the arc to credit will be added later

    oldFormula = (orig->getFinalCondition().getFormula().clone());
    // remember
    this->net = orig;
    // std::cout << pnapi::io::owfn << *orig;
}

unsigned int Usecase::getI() { return i; }
void Usecase::setToValue(unsigned int newI) {
    i = newI;

    if(UC_FASTER) {
        pnapi::Condition* c = &this->net->getFinalCondition();
        *c = (*oldFormula) && (*invoice <= i || *finish == 0);
    }

    if(!UC_FASTER) {
        credit->setTokenCount(i);
    }

    // std::cout << pnapi::io::owfn << *net;
}

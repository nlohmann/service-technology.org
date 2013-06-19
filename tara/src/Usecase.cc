
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

/**
 * apply a usecase
 */
void applyUsecase(
        pnapi::PetriNet* orig,
        pnapi::PetriNet* usecase,
        std::map<pnapi::Transition*,unsigned int>* costfunction
    ) {
    status("applying usecase");

    /*------------------*\
    | 1. Modify usecase  |
    \*------------------*/
    
    // DEBUG
    // std::cout << pnapi::io::owfn << *usecase;

     orig->prefixNames("orig-");

    // Add INVOICE place for costs
    pnapi::Place* invoice = & usecase->createPlace("invoice");

    // Add invoice args
    // for all elements in costfunction
    for(std::map<pnapi::Transition*,unsigned int>::iterator it = costfunction->begin(); it !=  costfunction->end(); ++it) {
        // transition in usecase
        pnapi::Transition* t = usecase->findTransition(it->first->getName());
        // put cost tokens on invoice
        if(t) {
            usecase->createArc(*t,*invoice,it->second);
        }
        it->second = 0;
    }


    //  TODO:
    //  Compute #p (max value) for each place
    //  -> Lola
#define SOME_BIG_INT 99999


   /*----------------------------*\
   | 1.1 Create Complement places |
   \*----------------------------*/
    status("before creating complement places");

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

       status("Created complement place (%s)  for: %s with %i arcs", curCompl->getName().c_str() , (*it)->getName().c_str(), i);

       // remember in complement map
       complement[*it] = curCompl;

       // set the initial marking
       curCompl->setTokenCount(SOME_BIG_INT - (*it)->getTokenCount());
       
   }
    
    // add IN_ORIG place and IN_USECASE place
    pnapi::Place* in_usecase = &usecase->createPlace("in_usecase");
    // create loops to in_usecase place
    std::set<pnapi::Transition*> allUsecaseTransitions = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUsecaseTransitions.begin(); it != allUsecaseTransitions.end(); ++it) {
        usecase->createArc(*(*it), *in_usecase, 1);
        usecase->createArc(*in_usecase, *(*it), 1);
    }
    status("created in_usecase place with arcs");


    // add conditional jump back
    pnapi::Transition* cond_jump_back = &orig->createTransition("cond_jump_back");
    // IN_USECASE + FINAL_COND -> WAY_HOME_DUTY
    pnapi::Place* way_home = &usecase->createPlace("way_home");

    pnapi::Condition* finalCond = &usecase->getFinalCondition();

    // here, we transform final condition to a transition-precondition
    // using pnapi interval
    // TODO: for each place
    // pnapi::formula::Intervall* interval = &finalCond->getPlaceInterval( --place-- );
    // int upper =  interval->getUpper();
    // int lower =  interval->getLower();

    // add PAY_FOR_INVOICE trans
    // WAY_HOME_DUTY + INVOICE -> COMPLEMENT (INVOICE)
    pnapi::Transition* pay_for_invoice = &orig->createTransition("pay_for_invoice");
    // costs = 1


    
    /*---------------------*\
    | 2. compose with orig  |
    \*---------------------*/

    // create in_orig place
    pnapi::Place* in_orig = &orig->createPlace("in_orig", 1);
    // add loops to in_orig place
    std::set<pnapi::Transition*> allOrigTransitions = orig->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allOrigTransitions.begin(); it != allOrigTransitions.end(); ++it) {
        orig->createArc(*(*it), *in_orig, 1);
        orig->createArc(*in_orig, *(*it), 1);
    }

    // copy all places from usecase to orig
    std::set<pnapi::Place*> allUcPlaces = usecase->getPlaces();
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        // create prefix
        orig->createPlace("usecase-" + (*it)->getName(), (*it)->getTokenCount());
    }
    // copy all transitions from usecase to orig
    std::set<pnapi::Transition*> allUcTrans = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUcTrans.begin(); it != allUcTrans.end(); ++it) {
        // create prefix
        orig->createTransition("usecase-" + (*it)->getName());
    }

    // add conditional jump in
    // PRE_USECASE place
    pnapi::Place* pre_usecase = &orig->createPlace("pre_usecase", 1);
    pnapi::Transition* cond_jump_in = &orig->createTransition("cond_jump_in");
    orig->createArc(*in_orig, *cond_jump_in);
    orig->createArc(*cond_jump_in, *in_usecase);

    // jump_in: set the pre-condition
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        pnapi::Place* ucPlace = orig->findPlace("usecase-" + (*it)->getName());
        // inital marking (in usecase) for the current Placd
        int tokenCount = (*it)->getTokenCount();
        orig->createArc(*ucPlace, *cond_jump_in, tokenCount);
        orig->createArc(*cond_jump_in, *ucPlace, tokenCount);
        orig->createArc(*(complement[ucPlace]), *cond_jump_in, SOME_BIG_INT - tokenCount);
        orig->createArc(*cond_jump_in, *(complement[ucPlace]), SOME_BIG_INT - tokenCount);
    }


    //DEBUG
    std::cout << pnapi::io::owfn << *orig;

    std::set<pnapi::Arc*> allUcArcs = usecase->getArcs();
    for(std::set<pnapi::Arc*>::iterator it = allUcArcs.begin(); it != allUcArcs.end(); ++it) {
        pnapi::Node* src;
        pnapi::Node* target;
        std::cout << "interface source place: " <<(*it)->getSourceNode().getName() << std::endl;
        std::cout << "interface target place: " <<(*it)->getTargetNode().getName() << std::endl;
        if(orig->containsNode("usecase-" + (*it)->getSourceNode().getName())) {
            src = orig->findNode("usecase-" + (*it)->getSourceNode().getName());
        } else { // place is from interface
            src = orig->findNode((*it)->getSourceNode().getName());
        }
        std::cout << "zweite haelfte" << std::endl;
        if(orig->containsNode("usecase-" + (*it)->getTargetNode().getName())) {
            target = orig->findNode("usecase-" + (*it)->getTargetNode().getName());
        } else { // place is from interface
            target = orig->findNode((*it)->getTargetNode().getName());
        }
        std::cout << "o interface source place: " << src->getName() << std::endl;
        std::cout << "o interface target place: " << target->getName() << std::endl;
        orig->createArc(*src, *target);
    }
    status("union of usecase and orig created");


    /*--------------------------------------------*\
    | 3. create connection, jump in and jump back  |
    \*--------------------------------------------*/


    // add unconditional jump back
    pnapi::Transition* uncond_jump_back = &orig->createTransition("unconditional_jump_back");
    pnapi::Place* uc_in_usecase = orig->findPlace("usecase-in_usecase");
    orig->createArc(*uc_in_usecase, *uncond_jump_back, 1);
    orig->createArc(*uncond_jump_back, *in_orig, 1);

    // add INVOICE_PAID trans
    // COMPLEMENT((INVOICE) -> IN_S
    pnapi::Transition* invoice_paid = &usecase->createTransition("invoice_paid");
    usecase->createArc(*complement[invoice], *invoice_paid, SOME_BIG_INT);
    usecase->createArc(*way_home, *invoice_paid, 1);
    usecase->createArc(*invoice_paid, *in_orig, 1);

    std::cout << pnapi::io::owfn << *orig;
}



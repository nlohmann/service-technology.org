
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
    //status("applying usecase");

    /*------------------*\
    | 1. Modify usecase  |
    \*------------------*/
    
    // DEBUG
    // std::cout << pnapi::io::owfn << *usecase;
    // std::cout << pnapi::io::owfn << *orig;


    // Add INVOICE place for costs
    pnapi::Place* invoice = & usecase->createPlace("invoice");

    // Add invoice archs
    // for all elements in costfunction
    for(std::map<pnapi::Transition*,unsigned int>::iterator it = costfunction->begin(); it !=  costfunction->end(); ++it) {
        // transition in usecase
        pnapi::Transition* t = usecase->findTransition(it->first->getName());
        // put cost tokens on invoice
        if(t) {
            //status("costarc: %d", it->second);
            usecase->createArc(*t,*invoice,it->second);
        }
        it->second = 0;
    }

    orig->prefixNodeNames("orig_");

    //  TODO:
    //  Compute #p (max value) for each place
    //  -> Lola
#define SOME_BIG_INT 99999


   /*----------------------------*\
   | 1.1 Create Complement places |
   \*----------------------------*/
    //status("before creating complement places");

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
       
   }
    
    // add IN_ORIG place and IN_USECASE place
    pnapi::Place* in_usecase = &usecase->createPlace("in_usecase");
    // create loops to in_usecase place
    std::set<pnapi::Transition*> allUsecaseTransitions = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUsecaseTransitions.begin(); it != allUsecaseTransitions.end(); ++it) {
        usecase->createArc(*(*it), *in_usecase, 1);
        usecase->createArc(*in_usecase, *(*it), 1);
    }
    //status("created in_usecase place with arcs");


    
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
        orig->createPlace("uc_" + (*it)->getName(), (*it)->getTokenCount());
    }
    // copy all transitions from usecase to orig
    std::set<pnapi::Transition*> allUcTrans = usecase->getTransitions();
    for(std::set<pnapi::Transition*>::iterator it = allUcTrans.begin(); it != allUcTrans.end(); ++it) {
        // create prefix
        pnapi::Transition* newTrans = &orig->createTransition("uc_" + (*it)->getName());
        //copy labels
        std::map<pnapi::Label*,unsigned int> allLabels = (*it)->getLabels();
        for(std::map<pnapi::Label*,unsigned int>::iterator lIt = allLabels.begin(); lIt != allLabels.end(); ++lIt) {
            pnapi::Label * label = orig->getInterface().findLabel(lIt->first->getName());
            //newTrans->addLabel(*(lIt->first), lIt->second);
            newTrans->addLabel(*label, lIt->second);
        }
        
    }
    // status("merged usecase and orig net");

    std::set<pnapi::Arc*> allUcArcs = usecase->getArcs();
    for(std::set<pnapi::Arc*>::iterator it = allUcArcs.begin(); it != allUcArcs.end(); ++it) {
        pnapi::Node* src;
        pnapi::Node* target;
        if(orig->containsNode("uc_" + (*it)->getSourceNode().getName())) {
            src = orig->findNode("uc_" + (*it)->getSourceNode().getName());
        } else { // place is from interface
            src = orig->findNode((*it)->getSourceNode().getName());
        }
        if(orig->containsNode("uc_" + (*it)->getTargetNode().getName())) {
            target = orig->findNode("uc_" + (*it)->getTargetNode().getName());
        } else { // place is from interface
            target = orig->findNode((*it)->getTargetNode().getName());
        }
        orig->createArc(*src, *target, (*it)->getWeight());

        // also create arcs to orig
        if(orig->findPlace("orig_" + (*it)->getSourceNode().getName())) {
            src = orig->findPlace("orig_" + (*it)->getSourceNode().getName());
            orig->createArc(*src, *target, (*it)->getWeight());
        } else if(orig->findPlace("orig_" + (*it)->getTargetNode().getName())) {
            target = orig->findPlace("orig_" + (*it)->getTargetNode().getName());
            orig->createArc(*src, *target, (*it)->getWeight());
        } 
    }
    // status("union of usecase and orig created");

    /*----------------------*\
    | 3. jump in / jump back |
    \*----------------------*/

    // add unconditional jump back
    pnapi::Transition* uncond_jump_back = &orig->createTransition("unconditional_jump_back");
    pnapi::Place* uc_in_usecase = orig->findPlace("uc_in_usecase");
    orig->createArc(*uc_in_usecase, *uncond_jump_back, 1);
    orig->createArc(*uncond_jump_back, *in_orig, 1);

    // add conditional jump in
    // PRE_USECASE place
    pnapi::Place* pre_usecase = &orig->createPlace("pre_usecase", 1);
    pnapi::Transition* cond_jump_in = &orig->createTransition("cond_jump_in");
    orig->createArc(*pre_usecase, *cond_jump_in);
    orig->createArc(*in_orig, *cond_jump_in);
    orig->createArc(*cond_jump_in, *uc_in_usecase);

    // status("added conditional jump in");

    // jump_in: set the pre-condition
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        // status("orig_%s", (*it)->getName().c_str());
        pnapi::Place* ucPlace = orig->findPlace("orig_" + (*it)->getName());
        if(!ucPlace) continue;
//        status("inital cond for uc_%s", (*it)->getName().c_str());
        // pnapi::Place* ucComplPlace = orig->findPlace("usecase-~" + (*it)->getName());
 // without compl place it is not interesting
        // if(!ucComplPlace) continue;
        // inital marking (in usecase) for the current Placd
        int tokenCount = (*it)->getTokenCount();
        //status("orig_%s wiith tokencount: %d", (*it)->getName().c_str(), tokenCount);
        if(tokenCount > 0) {
            orig->createArc(*ucPlace, *cond_jump_in, tokenCount);
            orig->createArc(*cond_jump_in, *ucPlace, tokenCount);
        }
        // TODO: else
        // orig->createArc(*ucComplPlace, *cond_jump_in, SOME_BIG_INT - tokenCount);
        // orig->createArc(*cond_jump_in, *ucComplPlace, SOME_BIG_INT - tokenCount);
    }

    //status("added conditional jump in conditions");

    // add conditional jump back
    pnapi::Transition* cond_jump_back = &orig->createTransition("cond_jump_back");
    // IN_USECASE + FINAL_COND -> WAY_HOME_DUTY
    pnapi::Place* way_home = &orig->createPlace("way_home");
    orig->createArc(*uc_in_usecase, *cond_jump_back);
    orig->createArc(*cond_jump_back, *way_home);

    pnapi::Condition* finalCond = &usecase->getFinalCondition();

    // here, we transform final condition to a transition-precondition
    // using pnapi interval
    // jump_back: set the pre-condition
    for(std::set<pnapi::Place*>::iterator it = allUcPlaces.begin(); it != allUcPlaces.end(); ++it) {
        pnapi::Place* ucPlace = orig->findPlace("uc_" + (*it)->getName());
        pnapi::Place* ucComplPlace = orig->findPlace("uc_~" + (*it)->getName());
        // without compl place it is not interesting
        if(!ucComplPlace) continue;
        pnapi::formula::Interval interval = finalCond->getPlaceInterval(*(*it));
        int upper =  interval.getUpper();
        int lower =  interval.getLower();
        //status("at place  uc_%s", (*it)->getName().c_str());
        //status("at interval %d   %d", lower, upper);
        if(lower > 0) {
            orig->createArc(*ucPlace, *cond_jump_back, lower);
            orig->createArc(*cond_jump_in, *ucPlace, lower);
        }
        //status("at interval 2");
        if(upper > 0) {
            orig->createArc(*ucComplPlace, *cond_jump_back, SOME_BIG_INT - upper);
            orig->createArc(*cond_jump_in, *ucComplPlace, SOME_BIG_INT - upper);
        }
    }

    //status("added conditional jump back with conditions");

    // add PAY_FOR_INVOICE trans
    // WAY_HOME_DUTY + INVOICE -> COMPLEMENT (INVOICE)
    pnapi::Transition* pay_for_invoice = &orig->createTransition("pay_for_invoice");
    (*costfunction)[pay_for_invoice] = 1;
    pnapi::Place* complInvoice = orig->findPlace("uc_~invoice");
    pnapi::Place* ucInvoice = orig->findPlace("uc_invoice");
    orig->createArc(*ucInvoice, *pay_for_invoice);
    orig->createArc(*pay_for_invoice, *complInvoice);
    orig->createArc(*pay_for_invoice, *way_home);
    orig->createArc(*way_home, *pay_for_invoice);


    // aadd INVOICE_PAID trans
    pnapi::Transition* invoice_paid = &orig->createTransition("invoice_paid");
    orig->createArc(*complInvoice, *invoice_paid, SOME_BIG_INT);
    orig->createArc(*way_home, *invoice_paid, 1);
    orig->createArc(*invoice_paid, *in_orig, 1);

    // std::cout << pnapi::io::owfn << *orig;
}

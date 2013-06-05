
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
        pnapi::PetriNet* origin,
        pnapi::PetriNet* usecase,
        std::map<pnapi::Transition*,unsigned int>* costfunction
    ) {
    status("applying usecase");

    /*------------------*\
    | 1. Modify usecase  |
    \*------------------*/
    
    // DEBUG
    std::cout << pnapi::io::owfn << *usecase;

    // Add INVOICE place for costs
    pnapi::Place* invoice = & usecase->createPlace("invoice");

    // Add invoice args
    // for all elements in costfunction
    for(std::map<pnapi::Transition*,unsigned int>::iterator it = costfunction->begin(); it !=  costfunction->end(); ++it) {
        // transition in usecase
        pnapi::Transition* t = usecase->findTransition(it->first->getName());
        // put cost tokens on invoice
        usecase->createArc(*t,*invoice,it->second);
        it->second = 0;
    }


    //  TODO:
    //  Compute #p (max value) for each place
    //  -> Lola
#define SOME_BIG_INT 99999


   /*----------------------------*\
   | 1.1 Create Complement places |
   \*----------------------------*/

    
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
           usecase->createArc(*(*preIt), *curCompl, curArc->getWeight());
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
    pnapi::Place* in_orig = &usecase->createPlace("in_orig");
    pnapi::Place* in_usecase = &usecase->createPlace("in_usecase");

    // add unconditional jump back
    pnapi::Transition* uncond_jump_back = &usecase->createTransition("unconditional_jump_back");
    usecase->createArc(*in_usecase, *uncond_jump_back, 1);
    usecase->createArc(*uncond_jump_back, *in_orig, 1);

    // add conditional jump in
    // PRE_USECASE place
    pnapi::Place* pre_usecase = &usecase->createPlace("pre_usecase");
    // TODO: jump_in: what is the pre-condition

    // add conditional jump back
    pnapi::Transition* cond_jump_back = &usecase->createTransition("cond_jump_back");
    // TODO
    // IN_USECASE + FINAL_COND -> WAY_HOME_DUTY

    // add PAY_FOR_INVOICE trans
    // WAY_HOME_DUTY + INVOICE -> COMPLEMENT (INVOICE)
    pnapi::Transition* pay_for_invoice = &usecase->createTransition("pay_for_invoice");
    // costs = 1

    // add INVOICE_PAID trans
    // COMPLEMENT(WAY_HOME_DUTY) -> IN_S

   std::cout << pnapi::io::owfn << *usecase;
    
    /*---------------------*\
    | 2. compose with orig  |
    \*---------------------*/

    // prefix orig

    // compose
    
    // Add Loops for each trans, resp.
    // IN_ORIG, IN_USECASE


}



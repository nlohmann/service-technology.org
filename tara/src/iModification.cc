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

#include <iModification.h>
#include <list>
#include <pnapi/pnapi.h>
#include <stdio.h>

extern unsigned int cost(pnapi::Transition* t);

// arguments are the net and the inter i
iModification::iModification(pnapi::PetriNet* netToModify, unsigned int startI)
   : net(netToModify), i(startI), maxTransCost(0)
{
   // do the init modification
   this->init();
} 

void iModification::iterate() {
   
   if(this->i ==0)
      printf("error, cannot iterate under zero");
   // TODO: insert some nice error exit here...

   this->i--; // optimize ??

   // add arc from expensed costs to  out of credit place
   this->outOfCreditArc->setWeight(this->i+1);

   // finally set the availble costs
   this->availableCost->setTokenCount(this->maxTransCost+this->i);

   /// update available costs
   //unsigned int newAvailable=this->availableCost->getTokenCount()+1;
  // this->availableCost->setTokenCount(newAvailable);

   // update out of Credit Arc
  // unsigned int newOutOfCredit=this->outOfCreditArc->getWeight()+1;
  // this->outOfCreditArc->setWeight(newOutOfCredit);   
}

void iModification::update() {
    
   // update the arc weight
   outOfCreditArc->setWeight(i+1);

   // update the available costs
   availableCost->setTokenCount(maxTransCost+i);
        

}


void iModification::setToValue(unsigned int newI) {
    
    i = newI;
    update();
        

}

unsigned int iModification::decrease() {
   
   if(i ==0)
      printf("error, cannot iterate under zero");
   // TODO: insert some nice error exit here...

   --i; // optimize ??
    
   update();
   
   return i;
}

unsigned int iModification::increase() {
   
   ++i; // optimize ??

   update();
   
   return i;
}



  
unsigned int iModification::getI() { return this->i; }

void iModification::init() {

   //create the place for the availble costs
   // tokens will be set later
   this->availableCost= &net->createPlace();

   // place for the expensed costs,
   // we may forget that one after init
   pnapi::Place* expensedCosts = &net->createPlace();
  
   //iterate over all transitions of the net
   std::set<pnapi::Transition*> allTransitions=net->getTransitions();
   for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();it++) {

      // the cost of that transition
      int curCost=cost(*it);

      //if the transition is free, do nothing
      if(curCost == 0) continue;

      // remember the most expensive transition
      maxTransCost=maxTransCost>curCost? maxTransCost: curCost;
     
      //add arc from availableCost to that transition
      net->createArc(*availableCost,**it, curCost);

      //arc from that transition to expensedCosts
      net->createArc( **it, *expensedCosts,curCost);
 }
   // add out of credit place
   // C-Blitz, game over
   pnapi::Place* outOfCreditPlace = & net->createPlace();

   net->getFinalCondition() = (net->getFinalCondition().getFormula() && ( *outOfCreditPlace == 0));
  
   // add out of credit transition
   pnapi::Transition* outOfCreditTransition = &net->createTransition();
  
   // an arc from the transition to the place
   net->createArc( *outOfCreditTransition, *outOfCreditPlace);
  
    // add arc from expensed costs to  out of credit place
    this->outOfCreditArc = & net->createArc( *expensedCosts, *outOfCreditTransition, this->i+1);

   // finally set the availble costs
   this->availableCost->setTokenCount(maxTransCost+i);
}

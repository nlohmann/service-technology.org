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
#include "verbose.h"

extern unsigned int cost(pnapi::Transition* t);
extern int hiCosts;
// arguments are the net and the inter i
iModification::iModification(pnapi::PetriNet* netToModify, unsigned int startI)
   : net(netToModify), i(startI)
{
   // do the init modification
   this->init();
} 

void iModification::iterate() {
   
   decrease();
   /// update available costs
   //unsigned int newAvailable=this->availableCost->getTokenCount()+1;
  // this->availableCost->setTokenCount(newAvailable);

   // update out of Credit Arc
  // unsigned int newOutOfCredit=this->outOfCreditArc->getWeight()+1;
  // this->outOfCreditArc->setWeight(newOutOfCredit);   
}

void iModification::update() {
    
   // update the available costs
   availableCost->setTokenCount(hiCosts+i);
        

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

   status("Initializing modification. Highest transition costs are: %d", hiCosts);

   //create the place for the availble costs
   // tokens will be set later
   this->availableCost= &net->createPlace();


  
   //iterate over all transitions of the net
   std::set<pnapi::Transition*> allTransitions=net->getTransitions();
   for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();it++) {

      // the cost of that transition
      int curCost=cost(*it);

      unsigned int in = 0;
      unsigned int out = 0;
		
      //if the transition is free, add the self loop to the budget place and continue
      if(curCost == 0) {
	
	in = hiCosts;
	out = hiCosts;
      } else {
	in = hiCosts + curCost - 1 ;
	if (hiCosts > 1) out = hiCosts - 1;
      }

	//add arc from availableCost to that transition
	if (in > 0) net->createArc(*availableCost,**it, in);
	
	//add arc from transition to availableCost
	if (out > 0) net->createArc(**it, *availableCost, out);

   }
   
   net->getFinalCondition() = (net->getFinalCondition().getFormula() && ( *availableCost >= hiCosts ));
   // finally set the availble costs
   this->availableCost->setTokenCount(hiCosts+i);
}

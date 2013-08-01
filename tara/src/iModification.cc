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
#include "Tara.h"

// create the modification based on the net
iModification::iModification(pnapi::PetriNet* netToModify)
   : net(netToModify)
{
   // do the init modification
   // this->init();
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
   availableCost->setTokenCount(Tara::highestTransitionCosts+i);
        

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

   status("Initializing modification. Highest transition costs are: %d", Tara::highestTransitionCosts);
   status("max i is: %d", this->i);

   //create the place for the availble costs
   // tokens will be set later
   this->availableCost= &net->createPlace();


  
   //iterate over all transitions of the net
   std::set<pnapi::Transition*> allTransitions=net->getTransitions();
   for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();++it) {

      // the cost of that transition
      int curCost = Tara::cost(*it);

      unsigned int in = 0;
      unsigned int out = 0;
		
     in = Tara::highestTransitionCosts;
	 out = Tara::highestTransitionCosts - curCost;
     
	 //add arc from availableCost to that transition
	 if (in > 0) net->createArc(*availableCost, **it, in);
	
	 //add arc from transition to availableCost
	 if (out > 0) net->createArc(**it, *availableCost, out);

   }
   
   net->getFinalCondition() = (net->getFinalCondition().getFormula() && ( *availableCost >= Tara::highestTransitionCosts ));
   // finally set the availble costs
   this->availableCost->setTokenCount(Tara::highestTransitionCosts+i);
}

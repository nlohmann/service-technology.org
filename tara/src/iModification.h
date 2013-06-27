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

#ifndef I_MODIFICATION_H
#define I_MODIFICATION_H

#include <list>
#include <pnapi/pnapi.h>
#include "Modification.h"


// do the i-Modification with the a given petri net and an integer
// inital modification starts by creating an object
// iterative modification can be achieved by calling iterare
// changes cannot be undone
class iModification : public Modification {

   public:
      iModification(pnapi::PetriNet*);
      virtual void init();
      virtual unsigned int getI();
      virtual void setToValue(unsigned int);

      //TODO: these arent used ???
      void iterate();
      unsigned int decrease();
      unsigned int increase();
  
   private:

      void update();
    
      pnapi::PetriNet* net;
      // unsigned int i;


      pnapi::Place* availableCost;
      pnapi::Arc* outOfCreditArc;

};


#endif

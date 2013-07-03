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

#ifndef MODIFICATION_H
#define MODIFICATION_H

#include <list>
#include <pnapi/pnapi.h>
#include "verbose.h"


/**
 * abstract class
 */
class Modification {
   public:
      Modification() {}; 
      virtual ~Modification() {};

      virtual unsigned int getI() = 0;
      virtual void setToValue(unsigned int) = 0;
      virtual void init() = 0;

      void init(unsigned int newI) {
          i = newI;
          this->init();
      }
   protected:
      unsigned int i;
};


#endif

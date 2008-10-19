/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2007, 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,
                            Martin Znamirowski <znamirow@informatik.hu-berlin.de>
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef PETRINET_API_H
#define PETRINET_API_H

/// the Petri Net API is linked to BPEL2oWFN
#define USING_BPEL2OWFN


#include "uml2owfn.h"
#include "petrinet.h"			// to define member functions
#include "petrinet-formula.h" 	// <Dirk.F> soundness, proper termination predicate
#include "petrinet-workflow.h" 	// workflow specific extensions of Petri nets
#include "globals.h"


//#warning "using Petri Net API outside BPEL2oWFN"
//class ASTE; 			// forward declaration of class ASTE
//#ifndef PACKAGE_STRING
//#define PACKAGE_STRING "Petri Net API"



#endif

/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file ast-details.cc
 *
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/07/02
 *          - last changed: \$Date: 2006/07/05 15:42:43 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.5 $
 */

#include "ast-details.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <string>

using namespace std;


extern map<unsigned int, map<string, string> > temporaryAttributeMap;


ASTE::ASTE(kc::impl_abstract_phylum *mynode, int mytype)
{
  assert(mynode != NULL);

  switch (mytype)
  {
    case(K_ASSIGN):		id = ((kc::impl_tAssign*)mynode)->id->value; break;
    case(K_CASE):		id = ((kc::impl_tCase*)mynode)->id->value; break;
    case(K_CATCH):		id = ((kc::impl_tCatch*)mynode)->id->value; break;
    case(K_CATCHALL):		id = ((kc::impl_tCatchAll*)mynode)->id->value; break;
    case(K_COMPENSATE):		id = ((kc::impl_tCompensate*)mynode)->id->value; break;
    case(K_COMPENSATIONHANDLER):id = ((kc::impl_tCompensationHandler*)mynode)->id->value; break;
    case(K_EMPTY):		id = ((kc::impl_tEmpty*)mynode)->id->value; break;
    case(K_EVENTHANDLERS):	id = ((kc::impl_tEventHandlers*)mynode)->id->value; break;
    case(K_FAULTHANDLERS):	id = ((kc::impl_tFaultHandlers*)mynode)->id->value; break;
    case(K_FROM):		id = ((kc::impl_tFrom*)mynode)->id->value; break;
    case(K_FLOW):		id = ((kc::impl_tFlow*)mynode)->id->value; break;
    case(K_INVOKE):		id = ((kc::impl_tInvoke*)mynode)->id->value; break;
    case(K_LINK):		id = ((kc::impl_tLink*)mynode)->id->value; break;
    case(K_OTHERWISE):		id = ((kc::impl_tOtherwise*)mynode)->id->value; break;
    case(K_PICK):		id = ((kc::impl_tPick*)mynode)->id->value; break;
    case(K_PARTNERLINK):	id = ((kc::impl_tPartnerLink*)mynode)->id->value; break;
    case(K_PROCESS):		id = ((kc::impl_tProcess*)mynode)->id->value; break;
    case(K_RECEIVE):		id = ((kc::impl_tReceive*)mynode)->id->value; break;
    case(K_REPLY):		id = ((kc::impl_tReply*)mynode)->id->value; break;
    case(K_SCOPE):		id = ((kc::impl_tScope*)mynode)->id->value; break;
    case(K_SOURCE):		id = ((kc::impl_tSource*)mynode)->id->value; break;
    case(K_SEQUENCE):		id = ((kc::impl_tSequence*)mynode)->id->value; break;
    case(K_SWITCH):		id = ((kc::impl_tSwitch*)mynode)->id->value; break;
    case(K_TERMINATE):		id = ((kc::impl_tTerminate*)mynode)->id->value; break;
    case(K_TARGET):		id = ((kc::impl_tTarget*)mynode)->id->value; break;
    case(K_THROW):		id = ((kc::impl_tThrow*)mynode)->id->value; break;
    case(K_TO):			id = ((kc::impl_tTo*)mynode)->id->value; break;
    case(K_VARIABLE):		id = ((kc::impl_tVariable*)mynode)->id->value; break;
    case(K_WAIT):		id = ((kc::impl_tWait*)mynode)->id->value; break;
    case(K_WHILE):		id = ((kc::impl_tWhile*)mynode)->id->value; break;
    default:			id = 0;
  }

  assert(id != 0);

  node = mynode;
  type = mytype;
  attributes = temporaryAttributeMap[id];
  suppressJF = false; // required initialization!
  controlFlow = POSITIVECF;

//  cerr << mynode->op_name() << " (" << mynode->phylum_name() << ", id=" << id << ", ";
//  cerr << attributes.size() << " attributes" << ")" << endl;
}

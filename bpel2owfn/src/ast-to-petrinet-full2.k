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
\*****************************************************************************/

%{
/*!
 * \file bpel-unparse-petri.cc
 *
 * \brief Petri net unparser (implementation)
 *
 * This file defines and implements the conversion of each BPEL activity to its
 * associated Petri net pattern followed by neccessary merge operations.
 *
 * Patterns:
 *   - <empty>
 *   - <invoke>
 *   - <receive>
 *   - <reply>
 *   - <assign>
 *   - <wait>
 *   - <throw>
 *   - <compensate>
 *   - <terminate>
 *   - <flow>
 *   - <switch>
 *   - <while>
 *   - <sequence>
 *   - <pick>
 *   - <scope>
 *   - standard elements
 *   
 *   
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created 2005-11-01
 *          - last changed: \$Date: 2006/05/29 11:04:41 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.11 $
 */
%}


%{
/*!
 * \file bpel-unparse-petri.h
 * \brief Petri net unparser (interface)
 *
 * See \ref bpel-unparse-petri.cc for more information.
 */
%}




/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
/* These lines are from bpel-unparse-petri.k */
#include <iostream>
#include "petrinet.h"
#include "check-symbols.h"
#include "symbol-table.h"
#include "options.h"
#include "bpel-unparse-tools.h"
#include "ast-printers.h"

using namespace std;

// introduced in main.c
extern PetriNet *TheNet;

// introduced in bpel-syntax.y
extern SymbolTable symTab;

/// prefix of the current scope (read by the stop pattern and used to direct
/// failures to the correct fault_in place)
extern string currentScope;

%}




/******************************************************************************
  EMPTY
******************************************************************************/

/*
 * The <empty> activity as depicted in Fig. 3.
 */

This=Empty(a) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".internal.";
      header(This->id);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);


      // Transitions for the negative control flow.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  INVOKE
******************************************************************************/

/*
 * The (asynchronous) <invoke> activity as depicted in Fig. 10. Since
 * correlation sets are ignored, the resulting pattern is the same as in
 * Fig. 11.
 */

This=Invoke(a,b) provided ((dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id)))->isAsynchronousInvoke) -> //(This->outputVariable == mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".internal.";
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      header(This->id);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1); 
      TheNet->newArc(TheNet->findPlace("variable." + string(myself->inputVariable->name)), t1, READ);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      t2->communicating = true;


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, This->negativeControlFlow);

      Transition *t4 = stop(p1, "initial", prefix);
      Transition *t5 = stop(p2, "running", prefix);
      Transition *t6 = stop(p4, "p4", prefix);
      Transition *t7 = stop(p3, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/*
 * The (synchronous) <invoke> activity as depicted in Fig. 12. Since
 * correlation sets are ignored, the resulting pattern is the same as in
 * Fig. 13.
 */

This=Invoke(a,b) provided (!(dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id)))->isAsynchronousInvoke) -> //This->outputVariable != mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running1");
      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "running2");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "p6");
      Place *p7 = TheNet->newPlace(prefix + "stop");
      Place *p8 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(TheNet->findPlace("variable." + string(myself->inputVariable->name)), t1, READ);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->communicating = true;

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)), t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);
      t3->communicating = true;

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      TheNet->newArc(TheNet->findPlace("variable." + string(myself->outputVariable->name)), t4);
      TheNet->newArc(t4, TheNet->findPlace("variable." + string(myself->outputVariable->name)));


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Transition *t5 = throwFault(p4, p6, "running2", prefix, This->negativeControlFlow);
	Transition *t6 = throwFault(p2, p6, "running1", prefix, This->negativeControlFlow);
      }

      Transition *t7  = stop(p1, "initial", prefix);
      Transition *t8  = stop(p2, "running1", prefix);
      Transition *t9  = stop(p3, "p3", prefix);
      Transition *t10 = stop(p4, "running2", prefix);
      Transition *t11 = stop(p6, "p6", prefix);
      Transition *t12 = stop(p5, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

/*
 * The <receive> activity as depicted in Fig. 1. Since correlation sets are
 * ignored, the resulting pattern is the same as in Fig. 2.
 */

This=Receive(a,b) ->
  [petrinet:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)), t1);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->communicating = true;

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("variable." + string(myself->variable->name)), t2);
      TheNet->newArc(t2, TheNet->findPlace("variable." + string(myself->variable->name)));


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, This->negativeControlFlow);

      Transition *t4 = stop(p1, "initial", prefix);
      Transition *t5 = stop(p2, "running", prefix);
      Transition *t6 = stop(p4, "p4", prefix);
      Transition *t7 = stop(p3, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  REPLY
******************************************************************************/

/*
 * The <reply> activity as depicted in Fig. 8. Since correlation sets are
 * ignored, the resulting pattern is the same as in Fig. 9.
 */

This=Reply(a,b) ->
  [petrinet:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(TheNet->findPlace("variable." + string(myself->variable->name)), t1, READ);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->communicating = true;


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, This->negativeControlFlow);

      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t5 = stop(p2, "running", prefix);
      Transition *t6 = stop(p4, "p4", prefix);
      Transition *t7 = stop(p3, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  ASSIGN
******************************************************************************/

/* The <assign> activity -- a mixture of the patterns depicted in Fig. 6 and
 * Fig. 7. The patterns have been changed to fulfill the BPEL specification:
 *
 *    The assign activities are sufficiently short-lived that they are allowed
 *    to complete rather than being interrupted when termination is forced.
 *
 * Therefore the pattern now has only stop-transitions for the initial and
 * final place, as well as for all places after a fault has occured.
 */

This=Assign(a,b) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t4 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p3, "final", prefix);
    }
    a // standardElements
    {
      int copyCount = 1;
      Place *lastPlace = TheNet->findPlace(prefix + "initial");

      foreach (copyBlock=Copy(from,to); tCopy_list b)
      {
	string newPrefix = prefix + "copy." + intToString(copyCount) + ".";
	STFromTo* fromST = dynamic_cast<STFromTo*> (symTab.lookup(from->id));
	STFromTo* toST = dynamic_cast<STFromTo*> (symTab.lookup(to->id));

	if (fromST->variable != NULL) // mkcasestring(""))
	{
	  Place *p2 = TheNet->newPlace(newPrefix + "running");
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t1 = TheNet->newTransition(newPrefix + "t1");
	  TheNet->newArc(lastPlace, t1);
	  TheNet->newArc(TheNet->findPlace("variable." + string(fromST->variable->name)), t1, READ);
	  TheNet->newArc(t1, p2);

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	  TheNet->newArc(p2, t2);
	  TheNet->newArc(t2, p8);
	  TheNet->newArc(TheNet->findPlace("variable." + string(toST->variable->name)), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(toST->variable->name)));


	  // Transitions for the negative control flow.
	  if (parameters[P_NOSTANDARDFAULTS] == false)
	    Transition *t3 = throwFault(p2, p4, "running", newPrefix, This->negativeControlFlow);

	  Transition *t6 = stop(p4, "p4at" + intToString(copyCount), prefix);


	  lastPlace = p8;
	}
	else // if (from->literalValue != mkcasestring(""));
	{
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	  TheNet->newArc(lastPlace, t2);
	  TheNet->newArc(TheNet->findPlace("variable." + string(toST->variable->name)), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(toST->variable->name)));
	  TheNet->newArc(t2, p8);


	  // Transitions for the negative control flow.
	  if (parameters[P_NOSTANDARDFAULTS] == false)
	    Transition *t3 = throwFault(lastPlace, p4, "initial", newPrefix, This->negativeControlFlow);

	  Transition *t6 = stop(p4, "p4at" + intToString(copyCount), prefix);


	  lastPlace = p8;
	}
	copyCount++;
      }

      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), lastPlace);

      footer(This->id);
    }
  ]
;






/******************************************************************************
  WAIT
******************************************************************************/

/*
 * The <wait until=""> activity as depicted in Fig. 4.
 */

This=WaitUntil(a) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t1, READ);


      // Transitions for the negative control flow.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/*
 * The <wait for=""> activity as depicted in Fig. 5.
 */

This=WaitFor(a) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t1, READ);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t2, READ);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "TimeStamp", prefix);
      Transition *t5 = stop(p3, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  THROW
******************************************************************************/

/*
 * The <throw> activity as it is depicted in Fig. 14.
 */

This=Throw(a) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");


      // Transitions for the negative control flow.
      Transition *t1 = throwFault(p1, p2, "initial", prefix, This->negativeControlFlow);
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "p2", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  COMPENSATE
******************************************************************************/

/*
 * The compensate patterns for the four described scnearios of Fig. 38-41:
 *  - <compensate /> in a compensation handler
 *  - <compensate /> in a fault handler
 *  - <compensate scope="C"/> in a compensation handler
 *  - <compensate scope="C"/> in a fault handler
 */

/*
 * The <compensate/> activity as depicted in Fig. 38. As the context (embedded
 * in a compensation handler) is clear, the pattern is directly embedded to the
 * compensation handler of the current scope.
 */

This=Compensate(a) provided (!This->inFaultHandler && (symTab.readAttributeValue(This->id, "scope") == "")) -> //(!This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "comp");
      Place *p6 = TheNet->newPlace(prefix + "done");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate /> activity in compensation handler.
      TheNet->mergePlaces(p4, TheNet->findPlace(currentScope + "internal.compensationHandler.p10"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "internal.compensationHandler.p12"));
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/*
 * The <compensate/> activity as depicted in Fig. 39. As the context (embedded
 * in a fault handler) is clear, the pattern is directly embedded to the fault
 * handler of the current scope.
 */

This=Compensate(a) provided (This->inFaultHandler && (symTab.readAttributeValue(This->id, "scope") == "")) ->  //(This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *p6 = TheNet->newPlace(prefix + "ch_out");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate /> activity in fault handler.
      TheNet->mergePlaces(p5, TheNet->findPlace(currentScope + "faultHandler.ch_fh"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "faultHandler.ch_out"));
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/*
 * The <compensate scope="C"/> activity as depicted in Fig.40. As the context
 * (embedded in a compensation handler) is clear, the pattern is directly
 * embedded to the compensation handler of the current scope.
 */

This=Compensate(a) provided (!This->inFaultHandler && (symTab.readAttributeValue(This->id, "scope") != "")) -> //(!This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "compScope");
      Place *p6 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate scope="C"/> activity in compensation handler.
      TheNet->mergePlaces(p4, TheNet->findPlace(currentScope + "internal.compensationHandler.p10"));
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/*
 * The <compensate scope="C"/> activity as depicted in Fig.41. As the context
 * (embedded in a fault handler) is clear, the pattern is directly embedded to
 * the fault handler of the current scope.
 */

This=Compensate(a) provided (This->inFaultHandler && (symTab.readAttributeValue(This->id, "scope") != "")) -> //(This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_in");
      Place *p6 = TheNet->newPlace(prefix + "compensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate scope="C"/> activity in fault handler.
      TheNet->mergePlaces(p5, TheNet->findPlace(currentScope + "faultHandler.ch_fh"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "faultHandler.ch_out"));
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  TERMINATE
******************************************************************************/

/*
 * The <terminate> activity as it is depicted in Fig. 22. Note that places p4
 * and p5 are correctly correlated with "(!)Terminated" here.
 */

This=Terminate(a) ->
  [petrinet:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "!Terminated");
      Place *p5 = TheNet->newPlace(prefix + "Terminated");
      Place *p6 = TheNet->newPlace(prefix + "terminate");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(p5, t1, READ);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p6);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "final", prefix);


      // Embed the <terminate /> activity in the process.
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p4);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p5);
      TheNet->mergePlaces(TheNet->findPlace(currentScope + "upperTerminate"), p6);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;






/******************************************************************************
  FLOW
******************************************************************************/

/*
 * The <flow> activity as it is depicted in Fig. 17.
 */

This=Flow(a,b,c) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p7  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p9  = TheNet->newPlace(prefix + "running");
      Place *p15 = TheNet->newPlace(prefix + "stop");
      Place *p16 = TheNet->newPlace(prefix + "stopped");

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p7, t2);
      TheNet->newArc(t2, p9);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(t3, p8);
      TheNet->newArc(p9, t3);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p9, t5);
      TheNet->newArc(p15, t5);

      Transition *t6 = TheNet->newTransition(prefix + "innerStopped");
      TheNet->newArc(t6, p16);


      // Transitions for the negative control flow.
      Transition *t4 = stop(p7, "initial", prefix);
      Transition *t7 = stop(p8, "final", prefix);


      // Dead-Path-Elimination.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	Place *p1  = TheNet->newPlace(prefix + "negLink");
	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p1, t1);
      }
    }
    a // standardElements
    c // inner activities
    {
      foreach (innerActivity; activity_list c)
      {
        string newPrefix = intToString(innerActivity->id->value);
        // Embed the inner activity.
	if (This->dpe->value > 0 && TheNet->findPlace(newPrefix + ".negLink") != NULL)
	{
	  TheNet->newArc(t1, TheNet->findPlace(newPrefix + ".negLink"));
	}
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(t5, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".stopped"), t6);
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t3);
      }
      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SWITCH
******************************************************************************/

/*
 * The <switch> activity as it is depicted in Fig. 19. Please note that during
 * parsing an empty otherwise branch was added if it was not specified.
 */

This=activitySwitch(Switch(a,b,Otherwise(c))) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p4 =  TheNet->newPlace(prefix + "final");
      Place *p5 =  TheNet->newPlace(prefix + "initial");
      Place *p6 =  TheNet->newPlace(prefix + "running");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p5, t3);
      TheNet->newArc(t3, p6);


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t7 = throwFault(p6, p10, "running", prefix, This->negativeControlFlow);

      Transition *t8  = stop(p5, "initial", prefix);
      Transition *t9  = stop(p6, "running", prefix);
      Transition *t10 = stop(p10, "p10", prefix);


      // Dead Path Elimination
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	t1 = TheNet->newTransition(prefix + "t1");
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(p1, t1);
      }
    }
    a // standardElements
    b // case braches
    c // otherwise branch
    {
      int currentCase = 1;
      Transition *last;
      foreach (caseBranch=Case(innerActivity); tCase_list b)
      {
	string caseNumber = intToString(currentCase);

	trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding case branch " + caseNumber + "...\n");


	Transition *t2 = NULL;
	Transition *t4 = NULL;

	if (currentCase == 1)
	{
	  // two transitions: guard fulfilled or not
	  t2 = TheNet->newTransition(prefix + "t2.case1");
	  t4 = TheNet->newTransition(prefix + "t4.case1");

	  // arcs connecting "running"-place
	  TheNet->newArc(TheNet->findPlace(This, ".internal.running"), t2);
	  TheNet->newArc(TheNet->findPlace(This, ".internal.running"), t4);

	  last = t4;
	}
	else
	{
	  // two transitions: guard fulfilled or not
	  t2 = TheNet->newTransition(prefix + "t2.case" + caseNumber);
	  t4 = TheNet->newTransition(prefix + "t4.case" + caseNumber);

	  // "running"-place and connecting arcs
	  Place *running =  TheNet->newPlace(prefix + "running.case" + caseNumber);
	  TheNet->newArc(last, running);
	  TheNet->newArc(running, t2);
	  TheNet->newArc(running, t4);

	  Transition *t11 = stop(running, "runningCase" + caseNumber, prefix);

	  last = t4;
	}

	// Embed the inner activity.
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(This, ".internal.final",   innerActivity, ".final");
	TheNet->mergePlaces(This, ".internal.stop",    innerActivity, ".stop");
	TheNet->mergePlaces(This, ".internal.stopped", innerActivity, ".stopped");

	currentCase++;
      }


      trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding otherwise branch...\n");
      // The <otherwise> branch.
      Transition *t2 = TheNet->newTransition(prefix + "t2.otherwise");

      // "running"-place and connecting arcs
      Place *running = TheNet->newPlace(prefix + "running.otherwise");
      TheNet->newArc(running, t2);
      TheNet->newArc(last, running);

      Transition *tnoname = stop(running, "runningOtherwise", prefix);

      // embed inner activity
      TheNet->newArc(t2, TheNet->findPlace(c, ".initial"));
      TheNet->mergePlaces(This, ".internal.final",   c, ".final");
      TheNet->mergePlaces(This, ".internal.stop",    c, ".stop");
      TheNet->mergePlaces(This, ".internal.stopped", c, ".stopped");


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2-transition) has to be connected with
      // the negLink places of all other cases' activities as well as to the
      // activity in the otherwise branch. Of course, the arcs are only added
      // if the negLink place is existing (i.e. a source link can be reached
      // after all.
      currentCase = 1;
      foreach (thisCaseBranch=Case(innerActivity1); tCase_list b)
      {
	foreach (otherCaseBranch=Case(innerActivity2); tCase_list b)
	{
	  if ((innerActivity1 != innerActivity2) && (otherCaseBranch->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(innerActivity2, ".negLink"));

	  if (c->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(c, ".negLink"));
	}

	if (thisCaseBranch->dpe->value > 0)
	{
	  TheNet->newArc(TheNet->findTransition(prefix + "t2.otherwise"), TheNet->findPlace(innerActivity1, ".negLink"));
	}
	  

	currentCase++;

	if ((This->dpe->value > 0) && (thisCaseBranch->dpe->value > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

/*
 * The <while> activity as it is depicted in Fig. 18, but with two possible
 * scenarios:
 *
 *   - cyclic <while>: the standard case as it is depicted in Fig. 18.
 *   - acyclic <while>: a different case leading to an acyclic pattern.
 *
 * The scenario is selected by setting the parameter "-p cyclicwhile". For more
 * information refer to the comments below or the User's Manual.
 */

This=activityWhile(TheWhile=While(a,b)) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(TheWhile->id->value) + ".internal.";
      string condition = string(TheWhile->condition->name);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "p5");
      Place *p6 = TheNet->newPlace(prefix + "p6");
      Place *p7 = TheNet->newPlace(prefix + "stop");
      Place *p8 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p2, t1);
      TheNet->newArc(t1, p3);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p2, t3);
      TheNet->newArc(t3, p4);


      // When the parameter "-p cyclicwhile" is set, the pattern is created as
      // it is depicted in Fig. 18. Setting this option might lead to not
      // 1-safe Petri nets as links embedded in the inner activity might be
      // initialized more than once.
      if (parameters[P_CYCLICWHILE] == true)
      {
	Transition *t4 = TheNet->newTransition(prefix + "t4");
	TheNet->newArc(t4, p1);
	TheNet->newArc(p5, t4);
      }


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t5 = throwFault(p2, p6, "running", prefix, This->negativeControlFlow);

      Transition *t6 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p2, "running", prefix);
      Transition *t8 = stop(p6, "p6", prefix);
      Transition *t9 = stop(p3, "final", prefix);
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(This, ".internal.p4",      b, ".initial");
      TheNet->mergePlaces(This, ".internal.p5",      b, ".final");
      TheNet->mergePlaces(This, ".internal.stop",    b, ".stop");
      TheNet->mergePlaces(This, ".internal.stopped", b, ".stopped");


      // When the parameter "-p cyclicwhile" is not set, the created pattern
      // differs from the original pattern depicted in Fig. 18: instead of
      // executing the inner activity each time the given condition holds, it
      // is executed at most once to avoid problems with links embedded in the
      // inner activity.
      if (parameters[P_CYCLICWHILE] == false)
	TheNet->mergePlaces(This, ".internal.p5", This, ".internal.final");

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

/*
 * The <sequence> activity as it is depicted in Fig. 16.
 */

This=activitySequence(Sequence(a,b)) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "stopped");


      // If needed, transition t1 and a place negLink is created.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	Place *p1  = TheNet->newPlace(prefix + "negLink");
  	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p1, t1);
      }

    }
    a // standardElements for this activity
    b // inner activities
    {
      int activityCount = 1;
      kc::impl_activity* lastActivity;

      foreach (innerActivity; activity_list b)
      {
	if (activityCount == 1)
	  TheNet->mergePlaces(This, ".internal.initial", innerActivity, ".initial");
	else
	  TheNet->mergePlaces(lastActivity, ".final", innerActivity, ".initial");

	if (activityCount == b->length())
	  TheNet->mergePlaces(This, ".internal.final", innerActivity, ".final");

	// If an inner activity has a place negLink, it is connected to the transition
	// t1.
        string newPrefix = intToString(innerActivity->id->value);
	if (This->dpe->value > 0 && TheNet->findPlace(newPrefix + ".negLink") != NULL)
	  TheNet->newArc(t1, TheNet->findPlace(newPrefix + ".negLink"));


	TheNet->mergePlaces(This, ".internal.stop", innerActivity, ".stop");
	TheNet->mergePlaces(This, ".internal.stopped", innerActivity, ".stopped");

	lastActivity = innerActivity;
	activityCount++;
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  PICK
******************************************************************************/

/*
 * The <pick> activity as it is depicted in Fig. 20.
 */

TheActivity=activityPick(This=Pick(a,b,c)) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");


      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ);
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7);


      // If needed, transition t1 and a place negLink is created.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	t1 = TheNet->newTransition(prefix + "t1");
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(p1, t1);
      }


      // Transitions for the negative control flow.
      Transition *t7 = stop(p6, "initial", prefix);
      Transition *t8 = stop(p7, "TimeStamp", prefix);
    }
    a // standardElements
    b // onMessage branches
    c // onAlarm branches
    {
      // Traverse the <onMessage> branches.
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list b)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onMessage>\n");
	string message = intToString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Place *p3 = TheNet->newPlace(newPrefix + "p3");
	Place *p10 = TheNet->newPlace(newPrefix + "p10");

	Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->channelId)), t2);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".internal.TimeStamp"), t2);
	TheNet->newArc(t2, p3);
	t2->communicating = true;

	Transition *t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->variable->name)), t3);
  	TheNet->newArc(t3, TheNet->findPlace("variable." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->variable->name)));
	TheNet->newArc(p3, t3);
	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));


	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t4 = throwFault(p3, p10, "p3", newPrefix, This->negativeControlFlow);

        Transition *t9  = stop(p3, "p3at" + message, prefix);
	Transition *t10 = stop(p10, "p10at" + message, prefix);


	// Embed the inner activity.
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
      }


      // Traverse the <onAlarm> branches.
      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount++) + ".";

	Transition *t6 = TheNet->newTransition(newPrefix + "t6");
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), t6, READ);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".internal.TimeStamp"), t6);
	TheNet->newArc(t6, TheNet->findPlace(innerActivity, ".initial"));


	// Embed the inner activity.
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
      }


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2-transition) has to be connected with
      // the negLink places of all other <onAlarm> or <onMessage> activities.
      // Of course, the arcs are only added if the negLink place is existing
      // (i.e. a source link can be reached after all.
      int currentMessage = 1;
      foreach (thisonmessage=OnMessage(innerActivity1); tOnMessage_list b)
      {
	foreach (otheronmessage=OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if ((innerActivity1 != innerActivity2) && (otheronmessage->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (thisonalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if (thisonalarm->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentMessage++;

	if ((thisonmessage->dpe->value > 0) && (This->dpe->value > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }

      
      int currentAlarm = 1;
      foreach (thisonalarm=OnAlarm(innerActivity1); tOnAlarm_list c)
      {
	foreach (otheronalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if ((innerActivity1 != innerActivity2) && (otheronalarm->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (thisonmessage=OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if (thisonmessage->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentAlarm++;

	if ((thisonalarm->dpe->value > 0) && (This->dpe->value > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SCOPE
******************************************************************************/

/*
 * The <scope> activity as it is depicted in Fig. 27. Note that it contains
 * only one event handler since alarm-event handler and message-event handler
 * are merged.
 */

This=Scope(a,b,c,d,e,f,g) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;
      STScope* myself = dynamic_cast<STScope*> (symTab.lookup(This->id));

      // Create push-places for the children of the scope.
      for (list< STScope * >::iterator child = myself->childScopes.begin(); child != myself->childScopes.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->entryKey) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->entryKey) + ".!push");
      }


      // Create the internal places of the scope pattern.
      if (This->dpe->value > 0)
      {
	Place *p1  = TheNet->newPlace(prefix + "negLink");
      }
      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p3  = TheNet->newPlace(prefix + "ch_in");
      Place *p4  = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "upperTerminate");
      Place *p8  = TheNet->newPlace(prefix + "!Terminated");
      Place *p9  = TheNet->newPlace(prefix + "Terminated");
      Place *p10 = TheNet->newPlace(prefix + "Active");
      Place *p11 = TheNet->newPlace(prefix + "!Completed");
      Place *p12 = TheNet->newPlace(prefix + "Completed");
      Place *p13 = TheNet->newPlace(prefix + "!Compensated");
      Place *p14 = TheNet->newPlace(prefix + "!Active");
      Place *p15 = TheNet->newPlace(prefix + "Compensated");
      Place *p16 = TheNet->newPlace(prefix + "!Ended");
      Place *p17 = TheNet->newPlace(prefix + "Ended");
      Place *p18 = TheNet->newPlace(prefix + "!Faulted");
      Place *p19 = TheNet->newPlace(prefix + "Faulted");
      Place *p20 = TheNet->newPlace(prefix + "stop");
      Place *p21 = TheNet->newPlace(prefix + "p21");
      Place *p22 = TheNet->newPlace(prefix + "p22");
      Place *p23 = TheNet->newPlace(prefix + "p23");
      Place *p24 = TheNet->newPlace(prefix + "p24");
      Place *p35 = TheNet->newPlace(prefix + "p35");
      Place *p36 = TheNet->newPlace(prefix + "p36");
      Place *p37 = TheNet->newPlace(prefix + "p37");
      Place *p38 = TheNet->newPlace(prefix + "p38");
      Place *p39 = TheNet->newPlace(prefix + "p39");
      Place *p40 = TheNet->newPlace(prefix + "p40");
      Place *p41 = TheNet->newPlace(prefix + "p41");
      Place *p42 = TheNet->newPlace(prefix + "p42");
      Place *p43 = TheNet->newPlace(prefix + "p43");
      Place *p44 = TheNet->newPlace(prefix + "p44");
      Place *p45 = TheNet->newPlace(prefix + "stopped");
      Place *p46 = TheNet->newPlace(prefix + "p46");
      Place *p47 = TheNet->newPlace(prefix + "p47");
      Place *p48 = TheNet->newPlace(prefix + "p48");
      Place *p49 = TheNet->newPlace(prefix + "p49");
      Place *p50 = TheNet->newPlace(prefix + "p50");
      Place *p51 = TheNet->newPlace(prefix + "final");
      Place *p52 = TheNet->newPlace(prefix + "p52");
      Place *p53 = TheNet->newPlace(prefix + "p53");
      Place *p54 = TheNet->newPlace(prefix + "p54");
      Place *p56 = TheNet->newPlace(prefix + "compensated");


      // State places of the scope modeling the new fault model: The places
      // guarantee that the occurrence of more than one fault inside the fault
      // or compensation handler is prevented. The places are used as guards
      // by the transitions signalling faults.
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");


      // The transitions of the process pattern.
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p13);
      TheNet->newArc(t2, p16);
      TheNet->newArc(t2, p18);
      TheNet->newArc(t2, p21);
      TheNet->newArc(t2, fh_not_faulted);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p10, t4);
      TheNet->newArc(p11, t4);
      TheNet->newArc(t4, p12);
      TheNet->newArc(t4, p14);
      TheNet->newArc(TheNet->findPlace(intToString(This->id->value) + ".!push"), t4);
      TheNet->newArc(t4, TheNet->findPlace(intToString(This->id->value) + ".push"));
      TheNet->newArc(p35, t4);
      TheNet->newArc(t4, p51);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p8, t7, READ);
      TheNet->newArc(p10, t7);
      TheNet->newArc(t7, p14);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p36);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p9, t8, READ);
      TheNet->newArc(p10, t8);
      TheNet->newArc(t8, p14);
      TheNet->newArc(p20, t8);
      TheNet->newArc(t8, p37);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(p53, t16);


      // Transitions for the negative control flow.
      Transition *t9  = stop(p2, "initial", prefix);
      Transition *t10 = stop(p17, "Ended", prefix);
      TheNet->newArc(t10, p17); // p17 is only read
      Transition *t11 = stop(p51, "final", prefix);


      // If the process has an event handler several places and transitions
      // have to be added to embed it. If the process lacks an event handler
      // some places can be merged.
      if (myself->hasEventHandler)
      {
	Place *p25 = TheNet->newPlace(prefix + "p25");
	Place *p26 = TheNet->newPlace(prefix + "p26");
	Place *p27 = TheNet->newPlace(prefix + "p27");
	Place *p28 = TheNet->newPlace(prefix + "p28");
	Place *p29 = TheNet->newPlace(prefix + "p29");

  	TheNet->newArc(t2, p25);
	TheNet->newArc(p27, t4);

      	Transition *t1 = TheNet->newTransition(prefix + "t1");
    	TheNet->newArc(p29, t1);
  	TheNet->newArc(t1, p38);

	Transition *t3 = TheNet->newTransition(prefix + "t3");
      	TheNet->newArc(p22, t3);
    	TheNet->newArc(t3, p26);
  	TheNet->newArc(t3, p35);

	Transition *t5 = TheNet->newTransition(prefix + "t5");
	TheNet->newArc(p24, t5);
	TheNet->newArc(t5, p28);

	Transition *t6 = TheNet->newTransition(prefix + "t6");
	TheNet->newArc(p23, t6);
	TheNet->newArc(t6, p28);
	TheNet->newArc(p35, t6);
      }
      else
      {
	TheNet->mergePlaces(p22, p35);
	TheNet->mergePlaces(p24, p38);
      }

      // New transitions to pass fault to parent.
      Transition *t12 = TheNet->newTransition(prefix + "passFaultUp");
      TheNet->newArc(p48, t12);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.Active"), t12);
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active")); //newer fault management
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.stop.fault_in"));

      Transition *t13 = TheNet->newTransition(prefix + "ignoreFault");
      TheNet->newArc(p48, t13);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active"), t13, READ);


      // Merge places with parent scope.
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) +
	    ".internal.compensationHandler.compScope." + intToString(This->id->value)), p3); // was "....internal.compScope."
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.scopeCompensated"), p56);
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.upperTerminate"), p46);
    }
    f // Stop Pattern
    a // standardElements
    c // Fault Handler
    d // Compensation Handler
    e // Event Handler
    g // innerActivity
    {
      // After the patterns of the inner activity of the process are generated
      // the push places of the child scopes are available and have to be
      // connected with the transition t2 of the process.
      for (list<STScope*>::iterator child = myself->childScopes.begin(); child != myself->childScopes.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString((*child)->entryKey) + ".!push" ));

      string prefix2 = intToString(This->id->value) + ".internal.";


      // Embed the fault handler of the scope.
      TheNet->mergePlaces(prefix + "p43",     prefix2 + "faultHandler.fault");
      TheNet->mergePlaces(prefix + "p44",     prefix2 + "faultHandler.faultSave");
      TheNet->mergePlaces(prefix + "p47",     prefix2 + "faultHandler.rethrow");
      TheNet->mergePlaces(prefix + "p41",     prefix2 + "faultHandler.ft");
      TheNet->mergePlaces(prefix + "p42",     prefix2 + "faultHandler.ftSave");
      TheNet->mergePlaces(prefix + "!Ended",  prefix2 + "faultHandler.!Ended");
      TheNet->mergePlaces(prefix + "Ended",   prefix2 + "faultHandler.Ended");
      TheNet->mergePlaces(prefix + "p49",     prefix2 + "faultHandler.ch_fh");
      TheNet->mergePlaces(prefix + "p50",     prefix2 + "faultHandler.ch_out");
      TheNet->mergePlaces(prefix + "final",   prefix2 + "faultHandler.out");
      TheNet->mergePlaces(prefix + "p52",     prefix2 + "faultHandler.trueOut");
      TheNet->mergePlaces(prefix + "p53",     prefix2 + "faultHandler.sourceFalse");
      TheNet->mergePlaces(prefix + "p54",     prefix2 + "faultHandler.falseOut");
      TheNet->mergePlaces(prefix + "p48",     prefix2 + "faultHandler.upperFH");
      TheNet->mergePlaces(prefix + "stopped", prefix2 + "faultHandler.stopped");


      // Embed the compensation handler of the scope.
      TheNet->mergePlaces(prefix + "ch_in",            prefix2 + "compensationHandler.ch_in");
      TheNet->mergePlaces(prefix + "Completed",        prefix2 + "compensationHandler.Completed");
      TheNet->mergePlaces(prefix + "!Completed",       prefix2 + "compensationHandler.!Completed");
      TheNet->mergePlaces(prefix + "Compensated",      prefix2 + "compensationHandler.Compensated");
      TheNet->mergePlaces(prefix + "!Compensated",     prefix2 + "compensationHandler.!Compensated");
      TheNet->mergePlaces(prefix + "p49",              prefix2 + "compensationHandler.fh");
      TheNet->mergePlaces(prefix + "scopeCompensated", prefix2 + "compensationHandler.scopeCompensated");
      TheNet->mergePlaces(prefix + "p39",              prefix2 + "compensationHandler.clean");
      TheNet->mergePlaces(prefix + "p40",              prefix2 + "compensationHandler.cleaned");
      TheNet->mergePlaces(prefix + "p50",              prefix2 + "compensationHandler.ch_out");
      TheNet->mergePlaces(prefix + "compensated",      prefix2 + "compensationHandler.compensated");


      // If present, embed the event handler of the scope.
      if (myself->hasEventHandler)
      {
	TheNet->mergePlaces(prefix + "p25", prefix2 + "eventHandler.initial");
	TheNet->mergePlaces(prefix + "p26", prefix2 + "eventHandler.finish");
	TheNet->mergePlaces(prefix + "p27", prefix2 + "eventHandler.final");
	TheNet->mergePlaces(prefix + "p28", prefix2 + "eventHandler.stop");
	TheNet->mergePlaces(prefix + "p29", prefix2 + "eventHandler.stopped");
      }


      // Embed the stop-pattern of the scope.
      TheNet->mergePlaces(prefix + "p36",            prefix + "stop.ft_in");
      TheNet->mergePlaces(prefix + "Faulted",        prefix + "stop.Faulted");
      TheNet->mergePlaces(prefix + "p6",             prefix + "stop.fault_in");
      TheNet->mergePlaces(prefix + "Active",         prefix + "stop.Active");
      TheNet->mergePlaces(prefix + "p37",            prefix + "stop.terminate_up");
      TheNet->mergePlaces(prefix + "!Active",        prefix + "stop.!Active");
      TheNet->mergePlaces(prefix + "upperTerminate", prefix + "stop.terminate");
      TheNet->mergePlaces(prefix + "!Faulted",       prefix + "stop.!Faulted");
      TheNet->mergePlaces(prefix + "Ended",          prefix + "stop.Ended");
      TheNet->mergePlaces(prefix + "Compensated",    prefix + "stop.Compensated");
      TheNet->mergePlaces(prefix + "!Ended",         prefix + "stop.!Ended");
      TheNet->mergePlaces(prefix + "p23",            prefix + "stop.stop");
      TheNet->mergePlaces(prefix + "p38",            prefix + "stop.stopped");
      TheNet->mergePlaces(prefix + "p39",            prefix + "stop.cleanCH");
      TheNet->mergePlaces(prefix + "p40",            prefix + "stop.ch_cleaned");
      TheNet->mergePlaces(prefix + "p41",            prefix + "stop.ft");
      TheNet->mergePlaces(prefix + "p42",            prefix + "stop.ftSave");
      TheNet->mergePlaces(prefix + "p43",            prefix + "stop.fault");
      TheNet->mergePlaces(prefix + "p44",            prefix + "stop.faultSave");
      TheNet->mergePlaces(prefix + "stopped",        prefix + "stop.stopped_s");
      TheNet->mergePlaces(prefix + "p46",            prefix + "stop.upperTerminate");
      TheNet->mergePlaces(prefix + "p47",            prefix + "stop.rethrow");
      TheNet->mergePlaces(prefix + "p48",            prefix + "stop.upperFH");

      TheNet->mergePlaces(prefix + "FHFaulted",      prefix + "stop.FHFaulted");
      TheNet->mergePlaces(prefix + "!FHFaulted",     prefix + "stop.!FHFaulted");
      TheNet->mergePlaces(prefix + "CHFaulted",      prefix + "stop.CHFaulted");
      TheNet->mergePlaces(prefix + "!CHFaulted",     prefix + "stop.!CHFaulted");



      // Embed the inner activity of the scope.
      if (This->dpe->value > 0)
      {
        TheNet->mergePlaces(TheNet->findPlace(prefix + "negLink"), TheNet->findPlace(g, ".negLink"));
      }
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(g, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(g, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(g, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(g, ".stopped"));


      // Merge (!)Terminated places
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p8);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p9);


      // Setting all enclosed links to false in case of an error.
      for (list<STLink *>::iterator link = myself->enclosedLinks.begin(); link != myself->enclosedLinks.end(); link++)
      {
	Place *p61 = TheNet->newPlace(prefix + "p61.link." + (*link)->name);
	Place *p62 = TheNet->findPlace("link." + (*link)->name);
	Place *p63 = TheNet->findPlace("!link." + (*link)->name);
	Transition *t14 = TheNet->newTransition(prefix + "t14.link." + (*link)->name);
	Transition *t15 = TheNet->newTransition(prefix + "t15.link." + (*link)->name);
	TheNet->newArc(p61, t14);
	TheNet->newArc(t14, p62);
	TheNet->newArc(p63, t14);
	TheNet->newArc(p61, t15);
	TheNet->newArc(t16, p61);
	TheNet->newArc(p62, t15, READ);
      }

      currentScope = intToString(This->parentScopeId->value) + ".internal.";
      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");


      // If the inner activity has a place negLink, connect merge this place
      // with a new created one.
      if (This->dpe->value > 0)
      {
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->mergePlaces(p1, TheNet->findPlace(newPrefix + ".negLink"));
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p3, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p4, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p5, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 22: Pattern for an activity that is source of links.
 *****************************************************************************/

This=StandardElements(NiltTarget_list(), a, jc) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p2 = TheNet->newPlace(prefix + "initial");
//      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "stop");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p5);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p5);


      // Transition for the negative control flow.
      Transition *t4 = stop(p5, "final", prefix);


      // If needed, transition t1 and the place p1 is created and connected
      // with the negLink place of the inner activity if existing for dead path
      // elimination.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
        Place *p1 = TheNet->newPlace(prefix + "negLink");
	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p1, t1);

	Place *p3 = TheNet->findPlace(newPrefix + ".negLink");
	if (p3 != NULL)
	{
	  TheNet->newArc(t1, p3);
	  TheNet->mergePlaces(p3, TheNet->findPlace(newPrefix + ".negLink"));
	}
      }


      // Connect all source link places with t2 and, if needed, with transition
      // t1 for dead path elimination.
      foreach (link; tSource_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p8 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p10 = TheNet->findPlace("link." + string(linkST->name));

	TheNet->newArc(p8, t2);
	TheNet->newArc(t2, p10);
	TheNet->newArc(p10, t3, READ);

	if (This->dpe->value > 0)
	{
	  TheNet->newArc(t1, p10);
	  TheNet->newArc(p8, t1);
	}
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p4, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p6, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p7, TheNet->findPlace(newPrefix + ".stopped"));
   }
  ]
;





/******************************************************************************
 * Figure 23: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "no") ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 23\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");
      Place *p11 = TheNet->newPlace(prefix + "p11");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1);


      // Transitions for the negative control flow.
      Transition *t2 = throwFault(p8, p11, "initial", prefix, This->negativeControlFlow);

      Transition *t3 = stop(p8, "initial", prefix);
      Transition *t4 = stop(p11, "p11", prefix);


      // Connect the target links with transitions t1 and t2.
      foreach (link; tTarget_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p1 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p3 = TheNet->findPlace("link." + string(linkST->name));
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1);
	TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = TheNet->findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(t2, p5a);
	TheNet->mergePlaces(p5a, p5);
      }

      
      // Embed the inner activity.
      TheNet->mergePlaces(p6,  TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p7,  TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p9,  TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 24: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "yes") ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p7);


      // Transition for the negative control flow.
      Transition *t3 = stop(p8, "initial", prefix);


      // Connect the target links with transitions t1 and t2.
      foreach (link; tTarget_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p1 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p3 = TheNet->findPlace("link." + string(linkST->name));
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1);
	TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = TheNet->findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(t2, p5);
	TheNet->mergePlaces(p5a, p5);
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p6, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p7, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p9, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 25: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "yes") ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 25\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(t5, p13);
      TheNet->newArc(p16, t5);


      // Transitions for the negative control flow.
      Transition *t6 = stop(p16, "initial", prefix);
      Transition *t7 = stop(p13, "final", prefix);


      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	Place *p9  = TheNet->newPlace(prefix + "negLink");

	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p9, t1);

	Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  TheNet->newArc(t1, p10);
      }


      // Connect each target links with t2 and t5.
      foreach (link; tTarget_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p1 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p3 = TheNet->findPlace("link." + string(linkST->name));
        TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
        TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (link; tSource_list b)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p5 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p7 = TheNet->findPlace("link." + string(linkST->name));

        TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7);
        TheNet->newArc(p5, t5);
        TheNet->newArc(t5, p7);
	TheNet->newArc(p7, t4, READ);
	if (This->dpe->value > 0)
	{
	  TheNet->newArc(p5, t1);
	  TheNet->newArc(t1, p7);
	}
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	TheNet->newArc(t5, p10);
	if (This->dpe->value > 0)
	  TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".negLink"));
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p11, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p12, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p14, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p15, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;






/******************************************************************************
 * Figure 26: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "no") ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");
      Place *p17 = TheNet->newPlace(prefix + "p17");

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);


      // Transitions for the negative control flow.
      Transition *t5 = throwFault(p16, p17, "initial", prefix, This->negativeControlFlow);

      Transition *t6 = stop(p16, "initial", prefix);
      Transition *t7 = stop(p17, "p17", prefix);
      Transition *t8 = stop(p13, "final", prefix);


      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
	Place *p9  = TheNet->newPlace(prefix + "negLink");

	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p9, t1);

	Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  TheNet->newArc(t1, p10);
      }


      // Connect each target links with t2 and t5.
      foreach (link; tTarget_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p1 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p3 = TheNet->findPlace("link." + string(linkST->name));
        TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
	TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (link; tSource_list b)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p5 = TheNet->findPlace("!link." + string(linkST->name));
	Place *p7 = TheNet->findPlace("link." + string(linkST->name));

	TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7);
	TheNet->newArc(p7, t4, READ);

	if (This->dpe->value > 0)
	{
	  TheNet->newArc(p5, t1);
	  TheNet->newArc(t1, p7);
	}
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	TheNet->newArc(t5, p10);
	if (This->dpe->value > 0)
	  TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".negLink"));
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p11, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p12, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p14, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p15, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;

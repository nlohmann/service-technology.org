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
 * \file ast-to-petrinet-new2.cc
 *
 * \brief Petri net unparser (implementation)
 *
 * This file defines and implements the conversion of each BPEL activity to its
 * associated Petri net pattern followed by neccessary merge operations.
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created 2006-10-30
 *          - last changed: \$Date: 2006/10/30 12:44:10 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.1 $
 */
%}





/******************************************************************************
  EMPTY
******************************************************************************/

/*
 * The <empty> activity as depicted in Fig. 3.
 */

This=Empty(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=AsynchronousInvoke(a,b,_id) -> // provided (!This->isSynchronousInvoke) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1); 
      TheNet->newArc(t1, p2);
      if (!parameters[P_NOVARIABLES])
        TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[_id->value]->inputVariableName), t1, READ);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      t2->type = OUT;

      if (ASTEmap[_id->value]->inWhile) // was: (parameters[P_CYCLICWHILE] && $0->inWhile)
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

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

This=SynchronousInvoke(a,b,_id) -> //provided (This->isSynchronousInvoke) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
      TheNet->newArc(t1, p2);
      if (!parameters[P_NOVARIABLES])
        TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[_id->value]->inputVariableName), t1, READ);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->type = OUT;

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(TheNet->findPlace("in." + ASTEmap[_id->value]->channelName), t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);
      t3->type = IN;

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      if (!parameters[P_NOVARIABLES])
      {
        TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[_id->value]->outputVariableName), t4);
        TheNet->newArc(t4, TheNet->findPlace("variable." + ASTEmap[_id->value]->outputVariableName));
      }

      if (ASTEmap[_id->value]->inWhile)
      {
        TheNet->findPlace("in." + ASTEmap[_id->value]->channelName)->inWhile = true;
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;
      }

      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Transition *t5 = throwFault(p4, p6, "running2", prefix, _id, ASTEmap[_id->value]->controlFlow);
	Transition *t6 = throwFault(p2, p6, "running1", prefix, _id, ASTEmap[_id->value]->controlFlow);
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

This=Receive(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(TheNet->findPlace("in." + ASTEmap[_id->value]->channelName), t1);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = IN;

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      if (!parameters[P_NOVARIABLES])
      {
        TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[_id->value]->variableName), t2);
        TheNet->newArc(t2, TheNet->findPlace("variable." + ASTEmap[_id->value]->variableName));
      }

      if (ASTEmap[_id->value]->inWhile)
        TheNet->findPlace("in." + ASTEmap[_id->value]->channelName)->inWhile = true;


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

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

This=Reply(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      if (!parameters[P_NOVARIABLES])
        TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[_id->value]->variableName), t1, READ);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->type = OUT;

      if (ASTEmap[_id->value]->inWhile) // was: (parameters[P_CYCLICWHILE] && $0->inWhile)
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t3 = throwFault(p2, p4, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

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

This=Assign(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
	string newPrefix = prefix + "copy." + toString(copyCount) + ".";

	if (ASTEmap[from->id]->variableName != "")
	{
	  Place *p2 = TheNet->newPlace(newPrefix + "running");
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t1 = TheNet->newTransition(newPrefix + "t1");
	  TheNet->newArc(lastPlace, t1);
	  TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[from->id]->variableName), t1, READ);
	  TheNet->newArc(t1, p2);

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	  TheNet->newArc(p2, t2);
	  TheNet->newArc(t2, p8);
	  TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + ASTEmap[to->id]->variableName));


	  // Transitions for the negative control flow.
	  if (parameters[P_NOSTANDARDFAULTS] == false)
	    Transition *t3 = throwFault(p2, p4, "running", newPrefix, _id, ASTEmap[_id->value]->controlFlow);

	  Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);


	  lastPlace = p8;
	}
	else
	{
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	  TheNet->newArc(lastPlace, t2);
	  TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + ASTEmap[to->id]->variableName));
	  TheNet->newArc(t2, p8);


	  // Transitions for the negative control flow.
	  if (parameters[P_NOSTANDARDFAULTS] == false)
	    Transition *t3 = throwFault(lastPlace, p4, "initial", newPrefix, _id, ASTEmap[_id->value]->controlFlow);

	  Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);


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
  VALIDATE                                                       (WS-BPEL 2.0)
******************************************************************************/

This=Validate(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
        Transition *t3 = throwFault(p2, p4, "running", prefix, _id, ASTEmap[This->id]->controlFlow); // was: This->negativeControlFlow);

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
  WAIT
******************************************************************************/

/*
 * The <wait until=""> activity as depicted in Fig. 4.
 */

This=WaitUntil(a,_id) -> // provided (!This->isWaitFor)->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=WaitFor(a,_id) -> // provided (This->isWaitFor)->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=Throw(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");


      // Transitions for the negative control flow.
      Transition *t1 = throwFault(p1, p2, "initial", prefix, _id, ASTEmap[_id->value]->controlFlow);
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

This=Compensate(a,_id) provided (ASTEmap[_id->value]->controlFlow == CHCF) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=Compensate(a,_id) provided (ASTEmap[_id->value]->controlFlow == FHCF) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
 * The <compensate scope="C"/> activity as depicted in Fig. 40. As the context
 * (embedded in a compensation handler) is clear, the pattern is directly
 * embedded to the compensation handler of the current scope.
 */

This=CompensateScope(a,b,_id) provided (ASTEmap[_id->value]->controlFlow == CHCF) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=CompensateScope(a,b,_id) provided (ASTEmap[_id->value]->controlFlow == FHCF) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

/*
 * The <terminate> activity as it is depicted in Fig. 22. Note that places p4
 * and p5 are correctly correlated with "(!)Terminated" here.
 */

This=Terminate(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
  EXIT                                                           (BPEL4WS 1.1)
******************************************************************************/

/*
 * The <exit> activity.
 */

This=Exit(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

This=Flow(a,b,c,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

/*
      // Dead-Path-Elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	Place *p1  = TheNet->newPlace(prefix + "negLink");
	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p1, t1);
      }*/
    }
    a // standardElements
    c // inner activities
    {
      foreach (innerActivity; activity_list c)
      {
        string newPrefix = toString(innerActivity->id);
        // Embed the inner activity.
//	if (This->dpe > 0 && TheNet->findPlace(newPrefix + ".negLink") != NULL)
//	{
//	  TheNet->newArc(t1, TheNet->findPlace(newPrefix + ".negLink"));
//	}
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
  SWITCH                                                         (BPEL4WS 1.1)
******************************************************************************/

/*
 * The <switch> activity as it is depicted in Fig. 19. Please note that during
 * parsing an empty otherwise branch was added if it was not specified.
 */

This=Switch(a,b,Otherwise(c,d),_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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
	Transition *t7 = throwFault(p6, p10, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

      Transition *t8  = stop(p5, "initial", prefix);
      Transition *t9  = stop(p6, "running", prefix);
      Transition *t10 = stop(p10, "p10", prefix);

/*
      // Dead Path Elimination
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	t1 = TheNet->newTransition(prefix + "t1");
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(p1, t1);
      }*/
    }
    a // standardElements
    b // case braches
    c // otherwise branch
    {
      int currentCase = 1;
      Transition *last;
      foreach (caseBranch=Case(innerActivity); tCase_list b)
      {
	string caseNumber = toString(currentCase);

	trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding case branch " + caseNumber + "...\n");


	Transition *t2 = NULL;
	Transition *t4 = NULL;

	if (currentCase == 1)
	{
	  // two transitions: guard fulfilled or not
	  t2 = TheNet->newTransition(prefix + "t2.case1");
	  t4 = TheNet->newTransition(prefix + "t4.case1");

	  // arcs connecting "running"-place
	  TheNet->newArc(TheNet->findPlace(This->id, ".internal.running"), t2);
	  TheNet->newArc(TheNet->findPlace(This->id, ".internal.running"), t4);

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
	TheNet->newArc(t2, TheNet->findPlace(innerActivity->id, ".initial"));
	TheNet->mergePlaces(This->id, ".internal.final",   innerActivity->id, ".final");
	TheNet->mergePlaces(This->id, ".internal.stop",    innerActivity->id, ".stop");
	TheNet->mergePlaces(This->id, ".internal.stopped", innerActivity->id, ".stopped");

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
      TheNet->newArc(t2, TheNet->findPlace(c->id, ".initial"));
      TheNet->mergePlaces(This->id, ".internal.final",   c->id, ".final");
      TheNet->mergePlaces(This->id, ".internal.stop",    c->id, ".stop");
      TheNet->mergePlaces(This->id, ".internal.stopped", c->id, ".stopped");

/*
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
	  if ((innerActivity1 != innerActivity2) && (otherCaseBranch->dpe > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + toString(currentCase)), TheNet->findPlace(innerActivity2, ".negLink"));

	  if (c->dpe > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + toString(currentCase)), TheNet->findPlace(c, ".negLink"));
	}

	if (thisCaseBranch->dpe > 0)
	{
	  TheNet->newArc(TheNet->findTransition(prefix + "t2.otherwise"), TheNet->findPlace(innerActivity1, ".negLink"));
	}
	  

	currentCase++;

	if ((This->dpe > 0) && (thisCaseBranch->dpe > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }
*/
      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

/*
 * The <while> activity as it is depicted in Fig. 18.
 */

This=While(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

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

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t4, p1);
      TheNet->newArc(p5, t4);


      // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t5 = throwFault(p2, p6, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

      Transition *t6 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p2, "running", prefix);
      Transition *t8 = stop(p6, "p6", prefix);
      Transition *t9 = stop(p3, "final", prefix);
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(This->id, ".internal.p4",      b->id, ".initial");
      TheNet->mergePlaces(This->id, ".internal.p5",      b->id, ".final");
      TheNet->mergePlaces(This->id, ".internal.stop",    b->id, ".stop");
      TheNet->mergePlaces(This->id, ".internal.stopped", b->id, ".stopped");

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  REPEAT UNTIL                                                   (WS-BPEL 2.0)
******************************************************************************/

/*
 * The <repeatUntil> activity.
 */

This=RepeatUntil(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "running");
      Place *p4 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "p5");
      Place *p6 = TheNet->newPlace(prefix + "stop");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1"); /* read variable */
      TheNet->newArc(p2, t1);
      TheNet->newArc(t1, p3);

      Transition *t2 = TheNet->newTransition(prefix + "t2"); /* end */
      TheNet->newArc(p3, t2);
      TheNet->newArc(t2, p4);

      Transition *t3 = TheNet->newTransition(prefix + "t3"); /* repeat */
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p1);


       // Transitions for the negative control flow.
      if (parameters[P_NOSTANDARDFAULTS] == false)
	Transition *t4 = throwFault(p3, p6, "running", prefix, _id, ASTEmap[_id->value]->controlFlow);

      Transition *t5 = stop(p1, "initial", prefix);
      Transition *t6 = stop(p2, "running", prefix);
      Transition *t7 = stop(p5, "p5", prefix);
      Transition *t8 = stop(p4, "final", prefix);
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(This->id, ".internal.initial", b->id, ".initial");
      TheNet->mergePlaces(This->id, ".internal.p2",      b->id, ".final");
      TheNet->mergePlaces(This->id, ".internal.stop",    b->id, ".stop");
      TheNet->mergePlaces(This->id, ".internal.stopped", b->id, ".stopped");

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

This=Sequence(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "stopped");

/*
      // If needed, transition t1 and a place negLink is created.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	Place *p1  = TheNet->newPlace(prefix + "negLink");
  	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p1, t1);
      }
*/
    }
    a // standardElements for this activity
    b // inner activities
    {
      int activityCount = 1;
      kc::impl_activity* lastActivity;

      foreach (innerActivity; activity_list b)
      {
	if (activityCount == 1)
	  TheNet->mergePlaces(This->id, ".internal.initial", innerActivity->id, ".initial");
	else
	  TheNet->mergePlaces(lastActivity->id, ".final", innerActivity->id, ".initial");

	if (activityCount == b->length())
	  TheNet->mergePlaces(This->id, ".internal.final", innerActivity->id, ".final");
/*
	// If an inner activity has a place negLink, it is connected to the transition
	// t1.
        string newPrefix = toString(innerActivity->id);
	if (This->dpe > 0 && TheNet->findPlace(newPrefix + ".negLink") != NULL)
	  TheNet->newArc(t1, TheNet->findPlace(newPrefix + ".negLink"));
*/

	TheNet->mergePlaces(This->id, ".internal.stop", innerActivity->id, ".stop");
	TheNet->mergePlaces(This->id, ".internal.stopped", innerActivity->id, ".stopped");

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

This=Pick(a,b,c,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");


      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ);
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7);

/*
      // If needed, transition t1 and a place negLink is created.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	t1 = TheNet->newTransition(prefix + "t1");
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(p1, t1);
      }
*/

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
	string message = toString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Place *p3 = TheNet->newPlace(newPrefix + "p3");
	Place *p10 = TheNet->newPlace(newPrefix + "p10");

	Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + ASTEmap[ThisBranch->id]->channelName), t2);
	TheNet->newArc(TheNet->findPlace(This->id, ".internal.TimeStamp"), t2);
	TheNet->newArc(t2, p3);
	t2->type = IN;

	Transition *t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + ASTEmap[ThisBranch->id]->variableName), t3);
  	TheNet->newArc(t3, TheNet->findPlace("variable." + ASTEmap[ThisBranch->id]->variableName));
	TheNet->newArc(p3, t3);
	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));

        if (ASTEmap[ThisBranch->id]->inWhile) // was: (parameters[P_CYCLICWHILE] && ThisBranch->inWhile)
          TheNet->findPlace("in." + ASTEmap[ThisBranch->id]->channelName)->inWhile = true;


	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t4 = throwFault(p3, p10, "p3", newPrefix, _id, ASTEmap[_id->value]->controlFlow);

        Transition *t9  = stop(p3, "p3at" + message, prefix);
	Transition *t10 = stop(p10, "p10at" + message, prefix);


	// Embed the inner activity.
	TheNet->mergePlaces(This->id, ".stop",    innerActivity->id, ".stop");
	TheNet->mergePlaces(This->id, ".stopped", innerActivity->id, ".stopped");
	TheNet->mergePlaces(This->id, ".final",   innerActivity->id, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
      }


      // Traverse the <onAlarm> branches.
      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
	string newPrefix = prefix + "onAlarm" + toString(onAlarmCount++) + ".";

	Transition *t6 = TheNet->newTransition(newPrefix + "t6");
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), t6, READ);
	TheNet->newArc(TheNet->findPlace(This->id, ".internal.TimeStamp"), t6);
	TheNet->newArc(t6, TheNet->findPlace(innerActivity->id, ".initial"));


	// Embed the inner activity.
	TheNet->mergePlaces(This->id, ".stop",    innerActivity->id, ".stop");
	TheNet->mergePlaces(This->id, ".stopped", innerActivity->id, ".stopped");
	TheNet->mergePlaces(This->id, ".final",   innerActivity->id, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
      }

/*
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
	  if ((innerActivity1 != innerActivity2) && (otheronmessage->dpe > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + toString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (thisonalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if (thisonalarm->dpe > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + toString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentMessage++;

	if ((thisonmessage->dpe > 0) && (This->dpe > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }

      
      int currentAlarm = 1;
      foreach (thisonalarm=OnAlarm(innerActivity1); tOnAlarm_list c)
      {
	foreach (otheronalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if ((innerActivity1 != innerActivity2) && (otheronalarm->dpe > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + toString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (thisonmessage=OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if (thisonmessage->dpe > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + toString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentAlarm++;

	if ((thisonalarm->dpe > 0) && (This->dpe > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }
*/
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




/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinetnew:
    {
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");

/*
      // If the inner activity has a place negLink, connect merge this place
      // with a new created one.
      if (This->dpe > 0)
      {
	Place *p1 = TheNet->newPlace(prefix + "negLink");
	TheNet->mergePlaces(p1, TheNet->findPlace(newPrefix + ".negLink"));
      }
*/

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
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p2 = TheNet->newPlace(prefix + "initial");
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

/*
      // If needed, transition t1 and the place p1 is created and connected
      // with the negLink place of the inner activity if existing for dead path
      // elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
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
*/

      // Connect all source link places with t2 and, if needed, with transition
      // t1 for dead path elimination.
      foreach (source; tSource_list a)
      {
	Place *p8 = TheNet->findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p10 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);

	TheNet->newArc(p8, t2);
	TheNet->newArc(t2, p10);
	TheNet->newArc(p10, t3, READ);
/*
	if (This->dpe > 0)
	{
	  TheNet->newArc(t1, p10);
	  TheNet->newArc(p8, t1);
	}*/
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

This=StandardElements(a, NiltSource_list(), jc) provided (!This->suppressJF) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 23\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

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
      Transition *t2 = throwFault(p8, p11, "initial", prefix, mkinteger(This->parentId), ASTEmap[This->parentId]->controlFlow);

      Transition *t3 = stop(p8, "initial", prefix);
      Transition *t4 = stop(p11, "p11", prefix);


      // Connect the target links with transitions t1 and t2.
      foreach (target; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1);
	TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = TheNet->findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(t2, p5a);
	TheNet->mergePlaces(p5a, p5);
      }
*/
      
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

This=StandardElements(a, NiltSource_list(), jc) provided (This->suppressJF) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

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
      foreach (target; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1);
	TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = TheNet->findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = TheNet->newPlace(prefix + "negLink");
	TheNet->newArc(t2, p5);
	TheNet->mergePlaces(p5a, p5);
      }
*/

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

This=StandardElements(a,b, jc) provided (This->suppressJF) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 25\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

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

/*
      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	Place *p9  = TheNet->newPlace(prefix + "negLink");

	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p9, t1);

	Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  TheNet->newArc(t1, p10);
      }
*/

      // Connect each target links with t2 and t5.
      foreach (target; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
        TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
        TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (source; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p7 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);

        TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7);
        TheNet->newArc(p5, t5);
        TheNet->newArc(t5, p7);
	TheNet->newArc(p7, t4, READ);
/*	if (This->dpe > 0)
	{
	  TheNet->newArc(p5, t1);
	  TheNet->newArc(t1, p7);
	}*/
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	TheNet->newArc(t5, p10);
	if (This->dpe > 0)
	  TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".negLink"));
      }
*/

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

This=StandardElements(a,b, jc) provided (!This->suppressJF) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

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
      Transition *t5 = throwFault(p16, p17, "initial", prefix, mkinteger(This->parentId), ASTEmap[This->parentId]->controlFlow);

      Transition *t6 = stop(p16, "initial", prefix);
      Transition *t7 = stop(p17, "p17", prefix);
      Transition *t8 = stop(p13, "final", prefix);

/*
      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	Place *p9  = TheNet->newPlace(prefix + "negLink");

	t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p9, t1);

	Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  TheNet->newArc(t1, p10);
      }
*/

      // Connect each target links with t2 and t5.
      foreach (target; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
        TheNet->newArc(t2, p1);
	TheNet->newArc(p3, t2);
	TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (source; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p7 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);

	TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7);
	TheNet->newArc(p7, t4, READ);
/*
	if (This->dpe > 0)
	{
	  TheNet->newArc(p5, t1);
	  TheNet->newArc(t1, p7);
	}*/
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	TheNet->newArc(t5, p10);
//	if (This->dpe > 0)
//	  TheNet->mergePlaces(p10, TheNet->findPlace(newPrefix + ".negLink"));
      }


      // Embed the inner activity.
      TheNet->mergePlaces(p11, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p12, TheNet->findPlace(newPrefix + ".final"));
      TheNet->mergePlaces(p14, TheNet->findPlace(newPrefix + ".stop"));
      TheNet->mergePlaces(p15, TheNet->findPlace(newPrefix + ".stopped"));
    }
  ]
;

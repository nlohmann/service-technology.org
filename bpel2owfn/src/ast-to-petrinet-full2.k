/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann                                        *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

%{
/*!
 * \file    ast-to-petrinet-full2.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2006/12/30 12:48:01 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.80 $
 */
%}





/******************************************************************************
  BASIC ACTIVITIES
******************************************************************************/

Empty -> [petrinet petrinetnew petrinetsmall:
  { PEmpty($0, kc_current_view_base); }
];

AsynchronousInvoke -> [petrinet petrinetnew petrinetsmall:
  { PAsynchronousInvoke($0, kc_current_view_base); }
];

SynchronousInvoke -> [petrinet petrinetnew petrinetsmall:
  { PSynchronousInvoke($0, kc_current_view_base); }
];

Receive -> [petrinet petrinetnew petrinetsmall:
  { PReceive($0, kc_current_view_base); }
];

Reply -> [petrinet petrinetnew petrinetsmall:
  { PReply($0, kc_current_view_base); }
];

Assign -> [petrinet petrinetnew petrinetsmall:
  { PAssign($0, kc_current_view_base); }
];

Validate -> [petrinet petrinetnew petrinetsmall:
  { PValidate($0, kc_current_view_base); }
];

WaitUntil -> [petrinet petrinetnew petrinetsmall:
  { PWaitUntil($0, kc_current_view_base); }
];

WaitFor -> [petrinet petrinetnew petrinetsmall:
  { PWaitFor($0, kc_current_view_base); }
];

Throw -> [petrinet petrinetnew petrinetsmall:
  { PThrow($0, kc_current_view_base); }
];

Rethrow -> [petrinetnew:
  { PRethrow($0, kc_current_view_base); }
];

Terminate -> [petrinet petrinetnew petrinetsmall:
  { PTerminate($0, kc_current_view_base); }
];

Exit -> [petrinet petrinetnew petrinetsmall:
  { PExit($0, kc_current_view_base); }
];

Compensate -> [petrinetsmall:
  { PCompensate($0, kc_current_view_base); }
];

CompensateScope -> [petrinetsmall:
  { PCompensateScope($0, kc_current_view_base); }
];





/******************************************************************************
  STRUCTURED ACTIVITIES
******************************************************************************/

While -> [petrinet petrinetnew petrinetsmall:
  { PWhile($0, kc_current_view_base); }
];

RepeatUntil -> [petrinet petrinetnew petrinetsmall:
  { PRepeatUntil($0, kc_current_view_base); }
];

Sequence -> [petrinet petrinetnew petrinetsmall:
  { PSequence($0, kc_current_view_base); }
];

Flow -> [petrinet petrinetnew petrinetsmall:
  { PFlow($0, kc_current_view_base); }
];

Switch -> [petrinet petrinetnew petrinetsmall:
  { PSwitch($0, kc_current_view_base); }
];

Pick -> [petrinet petrinetnew petrinetsmall:
  { PPick($0, kc_current_view_base); }
];

If -> [petrinet petrinetnew petrinetsmall:
  { PIf($0, kc_current_view_base); }
];

ForEach -> [petrinet petrinetnew petrinetsmall:
  { PForEach($0, kc_current_view_base); }
];





/******************************************************************************
  SCOPE / PROCESS
******************************************************************************/

Process -> [petrinet petrinetnew petrinetsmall:
  { PProcess($0, kc_current_view_base);

    // clear global mappings
    ASTE_scopeNames.clear(); }
];

Scope -> [petrinet petrinetnew petrinetsmall:
  { PScope($0, kc_current_view_base); }
];

StopInProcess -> [petrinet:
  { PStop($0, kc_current_view_base); }
];

StopInScope -> [petrinet:
  { PStop($0, kc_current_view_base); }
];

userDefinedEventHandler -> [petrinetnew:
  { PEventHandlers($0, kc_current_view_base); }
];




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
  [petrinet:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "comp");
      Place *p6 = PN.newPlace(prefix + "done");
      Place *p7 = PN.newPlace(prefix + "stopped");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);
      PN.newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate /> activity in compensation handler.
      PN.mergePlaces(p4, PN.findPlace(currentScope + "internal.compensationHandler.p10"));
      PN.mergePlaces(p6, PN.findPlace(currentScope + "internal.compensationHandler.p12"));
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
  [petrinet:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "ch_fh");
      Place *p6 = PN.newPlace(prefix + "ch_out");
      Place *p7 = PN.newPlace(prefix + "stopped");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);
      PN.newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate /> activity in fault handler.
      PN.mergePlaces(p5, PN.findPlace(currentScope + "faultHandler.ch_fh"));
      PN.mergePlaces(p6, PN.findPlace(currentScope + "faultHandler.ch_out"));
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
  [petrinet:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "compScope");
      Place *p6 = PN.newPlace(prefix + "scopeCompensated");
      Place *p7 = PN.newPlace(prefix + "stopped");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);
      PN.newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate scope="C"/> activity in compensation handler.
      PN.mergePlaces(p4, PN.findPlace(currentScope + "internal.compensationHandler.p10"));
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
  [petrinet:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "ch_in");
      Place *p6 = PN.newPlace(prefix + "compensated");
      Place *p7 = PN.newPlace(prefix + "stopped");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);
      PN.newArc(p6, t2);


      // Transitions for the negative control flow.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);


      // Embed this <compensate scope="C"/> activity in fault handler.
      PN.mergePlaces(p5, PN.findPlace(currentScope + "faultHandler.ch_fh"));
      PN.mergePlaces(p6, PN.findPlace(currentScope + "faultHandler.ch_out"));
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinet petrinetnew:
    {
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p2 = PN.newPlace(prefix + "initial");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "stopped");

/*
      // If the inner activity has a place negLink, connect merge this place
      // with a new created one.
      if (This->dpe > 0)
      {
	Place *p1 = PN.newPlace(prefix + "negLink");
	PN.mergePlaces(p1, PN.findPlace(newPrefix + ".negLink"));
      }
*/

      // Embed the inner activity.
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p3, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p4, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p5, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 22: Pattern for an activity that is source of links.
 *****************************************************************************/

This=StandardElements(NiltTarget_list(), a, jc) ->
  [petrinet petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p2 = PN.newPlace(prefix + "initial");
      Place *p4 = PN.newPlace(prefix + "p4");
      Place *p5 = PN.newPlace(prefix + "final");
      Place *p6 = PN.newPlace(prefix + "stop");
      Place *p7 = PN.newPlace(prefix + "stopped");

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p4, t2);
      PN.newArc(t2, p5);

      Transition *t3 = PN.newTransition(prefix + "t3");
      PN.newArc(p4, t3);
      PN.newArc(t3, p5);


      // Transition for the negative control flow.
      Transition *t4 = stop(p5, "final", prefix);

/*
      // If needed, transition t1 and the place p1 is created and connected
      // with the negLink place of the inner activity if existing for dead path
      // elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
        Place *p1 = PN.newPlace(prefix + "negLink");
	t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);

	Place *p3 = PN.findPlace(newPrefix + ".negLink");
	if (p3 != NULL)
	{
	  PN.newArc(t1, p3);
	  PN.mergePlaces(p3, PN.findPlace(newPrefix + ".negLink"));
	}
      }
*/

      // Connect all source link places with t2 and, if needed, with transition
      // t1 for dead path elimination.
      foreach (source; tSource_list a)
      {
	Place *p8 = PN.findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p10 = PN.findPlace("link." + ASTEmap[source->id]->linkName);

	PN.newArc(p8, t2);
	PN.newArc(t2, p10);
	PN.newArc(p10, t3, READ);
/*
	if (This->dpe > 0)
	{
	  PN.newArc(t1, p10);
	  PN.newArc(p8, t1);
	}*/
      }


      // Embed the inner activity.
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p4, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p6, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stopped"));
   }
  ]
;





/******************************************************************************
 * Figure 23: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided (!This->suppressJF) ->
  [petrinet petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 23\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p6  = PN.newPlace(prefix + "p6");
      Place *p7  = PN.newPlace(prefix + "final");
      Place *p8  = PN.newPlace(prefix + "initial");
      Place *p9  = PN.newPlace(prefix + "stop");
      Place *p10 = PN.newPlace(prefix + "stopped");
      Place *p11 = PN.newPlace(prefix + "p11");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(t1, p6);
      PN.newArc(p8, t1);


      // Transitions for the negative control flow.
      Transition *t2 = throwFault(p8, p11, "initial", prefix, mkinteger(This->parentId), ASTEmap[This->parentId]->controlFlow);

      Transition *t3 = stop(p8, "initial", prefix);
      Transition *t4 = stop(p11, "p11", prefix);


      // Connect the target links with transitions t1 and t2.
      foreach (target; tTarget_list a)
      {
	Place *p1 = PN.findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	PN.newArc(t1, p1);
	PN.newArc(p3, t1);
	PN.newArc(t2, p1);
	PN.newArc(p3, t2);
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = PN.findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = PN.newPlace(prefix + "negLink");
	PN.newArc(t2, p5a);
	PN.mergePlaces(p5a, p5);
      }
*/
      
      // Embed the inner activity.
      PN.mergePlaces(p6,  PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p7,  PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p9,  PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p10, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 24: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided (This->suppressJF) ->
  [petrinet petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p6  = PN.newPlace(prefix + "p6");
      Place *p7  = PN.newPlace(prefix + "final");
      Place *p8  = PN.newPlace(prefix + "initial");
      Place *p9  = PN.newPlace(prefix + "stop");
      Place *p10 = PN.newPlace(prefix + "stopped");

      Transition *t1 = PN.newTransition(prefix + "t1");
      PN.newArc(t1, p6);
      PN.newArc(p8, t1);

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(p8, t2);
      PN.newArc(t2, p7);


      // Transition for the negative control flow.
      Transition *t3 = stop(p8, "initial", prefix);


      // Connect the target links with transitions t1 and t2.
      foreach (target; tTarget_list a)
      {
	Place *p1 = PN.findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	PN.newArc(t1, p1);
	PN.newArc(p3, t1);
	PN.newArc(t2, p1);
	PN.newArc(p3, t2);
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = PN.findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	Place *p5a = PN.newPlace(prefix + "negLink");
	PN.newArc(t2, p5);
	PN.mergePlaces(p5a, p5);
      }
*/

      // Embed the inner activity.
      PN.mergePlaces(p6, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p9, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p10, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 25: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided (This->suppressJF) ->
  [petrinet petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 25\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p11 = PN.newPlace(prefix + "p11");
      Place *p12 = PN.newPlace(prefix + "p12");
      Place *p13 = PN.newPlace(prefix + "final");
      Place *p14 = PN.newPlace(prefix + "stop");
      Place *p15 = PN.newPlace(prefix + "stopped");
      Place *p16 = PN.newPlace(prefix + "initial");

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(t2, p11);
      PN.newArc(p16, t2);

      Transition *t3 = PN.newTransition(prefix + "t3");
      PN.newArc(p12, t3);
      PN.newArc(t3, p13);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p12, t4);
      PN.newArc(t4, p13);

      Transition *t5 = PN.newTransition(prefix + "t5");
      PN.newArc(t5, p13);
      PN.newArc(p16, t5);


      // Transitions for the negative control flow.
      Transition *t6 = stop(p16, "initial", prefix);
      Transition *t7 = stop(p13, "final", prefix);

/*
      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe > 0)
      {
	Place *p9  = PN.newPlace(prefix + "negLink");

	t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p9, t1);

	Place *p10 = PN.findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  PN.newArc(t1, p10);
      }
*/

      // Connect each target links with t2 and t5.
      foreach (target; tTarget_list a)
      {
	Place *p1 = PN.findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = PN.findPlace("link." + ASTEmap[target->id]->linkName);
        PN.newArc(t2, p1);
	PN.newArc(p3, t2);
        PN.newArc(t5, p1);
	PN.newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (source; tSource_list b)
      {
	Place *p5 = PN.findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);

        PN.newArc(p5, t3);
        PN.newArc(t3, p7);
        PN.newArc(p5, t5);
        PN.newArc(t5, p7);
	PN.newArc(p7, t4, READ);
/*	if (This->dpe > 0)
	{
	  PN.newArc(p5, t1);
	  PN.newArc(t1, p7);
	}*/
      }

/*
      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = PN.findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	PN.newArc(t5, p10);
	if (This->dpe > 0)
	  PN.mergePlaces(p10, PN.findPlace(newPrefix + ".negLink"));
      }
*/

      // Embed the inner activity.
      PN.mergePlaces(p11, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p12, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p14, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p15, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;






/******************************************************************************
 * Figure 26: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided (!This->suppressJF) ->
  [petrinet petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p11 = PN.newPlace(prefix + "p11");
      Place *p12 = PN.newPlace(prefix + "p12");
      Place *p13 = PN.newPlace(prefix + "final");
      Place *p14 = PN.newPlace(prefix + "stop");
      Place *p15 = PN.newPlace(prefix + "stopped");
      Place *p16 = PN.newPlace(prefix + "initial");
      Place *p17 = PN.newPlace(prefix + "p17");

      Transition *t2 = PN.newTransition(prefix + "t2");
      PN.newArc(t2, p11);
      PN.newArc(p16, t2);

      Transition *t3 = PN.newTransition(prefix + "t3");
      PN.newArc(p12, t3);
      PN.newArc(t3, p13);

      Transition *t4 = PN.newTransition(prefix + "t4");
      PN.newArc(p12, t4);
      PN.newArc(t4, p13);


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
	Place *p9  = PN.newPlace(prefix + "negLink");

	t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p9, t1);

	Place *p10 = PN.findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  PN.newArc(t1, p10);
      }
*/

      // Connect each target links with t2 and t5.
      foreach (target; tTarget_list a)
      {
	Place *p1 = PN.findPlace("!link." + ASTEmap[target->id]->linkName);
	Place *p3 = PN.findPlace("link." + ASTEmap[target->id]->linkName);
        PN.newArc(t2, p1);
	PN.newArc(p3, t2);
	PN.newArc(t5, p1);
	PN.newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (source; tSource_list b)
      {
	Place *p5 = PN.findPlace("!link." + ASTEmap[source->id]->linkName);
	Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);

	PN.newArc(p5, t3);
        PN.newArc(t3, p7);
	PN.newArc(p7, t4, READ);
/*
	if (This->dpe > 0)
	{
	  PN.newArc(p5, t1);
	  PN.newArc(t1, p7);
	}*/
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t5 for dead path elimination.
      Place *p10 = PN.findPlace(newPrefix + ".negLink");
      if (p10 != NULL)
      {
	PN.newArc(t5, p10);
//	if (This->dpe > 0)
//	  PN.mergePlaces(p10, PN.findPlace(newPrefix + ".negLink"));
      }


      // Embed the inner activity.
      PN.mergePlaces(p11, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p12, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p14, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p15, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;

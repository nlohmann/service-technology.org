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
 * \file    ast-to-petrinet-full2.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2006/11/29 11:22:37 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.67 $
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

If -> [petrinetsmall:
  { PIf($0, kc_current_view_base); }
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
  [petrinet:
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
  [petrinet:
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
  [petrinet:
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
  SCOPE
******************************************************************************/

/*
 * The <scope> activity as it is depicted in Fig. 27. Note that it contains
 * only one event handler since alarm-event handler and message-event handler
 * are merged.
 */

This=Scope(a,b,c,d,e,f,g,h,i,j,_id) ->
  [petrinet:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      currentScope = prefix;

      // Create push-places for the children of the scope.
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *push_child    = TheNet->newPlace(toString(*child) + ".push");
	Place *notPush_child = TheNet->newPlace(toString(*child) + ".!push");
      }


      // Create the internal places of the scope pattern.
//      if (This->dpe > 0)
//      {
//	Place *p1  = TheNet->newPlace(prefix + "negLink");
//      }
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
      TheNet->newArc(TheNet->findPlace(toString(This->id) + ".!push"), t4);
      TheNet->newArc(t4, TheNet->findPlace(toString(This->id) + ".push"));
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
      if (ASTEmap[_id->value]->hasEH)
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
      TheNet->newArc(TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.Active"), t12);
      TheNet->newArc(t12, TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.!Active")); //newer fault management
      TheNet->newArc(t12, TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.stop.fault_in"));

      Transition *t13 = TheNet->newTransition(prefix + "ignoreFault");
      TheNet->newArc(p48, t13);
      TheNet->newArc(TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.!Active"), t13, READ);

      // Merge places with parent scope.
      TheNet->mergePlaces(TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) +
	    ".internal.compensationHandler.compScope." + toString(This->id)), p3); // was "....internal.compScope."
      TheNet->mergePlaces(TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.scopeCompensated"), p56);
      TheNet->mergePlaces(TheNet->findPlace(toString(ASTEmap[_id->value]->parentScopeId) + ".internal.upperTerminate"), p46);
    }
    h // Stop Pattern
    a // standardElements
    c // Fault Handler
    d // Compensation Handler
    f // Event Handler
    g // correlation sets
    i // innerActivity
    {
      // After the patterns of the inner activity of the process are generated
      // the push places of the child scopes are available and have to be
      // connected with the transition t2 of the process.
      for (set<unsigned int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace(toString(*child) + ".!push"));

      string prefix2 = toString(This->id) + ".internal.";


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
      TheNet->mergePlaces(prefix + "final",   prefix2 + "faultHandler.out");	// only for user-defined FH
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
      if (ASTEmap[_id->value]->hasEH)
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
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(i->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(i->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(i->id, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(i->id, ".stopped"));


      // Merge (!)Terminated places
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p8);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p9);

//TODO: A list of enclosed links!

/*
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
*/

      currentScope = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.";
      footer(This->id, true);
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
  [petrinet petrinetnew:
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
  [petrinet petrinetnew:
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
  [petrinet petrinetnew:
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
  [petrinet petrinetnew:
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
  [petrinet petrinetnew:
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

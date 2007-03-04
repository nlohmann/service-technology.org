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
 * \date    \$Date: 2007/03/04 13:15:34 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.86 $
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

Exit -> [petrinet petrinetnew petrinetsmall:
  { PExit($0, kc_current_view_base); }
];

Compensate -> [petrinetnew petrinetsmall:
  { PCompensate($0, kc_current_view_base); }
];

CompensateScope -> [petrinetnew petrinetsmall:
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
      PN.mergePlaces(p4, currentScope + "internal.compensationHandler.p10");
      PN.mergePlaces(p6, currentScope + "internal.compensationHandler.p12");
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
      PN.mergePlaces(p5, currentScope + "faultHandler.ch_fh");
      PN.mergePlaces(p6, currentScope + "faultHandler.ch_out");
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
      PN.mergePlaces(p4, currentScope + "internal.compensationHandler.p10");
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
      PN.mergePlaces(p5, currentScope + "faultHandler.ch_fh");
      PN.mergePlaces(p6, currentScope + "faultHandler.ch_out");
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;

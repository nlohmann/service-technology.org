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
 * Patters:
 *   - <process>
 *   - stop patterns
 *   - fault handlers
 *   - event handlers
 *   - compensation handlers
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created 2005-11-01
 *          - last changed: \$Date: 2006/07/11 22:32:29 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.187 $
 *
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
#include "options.h"
#include "bpel-unparse-tools.h"
#include "ast-printers.h"

using namespace std;


/******************************************************************************
 * External variables
 *****************************************************************************/

// introduced in main.c
extern PetriNet *TheNet;

/// prefix of the current scope (read by the stop pattern and used to direct
/// failures to the correct fault_in place)
string currentScope;

%}




/******************************************************************************
  PROCESS
******************************************************************************/

/*
 * Pattern of BPEL's <process> as it is depicted in Fig. 28. Note that it
 * contains only one event handler since alarm-event handler and message-event
 * handler are merged.
 */

This=Process(a,b,c,d,e,f,g,h,i) ->
  [petrinet:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
      header(This->id, true);

      string prefix = intToString(This->id) + ".internal.";
      currentScope = prefix;


      // Create the interface places.
      // The used lists are filled during parsing mapping each combination of
      // partnerLink, portType and operation to one input (resp. output) place.
      for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	TheNet->newPlace("in." + *in, IN);
      for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	TheNet->newPlace("out." + *out, OUT);


      // Create push-places for the children of the process.
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	TheNet->newPlace(intToString(*child) + ".push");
	TheNet->newPlace(intToString(*child) + ".!push");
      }


      // Create the places for the links.
      // The used list is filled during parsing and contains all links of the
      // whole process -- also links defined in a deeper hierarchy.
      for (set<string>::iterator link = ASTE_links.begin(); link != ASTE_links.end(); link++)
      {
	TheNet->newPlace("link." + *link);
	TheNet->newPlace("!link." + *link);
      }


      // Create variable places and initially mark them.
      for (set<string>::iterator variable = ASTE_variables.begin(); variable != ASTE_variables.end(); variable++)
      {
	Place *v = TheNet->newPlace("variable." + *variable);
	v->mark();
      }

      // Create a place for the global clock and mark it.
      Place *clock = TheNet->newPlace(prefix + "clock");
      clock->mark();


      // Create the initial place of the process pattern and mark it.
      Place *p1 = TheNet->newPlace(prefix + "initial");
      p1->mark();
      
      // Create the internal places of the process pattern.
      Place *p3  = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p5  = TheNet->newPlace(prefix + "upperFH");
      Place *p6  = TheNet->newPlace(prefix + "upperTerminate");
      Place *p7  = TheNet->newPlace(prefix + "Active");
      Place *p8  = TheNet->newPlace(prefix + "!Completed");
      Place *p9  = TheNet->newPlace(prefix + "Completed");
      Place *p10 = TheNet->newPlace(prefix + "!Compensated");
      Place *p11 = TheNet->newPlace(prefix + "!Active");
      Place *p12 = TheNet->newPlace(prefix + "Compensated");
      Place *p13 = TheNet->newPlace(prefix + "!Ended");
      Place *p14 = TheNet->newPlace(prefix + "Ended");
      Place *p15 = TheNet->newPlace(prefix + "!Faulted");
      Place *p16 = TheNet->newPlace(prefix + "Faulted");
      Place *p17 = TheNet->newPlace(prefix + "!Terminated");
      Place *p18 = TheNet->newPlace(prefix + "Terminated");
      Place *p19 = TheNet->newPlace(prefix + "p19");
      Place *p20 = TheNet->newPlace(prefix + "p20");
      Place *p21 = TheNet->newPlace(prefix + "p21");
      Place *p22 = TheNet->newPlace(prefix + "p22");
      Place *p33 = TheNet->newPlace(prefix + "p33");
      Place *p34 = TheNet->newPlace(prefix + "p34");
      Place *p35 = TheNet->newPlace(prefix + "p35");
      Place *p36 = TheNet->newPlace(prefix + "p36");
      Place *p37 = TheNet->newPlace(prefix + "fault");
      Place *p38 = TheNet->newPlace(prefix + "faultSave");
      Place *p39 = TheNet->newPlace(prefix + "final");
      Place *p40 = TheNet->newPlace(prefix + "rethrow");
      Place *p41 = TheNet->newPlace(prefix + "p41");
      Place *p42 = TheNet->newPlace(prefix + "ch_out");


      // State places of the process modeling the new fault model: The places
      // guarantee that the occurrence of more than one fault inside the fault
      // or compensation handler is prevented. The places are used as guards
      // by the transitions signalling faults.
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");


      // If the parameter "-p finalloop" is used, a transition
      // "1.internal.finalloop" is added to livelock the net as soon as the
      // final place of the process ("1.internal.final") is marked. With this
      // loop the net will only deadlock in the case ill-designed BPEL
      // processes (e.g. cyclic links). These deadlocks then can be found
      // using model checkers.
      if (parameters[P_FINALLOOP])
      {
        Transition *process_loop = TheNet->newTransition(prefix + "finalloop");
        TheNet->newArc(p39, process_loop, READ);
      }


      // The transitions of the process pattern.
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p7);
      TheNet->newArc(t2, p8);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p13);
      TheNet->newArc(t2, p15);
      TheNet->newArc(t2, p17);
      TheNet->newArc(t2, p19);
      TheNet->newArc(t2, fh_not_faulted);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p7, t4);
      TheNet->newArc(p8, t4);
      TheNet->newArc(t4, p9);
      TheNet->newArc(t4, p11);
      TheNet->newArc(p33, t4);
      TheNet->newArc(t4, p39);


      // If the process has an event handler several places and transitions
      // have to be added to embed it. If the process lacks an event handler
      // some places can be merged.
      if (This->hasEH)
      {
	Place *p23 = TheNet->newPlace(prefix + "p23");
	Place *p24 = TheNet->newPlace(prefix + "p24");
	Place *p25 = TheNet->newPlace(prefix + "p25");
	Place *p26 = TheNet->newPlace(prefix + "p26");
	Place *p27 = TheNet->newPlace(prefix + "p27");

	TheNet->newArc(t2, p23);
	TheNet->newArc(p25, t4);

	Transition *t1 = TheNet->newTransition(prefix + "t1");
    	TheNet->newArc(p27, t1);
  	TheNet->newArc(t1, p34);

	Transition *t3 = TheNet->newTransition(prefix + "t3");
	TheNet->newArc(p20, t3);
	TheNet->newArc(t3, p33);
	TheNet->newArc(t3, p24);

	Transition *t5 = TheNet->newTransition(prefix + "t5");
	TheNet->newArc(p22, t5);
	TheNet->newArc(t5, p26);

	Transition *t6 = TheNet->newTransition(prefix + "t6");
	TheNet->newArc(t6, p26);
	TheNet->newArc(p21, t6);
	TheNet->newArc(p33, t6);
      }
      else
      {
	TheNet->mergePlaces(p20, p33);
	TheNet->mergePlaces(p22, p34);
      }
    }
      h // Stop Pattern
      e // FaultHandler
//TODO      f // Compensation Handler
      g // Event Handler
      i // innerActivity
    {
      // After the patterns of the inner activity of the process are generated
      // the push places of the child scopes are available and have to be
      // connected with the transition t2 of the process.
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString(*child) + ".!push" ));


      // Initialize the link places of the process.
      for (set<string>::iterator link = ASTE_links.begin(); link != ASTE_links.end(); link++)
	TheNet->newArc(t2, TheNet->findPlace("!link." + *link));


      // Embed the fault handler of the process.
      TheNet->mergePlaces("1.internal.fault",     "1.internal.faultHandler.fault");
      TheNet->mergePlaces("1.internal.faultSave", "1.internal.faultHandler.faultSave");
      TheNet->mergePlaces("1.internal.rethrow",   "1.internal.faultHandler.rethrow");
      TheNet->mergePlaces("1.internal.!Ended",    "1.internal.faultHandler.!Ended");
      TheNet->mergePlaces("1.internal.Ended",     "1.internal.faultHandler.Ended");
      TheNet->mergePlaces("1.internal.p41",       "1.internal.faultHandler.ch_fh");
      TheNet->mergePlaces("1.internal.ch_out",    "1.internal.faultHandler.ch_out");
      TheNet->mergePlaces("1.internal.final",     "1.internal.faultHandler.final");


      // Embed the compensation handler of the process.
      TheNet->mergePlaces("1.internal.Completed",        "1.internal.compensationHandler.Completed");
      TheNet->mergePlaces("1.internal.!Completed",       "1.internal.compensationHandler.!Completed");
      TheNet->mergePlaces("1.internal.Compensated",      "1.internal.compensationHandler.Compensated");
      TheNet->mergePlaces("1.internal.p41",              "1.internal.compensationHandler.fh");
      TheNet->mergePlaces("1.internal.scopeCompensated", "1.internal.compensationHandler.scopeCompensated");
      TheNet->mergePlaces("1.internal.p34",              "1.internal.compensationHandler.clean");
      TheNet->mergePlaces("1.internal.p35",              "1.internal.compensationHandler.cleaned");
      TheNet->mergePlaces("1.internal.ch_out",           "1.internal.compensationHandler.ch_out");


      // If present, embed the event handler of the process.
      if (This->hasEH)
      {
	TheNet->mergePlaces("1.internal.p23",      "1.internal.eventHandler.initial");
	TheNet->mergePlaces("1.internal.p24",      "1.internal.eventHandler.finish");
	TheNet->mergePlaces("1.internal.p25",      "1.internal.eventHandler.final");
	TheNet->mergePlaces("1.internal.p26",      "1.internal.eventHandler.stop");
	TheNet->mergePlaces("1.internal.p27",      "1.internal.eventHandler.stopped");
      }


      // Embed the stop-pattern of the process.
      TheNet->mergePlaces("1.internal.Faulted",        "1.internal.stop.Faulted");
      TheNet->mergePlaces("1.internal.upperFH",        "1.internal.stop.fault_in");
      TheNet->mergePlaces("1.internal.fault",          "1.internal.stop.fault");
      TheNet->mergePlaces("1.internal.faultSave",      "1.internal.stop.faultSave");
      TheNet->mergePlaces("1.internal.Active",         "1.internal.stop.Active");
      TheNet->mergePlaces("1.internal.!Active",        "1.internal.stop.!Active");
      TheNet->mergePlaces("1.internal.final",          "1.internal.stop.final");
      TheNet->mergePlaces("1.internal.upperTerminate", "1.internal.stop.terminate");
      TheNet->mergePlaces("1.internal.!Faulted",       "1.internal.stop.!Faulted");
      TheNet->mergePlaces("1.internal.rethrow",        "1.internal.stop.rethrow");
      TheNet->mergePlaces("1.internal.Ended",          "1.internal.stop.Ended");
      TheNet->mergePlaces("1.internal.Compensated",    "1.internal.stop.Compensated");
      TheNet->mergePlaces("1.internal.!Ended",         "1.internal.stop.!Ended");
      TheNet->mergePlaces("1.internal.p21",            "1.internal.stop.stop");
      TheNet->mergePlaces("1.internal.p34",            "1.internal.stop.stopped");
      TheNet->mergePlaces("1.internal.p35",            "1.internal.stop.cleanCH");
      TheNet->mergePlaces("1.internal.p36",            "1.internal.stop.ch_cleaned");
      TheNet->mergePlaces("1.internal.FHFaulted",      "1.internal.stop.FHFaulted");
      TheNet->mergePlaces("1.internal.!FHFaulted",     "1.internal.stop.!FHFaulted");
      TheNet->mergePlaces("1.internal.CHFaulted",      "1.internal.stop.CHFaulted");
      TheNet->mergePlaces("1.internal.!CHFaulted",     "1.internal.stop.!CHFaulted");


      // Embed the inner activity of the process.
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p19"), TheNet->findPlace(i, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p20"), TheNet->findPlace(i, ".final"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p21"), TheNet->findPlace(i, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p22"), TheNet->findPlace(i, ".stopped"));

      footer(This->id, true);
      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
;





/******************************************************************************
  STOP PATTERNS
******************************************************************************/

/*
 * Stop pattern embedded in a process as it is depicted in Fig. 33.
 *
 * Things that have been changed:
 *   - instead of one place "fault_in" there also exist "ch_fault_in" and
 *     "fh_fault_in".
 *   - "fh_fault_in" is the fault place for faults occuring in the fault
 *     handler. This place is connected with transition t1. The arc [p5,t1] is
 *     not needed any more.
 *   - "ch_fault_in" is the fault place for faults occuring in the compensation
 *     handler. This place is connected with transition t10. The arc [p5,t10]
 *     is not needed any more. Furthermore, the transition t9 (kill) can be
 *     removed.
 *   - The reset-arc [p5,t3] is not needed any more.
 *   - The semantics now is 1-safe. To ensure this, each fault-throwing
 *     activity is guarded:
 *     * Faults in the process can only be thrown when the place "Active" is
 *       marked. The first fault causes "!Active" to be marked. Due to this
 *       change the arc [p9,t4] has to be removed.
 *     * Faults in the fault handler can only be thrown when the (new) place
 *       "!FHFaulted" is marked. The first fault causes "FHFaulted" to be
 *       marked.
 *     * Faults in the compensation handler can only be thrown when the (new)
 *       place "!CHFaulted" is marked. The first fault causes "CHFaulted" to be
 *       marked.
 */

StopInProcess() ->
  [petrinet:
    {
      string prefix = "1.internal.stop.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<stopPatternInsideProcess>\n");

      Place *p1  = TheNet->newPlace(prefix + "Faulted");
      Place *p2  = TheNet->newPlace(prefix + "p2");
      Place *p3  = TheNet->newPlace(prefix + "p3");
      Place *p4  = TheNet->newPlace(prefix + "p4");
      Place *p5  = TheNet->newPlace(prefix + "fault_in");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "fault");
      Place *p8  = TheNet->newPlace(prefix + "faultSave");
      Place *p9  = TheNet->newPlace(prefix + "Active");
      Place *p10 = TheNet->newPlace(prefix + "!Active");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "terminate");
      Place *p14 = TheNet->newPlace(prefix + "!Faulted");
      Place *p15 = TheNet->newPlace(prefix + "rethrow");
      Place *p16 = TheNet->newPlace(prefix + "Ended");
      Place *p17 = TheNet->newPlace(prefix + "Compensated");
      Place *p19 = TheNet->newPlace(prefix + "p19");
      Place *p20 = TheNet->newPlace(prefix + "!Ended");
      Place *p21 = TheNet->newPlace(prefix + "stop");
      Place *p22 = TheNet->newPlace(prefix + "stopped");
      Place *p23 = TheNet->newPlace(prefix + "cleanCH");
      Place *p24 = TheNet->newPlace(prefix + "ch_cleaned");
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *fh_fault_in = TheNet->newPlace(prefix + "fh_fault_in");  // for faults from the FH
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");
      Place *ch_fault_in = TheNet->newPlace(prefix + "ch_fault_in");  // for faults from the CH

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1, READ);
      TheNet->newArc(fh_fault_in, t1);
      TheNet->newArc(t1, p15);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p21);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p3, t3);
      TheNet->newArc(p22, t3);

      Transition *t4 =  TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t3, p4);
      TheNet->newArc(p5, t4);
      TheNet->newArc(t4, p2);
      TheNet->newArc(t4, p6);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p6, t5);
      TheNet->newArc(p14, t5);
      TheNet->newArc(t5, p1);
      TheNet->newArc(t5, p7);
      TheNet->newArc(t5, p8);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p9, t6);
      TheNet->newArc(p13, t6);
      TheNet->newArc(t6, p2);
      TheNet->newArc(t6, p10);
      TheNet->newArc(t6, p11);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p4, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p16);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p10, t8, READ);
      TheNet->newArc(p13, t8);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(ch_fault_in, t10);
      TheNet->newArc(p17, t10, READ);
      TheNet->newArc(p20, t10);
      TheNet->newArc(t10, p19);
      TheNet->newArc(t10, p23);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p19, t12);
      TheNet->newArc(p24, t12);
      TheNet->newArc(t12, p16);
      TheNet->newArc(t12, p12);

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</stopPatternInsideProcess>\n");
    }
  ]
;





/*
 * Stop pattern embedded in a scope as it is depicted in Fig. 32.
 *
 * Changes here are analogous to those made in the stop pattern of the process.
 */

StopInScope() ->
  [petrinet:
    {
      string prefix = currentScope + "stop.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<stopPatternInsideScope scope=\"" + currentScope + "\">\n");

      Place *p1 =  TheNet->newPlace(prefix + "ft_in");
      Place *p2 =  TheNet->newPlace(prefix + "p2");
      Place *p3 =  TheNet->newPlace(prefix + "ftSave");
      Place *p4 =  TheNet->newPlace(prefix + "ft");
      Place *p5 =  TheNet->newPlace(prefix + "Faulted");
      Place *p6 =  TheNet->newPlace(prefix + "p6");
      Place *p7 =  TheNet->newPlace(prefix + "p7");
      Place *p8 =  TheNet->newPlace(prefix + "p8");
      Place *p9 =  TheNet->newPlace(prefix + "!Faulted");
      Place *p10 = TheNet->newPlace(prefix + "fault_in");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "fault");
      Place *p13 = TheNet->newPlace(prefix + "faultSave");
      Place *p14 = TheNet->newPlace(prefix + "Active");
      Place *p15 = TheNet->newPlace(prefix + "terminate_up");
      Place *p16 = TheNet->newPlace(prefix + "p16");
      Place *p17 = TheNet->newPlace(prefix + "stopped_s");
      Place *p18 = TheNet->newPlace(prefix + "!Active");
      Place *p19 = TheNet->newPlace(prefix + "p19");
      Place *p20 = TheNet->newPlace(prefix + "upperTerminate");
      Place *p21 = TheNet->newPlace(prefix + "terminate");
      Place *p22 = TheNet->newPlace(prefix + "Ended");
      Place *p23 = TheNet->newPlace(prefix + "rethrow");
      Place *p24 = TheNet->newPlace(prefix + "Compensated");
      Place *p25 = TheNet->newPlace(prefix + "p25");
      Place *p26 = TheNet->newPlace(prefix + "p26");
      Place *p27 = TheNet->newPlace(prefix + "upperFH");
      Place *p28 = TheNet->newPlace(prefix + "!Ended");
      Place *p29 = TheNet->newPlace(prefix + "stop");
      Place *p30 = TheNet->newPlace(prefix + "stopped");
      Place *p31 = TheNet->newPlace(prefix + "cleanCH");
      Place *p32 = TheNet->newPlace(prefix + "ch_cleaned");
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *fh_fault_in = TheNet->newPlace(prefix + "fh_fault_in"); // for faults from the FH
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");
      Place *ch_fault_in = TheNet->newPlace(prefix + "ch_fault_in"); // for faults from the CH

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p6);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p4);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p8, t2);
      TheNet->newArc(p9, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p5, t3, READ);
      TheNet->newArc(fh_fault_in, t3);
      TheNet->newArc(t3, p23);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(t4, p29);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p7, t5);
      TheNet->newArc(t5, p8);
      TheNet->newArc(p30, t5);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p6);
      TheNet->newArc(p10, t6);
      TheNet->newArc(t6, p11);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p5);
      TheNet->newArc(p8, t7);
      TheNet->newArc(p9, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p13);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p6);
      TheNet->newArc(p15, t8);
      TheNet->newArc(t8, p16);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p8, t9);
      TheNet->newArc(p16, t9);
      TheNet->newArc(t9, p17);
      TheNet->newArc(t9, p22);
      TheNet->newArc(p28, t9);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p6);
      TheNet->newArc(p14, t10);
      TheNet->newArc(t10, p19);
      TheNet->newArc(t10, p18);
      TheNet->newArc(p21, t10);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(p8, t11);
      TheNet->newArc(p19, t11);
      TheNet->newArc(t11, p20);
      TheNet->newArc(t11, p22);
      TheNet->newArc(p28, t11);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p18, t12, READ);
      TheNet->newArc(p21, t12);
      TheNet->newArc(t12, p20);

      Transition *t14 = TheNet->newTransition(prefix + "faultInCH");
      TheNet->newArc(ch_fault_in, t14);
      TheNet->newArc(p24, t14, READ);
      TheNet->newArc(t14, p26);
      TheNet->newArc(p28, t14);
      TheNet->newArc(t14, p31);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p22);
      TheNet->newArc(p26, t16);
      TheNet->newArc(t16, p27);
      TheNet->newArc(p32, t16);

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</stopPatternInsideScope scope=\"" + currentScope + "\">\n");
    }
  ]
;





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

/*
 * The user-defined fault handler inside a process. See Fig. 37 for details.
 */

This=userDefinedFaultHandler(a,b) provided (This->inProcess) ->
  [petrinet:
    {
      string prefix = "1.internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedFaultHandlerInsideProcess>\n");

      Place *p3 =  TheNet->newPlace(prefix + "final");
      Place *p4 =  TheNet->newPlace(prefix + "fault");
      Place *p7 =  TheNet->newPlace(prefix + "faultSave");
      Place *p8 =  TheNet->newPlace(prefix + "p8");
      Place *p9 =  TheNet->newPlace(prefix + "p9");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "rethrow");
      Place *p14 = TheNet->newPlace(prefix + "p14");
      Place *p15 = TheNet->newPlace(prefix + "Ended");
      Place *p16 = TheNet->newPlace(prefix + "!Ended");
      Place *p17 = TheNet->newPlace(prefix + "ch_fh");
      Place *p18 = TheNet->newPlace(prefix + "ch_out");

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p7, t7);
      TheNet->newArc(t7, p11);
      TheNet->newArc(p13, t7);
      TheNet->newArc(t7, p14);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p3);
      TheNet->newArc(p12, t8);
      TheNet->newArc(p14, t8);
      TheNet->newArc(t8, p15);
      TheNet->newArc(p16, t8);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p13, t9);
      TheNet->newArc(p15, t9, READ);


      // The rethrow part is only necessary if there is no <catchAll> element.
      if (!This->hasCatchAll)
      {
	Transition *t5 = TheNet->newTransition(prefix + "t5");
	TheNet->newArc(p4, t5);
	TheNet->newArc(p7, t5, READ);
	TheNet->newArc(t5, p8);

	Transition *t6 = TheNet->newTransition(prefix + "t6");
	TheNet->newArc(t6, p3);
	TheNet->newArc(p7, t6);
	TheNet->newArc(p9, t6);
	TheNet->newArc(t6, p15);
	TheNet->newArc(p16, t6);


	// The <compensate />-element.
	string newPrefix = "1.internal.faultHandler.compensate.";

	Place *compensate_p1 = TheNet->newPlace(newPrefix + "initial");
	Place *compensate_p2 = TheNet->newPlace(newPrefix + "running");
	Place *compensate_p3 = TheNet->newPlace(newPrefix + "final");
	Place *compensate_p4 = TheNet->newPlace(newPrefix + "stop");
	Place *compensate_p5 = TheNet->newPlace(newPrefix + "ch_fh");
	Place *compensate_p6 = TheNet->newPlace(newPrefix + "ch_out");
	Place *compensate_p7 = TheNet->newPlace(newPrefix + "stopped");

	Transition *compensate_t1 = TheNet->newTransition(newPrefix + "t1");
	TheNet->newArc(compensate_p1, compensate_t1);
	TheNet->newArc(compensate_t1, compensate_p2);
	TheNet->newArc(compensate_t1, compensate_p5);

	Transition *compensate_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(compensate_p2, compensate_t2);
	TheNet->newArc(compensate_t2, compensate_p3);
	TheNet->newArc(compensate_p6, compensate_t2);

	Transition *compensate_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(compensate_p1, compensate_t3);
	TheNet->newArc(compensate_p4, compensate_t3);
	TheNet->newArc(compensate_t3, compensate_p7);

	Transition *compensate_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(compensate_p2, compensate_t4);
	TheNet->newArc(compensate_p4, compensate_t4);
	TheNet->newArc(compensate_t4, compensate_p7);

	Transition *compensate_t5 = TheNet->newTransition(newPrefix + "t5");
	TheNet->newArc(compensate_p2, compensate_t5);
	TheNet->newArc(compensate_p3, compensate_t5);
	TheNet->newArc(compensate_t5, compensate_p7);


	// Embed the <compensate />-element.
	TheNet->mergePlaces(p8,  compensate_p1);
	TheNet->mergePlaces(p9,  compensate_p3);
	TheNet->mergePlaces(p11, compensate_p4);
	TheNet->mergePlaces(p17, compensate_p5);
	TheNet->mergePlaces(p18, compensate_p6);
	TheNet->mergePlaces(p12, compensate_p7);
      }
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = catchBranch->A_faultName;
	Transition *t1 = TheNet->newTransition(prefix + "t1.internal." + faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	// Embed innerActivity_1 to innerActivity_n-1.
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));

	TheNet->newArc(p4, t1);
	TheNet->newArc(p7, t1, READ);
	TheNet->newArc(t2, p3);
	TheNet->newArc(p7, t2);
      }

      with(b) {
	CatchAll(innerActivity):
	{
	  Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll");
	  Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");

	  // Embed innerActivity_n.
	  TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	  TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));

	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p7, t3, READ);
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(p7, t4);
	}
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandlerInsideProcess>\n");
    }
  ]
;






/*
 * The implicit fault handler inside a process. See Fig. 35 for details.
 */

This=implicitFaultHandler() provided (This->inProcess) ->
  [petrinet:
    {
      string prefix = "1.internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitFaultHandlerInsideProcess>\n");

      Place *p1 =  TheNet->newPlace(prefix + "fault");
      Place *p2 =  TheNet->newPlace(prefix + "p2");
      Place *p3 =  TheNet->newPlace(prefix + "p3");
      Place *p4 =  TheNet->newPlace(prefix + "ch_fh");
      Place *p5 =  TheNet->newPlace(prefix + "ch_out");
      Place *p6 =  TheNet->newPlace(prefix + "p6");
      Place *p7 =  TheNet->newPlace(prefix + "faultSave");
      Place *p8 =  TheNet->newPlace(prefix + "p8");
      Place *p9 =  TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "rethrow");
      Place *p11 = TheNet->newPlace(prefix + "!Ended");
      Place *p12 = TheNet->newPlace(prefix + "Ended");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1);
      TheNet->newArc(t1, p9);
      TheNet->newArc(p11, t1);
      TheNet->newArc(t1, p12);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, p3);
      TheNet->newArc(p7, t2);
      TheNet->newArc(t2, p8);
      TheNet->newArc(p10, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p6, t3);
      TheNet->newArc(t3, p9);
      TheNet->newArc(p8, t3);
      TheNet->newArc(p11, t3);
      TheNet->newArc(t3, p12);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p10, t4);
      TheNet->newArc(p12, t4, READ);


      // The <compensate />-element.
      prefix += "compensate.";

      Place *compensate_p1 = TheNet->newPlace(prefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(prefix + "running");
      Place *compensate_p3 = TheNet->newPlace(prefix + "final");
      Place *compensate_p4 = TheNet->newPlace(prefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(prefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(prefix + "stopped");

      Transition *compensate_t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(compensate_p2, compensate_t5);
      TheNet->newArc(compensate_p3, compensate_t5);
      TheNet->newArc(compensate_t5, compensate_p7);


      // Embed the <compensate />-element.
      TheNet->mergePlaces(p1, compensate_p1);
      TheNet->mergePlaces(p2, compensate_p3);
      TheNet->mergePlaces(p3, compensate_p4);
      TheNet->mergePlaces(p4, compensate_p5);
      TheNet->mergePlaces(p5, compensate_p6);
      TheNet->mergePlaces(p6, compensate_p7);

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitFaultHandlerInsideProcess>\n");
    }
  ]
;


/*---------------------------------------------------------------------------*/


/*
 * The user-defined fault handler inside a scope. See Fig. 36 for details.
 */

This=userDefinedFaultHandler(a,b) provided (!This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");

      Place *p3 =  TheNet->newPlace(prefix + "out");
      Place *p4 =  TheNet->newPlace(prefix + "fault");
      Place *p7 =  TheNet->newPlace(prefix + "trueOut");
      Place *p8 =  TheNet->newPlace(prefix + "sourceFalse");
      Place *p9 =  TheNet->newPlace(prefix + "faultSave");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "falseOut");
      Place *p13 = TheNet->newPlace(prefix + "p13");
      Place *p14 = TheNet->newPlace(prefix + "p14");
      Place *p15 = TheNet->newPlace(prefix + "rethrow");
      Place *p16 = TheNet->newPlace(prefix + "p16");
      Place *p17 = TheNet->newPlace(prefix + "upperFH");
      Place *p18 = TheNet->newPlace(prefix + "Ended");
      Place *p19 = TheNet->newPlace(prefix + "ftSave");
      Place *p20 = TheNet->newPlace(prefix + "ft");
      Place *p21 = TheNet->newPlace(prefix + "p21");
      Place *p22 = TheNet->newPlace(prefix + "p22");
      Place *p23 = TheNet->newPlace(prefix + "stopped");
      Place *p24 = TheNet->newPlace(prefix + "!Ended");
      Place *p25 = TheNet->newPlace(prefix + "ch_fh");
      Place *p26 = TheNet->newPlace(prefix + "ch_out");

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p9, t7);
      TheNet->newArc(t7, p13);
      TheNet->newArc(p15, t7);
      TheNet->newArc(t7, p16);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p8);
      TheNet->newArc(t8, p12);
      TheNet->newArc(p14, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);
      TheNet->newArc(t8, p18);
      TheNet->newArc(p24, t8);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p15, t9);
      TheNet->newArc(p18, t9, READ);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(p15, t10);
      TheNet->newArc(p19, t10, READ);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p7);
      TheNet->newArc(t11, p8);
      TheNet->newArc(p19, t11);
      TheNet->newArc(p22, t11);
      TheNet->newArc(t11, p23);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p20, t12);
      TheNet->newArc(t12, p21);


      // The rethrow part is only necessary if there is no <catchAll> element.
      if (!This->hasCatchAll)
      {
  	Transition *t5 = TheNet->newTransition(prefix + "t5");
    	TheNet->newArc(p4, t5);
      	TheNet->newArc(p9, t5, READ);
	TheNet->newArc(t5, p10);

	Transition *t6 = TheNet->newTransition(prefix + "t6");
	TheNet->newArc(t6, p8);
	TheNet->newArc(p9, t6);
	TheNet->newArc(p11, t6);
	TheNet->newArc(t6, p12);
	TheNet->newArc(t6, p17);
	TheNet->newArc(t6, p18);
	TheNet->newArc(p24, t6);


	// The left <compensate />-element.
	string newPrefix = prefix + "compensate.left.";

	Place *compensate1_p1 = TheNet->newPlace(newPrefix + "initial");
	Place *compensate1_p2 = TheNet->newPlace(newPrefix + "running");
	Place *compensate1_p3 = TheNet->newPlace(newPrefix + "final");
	Place *compensate1_p4 = TheNet->newPlace(newPrefix + "stop");
	Place *compensate1_p5 = TheNet->newPlace(newPrefix + "ch_fh");
	Place *compensate1_p6 = TheNet->newPlace(newPrefix + "ch_out");
	Place *compensate1_p7 = TheNet->newPlace(newPrefix + "stopped");

	Transition *compensate1_t1 = TheNet->newTransition(newPrefix + "t1");
	TheNet->newArc(compensate1_p1, compensate1_t1);
	TheNet->newArc(compensate1_t1, compensate1_p2);
	TheNet->newArc(compensate1_t1, compensate1_p5);

	Transition *compensate1_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(compensate1_p2, compensate1_t2);
	TheNet->newArc(compensate1_t2, compensate1_p3);
	TheNet->newArc(compensate1_p6, compensate1_t2);

	Transition *compensate1_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(compensate1_p1, compensate1_t3);
	TheNet->newArc(compensate1_p4, compensate1_t3);
	TheNet->newArc(compensate1_t3, compensate1_p7);

	Transition *compensate1_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(compensate1_p2, compensate1_t4);
	TheNet->newArc(compensate1_p4, compensate1_t4);
	TheNet->newArc(compensate1_t4, compensate1_p7);

	Transition *compensate1_t5 = TheNet->newTransition(newPrefix + "t5");
	TheNet->newArc(compensate1_p2, compensate1_t5);
	TheNet->newArc(compensate1_p3, compensate1_t5);
	TheNet->newArc(compensate1_t5, compensate1_p7);


	// Embed the left <compensate />-element.
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p10"), compensate1_p1);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p11"), compensate1_p3);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), compensate1_p4);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_fh"), compensate1_p5);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_out"), compensate1_p6);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), compensate1_p7);
      }

      // The right <compensate />-element.
      string newPrefix = prefix + "compensate.right.";

      Place *compensate2_p1 = TheNet->newPlace(newPrefix + "initial");
      Place *compensate2_p2 = TheNet->newPlace(newPrefix + "running");
      Place *compensate2_p3 = TheNet->newPlace(newPrefix + "final");
      Place *compensate2_p4 = TheNet->newPlace(newPrefix + "stop");
      Place *compensate2_p5 = TheNet->newPlace(newPrefix + "ch_fh");
      Place *compensate2_p6 = TheNet->newPlace(newPrefix + "ch_out");
      Place *compensate2_p7 = TheNet->newPlace(newPrefix + "stopped");

      Transition *compensate2_t1 = TheNet->newTransition(newPrefix + "t1");
      TheNet->newArc(compensate2_p1, compensate2_t1);
      TheNet->newArc(compensate2_t1, compensate2_p2);
      TheNet->newArc(compensate2_t1, compensate2_p5);

      Transition *compensate2_t2 = TheNet->newTransition(newPrefix + "t2");
      TheNet->newArc(compensate2_p2, compensate2_t2);
      TheNet->newArc(compensate2_t2, compensate2_p3);
      TheNet->newArc(compensate2_p6, compensate2_t2);

      Transition *compensate2_t3 = TheNet->newTransition(newPrefix + "t3");
      TheNet->newArc(compensate2_p1, compensate2_t3);
      TheNet->newArc(compensate2_p4, compensate2_t3);
      TheNet->newArc(compensate2_t3, compensate2_p7);

      Transition *compensate2_t4 = TheNet->newTransition(newPrefix + "t4");
      TheNet->newArc(compensate2_p2, compensate2_t4);
      TheNet->newArc(compensate2_p4, compensate2_t4);
      TheNet->newArc(compensate2_t4, compensate2_p7);

      Transition *compensate2_t5 = TheNet->newTransition(newPrefix + "t5");
      TheNet->newArc(compensate2_p2, compensate2_t5);
      TheNet->newArc(compensate2_p3, compensate2_t5);
      TheNet->newArc(compensate2_t5, compensate2_p7);


      // Embed the right <compensate />-element.
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), compensate2_p1);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), compensate2_p3);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_fh"), compensate2_p5);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_out"), compensate2_p6);
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = catchBranch->A_faultName;

	Transition *t1 = TheNet->newTransition(prefix + "t1.internal." + faultToCatch);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(p4, t1);
	TheNet->newArc(p9, t1, READ);

	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->newArc(t2, p3);
	TheNet->newArc(t2, p7);
	TheNet->newArc(t2, p8);
	TheNet->newArc(p9, t2);


	// Embed innerActivity_1 to innerActivity_1-n.
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
      }

      with(b) {
	CatchAll(innerActivity):
	{
	  Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll");
	  TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p9, t3, READ);

	  Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");
	  TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(t4, p7);
	  TheNet->newArc(t4, p8);
	  TheNet->newArc(p9, t4);


	  // Embed innerActivity_n.
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
	}
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");
    }
  ]
;





/*
 * The implicit fault handler inside a scope. See Fig. 34 for details.
 */

This=implicitFaultHandler() provided (!This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");

      Place *p1 =  TheNet->newPlace(prefix + "ft");
      Place *p1a = TheNet->newPlace(prefix + "fault");
      Place *p2 =  TheNet->newPlace(prefix + "p2");
      Place *p3 =  TheNet->newPlace(prefix + "p3");
      Place *p4 =  TheNet->newPlace(prefix + "ch_fh");
      Place *p5 =  TheNet->newPlace(prefix + "ch_out");
      Place *p6 =  TheNet->newPlace(prefix + "p6");
      Place *p7 =  TheNet->newPlace(prefix + "faultSave");
      Place *p8 =  TheNet->newPlace(prefix + "ftSave");
      Place *p9 =  TheNet->newPlace(prefix + "upperFH");
      Place *p10 = TheNet->newPlace(prefix + "trueOut");
      Place *p11 = TheNet->newPlace(prefix + "sourceFalse");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "rethrow");
      Place *p14 = TheNet->newPlace(prefix + "stopped");
      Place *p15 = TheNet->newPlace(prefix + "falseOut");
      Place *p16 = TheNet->newPlace(prefix + "!Ended");
      Place *p17 = TheNet->newPlace(prefix + "Ended");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1);
      TheNet->newArc(t1, p9);
      TheNet->newArc(t1, p10);
      TheNet->newArc(t1, p11);
      TheNet->newArc(p16, t1);
      TheNet->newArc(t1, p17);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p14);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(t3, p3);
      TheNet->newArc(p7, t3);
      TheNet->newArc(t3, p12);
      TheNet->newArc(p13, t3);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p9);
      TheNet->newArc(t4, p11);
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p15);
      TheNet->newArc(p16, t4);
      TheNet->newArc(t4, p17);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p8, t5, READ);
      TheNet->newArc(p13, t5);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p13, t6);
      TheNet->newArc(p17, t6, READ);


      // The <compensate />-element
      string compensate_prefix = prefix + "compensate.";

      Place *compensate_p1 = TheNet->newPlace(compensate_prefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(compensate_prefix + "running");
      Place *compensate_p3 = TheNet->newPlace(compensate_prefix + "final");
      Place *compensate_p4 = TheNet->newPlace(compensate_prefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(compensate_prefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(compensate_prefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(compensate_prefix + "stopped");

      Transition *compensate_t1 = TheNet->newTransition(compensate_prefix + "t1");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2 = TheNet->newTransition(compensate_prefix + "t2");
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3 = TheNet->newTransition(compensate_prefix + "t3");
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4 = TheNet->newTransition(compensate_prefix + "t4");
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5 = TheNet->newTransition(compensate_prefix + "t5");
      TheNet->newArc(compensate_p2, compensate_t5);
      TheNet->newArc(compensate_p3, compensate_t5);
      TheNet->newArc(compensate_t5, compensate_p7);


      // Embed the <compensate />-element
      TheNet->mergePlaces(p1, compensate_p1);
      TheNet->mergePlaces(p2, compensate_p3);
      TheNet->mergePlaces(p3, compensate_p4);
      TheNet->mergePlaces(p4, compensate_p5);
      TheNet->mergePlaces(p5, compensate_p6);
      TheNet->mergePlaces(p6, compensate_p7);

      TheNet->mergePlaces(prefix + "ft", prefix + "fault");

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitFaultHandlerInsideScope scope=\"" + currentScope + "\">\n");
    }
  ]
;





CatchAll(innerActivity) ->
  [petrinet: innerActivity]
;





/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

/*
 * Instead of creating patterns for a alarm event handler and a message event
 * handler we united both patterns to a single event handler. In order to do
 * so, we used the place and transition numberings as they are introduced in
 * Fig. 29. This situation leads to different numberings when embedding the
 * onAlarm-activities into the event handler. In this case, both names (the
 * name in Fig. 29 and in Fig. 30) are mentioned in a comment.
 */

This=userDefinedEventHandler(a,b) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<eventHandler scope=\"" + currentScope + "\">\n");

      string prefix = intToString(This->parentScopeId) + ".internal.eventHandler.";
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");

      Transition *t5 = TheNet->newTransition(prefix + "t5"); // t6 in MEH
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p17);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ);

      Transition *t7 = TheNet->newTransition(prefix + "allFinished");
      TheNet->newArc(t7, p8);
      TheNet->newArc(p18, t7);

      Transition *t11 = TheNet->newTransition(prefix + "t11"); // t13 in MEH
      TheNet->newArc(p13, t11); // finish
      TheNet->newArc(p17, t11); // running
      TheNet->newArc(t11, p18); // finishing

      Transition *t16 = TheNet->newTransition(prefix + "allStopped"); // t24 in MEH (allStopped)
      TheNet->newArc(t16, p23);

      Transition *t17 = TheNet->newTransition(prefix + "stop+finish"); // t18 in MEH (stop+finish)
      TheNet->newArc(p18, t17);
      TheNet->newArc(p22, t17);

      Transition *t18 = TheNet->newTransition(prefix + "t18");
      TheNet->newArc(p6, t18);
      TheNet->newArc(p22, t18);
      TheNet->newArc(t18, p23);

      Transition *t19 = TheNet->newTransition(prefix + "normalStop"); // t27 in MEH (normalStop)
      TheNet->newArc(p17, t19);
      TheNet->newArc(p22, t19);

      Transition *t20 = TheNet->newTransition(prefix + "t20");
      TheNet->newArc(p8, t20);
      TheNet->newArc(p22, t20);
      TheNet->newArc(t20, p23);
    }
    a // onMessage branches
    b // onAlarm branches
    {
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list a)
      {
	string message = intToString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Place *onMessage_p1  = TheNet->newPlace(newPrefix + "p1");
	Place *onMessage_p2  = TheNet->newPlace(newPrefix + "p2");
	Place *onMessage_p3  = TheNet->newPlace(newPrefix + "p3");
	Place *onMessage_p4  = TheNet->newPlace(newPrefix + "p4");
	Place *onMessage_p5  = TheNet->newPlace(newPrefix + "wait" + message );
	Place *onMessage_p6  = TheNet->newPlace(newPrefix + "p6");
	Place *onMessage_p7  = TheNet->newPlace(newPrefix + "finish" + message );
	Place *onMessage_p9  = TheNet->newPlace(newPrefix + "finish+stop" + message );
	Place *onMessage_p11 = TheNet->newPlace(newPrefix + "stop" + message );
	Place *onMessage_p12 = TheNet->newPlace(newPrefix + "p12");

	Transition *onMessage_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + ThisBranch->channelName), onMessage_t2);
	TheNet->newArc(onMessage_t2, onMessage_p2);
	TheNet->newArc(onMessage_p5, onMessage_t2);
	onMessage_t2->type = IN;

	Transition *onMessage_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + ThisBranch->variableName), onMessage_t3);
  	TheNet->newArc(onMessage_t3, TheNet->findPlace("variable." + ThisBranch->variableName));
	TheNet->newArc(onMessage_p2, onMessage_t3);
	TheNet->newArc(onMessage_t3, onMessage_p3);



	Transition *onMessage_t12 = TheNet->newTransition(newPrefix + "t12");
	TheNet->newArc(onMessage_p11, onMessage_t12);
	TheNet->newArc(onMessage_t12, onMessage_p12);

  	// When the parameter "-p cycliceh" is set, the pattern is created as
    	// it is depicted in Fig. 29/30. Setting this option leads to cyclic
      	// Petri nets.
	Transition *onMessage_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onMessage_p4, onMessage_t4);
	if (parameters[P_CYCLICEH] == true)
        {
	  TheNet->newArc(onMessage_t4, onMessage_p5); // loop
          TheNet->newArc(onMessage_p5, onMessage_t12);
        }
	else
        {
	  TheNet->newArc(onMessage_t4, onMessage_p6); // abort
          TheNet->newArc(onMessage_p6, onMessage_t12);
        }


	Transition *onMessage_t5 = TheNet->newTransition(newPrefix + "t5");
	TheNet->newArc(onMessage_p5, onMessage_t5);
	TheNet->newArc(onMessage_t5, onMessage_p6);
	TheNet->newArc(onMessage_p7, onMessage_t5);

	Transition *onMessage_t7 = TheNet->newTransition(newPrefix + "t7");
	TheNet->newArc(onMessage_p7, onMessage_t7);
	TheNet->newArc(onMessage_p9, onMessage_t7);
	TheNet->newArc(onMessage_t7, onMessage_p11);

	Transition *onMessage_t8 = TheNet->newTransition(newPrefix + "t8");
	TheNet->newArc(onMessage_p6, onMessage_t8);
	TheNet->newArc(onMessage_p9, onMessage_t8);
	TheNet->newArc(onMessage_t8, onMessage_p12);

	Transition *onMessage_t10 = TheNet->newTransition(newPrefix + "t10");
	TheNet->newArc(onMessage_p1, onMessage_t10);
	TheNet->newArc(onMessage_p11, onMessage_t10);
	TheNet->newArc(onMessage_t10, onMessage_p12);

	Transition *onMessage_t11 = TheNet->newTransition(newPrefix + "t11");
	TheNet->newArc(onMessage_p2, onMessage_t11);
	TheNet->newArc(onMessage_p11, onMessage_t11);
	TheNet->newArc(onMessage_t11, onMessage_p12);




	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t1 = throwFault(onMessage_p2, onMessage_p1, "p2", prefix, 0);

	// arcs from the EH
	TheNet->newArc(t11, onMessage_p7); // t11 in AEH is t13 in MEH
	TheNet->newArc(t5,  onMessage_p5); // t5  in AEH is t6  in MEH
	TheNet->newArc(t17, onMessage_p9); // t17 in AEH is t18 in MEH (stop+finish)
	TheNet->newArc(t19, onMessage_p11); // t19 in AEH is t27 in MEH (normalStop)

	// arcs to the EH
	TheNet->newArc(onMessage_p6, t7); // "allFinished" in AEH is "finishAll" in MEH
	TheNet->newArc(onMessage_p12, t16);

	// embed the innerActivity
	TheNet->mergePlaces(onMessage_p3,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onMessage_p11, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onMessage_p12, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onMessage_p4,  TheNet->findPlace(innerActivity, ".final"));
      }


      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";

	Place *onAlarm_p1  = TheNet->newPlace(newPrefix + "p1");
	Place *onAlarm_p2  = TheNet->newPlace(newPrefix + "p2");
	Place *onAlarm_p3  = TheNet->newPlace(newPrefix + "wait" + intToString(onAlarmCount));
	Place *onAlarm_p4  = TheNet->newPlace(newPrefix + "p4");
	Place *onAlarm_p5  = TheNet->newPlace(newPrefix + "finish" + intToString(onAlarmCount));
	Place *onAlarm_p7  = TheNet->newPlace(newPrefix + "stop+finish" + intToString(onAlarmCount));
	Place *onAlarm_p9  = TheNet->newPlace(newPrefix + "stop" + intToString(onAlarmCount));
	Place *onAlarm_p10 = TheNet->newPlace(newPrefix + "p10");

	Transition *onAlarm_t1 = TheNet->newTransition(newPrefix + "t1");
	TheNet->newArc(onAlarm_p3, onAlarm_t1);
	TheNet->newArc(onAlarm_t1, onAlarm_p1);
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), onAlarm_t1, READ);

	Transition *onAlarm_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(onAlarm_p2, onAlarm_t2);
	TheNet->newArc(onAlarm_t2, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t2);

	Transition *onAlarm_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(onAlarm_p3, onAlarm_t3);
	TheNet->newArc(onAlarm_t3, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t3);

	Transition *onAlarm_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onAlarm_p5, onAlarm_t4);
	TheNet->newArc(onAlarm_p7, onAlarm_t4);
	TheNet->newArc(onAlarm_t4, onAlarm_p9);

	Transition *onAlarm_t6 = TheNet->newTransition(newPrefix + "t6");
	TheNet->newArc(onAlarm_p4, onAlarm_t6);
	TheNet->newArc(onAlarm_p7, onAlarm_t6);
	TheNet->newArc(onAlarm_t6, onAlarm_p10);

	Transition *onAlarm_t8 = TheNet->newTransition(newPrefix + "t8");
	TheNet->newArc(onAlarm_p3, onAlarm_t8);
	TheNet->newArc(onAlarm_p9, onAlarm_t8);
	TheNet->newArc(onAlarm_t8, onAlarm_p10);


	// Arcs from the EH.
	TheNet->newArc(t5,  onAlarm_p3);
	TheNet->newArc(t11, onAlarm_p5);
	TheNet->newArc(t17, onAlarm_p7);
	TheNet->newArc(t19, onAlarm_p9);


	// Arcs to the EH.
	TheNet->newArc(onAlarm_p4,  t7);
	TheNet->newArc(onAlarm_p10, t16);


	// Embed the innerActivity.
	TheNet->mergePlaces(onAlarm_p1,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onAlarm_p9,  TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onAlarm_p10, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onAlarm_p2,  TheNet->findPlace(innerActivity, ".final"));

	onAlarmCount++;
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</eventHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;






/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

/*
 * Implicit compensation handler as it is depicted in Fig. 42.
 */

This=implicitCompensationHandler() provided (!This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitCompensationHandler scope=\"" + currentScope + "\">\n");

      Place *p2  = TheNet->newPlace(prefix + "ch_in");
      Place *p3  = TheNet->newPlace(prefix + "Compensated");
      Place *p4  = TheNet->newPlace(prefix + "!Completed");
      Place *p5  = TheNet->newPlace(prefix + "!Compensated");
      Place *p6  = TheNet->newPlace(prefix + "Completed");
      Place *p9  = TheNet->newPlace(prefix + "compensated");
      Place *p11 = TheNet->newPlace(prefix + "comp");
      Place *p13 = TheNet->newPlace(prefix + "cleaned");
      Place *p14 = TheNet->newPlace(prefix + "fh");
      Place *p15 = TheNet->newPlace(prefix + "ch_call");
      Place *p16 = TheNet->newPlace(prefix + "fh_call");
      Place *p17 = TheNet->newPlace(prefix + "ch_out");
      Place *p18 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p24 = TheNet->newPlace(prefix + "clean");


      // The following subnet is only needed if the standard faults are
      // switched on (i.e. not using the parameter "nostandardfaults").
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Place *p1  = TheNet->newPlace(prefix + "p1");

	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, 2);
	TheNet->newArc(p3, t1, READ);

	Transition *t3 = TheNet->newTransition(prefix + "t3");
	TheNet->newArc(p1, t3);
	TheNet->newArc(t3, p13);
	TheNet->newArc(p24, t3);
      }


      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p2, t4);
      TheNet->newArc(t4, p3);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(t4, p11);
      TheNet->newArc(t4, p15);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p11);
      TheNet->newArc(t6, p16);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p9);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p15, t7);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p11);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p11);
      TheNet->newArc(p15, t10, READ);
      TheNet->newArc(p18, t10);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p13);
      TheNet->newArc(p15, t11);
      TheNet->newArc(p24, t11);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p9, t15);
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString(*child));
	Place *p20 = TheNet->findPlace(intToString(*child) + ".!push");
	Place *p21 = TheNet->findPlace(intToString(*child) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString(*child));
	TheNet->newArc(p20, t7);
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new);
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</implicitCompensationHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;





/*
 * User-defined compensation handler as depicted in Fig. 42-45. Please note
 * that this pattern combines the four different compensation handler patterns.
 */

This=userDefinedCompensationHandler(innerActivity) provided (!This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedCompensationHandler scope=\"" + currentScope + "\">\n");

      Place *p2  = TheNet->newPlace(prefix + "ch_in");
      Place *p3  = TheNet->newPlace(prefix + "Compensated");
      Place *p4  = TheNet->newPlace(prefix + "!Completed");
      Place *p5  = TheNet->newPlace(prefix + "!Compensated");
      Place *p6  = TheNet->newPlace(prefix + "Completed");
      Place *p7  = TheNet->newPlace(prefix + "p7");
      Place *p8  = TheNet->newPlace(prefix + "p8");
      Place *p9  = TheNet->newPlace(prefix + "compensated");
      Place *p11 = TheNet->newPlace(prefix + "comp");
      Place *p12 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p13 = TheNet->newPlace(prefix + "cleaned");
      Place *p14 = TheNet->newPlace(prefix + "fh");
      Place *p16 = TheNet->newPlace(prefix + "fh_call");
      Place *p17 = TheNet->newPlace(prefix + "ch_out");
      Place *p24 = TheNet->newPlace(prefix + "clean");


      // The following subnet is only needed if the standard faults are
      // switched on (i.e. not using the parameter "nostandardfaults").
      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Place *p1  = TheNet->newPlace(prefix + "p1");

	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, 2);
	TheNet->newArc(p3, t1, READ);

	Transition *t3 = TheNet->newTransition(prefix + "t3");
	TheNet->newArc(p1, t3);
	TheNet->newArc(t3, p13);
	TheNet->newArc(p24, t3);
      }


      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p2, t4);
      TheNet->newArc(t4, p3);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(t4, p7);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p8, t5);
      TheNet->newArc(t5, p9);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p11);
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p16);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p11);
      TheNet->newArc(p12, t9);
      TheNet->newArc(p16, t9, READ);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p9, t15);
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);


      // Above is the standard case as depicted in Fig. 45. Below are the other
      // cases as depicted in Fig. 43 and Fig. 44.
      if (This->hasCompensateWithoutScope)
      {
	// The Pattern of Fig. 43 (with <compensate />).
	Place *p10 = TheNet->newPlace(prefix + "p10");
	Place *p12 = TheNet->newPlace(prefix + "p12");
	Place *p15 = TheNet->newPlace(prefix + "ch_call");

	Transition *t7 = TheNet->newTransition(prefix + "t7");
	TheNet->newArc(p11, t7);
	TheNet->newArc(t7, p12);
	TheNet->newArc(p15, t7, READ);

        for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
	  TheNet->newArc(TheNet->findPlace(intToString(*child) + ".!push"), t7);

	Transition *t10 = TheNet->newTransition(prefix + "t10");
	TheNet->newArc(t10, p11);
	TheNet->newArc(p12, t10);
	TheNet->newArc(p15, t10, READ);

	Transition *t11 = TheNet->newTransition(prefix + "t11");
	TheNet->newArc(t11, p10);
	TheNet->newArc(p15, t11);
	TheNet->newArc(p24, t11);
      }
      if (This->hasCompensateWithScope)
      {
	// Pattern of Fig. 44 (with <compensate scope="C"/>).
	Place *p10 = TheNet->newPlace(prefix + "p10");
	Place *p15 = TheNet->newPlace(prefix + "ch_call");

	Transition *t11 = TheNet->newTransition(prefix + "t11");
	TheNet->newArc(t4, p15);
	TheNet->newArc(p15, t5);
	TheNet->newArc(t11, p10);
	TheNet->newArc(p15, t11);
	TheNet->newArc(p24, t11);
      }


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString(*child));
	Place *p20 = TheNet->findPlace(intToString(*child) + ".!push");
	Place *p21 = TheNet->findPlace(intToString(*child) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString(*child));
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new);
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13);
      }
    }
    innerActivity
    {
      // Embed inner activity.
      TheNet->newArc(t4, TheNet->findPlace(innerActivity, ".initial"));
      TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t5);
      TheNet->mergePlaces(p24, TheNet->findPlace(innerActivity, ".stop"));
      TheNet->mergePlaces(p13, TheNet->findPlace(innerActivity, ".stopped"));

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedCompensationHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;




/*
 * Compensation handler inside of a process. Since this compensation handler
 * will never be called from a surrounding scope, the whole "left" part of the
 * pattern depicted in Fig. 42 is not necessary.
 */
This=implicitCompensationHandler() provided (This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<processCompensationHandler>\n");

      Place *p2  = TheNet->newPlace(prefix + "ch_in");
      Place *p3  = TheNet->newPlace(prefix + "Compensated");
      Place *p4  = TheNet->newPlace(prefix + "!Completed");
      Place *p5  = TheNet->newPlace(prefix + "!Compensated");
      Place *p6  = TheNet->newPlace(prefix + "Completed");
      Place *p9  = TheNet->newPlace(prefix + "compensated");
      Place *p11 = TheNet->newPlace(prefix + "comp");
      Place *p13 = TheNet->newPlace(prefix + "cleaned");
      Place *p14 = TheNet->newPlace(prefix + "fh");
      Place *p15 = TheNet->newPlace(prefix + "ch_call");
      Place *p16 = TheNet->newPlace(prefix + "fh_call");
      Place *p17 = TheNet->newPlace(prefix + "ch_out");
      Place *p18 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p24 = TheNet->newPlace(prefix + "clean");

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p11);
      TheNet->newArc(t6, p16);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p9);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p15, t7);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p11);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p11);
      TheNet->newArc(p15, t10, READ);
      TheNet->newArc(p18, t10);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p13);
      TheNet->newArc(p15, t11);
      TheNet->newArc(p24, t11);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p9, t15);
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString(*child));
	Place *p20 = TheNet->findPlace(intToString(*child) + ".!push");
	Place *p21 = TheNet->findPlace(intToString(*child) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString(*child));
	TheNet->newArc(p20, t7);
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new);
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</processCompensationHandler>\n");
    }
  ]
;



/* this pattern is just a copy from the pattern above */
This=userDefinedCompensationHandler(a) provided (This->inProcess) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<processCompensationHandler>\n");

      Place *p2  = TheNet->newPlace(prefix + "ch_in");
      Place *p3  = TheNet->newPlace(prefix + "Compensated");
      Place *p4  = TheNet->newPlace(prefix + "!Completed");
      Place *p5  = TheNet->newPlace(prefix + "!Compensated");
      Place *p6  = TheNet->newPlace(prefix + "Completed");
      Place *p9  = TheNet->newPlace(prefix + "compensated");
      Place *p11 = TheNet->newPlace(prefix + "comp");
      Place *p13 = TheNet->newPlace(prefix + "cleaned");
      Place *p14 = TheNet->newPlace(prefix + "fh");
      Place *p15 = TheNet->newPlace(prefix + "ch_call");
      Place *p16 = TheNet->newPlace(prefix + "fh_call");
      Place *p17 = TheNet->newPlace(prefix + "ch_out");
      Place *p18 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p24 = TheNet->newPlace(prefix + "clean");

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p11);
      TheNet->newArc(t6, p16);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p9);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p15, t7);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p11);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p11);
      TheNet->newArc(p15, t10, READ);
      TheNet->newArc(p18, t10);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p13);
      TheNet->newArc(p15, t11);
      TheNet->newArc(p24, t11);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p9, t15);
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);


      // Generate transitions t13 for the child scopes. This transition was
      // guarded in the original semantics and has to be unfolded for the
      // low-level representation. Therefore, each child gets its own place
      // "compScope".
      for (set<int>::iterator child = ASTEmap[This->id]->enclosedScopes.begin(); child != ASTEmap[This->id]->enclosedScopes.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString(*child));
	Place *p20 = TheNet->findPlace(intToString(*child) + ".!push");
	Place *p21 = TheNet->findPlace(intToString(*child) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString(*child));
	TheNet->newArc(p20, t7);
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new);
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13);
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</processCompensationHandler>\n");
    }
  ]
;

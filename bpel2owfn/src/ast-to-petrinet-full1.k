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
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date    
 *          - created 2005-11-01
 *          - last changed: \$Date: 2006/02/13 16:22:04 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.140 $
 * 
 * \todo
 *          - (nlohmann) outsource <compensate />-elements
 *          - (nlohmann) variables in communicating actions should be optional
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


%uview petrinet;

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
	
using namespace std;

// introduced in main.c 
extern PetriNet *TheNet;

// introduced in bpel-syntax.y
extern SymbolTable symTab;
extern SymbolManager symMan;

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

      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;
      SymbolScope *me = symMan.getScope(this->id);


      // Create the interface places.
      // The used lists are filled during parsing mapping each combination of
      // partnerLink, portType and operation to one input (resp. output) place.
      for (set<string>::iterator in = symMan.inChannels.begin(); in != symMan.inChannels.end(); in++)
	TheNet->newPlace("in." + *in, IN);
      for (set<string>::iterator out = symMan.outChannels.begin(); out != symMan.outChannels.end(); out++)
	TheNet->newPlace("out." + *out, OUT);

      
      // Create push-places for the children of the process.
      for (list<SymbolScope *>::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	TheNet->newPlace( intToString((*child)->id->value) + ".push");
	TheNet->newPlace( intToString((*child)->id->value) + ".!push");
      }

      
      // Create the places for the links.
      // The used list is filled during parsing and contains all links of the
      // whole process -- also links defined in a deeper hierarchy.
      for (list<string>::iterator link = symMan.links.begin(); link != symMan.links.end(); link++)
      {
	TheNet->newPlace("link." + *link);
	TheNet->newPlace("!link." + *link);
      }

      
      // Create variable places.
      for (list<string>::iterator variable = symMan.variables.begin(); variable != symMan.variables.end(); variable++)
	TheNet->newPlace("variable." + *variable);
      

      // Create a place for the global clock.
      TheNet->newPlace(prefix + "clock");      


      // Create the internal places of the process pattern.
      Place *p1  = TheNet->newPlace(prefix + "initial");
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
      

      // If the parameter "-p finishloop" is used, a transition
      // "1.internal.finishloop" is added to livelock the net as soon as the
      // final place of the process ("1.internal.final") is marked. With this
      // loop the net will only deadlock in the case ill-designed BPEL
      // processes (e.g. cyclic links). These deadlocks then can be found
      // using model checkers.
      if (parameters[P_FINISHLOOP])
      {
        Transition *process_loop = TheNet->newTransition(prefix + "finishloop");
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
      f // Compensation Handler
      g // Event Handler
      i // innerActivity
    {
      // After the patterns of the inner activity of the process are generated
      // the push places of the child scopes are available and have to be
      // connected with the transition t2 of the process.
      for (list<SymbolScope*>::iterator child = me->children.begin(); child != me->children.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString((*child)->id->value) + ".!push" ));

      
      // Initialize the link places of the process.
      for (list<string>::iterator link = symMan.links.begin(); link != symMan.links.end(); link++)
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
	string faultToCatch = catchBranch->faultName->name;
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
      string prefix = intToString(This->parentScopeId->value) + ".internal.faultHandler.";
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
	string faultToCatch = catchBranch->faultName->name;

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
      string prefix = intToString(This->parentScopeId->value) + ".internal.faultHandler.";
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
	    
      string prefix = intToString(This->parentScopeId->value) + ".internal.eventHandler.";
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p17);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ);

      Transition *t7 = TheNet->newTransition(prefix + "allFinished");
      TheNet->newArc(t7, p8);
      TheNet->newArc(p18, t7);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(p13, t11);
      TheNet->newArc(p17, t11);
      TheNet->newArc(t11, p18);

      Transition *t16 = TheNet->newTransition(prefix + "allStopped");
      TheNet->newArc(t16, p23);

      Transition *t17 = TheNet->newTransition(prefix + "normalStop");
      TheNet->newArc(p18, t17); 
      TheNet->newArc(p22, t17);

      Transition *t18 = TheNet->newTransition(prefix + "t18");
      TheNet->newArc(p6, t18);
      TheNet->newArc(p22, t18);
      TheNet->newArc(t18, p23);

      Transition *t19 = TheNet->newTransition(prefix + "stop+finish");
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
	TheNet->newArc(TheNet->findPlace("in." + string(ThisBranch->channelID->name)), onMessage_t2);
	TheNet->newArc(onMessage_t2, onMessage_p2);
	TheNet->newArc(onMessage_p5, onMessage_t2);

	Transition *onMessage_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), onMessage_t3);
  	TheNet->newArc(onMessage_t3, TheNet->findPlace("variable." + string(ThisBranch->variableID->name)));
	TheNet->newArc(onMessage_p2, onMessage_t3);
	TheNet->newArc(onMessage_t3, onMessage_p3);

	Transition *onMessage_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onMessage_p4, onMessage_t4);
	TheNet->newArc(onMessage_t4, onMessage_p5);

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

	Transition *onMessage_t12 = TheNet->newTransition(newPrefix + "t12");
	TheNet->newArc(onMessage_p5, onMessage_t12);
	TheNet->newArc(onMessage_p11, onMessage_t12);
	TheNet->newArc(onMessage_t12, onMessage_p12);

	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t1 = throwFault(onMessage_p2, onMessage_p1, "p2", prefix, mkinteger(0));
	
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
	string onAlarmGuard;
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y"; // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y"; // "<=" is escaped
	
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

This=implicitCompensationHandler() ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId->value) + ".internal.compensationHandler."; 
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<implicitCompensationHandler scope=\"" + currentScope + "\">\n");
      
      Place *p1  = TheNet->newPlace(prefix + "p1");
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

      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, mkinteger(2));
	TheNet->newArc(p3, t1, READ);
      }

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p24, t3);

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
      SymbolScope *me = symMan.getScope(This->parentScopeId); 
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString((*child)->id->value));
	Place *p20 = TheNet->findPlace(intToString((*child)->id->value) + ".!push");
	Place *p21 = TheNet->findPlace(intToString((*child)->id->value) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString((*child)->id->value));
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
 * user-defined compensation handler
 */

This=userDefinedCompensationHandler(innerActivity) ->
  [petrinet:
    {	    
      string prefix = intToString(This->parentScopeId->value) + ".internal.compensationHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedCompensationHandler scope=\"" + currentScope + "\">\n");
      SymbolScope *me = symMan.getScope(This->parentScopeId);

      Place *p1  = TheNet->newPlace(prefix + "p1");
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

      if (parameters[P_NOSTANDARDFAULTS] == false)
      {
	Transition *t1 = throwFault(p2, p1, "ch_in", prefix, mkinteger(2));
	TheNet->newArc(p3, t1, READ);
      }
      
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p24, t3);

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
      if (This->compensateWithoutScope)
      {
	// The Pattern of Fig. 43 (with <compensate />).
	Place *p10 = TheNet->newPlace(prefix + "p10");
	Place *p12 = TheNet->newPlace(prefix + "p12");
	Place *p15 = TheNet->newPlace(prefix + "ch_call");

	Transition *t7 = TheNet->newTransition(prefix + "t7");
	TheNet->newArc(p11, t7);
	TheNet->newArc(t7, p12);
	TheNet->newArc(p15, t7, READ);
	for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
	  TheNet->newArc(TheNet->findPlace(intToString((*child)->id->value) + ".!push"), t7);
	
	Transition *t10 = TheNet->newTransition(prefix + "t10");
	TheNet->newArc(t10, p11);
	TheNet->newArc(p12, t10);
	TheNet->newArc(p15, t10, READ);

	Transition *t11 = TheNet->newTransition(prefix + "t11");
	TheNet->newArc(t11, p10);
	TheNet->newArc(p15, t11);
	TheNet->newArc(p24, t11);
      }
      if (This->compensateWithScope)
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
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString((*child)->id->value));
	Place *p20 = TheNet->findPlace(intToString((*child)->id->value) + ".!push");
	Place *p21 = TheNet->findPlace(intToString((*child)->id->value) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString((*child)->id->value));
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new);
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13);
      }
    }
    innerActivity // the inner activity of the compensation handler
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












/*------------------- below is the positive control flow --------------------*/
/*------------------- below is the positive control flow --------------------*/
/*------------------- below is the positive control flow --------------------*/










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

This=Invoke(a,b) provided (This->outputVariable == mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".internal.";
      header(This->id);
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1); TheNet->findPlace("variable." + string(This->variableIDin->name));
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDin->name)), t1, READ);
      TheNet->newArc(TheNet->findPlace("out." + string(This->channelID->name)), t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);


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

This=Invoke(a,b) provided (This->outputVariable != mkcasestring("")) ->
  [petrinet:
    {
      header(This->id);
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
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDin->name)), t1, READ);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + string(This->channelID->name)));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(TheNet->findPlace("in." + string(This->channelID->name)), t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDout->name)), t4);
      TheNet->newArc(t4, TheNet->findPlace("variable." + string(This->variableIDout->name)));


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
      string prefix = intToString(This->id->value) + ".internal."; 
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(TheNet->findPlace("in." + string(This->channelID->name)), t1);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableID->name)), t2);
      TheNet->newArc(t2, TheNet->findPlace("variable." + string(This->variableID->name)));

      
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
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableID->name)), t1, READ);      
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, TheNet->findPlace("out." + string(This->channelID->name)));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);


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

      foreach (Copy(from,to); tCopy_list b)
      {
	string newPrefix = prefix + "copy." + intToString(copyCount) + ".";

	if (from->variable != mkcasestring(""))
	{
	  Place *p2 = TheNet->newPlace(newPrefix + "running");
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t1 = TheNet->newTransition(newPrefix + "t1");
	  TheNet->newArc(lastPlace, t1);
	  TheNet->newArc(TheNet->findPlace("variable." + string(from->variableID->name)), t1, READ);
	  TheNet->newArc(t1, p2);

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	  TheNet->newArc(p2, t2);
	  TheNet->newArc(t2, p8);
	  TheNet->newArc(TheNet->findPlace("variable." + string(to->variableID->name)), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(to->variableID->name)));

	  
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
	  TheNet->newArc(TheNet->findPlace("variable." + string(to->variableID->name)), t2);
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(to->variableID->name)));
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

This=Wait(a) provided (This->until != mkcasestring("")) ->
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

This=Wait(a) provided (This->For != mkcasestring("")) ->
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

This=Compensate(a) provided (!This->inFaultHandler && This->scope == mkcasestring("")) ->
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

This=Compensate(a) provided (This->inFaultHandler && This->scope == mkcasestring("")) ->
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

This=Compensate(a) provided (!This->inFaultHandler && This->scope != mkcasestring("")) ->
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

This=Compensate(a) provided (This->inFaultHandler && This->scope != mkcasestring("")) ->
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
	// Embed the inner activity.
	if (innerActivity->dpe->value > 0)
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".negLink"));
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
	string condition = caseBranch->condition->name;
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
	  if ((innerActivity1 != innerActivity2) && (innerActivity2->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(innerActivity2, ".negLink"));

	  if (c->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(c, ".negLink"));
	}
	
	if (innerActivity1->dpe->value > 0)
	  TheNet->newArc(TheNet->findTransition(prefix + "t2.otherwise"), TheNet->findPlace(innerActivity1, ".negLink"));
	
	currentCase++;

	if ((This->dpe->value > 0) && (innerActivity1->dpe->value > 0))
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
	if (innerActivity->dpe->value > 0)
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".negLink"));


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
	TheNet->newArc(TheNet->findPlace("in." + string(ThisBranch->channelID->name)), t2);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".internal.TimeStamp"), t2);
	TheNet->newArc(t2, p3);

	Transition *t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), t3);
  	TheNet->newArc(t3, TheNet->findPlace("variable." + string(ThisBranch->variableID->name)));
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
	string onAlarmGuard;

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y"; // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y"; // "<=" is escaped

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
      foreach (OnMessage(innerActivity1); tOnMessage_list b)
      {
	foreach (OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if ((innerActivity1 != innerActivity2) && (innerActivity2->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if (innerActivity2->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentMessage++;

	if ((innerActivity1->dpe->value > 0) && (This->dpe->value > 0))
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity1, ".negLink"));
      }
      
      int currentAlarm = 1;
      foreach (OnAlarm(innerActivity1); tOnAlarm_list c)
      {
	foreach (OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if ((innerActivity1 != innerActivity2) && (innerActivity2->dpe->value > 0))
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if (innerActivity2->dpe->value > 0)
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	currentAlarm++;

	if ((innerActivity1->dpe->value > 0) && (This->dpe->value > 0))
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
      SymbolScope *me = symMan.getScope(this->id);

      
      // Create push-places for the children of the scope.
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->id->value) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->id->value) + ".!push");
      }


      // Create the internal places of the scope pattern.
      Place *p1  = TheNet->newPlace(prefix + "negLink");
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
      if (This->hasEH)
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
      for (list<SymbolScope*>::iterator child = me->children.begin(); child != me->children.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString((*child)->id->value) + ".!push" ));
      
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
      if (This->hasEH)
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
      TheNet->mergePlaces(TheNet->findPlace(prefix + "negLink"), TheNet->findPlace(g, ".negLink"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(g, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(g, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(g, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(g, ".stopped"));

      
      // Merge (!)Terminated places
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p8);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p9);

      
      // Setting all enclosed links to false in case of an error.
      for (list<string>::iterator link = ((ScopeScope *)me)->innerLinks.begin(); link != ((ScopeScope *)me)->innerLinks.end(); link++)
      {
	Place *p61 = TheNet->newPlace(prefix + "p61.link." + *link);
	Place *p62 = TheNet->findPlace("link." + *link);
	Place *p63 = TheNet->findPlace("!link." + *link);
	Transition *t14 = TheNet->newTransition(prefix + "t14.link." + *link);
	Transition *t15 = TheNet->newTransition(prefix + "t15.link." + *link);
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

This=StandardElements(NiltTarget_list(), NiltSource_list()) ->
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

This=StandardElements(NiltTarget_list(), a) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22\n");
      string prefix = intToString(This->parentId->value) + "."; 
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p1 = TheNet->newPlace(prefix + "negLink");
      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "p3");
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


      // If needed, transition t1 is created and connected with the negLink
      // place of the inner activity if existing for dead path elimination.
      Transition *t1 = NULL;
      if (This->dpe->value > 0)
      {
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
	Place *p8 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p10 = TheNet->findPlace("link." + string(link->linkID->name));
	
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

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("no")) ->
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
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
	
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
	TheNet->mergePlaces(p5a, TheNet->findPlace(newPrefix + ".negLink"));
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

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
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
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1);
	TheNet->newArc(p3, t2);
      }


      // If the inner activity has a place negLink, connect this place with
      // transition t2 for dead path elimination.
      Place *p5 = TheNet->findPlace(newPrefix + ".negLink");
      if (p5 != NULL)
      {
	TheNet->newArc(t2, p5);
	TheNet->mergePlaces(p5, TheNet->findPlace(newPrefix + ".negLink"));
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

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
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
	
	Transition *t1 = TheNet->newTransition(prefix + "t1");
	TheNet->newArc(p9, t1);
	
	Place *p10 = TheNet->findPlace(newPrefix + ".negLink");
	if (p10 != NULL)
	  TheNet->newArc(t1, p10);
      }

      
      // Connect each target links with t2 and t5.
      foreach (link; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(t2, p1);	
	TheNet->newArc(p3, t2);
        TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }


      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (link; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p7 = TheNet->findPlace("link." + string(link->linkID->name));

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

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("no")) ->
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
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(t2, p1);	
	TheNet->newArc(p3, t2);
	TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5);
      }
      

      // Connect all source link places with t3 and t4 and, if needed, with
      // transition t1 for dead path elimination.
      foreach (link; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p7 = TheNet->findPlace("link." + string(link->linkID->name));
	
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


/*****************************************************************************/
// the printer functions

/// "fake" printer-function
void pseudoPrinter(const char *s, uview v)
{
}

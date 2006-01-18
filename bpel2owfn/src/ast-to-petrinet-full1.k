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
 *          - last changed: \$Date: 2006/01/18 10:34:44 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.107 $
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

using namespace std;

// introduced in main.c 
extern PetriNet *TheNet;

// introduced in bpel-syntax.y
extern SymbolManager symMan;

/// prefix of the current scope (read by the stop pattern and used to direct
/// failures to the correct fault_in place)
string currentScope;




/**
 * Generates transitions to throw a fault.
 * 
 * \param p1  the place in positive control flow from which the control flow
 *            enters the negative control flow
 *
 * \param p2  the place of the pattern on which a token shall be produced which
 *            can only be removed by stopping the pattern
 *
 * \param guard  guard of the transition which throws the fault (usually
 *               "guard")
 *
 * \param p1name  name of place p1 which is used to label the generated
 *                transitions (e.g. if p1 is called "running" generated
 *                transitions end with this name (e.g. "throwFault.running")
 *
 * \param inscription  inscription of the incoming arc to the fault transition
 *
 * \param prefix  prefix of the pattern to label generated places and
 *                transitions
 *
 * \param negativeControlFlow  signals where the activity is located:
 *                             - 0: inside a scope or the process
 *                             - 1: inside a fault handler
 *                             - 2: inside a compensation handler
 *
 * \param preventFurtherFaults  controls what happens to further faults
 *                               - true: these faults are prevented (standard)
 *                               - false: these faults are suppressed
*/
void throwFault(Place *p1, Place *p2,
    string guard, string p1name, string inscription, string prefix,
    integer negativeControlFlow, bool preventFurtherFaults = true)
{
  switch (negativeControlFlow->value)
  {
    case(0): // activity in scope or process
    {
      Transition *faultTransition1 = TheNet->newTransition(prefix + "throwFault." + p1name, guard);
      TheNet->newArc(TheNet->findPlace(currentScope + "Active"), faultTransition1);
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "!Active"));
      TheNet->newArc(p1, faultTransition1, inscription); // leave positive control flow
      TheNet->newArc(faultTransition1, p2); // place p2 will be connected with stop
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "stop.fault_in"), "fault");

      if (!preventFurtherFaults)
      {
	Transition *faultTransition2 = TheNet->newTransition(prefix + "ignoreFault." + p1name, guard);
	TheNet->newArc(TheNet->findPlace(currentScope + "!Active"), faultTransition2, READ);
	TheNet->newArc(p1, faultTransition2, inscription); // leave positive control flow
	TheNet->newArc(faultTransition2, p2); // place p2 will be connected with stop
      }
      break;
    }

    case(1): // activity in fault handler
    {
      Transition *faultTransition1 = TheNet->newTransition(prefix + "throwFault." + p1name, guard);
      TheNet->newArc(TheNet->findPlace(currentScope + "!FHFaulted"), faultTransition1);
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "FHFaulted"));
      TheNet->newArc(p1, faultTransition1, inscription); // leave positive control flow
      TheNet->newArc(faultTransition1, p2); // place p2 will be connected with stop
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "stop.fh_fault_in"), "fault");

      if (!preventFurtherFaults)
      {
	Transition *faultTransition2 = TheNet->newTransition(prefix + "ignoreFault." + p1name, guard);
	TheNet->newArc(TheNet->findPlace(currentScope + "FHFaulted"), faultTransition2, READ);
	TheNet->newArc(p1, faultTransition2, inscription); // leave positive control flow
	TheNet->newArc(faultTransition2, p2); // place p2 will be connected with stop
      }
      break;
    }

    case(2): // activity in compensation handler
    {
      Transition *faultTransition1 = TheNet->newTransition(prefix + "throwFault." + p1name, guard);
      TheNet->newArc(TheNet->findPlace(currentScope + "!CHFaulted"), faultTransition1);
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "CHFaulted"));
      TheNet->newArc(p1, faultTransition1, inscription); // leave positive control flow
      TheNet->newArc(faultTransition1, p2); // place p2 will be connected with stop
      TheNet->newArc(faultTransition1, TheNet->findPlace(currentScope + "stop.ch_fault_in"), "fault");

      if (!preventFurtherFaults)
      {
	Transition *faultTransition2 = TheNet->newTransition(prefix + "ignoreFault." + p1name, guard);
	TheNet->newArc(TheNet->findPlace(currentScope + "CHFaulted"), faultTransition2, READ);
	TheNet->newArc(p1, faultTransition2, inscription); // leave positive control flow
	TheNet->newArc(faultTransition2, p2); // place p2 will be connected with stop
      }
      break;
    }

    default:
    {
      /* should never happen */
    }    
  }
}



/**
 * Generates a transition to stop the activity, i.e. a transition moving a
 * token on the "stop" place to "stopped".
 * 
 * \param p  the place in control flow from which the token is move to
 *           "stop"
 *
 * \param p_name  name of place p which is used to label the generated
 *                transition (e.g. if p1 is called "running" generated
 *                transition end with this name (e.g. "stop.running")
 *
 * \param inscription  inscription of the incoming arc to the fault transition
 *
 * \param prefix  prefix of the pattern to label generated transition
 */
void stop(Place *p, string p_name, string inscription, string prefix)
{
  Transition *stopTransition = TheNet->newTransition(prefix + "stoppedAt." + p_name);
  TheNet->newArc(TheNet->findPlace(prefix + "stop"), stopTransition);
  TheNet->newArc(stopTransition, TheNet->findPlace(prefix + "stopped"));
  TheNet->newArc(p, stopTransition, inscription);
}




%}



/******************************************************************************
  PROCESS
******************************************************************************/

/*
 * Pattern of BPEL's process as it is depicted in Fig. 28. Note that it
 * contains only one event handler since alarm-event handler and message-event
 * handler are merged.
 */

This=Process(a,b,c,d,e,f,g,h,i) ->
  [petrinet:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
      trace(TRACE_DEBUG, "[PNU]\t<process>\n");
      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;
      SymbolScope *me = symMan.getScope(this->id);

      // create the interface places
      for (set<string>::iterator in = symMan.inChannels.begin(); in != symMan.inChannels.end(); in++)
	TheNet->newPlace("in." + *in, IN);
      for (set<string>::iterator out = symMan.outChannels.begin(); out != symMan.outChannels.end(); out++)
	TheNet->newPlace("out." + *out, OUT);

      // create push-places for the children of the process
      for (list<SymbolScope *>::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->id->value) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->id->value) + ".!push");
      }

      // create the places for the links
      for (list<string>::iterator link = symMan.links.begin(); link != symMan.links.end(); link++)
      {
	TheNet->newPlace("link." + *link);
	TheNet->newPlace("!link." + *link);
      }

      // create variable places
      for (list<string>::iterator variable = symMan.variables.begin(); variable != symMan.variables.end(); variable++)
	TheNet->newPlace("variable." + *variable);

      // the global clock
      Place *process_clock = TheNet->newPlace(prefix + "clock");      


      Place *process_p1  = TheNet->newPlace(prefix + "initial");
      //Place *process_p2  = TheNet->newPlace(prefix + "ch_in");
      Place *process_p3  = TheNet->newPlace(prefix + "scopeCompensated");
      Place *process_p5  = TheNet->newPlace(prefix + "upperFH");
      Place *process_p6  = TheNet->newPlace(prefix + "upperTerminate");
      Place *process_p7  = TheNet->newPlace(prefix + "Active");
      Place *process_p8  = TheNet->newPlace(prefix + "!Completed");
      Place *process_p9  = TheNet->newPlace(prefix + "Completed");
      Place *process_p10 = TheNet->newPlace(prefix + "!Compensated");
      Place *process_p11 = TheNet->newPlace(prefix + "!Active");
      Place *process_p12 = TheNet->newPlace(prefix + "Compensated");
      Place *process_p13 = TheNet->newPlace(prefix + "!Ended");
      Place *process_p14 = TheNet->newPlace(prefix + "Ended");
      Place *process_p15 = TheNet->newPlace(prefix + "!Faulted");
      Place *process_p16 = TheNet->newPlace(prefix + "Faulted");
      Place *process_p17 = TheNet->newPlace(prefix + "!Terminated");
      Place *process_p18 = TheNet->newPlace(prefix + "Terminated");
      Place *process_p19 = TheNet->newPlace(prefix + "p19");
      Place *process_p20 = TheNet->newPlace(prefix + "p20");
      Place *process_p21 = TheNet->newPlace(prefix + "p21");
      Place *process_p22 = TheNet->newPlace(prefix + "p22");
      Place *process_p23 = TheNet->newPlace(prefix + "p23");
      Place *process_p24 = TheNet->newPlace(prefix + "p24");
      Place *process_p25 = TheNet->newPlace(prefix + "p25");
      Place *process_p26 = TheNet->newPlace(prefix + "p26");
      Place *process_p27 = TheNet->newPlace(prefix + "p27");
      Place *process_p33 = TheNet->newPlace(prefix + "p33");
      Place *process_p34 = TheNet->newPlace(prefix + "p34");
      Place *process_p35 = TheNet->newPlace(prefix + "p35");
      Place *process_p36 = TheNet->newPlace(prefix + "p36");
      Place *process_p37 = TheNet->newPlace(prefix + "fault");
      Place *process_p38 = TheNet->newPlace(prefix + "faultSave");
      Place *process_p39 = TheNet->newPlace(prefix + "final");
      Place *process_p40 = TheNet->newPlace(prefix + "rethrow");
      Place *process_p41 = TheNet->newPlace(prefix + "p41");
      Place *process_p42 = TheNet->newPlace(prefix + "ch_out");

      // to prevent more than one error from the FH/CH
      Place *process_fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *process_fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *process_ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *process_ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");
      
      //Place *process_p43 = TheNet->newPlace(prefix + "compensated");
//      Place *process_p44 = TheNet->newPlace(prefix + "compScope");
      
      Transition *process_loop =  TheNet->newTransition(prefix + "finishloop"); // added so that all deadlocks are evil!
      TheNet->newArc(process_p39, process_loop, READ);
      
      Transition *process_t1 =  TheNet->newTransition(prefix + "t1");
      TheNet->newArc(process_p27, process_t1);
      TheNet->newArc(process_t1,  process_p34);

      Transition *process_t2 =  TheNet->newTransition(prefix + "t2");
      TheNet->newArc(process_p1,  process_t2);
      TheNet->newArc(process_t2,  process_p7);
      TheNet->newArc(process_t2,  process_p8);
      TheNet->newArc(process_t2,  process_p10);
      TheNet->newArc(process_t2,  process_p13);
      TheNet->newArc(process_t2,  process_p15);
      TheNet->newArc(process_t2,  process_p17);
      TheNet->newArc(process_t2,  process_p19);
      TheNet->newArc(process_t2,  process_p23);
      TheNet->newArc(process_t2,  process_fh_not_faulted);

      Transition *process_t3 =  TheNet->newTransition(prefix + "t3");
      TheNet->newArc(process_p20, process_t3);
      TheNet->newArc(process_t3,  process_p33);
      TheNet->newArc(process_t3,  process_p24);

      Transition *process_t4 =  TheNet->newTransition(prefix + "t4");
      TheNet->newArc(process_p7,  process_t4);
      TheNet->newArc(process_p8,  process_t4);
      TheNet->newArc(process_t4,  process_p9);
      TheNet->newArc(process_t4,  process_p11);
      TheNet->newArc(process_p25, process_t4);
      TheNet->newArc(process_p33, process_t4);
      TheNet->newArc(process_t4,  process_p39);

      Transition *process_t5 =  TheNet->newTransition(prefix + "t5");
      TheNet->newArc(process_p22, process_t5);
      TheNet->newArc(process_t5,  process_p26);

      Transition *process_t6 =  TheNet->newTransition(prefix + "t6");
      TheNet->newArc(process_t6,  process_p26);
      TheNet->newArc(process_p21, process_t6);
      TheNet->newArc(process_p33, process_t6);
    }
      h // Stop Pattern
      e // FaultHandler
      f // Compensation Handler
      g // Event Handler
      i // innerActivity
    {
      // connect t2 with the push-places of the child-scopes
      for (list<SymbolScope*>::iterator child = me->children.begin(); child != me->children.end(); child++)
	TheNet->newArc(process_t2, TheNet->findPlace( intToString((*child)->id->value) + ".!push" ));

      // initialize all links
      for (list<string>::iterator link = symMan.links.begin(); link != symMan.links.end(); link++)
	TheNet->newArc(process_t2, TheNet->findPlace("!link." + *link));
      
      // embed the fault handler
      TheNet->mergePlaces("1.internal.fault",     "1.internal.faultHandler.fault");
      TheNet->mergePlaces("1.internal.faultSave", "1.internal.faultHandler.faultSave");
      TheNet->mergePlaces("1.internal.rethrow",   "1.internal.faultHandler.rethrow");
      TheNet->mergePlaces("1.internal.!Ended",    "1.internal.faultHandler.!Ended");
      TheNet->mergePlaces("1.internal.Ended",     "1.internal.faultHandler.Ended");
      TheNet->mergePlaces("1.internal.p41",       "1.internal.faultHandler.ch_fh");
      TheNet->mergePlaces("1.internal.ch_out",    "1.internal.faultHandler.ch_out");
      TheNet->mergePlaces("1.internal.final",     "1.internal.faultHandler.final"); 

      // embed the compensation handler
      //TheNet->mergePlaces("1.internal.ch_in",            "1.internal.compensationHandler.ch_in");
      TheNet->mergePlaces("1.internal.Completed",        "1.internal.compensationHandler.Completed");
      TheNet->mergePlaces("1.internal.!Completed",       "1.internal.compensationHandler.!Completed");
      TheNet->mergePlaces("1.internal.Compensated",      "1.internal.compensationHandler.Compensated");
      TheNet->mergePlaces("1.internal.p41",              "1.internal.compensationHandler.fh");
      TheNet->mergePlaces("1.internal.scopeCompensated", "1.internal.compensationHandler.scopeCompensated");
      TheNet->mergePlaces("1.internal.p34",              "1.internal.compensationHandler.clean");
      TheNet->mergePlaces("1.internal.p35",              "1.internal.compensationHandler.cleaned");
      TheNet->mergePlaces("1.internal.upperFH",          "1.internal.compensationHandler.failed");
//      TheNet->mergePlaces("1.internal.compScope",        "1.internal.compensationHandler.compScope");
      TheNet->mergePlaces("1.internal.ch_out",           "1.internal.compensationHandler.ch_out");
      //TheNet->mergePlaces("1.internal.compensated",      "1.internal.compensationHandler.compensated");

      // embed event handler
      TheNet->mergePlaces("1.internal.p23",      "1.internal.eventHandler.initial");
      TheNet->mergePlaces("1.internal.p24",      "1.internal.eventHandler.finish");
      TheNet->mergePlaces("1.internal.p25",      "1.internal.eventHandler.final");
      TheNet->mergePlaces("1.internal.p26",      "1.internal.eventHandler.stop");
      TheNet->mergePlaces("1.internal.p27",      "1.internal.eventHandler.stopped");
   
      // embed the stop-pattern
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

      
      // embed the inner activity
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p19"), TheNet->findPlace(i, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p20"), TheNet->findPlace(i, ".final"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p21"), TheNet->findPlace(i, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.p22"), TheNet->findPlace(i, ".stopped"));

      trace(TRACE_DEBUG, "[PNU]\t</process>\n");
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
      Place *p18 = TheNet->newPlace(prefix + "p18");
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

      Transition *t1 =  TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1, READ);
      TheNet->newArc(fh_fault_in, t1, "x"); // was: take from p5
      TheNet->newArc(t1, p15, "x");

      Transition *t2 =  TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p21);

      Transition *t3 =  TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p3, t3);
      //TheNet->newArc(p5, t3, RESET, "X");
      TheNet->newArc(p22, t3);

      Transition *t4 =  TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t3, p4);
      TheNet->newArc(p5, t4, "x");
      //TheNet->newArc(p9, t4);
      TheNet->newArc(t4, p2);
      TheNet->newArc(t4, p6, "x");
      //TheNet->newArc(t4, p10);

      Transition *t5 =  TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p6, t5, "x");
      TheNet->newArc(p14, t5);
      TheNet->newArc(t5, p1);
      TheNet->newArc(t5, p7);
      TheNet->newArc(t5, p8, "x");

      Transition *t6 =  TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p9, t6);
      TheNet->newArc(p13, t6);
      TheNet->newArc(t6, p2);
      TheNet->newArc(t6, p10);
      TheNet->newArc(t6, p11);

      Transition *t7 =  TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p4, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p16);

      Transition *t8 =  TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p10, t8, READ);
      TheNet->newArc(p13, t8);

//      Transition *t9 =  TheNet->newTransition(prefix + "kill");
//      TheNet->newArc(p16, t9, READ);
//      TheNet->newArc(p18, t9, "x");

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      //TheNet->newArc(p5, t10, "x");
      TheNet->newArc(ch_fault_in, t10, "x");
      TheNet->newArc(p17, t10, READ);
      TheNet->newArc(t10, p18, "x");

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(p18, t11, "x");
      TheNet->newArc(p20, t11);
      TheNet->newArc(t11, p19, "x");
      TheNet->newArc(t11, p23);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p19, t12, "x");
      TheNet->newArc(p24, t12);
      TheNet->newArc(t12, p16);
      TheNet->newArc(t12, p12);
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

      Transition *t1 =  TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p6); 

      Transition *t2 =  TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p4);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p8, t2);
      TheNet->newArc(p9, t2);

      Transition *t3 =  TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p5, t3, READ);
      TheNet->newArc(fh_fault_in, t3, "x"); // was: read from p10
      TheNet->newArc(t3, p23, "x");

      Transition *t4 =  TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(t4, p29);

      Transition *t5 =  TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p7, t5);
      TheNet->newArc(t5, p8);
      //TheNet->newArc(p10, t5, RESET, "X");
      TheNet->newArc(p30, t5);

      Transition *t6 =  TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p6);
      TheNet->newArc(p10, t6, "x");
      TheNet->newArc(t6, p11, "x");
      //TheNet->newArc(p14, t6);
      //TheNet->newArc(t6, p18);

      Transition *t7 =  TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p5);
      TheNet->newArc(p8, t7);
      TheNet->newArc(p9, t7);
      TheNet->newArc(p11, t7, "x");
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p13, "x");

      Transition *t8 =  TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p6);
      TheNet->newArc(p15, t8);
      TheNet->newArc(t8, p16);

      Transition *t9 =  TheNet->newTransition(prefix + "t9");
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

//      Transition *t13 = TheNet->newTransition(prefix + "kill");
//      TheNet->newArc(p22, t13, READ);
//      TheNet->newArc(p25, t13, "x");

      Transition *t14 = TheNet->newTransition(prefix + "faultInCH");
//      TheNet->newArc(p10, t14, "x");
      TheNet->newArc(ch_fault_in, t14, "x");
      TheNet->newArc(p24, t14, READ);
      TheNet->newArc(t14, p25, "x");

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p25, t15, "x");
      TheNet->newArc(t15, p26, "x");
      TheNet->newArc(p28, t15);
      TheNet->newArc(t15, p31);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p22);
      TheNet->newArc(p26, t16, "x");
      TheNet->newArc(t16, p27, "x");
      TheNet->newArc(p32, t16);
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

      Transition *t5 = TheNet->newTransition(prefix + "t5", "rethrowComp");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p7, t5, READ, "x");
      TheNet->newArc(t5, p8);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p3);
      TheNet->newArc(p7, t6, "x");
      TheNet->newArc(p9, t6);
      TheNet->newArc(t6, p15);
      TheNet->newArc(p16, t6);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p7, t7, "x");
      TheNet->newArc(t7, p11);
      TheNet->newArc(p13, t7, "y");
      TheNet->newArc(t7, p14, "y");

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p3);
      TheNet->newArc(p12, t8);
      TheNet->newArc(p14, t8, "y");
      TheNet->newArc(t8, p15);
      TheNet->newArc(p16, t8);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p13, t9, "y");
      TheNet->newArc(p15, t9, READ);
      
      /*---------------------------------------------------------------------*/
      // the <compensate />-element
      string newPrefix = "1.internal.faultHandler.compensate.";

      Place *compensate_p1 = TheNet->newPlace(newPrefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(newPrefix + "running");
      Place *compensate_p3 = TheNet->newPlace(newPrefix + "final");
      Place *compensate_p4 = TheNet->newPlace(newPrefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(newPrefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(newPrefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(newPrefix + "stopped");

      Transition *compensate_t1  = TheNet->newTransition(newPrefix + "t1");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2  = TheNet->newTransition(newPrefix + "t2");
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3  = TheNet->newTransition(newPrefix + "t3");
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4  = TheNet->newTransition(newPrefix + "t4");
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5  = TheNet->newTransition(newPrefix + "t5");
      TheNet->newArc(compensate_p2, compensate_t5);
      TheNet->newArc(compensate_p3, compensate_t5);
      TheNet->newArc(compensate_t5, compensate_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(p8,  compensate_p1);
      TheNet->mergePlaces(p9,  compensate_p3);
      TheNet->mergePlaces(p11, compensate_p4);
      TheNet->mergePlaces(p17, compensate_p5);
      TheNet->mergePlaces(p18, compensate_p6);
      TheNet->mergePlaces(p12, compensate_p7);
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = catchBranch->faultName->name;
	Transition *t1 = TheNet->newTransition(prefix + "t1.internal." + faultToCatch, faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	// embed innerActivity_1 to innerActivity_n-1
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));

	TheNet->newArc(p4, t1);
	TheNet->newArc(p7, t1, READ, "x");	
	TheNet->newArc(t2, p3);
	TheNet->newArc(p7, t2, "x");
      }

      with(b) {
	CatchAll(innerActivity):
	{
	  Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll", "catchAll");
	  Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");
	  
	  // embed innerActivity_n
	  TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	  TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace("1.internal.faultHandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));
	  
	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p7, t3, READ, "x");
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(p7, t4, "x");
	}
	default: { /* this should never happen */ }
      }
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

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1, "x");
      TheNet->newArc(t1, p9);
      TheNet->newArc(p11, t1);
      TheNet->newArc(t1, p12);

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, p3);
      TheNet->newArc(p7, t2, "x");
      TheNet->newArc(t2, p8, "y");
      TheNet->newArc(p10, t2, "y");

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p6, t3);
      TheNet->newArc(t3, p9);
      TheNet->newArc(p8, t3, "y");
      TheNet->newArc(p11, t3);
      TheNet->newArc(t3, p12);

      Transition *t4  = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p10, t4, "y");
      TheNet->newArc(p12, t4, READ);

      /*---------------------------------------------------------------------*/
      // the <compensate />-element
      prefix += "compensate.";

      Place *compensate_p1 = TheNet->newPlace(prefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(prefix + "running");
      Place *compensate_p3 = TheNet->newPlace(prefix + "final");
      Place *compensate_p4 = TheNet->newPlace(prefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(prefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(prefix + "stopped");

      Transition *compensate_t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4  = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(compensate_p2, compensate_t5);
      TheNet->newArc(compensate_p3, compensate_t5);
      TheNet->newArc(compensate_t5, compensate_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(p1, compensate_p1);
      TheNet->mergePlaces(p2, compensate_p3);
      TheNet->mergePlaces(p3, compensate_p4);
      TheNet->mergePlaces(p4, compensate_p5);
      TheNet->mergePlaces(p5, compensate_p6);
      TheNet->mergePlaces(p6, compensate_p7);
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

      Transition *t5 = TheNet->newTransition(prefix + "t5", "rethrowComp");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p9, t5, READ, "x");
      TheNet->newArc(t5, p10);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p8);
      TheNet->newArc(p9, t6, "x");
      TheNet->newArc(p11, t6);
      TheNet->newArc(t6, p12);
      TheNet->newArc(t6, p17, "x");
      TheNet->newArc(t6, p18);
      TheNet->newArc(p24, t6);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p9, t7, "x");
      TheNet->newArc(t7, p13);
      TheNet->newArc(p15, t7, "y");
      TheNet->newArc(t7, p16, "y");

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p8);
      TheNet->newArc(t8, p12);
      TheNet->newArc(p14, t8);
      TheNet->newArc(p16, t8, "y");
      TheNet->newArc(t8, p17, "y");
      TheNet->newArc(t8, p18);
      TheNet->newArc(p24, t8);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p15, t9, "y");
      TheNet->newArc(p18, t9, READ);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(p15, t10, "y");
      TheNet->newArc(p19, t10, READ);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p7);
      TheNet->newArc(t11, p8);
      TheNet->newArc(p19, t11);
      TheNet->newArc(p22, t11);
      TheNet->newArc(t11, p23);

      Transition *t12 = TheNet->newTransition(prefix + "t12"); // forgotten
      TheNet->newArc(p20, t12);
      TheNet->newArc(t12, p21);

      /*---------------------------------------------------------------------*/
      // the left <compensate />-element
      string newPrefix = prefix + "compensate.left.";
      
      Place *compensate1_p1 = TheNet->newPlace(newPrefix + "initial");
      Place *compensate1_p2 = TheNet->newPlace(newPrefix + "running");
      Place *compensate1_p3 = TheNet->newPlace(newPrefix + "final");
      Place *compensate1_p4 = TheNet->newPlace(newPrefix + "stop");
      Place *compensate1_p5 = TheNet->newPlace(newPrefix + "ch_fh");
      Place *compensate1_p6 = TheNet->newPlace(newPrefix + "ch_out");
      Place *compensate1_p7 = TheNet->newPlace(newPrefix + "stopped");

      Transition *compensate1_t1  = TheNet->newTransition(newPrefix + "t1");
      TheNet->newArc(compensate1_p1, compensate1_t1);
      TheNet->newArc(compensate1_t1, compensate1_p2);
      TheNet->newArc(compensate1_t1, compensate1_p5);

      Transition *compensate1_t2  = TheNet->newTransition(newPrefix + "t2");
      TheNet->newArc(compensate1_p2, compensate1_t2);
      TheNet->newArc(compensate1_t2, compensate1_p3);
      TheNet->newArc(compensate1_p6, compensate1_t2);

      Transition *compensate1_t3  = TheNet->newTransition(newPrefix + "t3");
      TheNet->newArc(compensate1_p1, compensate1_t3);
      TheNet->newArc(compensate1_p4, compensate1_t3);
      TheNet->newArc(compensate1_t3, compensate1_p7);

      Transition *compensate1_t4  = TheNet->newTransition(newPrefix + "t4");
      TheNet->newArc(compensate1_p2, compensate1_t4);
      TheNet->newArc(compensate1_p4, compensate1_t4);
      TheNet->newArc(compensate1_t4, compensate1_p7);

      Transition *compensate1_t5  = TheNet->newTransition(newPrefix + "t5");
      TheNet->newArc(compensate1_p2, compensate1_t5);
      TheNet->newArc(compensate1_p3, compensate1_t5);
      TheNet->newArc(compensate1_t5, compensate1_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p10"), compensate1_p1);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p11"), compensate1_p3);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), compensate1_p4);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_fh"), compensate1_p5);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_out"), compensate1_p6);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), compensate1_p7);

      // the right <compensate />-element
      newPrefix = prefix + "compensate.right.";

      Place *compensate2_p1 = TheNet->newPlace(newPrefix + "initial");
      Place *compensate2_p2 = TheNet->newPlace(newPrefix + "running");
      Place *compensate2_p3 = TheNet->newPlace(newPrefix + "final");
      Place *compensate2_p4 = TheNet->newPlace(newPrefix + "stop");
      Place *compensate2_p5 = TheNet->newPlace(newPrefix + "ch_fh");
      Place *compensate2_p6 = TheNet->newPlace(newPrefix + "ch_out");
      Place *compensate2_p7 = TheNet->newPlace(newPrefix + "stopped");
      
      Transition *compensate2_t1  = TheNet->newTransition(newPrefix + "t1");
      TheNet->newArc(compensate2_p1, compensate2_t1);
      TheNet->newArc(compensate2_t1, compensate2_p2);
      TheNet->newArc(compensate2_t1, compensate2_p5);

      Transition *compensate2_t2  = TheNet->newTransition(newPrefix + "t2");
      TheNet->newArc(compensate2_p2, compensate2_t2);
      TheNet->newArc(compensate2_t2, compensate2_p3);
      TheNet->newArc(compensate2_p6, compensate2_t2);

      Transition *compensate2_t3  = TheNet->newTransition(newPrefix + "t3");
      TheNet->newArc(compensate2_p1, compensate2_t3);
      TheNet->newArc(compensate2_p4, compensate2_t3);
      TheNet->newArc(compensate2_t3, compensate2_p7);

      Transition *compensate2_t4  = TheNet->newTransition(newPrefix + "t4");
      TheNet->newArc(compensate2_p2, compensate2_t4);
      TheNet->newArc(compensate2_p4, compensate2_t4);
      TheNet->newArc(compensate2_t4, compensate2_p7);

      Transition *compensate2_t5  = TheNet->newTransition(newPrefix + "t5");
      TheNet->newArc(compensate2_p2, compensate2_t5);
      TheNet->newArc(compensate2_p3, compensate2_t5);
      TheNet->newArc(compensate2_t5, compensate2_p7);

      // embed the <compensate />-element
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

	Transition *t1 = TheNet->newTransition(prefix + "t1.internal." + faultToCatch, faultToCatch);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(p4, t1);
	TheNet->newArc(p9, t1, READ, "x");

	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->newArc(t2, p3);
	TheNet->newArc(t2, p7);
	TheNet->newArc(t2, p8);
	TheNet->newArc(p9, t2, "x");

	// embed innerActivity_1 to innerActivity_1-n
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
      }

      with(b) {
	CatchAll(innerActivity):
	{
	  Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll", "catchAll");
	  TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p9, t3, READ, "x");	

	  Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");	  
	  TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(t4, p7);
	  TheNet->newArc(t4, p8);
	  TheNet->newArc(p9, t4, "x");

	  // embed innerActivity_n
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
	}
	default: { /* this should never happen */ }
      }
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

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1, "x");
      TheNet->newArc(t1, p9, "x");
      TheNet->newArc(t1, p10);
      TheNet->newArc(t1, p11);
      TheNet->newArc(p16, t1);
      TheNet->newArc(t1, p17);

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p14);

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(t3, p3);
      TheNet->newArc(p7, t3, "x");
      TheNet->newArc(t3, p12, "y");
      TheNet->newArc(p13, t3, "y");

      Transition *t4  = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p9, "y");
      TheNet->newArc(t4, p11);
      TheNet->newArc(p12, t4, "y");
      TheNet->newArc(t4, p15);
      TheNet->newArc(p16, t4);
      TheNet->newArc(t4, p17);

      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p8, t5, READ);
      TheNet->newArc(p13, t5, "y");

      Transition *t6  = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p13, t6, "y");
      TheNet->newArc(p17, t6, READ);
      
      /*---------------------------------------------------------------------*/
      // the <compensate />-element
      string compensate_prefix = prefix + "compensate.";

      Place *compensate_p1 = TheNet->newPlace(compensate_prefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(compensate_prefix + "running");
      Place *compensate_p3 = TheNet->newPlace(compensate_prefix + "final");
      Place *compensate_p4 = TheNet->newPlace(compensate_prefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(compensate_prefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(compensate_prefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(compensate_prefix + "stopped");

      Transition *compensate_t1  = TheNet->newTransition(compensate_prefix + "t1");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);

      Transition *compensate_t2  = TheNet->newTransition(compensate_prefix + "t2");
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);

      Transition *compensate_t3  = TheNet->newTransition(compensate_prefix + "t3");
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);

      Transition *compensate_t4  = TheNet->newTransition(compensate_prefix + "t4");
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);

      Transition *compensate_t5  = TheNet->newTransition(compensate_prefix + "t5");
      TheNet->newArc(compensate_p2, compensate_t5);
      TheNet->newArc(compensate_p3, compensate_t5);
      TheNet->newArc(compensate_t5, compensate_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(p1, compensate_p1);
      TheNet->mergePlaces(p2, compensate_p3);
      TheNet->mergePlaces(p3, compensate_p4);
      TheNet->mergePlaces(p4, compensate_p5);
      TheNet->mergePlaces(p5, compensate_p6);
      TheNet->mergePlaces(p6, compensate_p7);

      TheNet->mergePlaces(prefix + "ft", prefix + "fault");
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


This=EventHandler(a,b) ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId->value) + ".internal.eventHandler.";
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");

      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p17);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ, "x");

      Transition *t7  = TheNet->newTransition(prefix + "allFinished");
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
	string message = intToString(onMessageCount);
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

	Transition *onMessage_t2  = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + string(ThisBranch->channelID->name)), onMessage_t2, "M" + message);
	TheNet->newArc(onMessage_t2, onMessage_p2, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_p5, onMessage_t2);

	Transition *onMessage_t3  = TheNet->newTransition(newPrefix + "t3", "!guard" + message );
	TheNet->newArc(TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), onMessage_t3, "V" + message);
  	TheNet->newArc(onMessage_t3, TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), "M" + message);
	TheNet->newArc(onMessage_p2, onMessage_t3, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_t3, onMessage_p3);

	Transition *onMessage_t4  = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onMessage_p4, onMessage_t4);
	TheNet->newArc(onMessage_t4, onMessage_p5);

	Transition *onMessage_t5  = TheNet->newTransition(newPrefix + "t5");
	TheNet->newArc(onMessage_p5, onMessage_t5);
	TheNet->newArc(onMessage_t5, onMessage_p6);
	TheNet->newArc(onMessage_p7, onMessage_t5);

	Transition *onMessage_t7  = TheNet->newTransition(newPrefix + "t7");
	TheNet->newArc(onMessage_p7, onMessage_t7);
	TheNet->newArc(onMessage_p9, onMessage_t7);
	TheNet->newArc(onMessage_t7, onMessage_p11);

	Transition *onMessage_t8  = TheNet->newTransition(newPrefix + "t8");
	TheNet->newArc(onMessage_p6, onMessage_t8);
	TheNet->newArc(onMessage_p9, onMessage_t8);
	TheNet->newArc(onMessage_t8, onMessage_p12);

	Transition *onMessage_t10 = TheNet->newTransition(newPrefix + "t10");
	TheNet->newArc(onMessage_p1, onMessage_t10);
	TheNet->newArc(onMessage_p11, onMessage_t10);
	TheNet->newArc(onMessage_t10, onMessage_p12);

	Transition *onMessage_t11 = TheNet->newTransition(newPrefix + "t11");
	TheNet->newArc(onMessage_p2, onMessage_t11, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_p11, onMessage_t11);
	TheNet->newArc(onMessage_t11, onMessage_p12);

	Transition *onMessage_t12 = TheNet->newTransition(newPrefix + "t12");
	TheNet->newArc(onMessage_p5, onMessage_t12);
	TheNet->newArc(onMessage_p11, onMessage_t12);
	TheNet->newArc(onMessage_t12, onMessage_p12);

	throwFault(onMessage_p2, onMessage_p1, "guard" + message, "p2", "(M" + message + ",CS" + message + ")", prefix, mkinteger(0)); // t1
	
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
	
	onMessageCount++;
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

	Transition *onAlarm_t1 = TheNet->newTransition(newPrefix + "t1", onAlarmGuard);
	TheNet->newArc(onAlarm_p3, onAlarm_t1, "x");
	TheNet->newArc(onAlarm_t1, onAlarm_p1);
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), onAlarm_t1, READ, "y");

	Transition *onAlarm_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(onAlarm_p2, onAlarm_t2);
	TheNet->newArc(onAlarm_t2, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t2);

	Transition *onAlarm_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(onAlarm_p3, onAlarm_t3, "x");
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
	TheNet->newArc(onAlarm_p3, onAlarm_t8, "x");
	TheNet->newArc(onAlarm_p9, onAlarm_t8);
	TheNet->newArc(onAlarm_t8, onAlarm_p10);

	// arcs from the EH
	TheNet->newArc(t5,  onAlarm_p3, "x");
	TheNet->newArc(t11, onAlarm_p5);
	TheNet->newArc(t17, onAlarm_p7);
	TheNet->newArc(t19, onAlarm_p9);

	// arcs to the EH
	TheNet->newArc(onAlarm_p4,  t7);
	TheNet->newArc(onAlarm_p10, t16);
	
	// embed the innerActivity
	TheNet->mergePlaces(onAlarm_p1,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onAlarm_p9,  TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onAlarm_p10, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onAlarm_p2,  TheNet->findPlace(innerActivity, ".final"));
	
	onAlarmCount++;
      }
    }
  ]
;





/*
 * This is an implicit eventhandler, i.e. a pattern that is inserted if no
 * event handler is given. This pattern is not inside the semantics of
 * [Sta05]. It is a "stub" with the only use of moving the tokes so that the
 * surrounding process or scope functions correctly.
 */

This=implicitEventHandler() ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId->value) + ".internal.eventHandler.";
      
      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "finish");
      Place *p3  = TheNet->newPlace(prefix + "final");
      Place *p4  = TheNet->newPlace(prefix + "stop");
      Place *p5  = TheNet->newPlace(prefix + "stopped");
      Place *p6  = TheNet->newPlace(prefix + "running");
      Place *p7  = TheNet->newPlace(prefix + "finishing");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p6);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(p6, t2);
      TheNet->newArc(t2, p7);

      Transition *t3 = TheNet->newTransition(prefix + "allFinished");
      TheNet->newArc(t3, p3);
      TheNet->newArc(p7, t3);

      Transition *t4 = TheNet->newTransition(prefix + "stop+finish");
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      TheNet->newArc(p6, t4);

      Transition *t5 = TheNet->newTransition(prefix + "normalStop");
      TheNet->newArc(p4, t5);
      TheNet->newArc(t5, p5);
      TheNet->newArc(p6, t5);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p3, t6); 
      TheNet->newArc(p4, t6);
      TheNet->newArc(t6, p5); 
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
//      Place *p19 = TheNet->newPlace(prefix + "compScope");
      Place *p24 = TheNet->newPlace(prefix + "clean");
      Place *p25 = TheNet->newPlace(prefix + "failed");
      
      Transition *t1  = TheNet->newTransition(prefix + "t1", "scope=s");
      TheNet->newArc(t1, p1); 
      TheNet->newArc(p2, t1, "s");
      TheNet->newArc(p3, t1, READ);
      TheNet->newArc(t1, p25, "fault");

      Transition *t2  = TheNet->newTransition(prefix + "t2", "scope=s");
      TheNet->newArc(p2, t2, "s");
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9, "scopeName");

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p24, t3);

      Transition *t4  = TheNet->newTransition(prefix + "t4", "scope=s");
      TheNet->newArc(p2, t4);
      TheNet->newArc(t4, p3);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(t4, p11);
      TheNet->newArc(t4, p15);

      Transition *t6  = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p11);
      TheNet->newArc(t6, p16);

      Transition *t7  = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(t7, p9, "scopeName");
      TheNet->newArc(p11, t7);
      TheNet->newArc(p15, t7);

      Transition *t8  = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9  = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p11);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9, "copeScopeName");

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p11);
      TheNet->newArc(p15, t10, READ);
      TheNet->newArc(p18, t10, "compScopeName");

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p13);
      TheNet->newArc(p15, t11);
      TheNet->newArc(p24, t11);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);

      Transition *t15 = TheNet->newTransition(prefix + "t15");
      TheNet->newArc(p9, t15, "scopeName");
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);

      // generate transitions t13 for the child scopes
      SymbolScope *me = symMan.getScope(This->parentScopeId); 
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString((*child)->id->value)); // a "compScope" for each child
	Place *p20 = TheNet->findPlace(intToString((*child)->id->value) + ".!push");
	Place *p21 = TheNet->findPlace(intToString((*child)->id->value) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString((*child)->id->value));
	TheNet->newArc(p20, t7);
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
//	TheNet->newArc(t13, p19, "s"); // this was one place for all children
	TheNet->newArc(t13, p19_new, "s");
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13, "s");
      }
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
//      Place *p19 = TheNet->newPlace(prefix + "compScope");
      Place *p24 = TheNet->newPlace(prefix + "clean");
      Place *p25 = TheNet->newPlace(prefix + "failed");
      
      Transition *t1  = TheNet->newTransition(prefix + "t1", "scope=s");
      TheNet->newArc(t1, p1);
      TheNet->newArc(p2, t1, "s");
      TheNet->newArc(p3, t1, READ);
      TheNet->newArc(t1, p25, "fault");

      Transition *t2  = TheNet->newTransition(prefix + "t2", "scope=s");
      TheNet->newArc(p2, t2, "s");
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9);

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p24, t3);

      Transition *t4  = TheNet->newTransition(prefix + "t4", "scope=s");
      TheNet->newArc(p2, t4, "s");
      TheNet->newArc(t4, p3);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(t4, p7);

      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p8, t5);
      TheNet->newArc(t5, p9);

      Transition *t6  = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(t6, p11);
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p16);

      Transition *t8  = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);

      Transition *t9  = TheNet->newTransition(prefix + "t9");
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

      // above is the standard case as depicted in Fig. 45
      if (This->compensateWithoutScope)
      {
	// pattern of Fig. 43
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
	TheNet->newArc(p12, t10, "compScopeName");
	TheNet->newArc(p15, t10, READ);

	Transition *t11 = TheNet->newTransition(prefix + "t11");
	TheNet->newArc(t11, p10);
	TheNet->newArc(p15, t11);
	TheNet->newArc(p24, t11);
      }
      if (This->compensateWithScope)
      {
	// pattern of Fig. 44
	Place *p10 = TheNet->newPlace(prefix + "p10");
	Place *p15 = TheNet->newPlace(prefix + "ch_call");

	Transition *t11 = TheNet->newTransition(prefix + "t11");
	TheNet->newArc(t4, p15);
	TheNet->newArc(p15, t5);
	TheNet->newArc(t11, p10);
	TheNet->newArc(p15, t11);
	TheNet->newArc(p24, t11);
      }
	
      
      // generate transitions t13 for the child scopes
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *p19_new = TheNet->newPlace(prefix + "compScope." + intToString((*child)->id->value)); // a "compScope" for each child
	Place *p20 = TheNet->findPlace(intToString((*child)->id->value) + ".!push");
	Place *p21 = TheNet->findPlace(intToString((*child)->id->value) + ".push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString((*child)->id->value));
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19_new, "s");
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13, "s");
      }
    }
    innerActivity // the inner activity of the compensation handler
    {
      // embed inner activity
      TheNet->newArc(t4, TheNet->findPlace(innerActivity, ".initial"));
      TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t5);
      TheNet->mergePlaces(p24, TheNet->findPlace(innerActivity, ".stop"));
      TheNet->mergePlaces(p13, TheNet->findPlace(innerActivity, ".stopped"));
    }
  ]
;












/*------------------- below is the positive control flow --------------------*/
/*------------------- below is the positive control flow --------------------*/
/*------------------- below is the positive control flow --------------------*/










/******************************************************************************
  EMPTY
******************************************************************************/

This=Empty(a) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<empty>\n");
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);

      stop(p1, "initial", "", prefix);	// t2
      stop(p2, "final", "", prefix);	// t3
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</empty>\n");
    }
  ]
;





/******************************************************************************
  INVOKE
******************************************************************************/

// asynchronous invoke (case initiate="no")
This=Invoke(a,b) provided (This->outputVariable == mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<invoke>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1); TheNet->findPlace("variable." + string(This->variableIDin->name));
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDin->name)), t1, READ, "X");
      TheNet->newArc(TheNet->findPlace("in." + string(This->channelID->name)), t1, "X");
      TheNet->newArc(t1, p2, "(X,CS)");

      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);

      throwFault(p2, p4, "guard", "running", "(X,CS)", prefix, This->negativeControlFlow); // t3      
      stop(p1, "initial", "", prefix);		// t4
      stop(p2, "running", "(X,CS)", prefix);	// t5
      stop(p3, "final", "", prefix);		// t7
      stop(p4, "p4", "", prefix);		// t6
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</invoke>\n");
    }
  ]
;

// synchronous invoke (case initiate="no")
This=Invoke(a,b) provided (This->outputVariable != mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<invoke>\n");
      string prefix = intToString(This->id->value) + ".internal.";
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running1");
      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "running2");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "p6");
      Place *p7 = TheNet->newPlace(prefix + "stop");
      Place *p8 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDin->name)), t1, READ, "X");
      TheNet->newArc(t1, p2, "(X,CS1)");

      Transition *t2  = TheNet->newTransition(prefix + "t2", "!guard1");
      TheNet->newArc(t2, TheNet->findPlace("out." + string(This->channelID->name)), "X");
      TheNet->newArc(p2, t2, "(X,CS1)");
      TheNet->newArc(t2, p3);

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(TheNet->findPlace("in." + string(This->channelID->name)), t3, "Y");
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4, "(Y,CS2)");

      Transition *t4  = TheNet->newTransition(prefix + "t4", "!guard2");
      TheNet->newArc(p4, t4, "(Y,CS2)");
      TheNet->newArc(t4, p5);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableIDout->name)), t4, "V");
      TheNet->newArc(t4, TheNet->findPlace("variable." + string(This->variableIDout->name)), "Y");

      throwFault(p4, p6, "guard2", "running2", "(Y,CS2)", prefix, This->negativeControlFlow); // t5
      throwFault(p2, p6, "guard1", "running1", "(X,CS1)", prefix, This->negativeControlFlow); // t6
      stop(p1, "initial", "", prefix);		// t7
      stop(p2, "running1", "(X,CS1)", prefix);	// t8
      stop(p3, "p3", "", prefix);		// t9
      stop(p4, "running2", "(Y,CS2)", prefix);	// t10
      stop(p5, "final", "", prefix);		// t12
      stop(p6, "p6", "", prefix);		// t11
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</invoke>\n");
    }
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

// receive in case of initiate="no"
This=Receive(a,b) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<receive>\n");
      string prefix = intToString(This->id->value) + ".internal."; 
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(TheNet->findPlace("in." + string(This->channelID->name)), t1, "X");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "(X,CS)");

      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableID->name)), t2, "V");
      TheNet->newArc(t2, TheNet->findPlace("variable." + string(This->variableID->name)), "X");

      throwFault(p2, p4, "guard", "running", "(X,CS)", prefix, This->negativeControlFlow); // t3
      stop(p1, "initial", "", prefix);		// t4
      stop(p2, "running", "(X,CS)", prefix);	// t5
      stop(p3, "final", "", prefix);		// t7
      stop(p4, "p4", "", prefix);		// t6
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</receive>\n");
    }
  ]
;





/******************************************************************************
  REPLY
******************************************************************************/

// reply in case of initiate="no"
This=Reply(a,b) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<reply>\n");
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(TheNet->findPlace("variable." + string(This->variableID->name)), t1, READ, "X");      
      TheNet->newArc(t1, p2, "(X,CS)");

      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      TheNet->newArc(t2, TheNet->findPlace("out." + string(This->channelID->name)), "X");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);
      
      throwFault(p2, p4, "guard", "running", "(X,CS)", prefix, This->negativeControlFlow); // t3      
      stop(p1, "initial", "", prefix);		// t4
      stop(p2, "running", "(X,CS)", prefix);	// t5
      stop(p3, "final", "", prefix);		// t7
      stop(p4, "p4", "", prefix);		// t6
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</reply>\n");
    }
  ]
;





/******************************************************************************
  ASSIGN
******************************************************************************/

/*
 * This pattern has been changed to fulfill the BPEL specification:
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
      trace(TRACE_DEBUG, "[PNU]\t<assign>\n");
      string prefix = intToString(This->id->value) + ".internal."; 

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");

      stop(p1, "initial", "", prefix);	// t4
      stop(p3, "final", "", prefix);	// t7
    }
    a // standardElements
    {
      int copyCount = 1;
      Place *lastPlace = TheNet->findPlace(prefix + "initial");

      foreach (copy=Copy(from,to); tCopy_list b)
      {
	string newPrefix = prefix + "copy." + intToString(copyCount) + ".";

	if (from->variable != mkcasestring(""))
	{
	  Place *p2 = TheNet->newPlace(newPrefix + "running");
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final");

	  Transition *t1 = TheNet->newTransition(newPrefix + "t1");
	  TheNet->newArc(lastPlace, t1);
	  TheNet->newArc(TheNet->findPlace("variable." + string(from->variableID->name)), t1, READ, "X");
	  TheNet->newArc(t1, p2, "X");

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2", "!guard");
	  TheNet->newArc(p2, t2, "X");
	  TheNet->newArc(t2, p8);
	  TheNet->newArc(TheNet->findPlace("variable." + string(to->variableID->name)), t2, "Y");
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(to->variableID->name)), "X");

	  throwFault(p2, p4, "guard", "running", "X", newPrefix, This->negativeControlFlow); // t3
	  stop(p4, "p4at" + intToString(copyCount), "", prefix); // t6
	  
	  lastPlace = p8;	  
	}
	else // if (from->literalValue != mkcasestring(""));
	{
	  Place *p4 = TheNet->newPlace(newPrefix + "p4");
	  Place *p8 = TheNet->newPlace(newPrefix + "final"); //new

	  Transition *t2 = TheNet->newTransition(newPrefix + "t2", "!guard");
	  TheNet->newArc(lastPlace, t2);
	  TheNet->newArc(TheNet->findPlace("variable." + string(to->variableID->name)), t2, "Y");
	  TheNet->newArc(t2, TheNet->findPlace("variable." + string(to->variableID->name)), string(from->literalValue->name));
	  TheNet->newArc(t2, p8);
	  	  
	  throwFault(lastPlace, p4, "guard", "initial", "", newPrefix, This->negativeControlFlow); // t3
	  stop(p4, "p4at" + intToString(copyCount), "", prefix); // t6
	    
	  lastPlace = p8;
	}
	copyCount++;
      }

      TheNet->mergePlaces(p3, lastPlace);
            
      trace(TRACE_DEBUG, "[PNU]\t</assign>\n");
    }
  ]
;






/******************************************************************************
  WAIT
******************************************************************************/

/*
 * The two patterns for a wait element:
 * - with an until condition (Fig. 4)
 * - with a for expression (Fig. 5)
 */

This=Wait(a) provided (This->until != mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<wait>\n"); 
      string prefix = intToString(This->id->value) + ".internal."; 
      string guard = "(" + string(This->until->name) + " \\<= x)"; // "<=" is escaped
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1", guard);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t1, READ, "x");

      stop(p1, "initial", "", prefix);	// t2
      stop(p2, "final", "", prefix);	// t3
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</wait>\n");
    }
  ]
;

This=Wait(a) provided (This->For != mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<wait>\n"); 
      string prefix = intToString(This->id->value) + ".internal."; 
      string guard = "(x + " + string(This->For->name) + " \\<= y)"; // "<=" is escaped
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "x");
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t1, READ, "x");

      Transition *t2  = TheNet->newTransition(prefix + "t2", guard);
      TheNet->newArc(p2, t2, "x");
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t2, READ, "y");

      stop(p1, "initial", "", prefix);		// t3
      stop(p2, "TimeStamp", "x", prefix);	// t4
      stop(p3, "final", "", prefix);		// t5
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</wait>\n");
    }      
  ]
;





/******************************************************************************
  THROW
******************************************************************************/

/*
 * Throw as it is depicted in Fig. 14.
 */

This=Throw(a) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<throw>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");
      
      throwFault(p1, p2, "", "initial", "", prefix, This->negativeControlFlow); // t1
      stop(p1, "initial", "", prefix);	// t2
      stop(p2, "p2", "", prefix);	// t3
    }
  a // standardElements
  {
    trace(TRACE_DEBUG, "[PNU]\t</throw>\n");
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

This=Compensate(a) provided (!This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<compensate>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "comp");
      Place *p6 = TheNet->newPlace(prefix + "done");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);

      stop(p1, "initial", "", prefix);	// t3
      stop(p2, "running", "", prefix);	// t4
      stop(p3, "final", "", prefix);	// t5
      
      // embed this <compensate /> activity in compensation handler
      TheNet->mergePlaces(p4, TheNet->findPlace(currentScope + "internal.compensationHandler.p10"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "internal.compensationHandler.p12"));
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</compensate>\n");
    }  
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<compensate>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *p6 = TheNet->newPlace(prefix + "ch_out");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);

      stop(p1, "initial", "", prefix);	// t3
      stop(p2, "running", "", prefix);	// t4
      stop(p3, "final", "", prefix);	// t5

      // embed this <compensate /> activity in fault handler
      TheNet->mergePlaces(p5, TheNet->findPlace(currentScope + "faultHandler.ch_fh"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "faultHandler.ch_out"));      
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</compensate>\n");
    }  
  ]
;

This=Compensate(a) provided (!This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<compensate>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "compScope");
      Place *p6 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, string(This->scope->name));

      Transition *t2  = TheNet->newTransition(prefix + "t2", string(This->scope->name) + "=scopeName");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2, "scopeName");

      stop(p1, "initial", "", prefix);	// t3
      stop(p2, "running", "", prefix);	// t4
      stop(p3, "final", "", prefix);	// t5
      
      // embed this <compensate scope="C"/> activity in compensation handler
      TheNet->mergePlaces(p4, TheNet->findPlace(currentScope + "internal.compensationHandler.p10"));
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</compensate>\n");
    }  
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<compensate>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_in");
      Place *p6 = TheNet->newPlace(prefix + "compensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, string(This->scope->name));

      Transition *t2  = TheNet->newTransition(prefix + "t2", string(This->scope->name) + "=scopeName");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2, "scopeName");

      stop(p1, "initial", "", prefix);	// t3
      stop(p2, "running", "", prefix);	// t4
      stop(p3, "final", "", prefix);	// t5

      // embed this <compensate scope="C"/> activity in fault handler
      TheNet->mergePlaces(p5, TheNet->findPlace(currentScope + "faultHandler.ch_fh"));
      TheNet->mergePlaces(p6, TheNet->findPlace(currentScope + "faultHandler.ch_out"));      
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</compensate>\n");
    }  
  ]
;





/******************************************************************************
  TERMINATE
******************************************************************************/

/*
 * Terminate as it is depicted in Fig. 22. Note that places p4 and p5 are
 * correctly correlated with "(!)Terminated" here.
 */

This=Terminate(a) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<terminate>\n");	    
      string prefix = intToString(This->id->value) + ".internal.";
      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "!Terminated");
      Place *p5 = TheNet->newPlace(prefix + "Terminated");
      Place *p6 = TheNet->newPlace(prefix + "terminate");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(p5, t1, READ);

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p6);

      stop(p1, "initial", "", prefix);	// t3
      stop(p2, "final", "", prefix);	// t4

      // embed the <terminate /> activity in the process      
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p4);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p5);
      TheNet->mergePlaces(TheNet->findPlace(currentScope + "upperTerminate"), p6);
    }
    a // standardElements
    {
      trace(TRACE_DEBUG, "[PNU]\t</terminate>\n");
    }  
  ]
;






/******************************************************************************
  FLOW
******************************************************************************/

/*
 * Flow as it is depicted in Fig. 17.
 */

TheActivity=activityFlow(This=Flow(a,b,c)) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<flow>\n");
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1  = TheNet->newPlace(prefix + "negLink"); //DPE//
      Place *p7  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p9  = TheNet->newPlace(prefix + "running");
      Place *p15 = TheNet->newPlace(prefix + "stop");
      Place *p16 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);

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

      stop(p7, "initial", "", prefix);	// t4
      stop(p8, "final", "", prefix);	// t7
    }
    a // standardElements
    c // inner activities
    {
      foreach (innerActivity; activity_list c)
      {
	// embed inner activity
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".negLink")); //DPE//
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(t5, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".stopped"), t6);
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t3);
      }
      trace(TRACE_DEBUG, "[PNU]\t</flow>\n"); 
    }
  ]  
;





/******************************************************************************
  SWITCH
******************************************************************************/

/*
 * Switch as it is depicted in Fig. 19. Please note that due applying rewrite
 * rules there is always an otherwise-branch (holding an empty if it is not
 * defined).
 */

This=activitySwitch(Switch(a,b,Otherwise(c))) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<switch>\n"); 
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p4 =  TheNet->newPlace(prefix + "final");
      Place *p5 =  TheNet->newPlace(prefix + "initial");
      Place *p6 =  TheNet->newPlace(prefix + "running");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");

      Transition *t3 =  TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p5, t3);
      TheNet->newArc(t3, p6, "x");

      throwFault(p6, p10, "guard", "running", "x", prefix, This->negativeControlFlow); // t7
      stop(p5, "initial", "", prefix);	// t8
      stop(p6, "running", "x", prefix);	// t9
      stop(p10, "p10", "", prefix);	// t10
    }
    a // standardElements
    b // case braches
    c // otherwise branch
    {
      int currentCase = 1;
      Transition *last;
      foreach (caseBranch=Case(innerActivity); tCase_list b)
      {
	trace(TRACE_DEBUG, "[PNU]\t<case>\n");
	string condition = caseBranch->condition->name;
	string caseNumber = intToString(currentCase);

	Transition *t2;
	Transition *t4;
	
	if (currentCase == 1)
	{
	  // two transitions: guard fulfilled or not
	  t2 = TheNet->newTransition(prefix + "t2.case1", "!guard & (" + condition + ")");
	  t4 = TheNet->newTransition(prefix + "t4.case1", "!guard & !(" + condition + ")");
	  
	  // arcs connecting "running"-place
	  TheNet->newArc(TheNet->findPlace(This, ".internal.running"), t2, "x");
	  TheNet->newArc(TheNet->findPlace(This, ".internal.running"), t4, "x");
		  
	  last = t4;
	}
	else
	{
	  // two transitions: guard fulfilled or not
	  t2 = TheNet->newTransition(prefix + "t2.case" + caseNumber, condition);
	  t4 = TheNet->newTransition(prefix + "t4.case" + caseNumber, "!(" + condition + ")");

	  // "running"-place and connecting arcs
	  Place *running =  TheNet->newPlace(prefix + "running.case" + caseNumber);
	  TheNet->newArc(last, running, "x");
	  TheNet->newArc(running, t2, "x");
	  TheNet->newArc(running, t4, "x");

	  stop(running, "runningCase" + caseNumber, "", prefix); // t11

	  last = t4;
	}
	
	// embed the innerActivity
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(This, ".internal.final",   innerActivity, ".final");
	TheNet->mergePlaces(This, ".internal.stop",    innerActivity, ".stop");
	TheNet->mergePlaces(This, ".internal.stopped", innerActivity, ".stopped");
	
	currentCase++;
	trace(TRACE_DEBUG, "[PNU]\t</case>\n");
      }

      
      trace(TRACE_DEBUG, "[PNU]\t<otherwise>\n");
      Transition *t2 = TheNet->newTransition(prefix + "t2.otherwise");
      
      // "running"-place and connecting arcs
      Place *running = TheNet->newPlace(prefix + "running.otherwise");
      TheNet->newArc(running, t2, "x");
      TheNet->newArc(last, running, "x");
      
      stop(running, "runningOtherwise", "", prefix);
      
      // embed inner activity
      TheNet->newArc(t2, TheNet->findPlace(c, ".initial"));
      TheNet->mergePlaces(This, ".internal.final",   c, ".final");
      TheNet->mergePlaces(This, ".internal.stop",    c, ".stop");
      TheNet->mergePlaces(This, ".internal.stopped", c, ".stopped");

      trace(TRACE_DEBUG, "[PNU]\t</otherwise>\n");


      // create the arcs for the DPE
      currentCase = 1;
      foreach (thisCaseBranch=Case(innerActivity1); tCase_list b)
      {
	foreach (otherCaseBranch=Case(innerActivity2); tCase_list b)
	{
	  if (innerActivity1 != innerActivity2)
	    TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(innerActivity2, ".negLink"));

	  TheNet->newArc(TheNet->findTransition(prefix + "t2.case" + intToString(currentCase)), TheNet->findPlace(c, ".negLink"));
	}
	TheNet->newArc(TheNet->findTransition(prefix + "t2.otherwise"), TheNet->findPlace(innerActivity1, ".negLink"));
	currentCase++;
      }
      
      trace(TRACE_DEBUG, "[PNU]\t</switch>\n");
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

/*
 * While as it is depicted in Fig. 18.
 */

This=activityWhile(TheWhile=While(a,b)) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<while>\n");
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

      Transition *t1 = TheNet->newTransition(prefix + "t1", "!(" + condition + ") & !guard");
      TheNet->newArc(p2, t1, "x");
      TheNet->newArc(t1, p3);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2, "x");

      Transition *t3 = TheNet->newTransition(prefix + "t3", "(" + condition + ") & !guard");
      TheNet->newArc(p2, t3, "x");
      TheNet->newArc(t3, p4);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t4, p1);
      TheNet->newArc(p5, t4);

      throwFault(p2, p6, "guard", "running", "x", prefix, This->negativeControlFlow); // t5      
      stop(p1, "initial", "", prefix);	// t6
      stop(p2, "running", "x", prefix);	// t7
      stop(p3, "final", "", prefix);	// t9
      stop(p6, "p6", "", prefix);	// t8
    }
    a // standardElements
    b // innerActivity
    {
      // embed innerActivity
      TheNet->mergePlaces(This, ".internal.p4",      b, ".initial");
      TheNet->mergePlaces(This, ".internal.p5",      b, ".final");
      TheNet->mergePlaces(This, ".internal.stop",    b, ".stop");
      TheNet->mergePlaces(This, ".internal.stopped", b, ".stopped");

      trace(TRACE_DEBUG, "[PNU]\t</while>\n");
    }
  ]  
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

/*
 * Sequence as it is depicted in Fig. 16.
 */

This=activitySequence(Sequence(a,b)) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<sequence>\n");
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1  = TheNet->newPlace(prefix + "negLink"); //DPE//
      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
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
	
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".negLink")); //DPE//
	TheNet->mergePlaces(This, ".internal.stop", innerActivity, ".stop");
	TheNet->mergePlaces(This, ".internal.stopped", innerActivity, ".stopped");
	
	lastActivity = innerActivity;
	activityCount++;
      }
      trace(TRACE_DEBUG, "[PNU]\t</sequence>\n");
    }
  ]  
;





/******************************************************************************
  PICK
******************************************************************************/

/*
 * Pick as it is depicted in Fig. 20.
 */

TheActivity=activityPick(This=Pick(a,b,c)) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<pick>\n");
      string prefix = intToString(This->id->value) + ".internal.";
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ, "x");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7, "x");

      stop(p6, "initial", "", prefix);		// t7
      stop(p7, "TimeStamp", "x", prefix);	// t8
    }
    a // standardElements
    b // onMessage branches
    c // onAlarm branches
    {
      int onMessageCount = 1; 
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list b)
      {
	trace(TRACE_DEBUG, "[PNU]\t<onMessage>\n");
	string message = intToString(onMessageCount);
	string newPrefix = prefix + "onMessage" + message + ".";
	
	Place *p3 = TheNet->newPlace(newPrefix + "p3");
	Place *p10 = TheNet->newPlace(newPrefix + "p10");

	Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + string(ThisBranch->channelID->name)), t2, "M" + message);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".internal.TimeStamp"), t2, "x");
	TheNet->newArc(t2, p3, "M" + message );

	Transition *t3 = TheNet->newTransition(newPrefix + "t3", "!guard" + message);
	TheNet->newArc(TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), t3, "V" + message);
  	TheNet->newArc(t3, TheNet->findPlace("variable." + string(ThisBranch->variableID->name)), "M" + message);
	TheNet->newArc(p3, t3, "M" + message );
	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));

	throwFault(p3, p10, "guard" + message, "p3", "M" + message, newPrefix, This->negativeControlFlow); // t4
        stop(p3, "p3at" + message, "M" + message, prefix);	// t9
	stop(p10, "p10at" + message, "", prefix);		// t10
	
	// embed the inner activity
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	onMessageCount++;
	trace(TRACE_DEBUG, "[PNU]\t</onMessage>\n");
      }


      int onAlarmCount = 1; 
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	trace(TRACE_DEBUG, "[PNU]\t<onAlarm>\n");
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";
	string onAlarmGuard;

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y"; // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y"; // "<=" is escaped

	Transition *t6 = TheNet->newTransition(newPrefix + "t6", onAlarmGuard);
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), t6, READ, "y");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".internal.TimeStamp"), t6, "x");
	TheNet->newArc(t6, TheNet->findPlace(innerActivity, ".initial"));

	// embed the inner activity	
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	onAlarmCount++;
	trace(TRACE_DEBUG, "[PNU]\t</onAlarm>\n");
      }


      // create the arcs for the DPE
      int currentMessage = 1;
      foreach (OnMessage(innerActivity1); tOnMessage_list b)
      {
	foreach (OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if (innerActivity1 != innerActivity2)
	    TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  TheNet->newArc(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t3"), TheNet->findPlace(innerActivity2, ".negLink"));
	}
	currentMessage++;
      }
      
      int currentAlarm = 1;
      foreach (OnAlarm(innerActivity1); tOnAlarm_list c)
      {
	foreach (OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if (innerActivity1 != innerActivity2)
	    TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}

	foreach (OnMessage(innerActivity2); tOnMessage_list b)
	{
	  TheNet->newArc(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), TheNet->findPlace(innerActivity2, ".negLink"));
	}
	currentAlarm++;
      }
      
     
      trace(TRACE_DEBUG, "[PNU]\t</pick>\n");
    }
  ]
;





/******************************************************************************
  SCOPE
******************************************************************************/

This=Scope(a,b,c,d,e,f,g)
 ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\t<scope>\n");
      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;
      SymbolScope *me = symMan.getScope(this->id);

      // create push-places for the children of the scope
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->id->value) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->id->value) + ".!push");
      }

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
      Place *p25 = TheNet->newPlace(prefix + "p25");
      Place *p26 = TheNet->newPlace(prefix + "p26");
      Place *p27 = TheNet->newPlace(prefix + "p27");
      Place *p28 = TheNet->newPlace(prefix + "p28");
      Place *p29 = TheNet->newPlace(prefix + "p29");
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
//      Place *p57 = TheNet->newPlace(prefix + "compScope");

      // to prevent more than one error from the FH/CH
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");
      
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p29, t1);
      TheNet->newArc(t1, p38); 

      Transition *t2  = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p13);
      TheNet->newArc(t2, p16);
      TheNet->newArc(t2, p18);
      TheNet->newArc(t2, p21);
      TheNet->newArc(t2, p25);
      TheNet->newArc(t2, fh_not_faulted);

      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p22, t3);
      TheNet->newArc(t3, p26);
      TheNet->newArc(t3, p35);

      Transition *t4  = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p10, t4);
      TheNet->newArc(p11, t4);
      TheNet->newArc(t4, p12);
      TheNet->newArc(t4, p14);
      TheNet->newArc(TheNet->findPlace(intToString(This->id->value) + ".!push"), t4);
      TheNet->newArc(t4, TheNet->findPlace(intToString(This->id->value) + ".push"));
      TheNet->newArc(p27, t4);
      TheNet->newArc(p35, t4);
      TheNet->newArc(t4, p51);

      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p24, t5);
      TheNet->newArc(t5, p28);

      Transition *t6  = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p23, t6);
      TheNet->newArc(t6, p28);
      TheNet->newArc(p35, t6);

      Transition *t7  = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p8, t7, READ);
      TheNet->newArc(p10, t7);
      TheNet->newArc(t7, p14);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p36); 

      Transition *t8  = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p9, t8, READ);
      TheNet->newArc(p10, t8);
      TheNet->newArc(t8, p14);
      TheNet->newArc(p20, t8);
      TheNet->newArc(t8, p37);

      Transition *t9  = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p2, t9);
      TheNet->newArc(p20, t9);
      TheNet->newArc(t9, p45);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(p17, t10, READ);
      TheNet->newArc(p20, t10);
      TheNet->newArc(t10, p45);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(p20, t11);
      TheNet->newArc(t11, p45);
      TheNet->newArc(p51, t11);

      Transition *t16 = TheNet->newTransition(prefix + "t16");      
      TheNet->newArc(p53, t16, "false");
      
      // new transitions to pass fault to parent
      Transition *t12 = TheNet->newTransition(prefix + "passFaultUp");
      TheNet->newArc(p48, t12);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.Active"), t12);
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active")); //newer fault management
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.stop.fault_in"));

      Transition *t13 = TheNet->newTransition(prefix + "ignoreFault");
      TheNet->newArc(p48, t13);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active"), t13, READ);

      // merge places with parent scope
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

      // connect t2 with the push-places of the child-scopes
      for (list<SymbolScope*>::iterator child = me->children.begin(); child != me->children.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString((*child)->id->value) + ".!push" ));
      
      string prefix2 = intToString(This->id->value) + ".internal.";
	    
      // embed fault handler
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

      // embed the compensation handler
      TheNet->mergePlaces(prefix + "ch_in",            prefix2 + "compensationHandler.ch_in");
      TheNet->mergePlaces(prefix + "Completed",        prefix2 + "compensationHandler.Completed");
      TheNet->mergePlaces(prefix + "!Completed",       prefix2 + "compensationHandler.!Completed");
      TheNet->mergePlaces(prefix + "Compensated",      prefix2 + "compensationHandler.Compensated");
      TheNet->mergePlaces(prefix + "!Compensated",     prefix2 + "compensationHandler.!Compensated");
      TheNet->mergePlaces(prefix + "p49",              prefix2 + "compensationHandler.fh");
      TheNet->mergePlaces(prefix + "scopeCompensated", prefix2 + "compensationHandler.scopeCompensated");
      TheNet->mergePlaces(prefix + "p39",              prefix2 + "compensationHandler.clean");
      TheNet->mergePlaces(prefix + "p40",              prefix2 + "compensationHandler.cleaned");
      TheNet->mergePlaces(prefix + "p6",               prefix2 + "compensationHandler.failed");
//      TheNet->mergePlaces(prefix + "compScope",        prefix2 + "compensationHandler.compScope");
      TheNet->mergePlaces(prefix + "p50",              prefix2 + "compensationHandler.ch_out");
      TheNet->mergePlaces(prefix + "compensated",      prefix2 + "compensationHandler.compensated");

      // embed event handler
      TheNet->mergePlaces(prefix + "p25", prefix2 + "eventHandler.initial");
      TheNet->mergePlaces(prefix + "p26", prefix2 + "eventHandler.finish");
      TheNet->mergePlaces(prefix + "p27", prefix2 + "eventHandler.final");
      TheNet->mergePlaces(prefix + "p28", prefix2 + "eventHandler.stop");
      TheNet->mergePlaces(prefix + "p29", prefix2 + "eventHandler.stopped");
      
      // embed the stop pattern
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
      TheNet->mergePlaces(prefix + "p41",            prefix + "stop.ft"); // switched this line with the line below
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
      
      // embed the inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "negLink"), TheNet->findPlace(g, ".negLink"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(g, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(g, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(g, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(g, ".stopped"));

      // merge (!)Terminated places
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p8);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p9);

      // setting all enclosed links to false in case of an error
      for (list<string>::iterator link = ((ScopeScope *)me)->innerLinks.begin(); link != ((ScopeScope *)me)->innerLinks.end(); link++)
      {
	Place *p61 = TheNet->newPlace(prefix + "p61.link." + *link);
	Place *p62 = TheNet->findPlace("link." + *link);
	Place *p63 = TheNet->findPlace("!link." + *link);
	Transition *t14 = TheNet->newTransition(prefix + "t14.link." + *link);
	Transition *t15 = TheNet->newTransition(prefix + "t15.link." + *link);
	TheNet->newArc(p61, t14, "b");
	TheNet->newArc(t14, p62, "b");
	TheNet->newArc(p63, t14);
	TheNet->newArc(p61, t15, "b");
	TheNet->newArc(t16, p61, "b");
	TheNet->newArc(p62, t15, READ, "b");
      }      

      currentScope = intToString(This->parentScopeId->value) + ".internal.";
      trace(TRACE_DEBUG, "[PNU]\t</scope>\n");
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
      Place *p1 = TheNet->newPlace(prefix + "negLink");
      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p1, TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p2, TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p3, TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p4, TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p5, TheNet->findPlace(prefix + ".stopped"));
    }
  ]
;

This=StandardElements(NiltTarget_list(), a) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\tFig. 22\n");
      string prefix = intToString(This->parentId->value) + "."; 
      Place *p1 = TheNet->newPlace(prefix + "negLink");
      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "stop");
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p3);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p5);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p5);

      stop(p5, "final", "", prefix);

      foreach (link; tSource_list a)
      {
	Place *p8 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p10 = TheNet->findPlace("link." + string(link->linkID->name));
	TheNet->newArc(p8, t1);
	TheNet->newArc(t1, p10, "false");
	TheNet->newArc(p8, t2);
	TheNet->newArc(t2, p10, string(link->transitionCondition->name));
	TheNet->newArc(p10, t3, READ);
      }

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p3, TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p2, TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p4, TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p6, TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p7, TheNet->findPlace(prefix + ".stopped"));
   }
  ]
;

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("no")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\tFig. 23\n");
      string prefix = intToString(This->parentId->value) + ".";
      Place *p5  = TheNet->newPlace(prefix + "negLink");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");
      Place *p11 = TheNet->newPlace(prefix + "p11");

      Transition *t1 = TheNet->newTransition(prefix + "t1", string(This->joinCondition->name));
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1); 

      throwFault(p8, p11, "!(" + string(This->joinCondition->name) + ")", "initial", "", prefix, This->negativeControlFlow);

      stop(p8, "initial", "", prefix);
      stop(p11, "p11", "", prefix);
      
      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1, "b" + intToString(i));

	Transition* faultTransition = TheNet->findTransition(prefix + "throwFault.initial");
	TheNet->newArc(faultTransition, p1);
	TheNet->newArc(p3, faultTransition, "b" + intToString(i));

	i++;
      }

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p5,  TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p6,  TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p7,  TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p9,  TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p10, TheNet->findPlace(prefix + ".stopped"));
    }
  ]
;

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\tFig. 24\n");
      string prefix = intToString(This->parentId->value) + ".";
      Place *p5  = TheNet->newPlace(prefix + "negLink");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");

      Transition *t1 = TheNet->newTransition(prefix + "t1", string(This->joinCondition->name));
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1);

      Transition *t2 = TheNet->newTransition(prefix + "t2", "!(" + string(This->joinCondition->name) + ")");
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(t2, p7);

      stop(p8, "initial", "", prefix);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
	TheNet->newArc(t1, p1);
	TheNet->newArc(p3, t1, "b" + intToString(i));
	TheNet->newArc(p3, t2, "b" + intToString(i));
	i++;
      }

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p5, TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p6, TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p7, TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p9, TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p10, TheNet->findPlace(prefix + ".stopped"));
    }
  ]
;

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\tFig. 25\n");
      string prefix = intToString(This->parentId->value) + ".";
      Place *p9  = TheNet->newPlace(prefix + "negLink");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p9, t1);
      TheNet->newArc(t1, p10);

      Transition *t2 = TheNet->newTransition(prefix + "t2", string(This->joinCondition->name));
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);

      Transition *t5 = TheNet->newTransition(prefix + "t5", "!(" + string(This->joinCondition->name) + ")");
      TheNet->newArc(t5, p10);
      TheNet->newArc(t5, p13);
      TheNet->newArc(p16, t5);

      stop(p16, "initial", "", prefix);
      stop(p13, "final", "", prefix);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(t2, p1);	
	TheNet->newArc(p3, t2, "b" + intToString(i));
        TheNet->newArc(t5, p1);
	TheNet->newArc(p3, t5, "b" + intToString(i));
	i++;
      }

      i = 1;
      foreach (link; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p7 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(p5, t1);
        TheNet->newArc(t1, p7, "false");
        TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7, string(link->transitionCondition->name));
        TheNet->newArc(p5, t5);
        TheNet->newArc(t5, p7, "false");
	TheNet->newArc(p7, t4, READ);
	i++;
      }

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p10, TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p11, TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p12, TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p14, TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p15, TheNet->findPlace(prefix + ".stopped"));
    }
  ]
;

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("no")) ->
  [petrinet:
    {
      trace(TRACE_DEBUG, "[PNU]\tFig. 26\n");
      string prefix = intToString(This->parentId->value) + ".";
      Place *p9  = TheNet->newPlace(prefix + "negLink");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");
      Place *p17 = TheNet->newPlace(prefix + "p17");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p9, t1);
      TheNet->newArc(t1, p10);

      Transition *t2 = TheNet->newTransition(prefix + "t2", string(This->joinCondition->name));
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);

      throwFault(p16, p17, "!(" + string(This->joinCondition->name) + ")", "initial", "", prefix, This->negativeControlFlow);

      stop(p16, "initial", "", prefix);
      stop(p17, "p17", "", prefix);
      stop(p13, "final", "", prefix);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *p1 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p3 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(t2, p1);	
	TheNet->newArc(p3, t2, "b" + intToString(i));
	
	Transition* faultTransition = TheNet->findTransition(prefix + "throwFault.initial");
	TheNet->newArc(faultTransition, p1);
	TheNet->newArc(p3, faultTransition, "b" + intToString(i));

	i++;
      }
      
      i = 1;
      foreach (link; tSource_list b)
      {
	Place *p5 = TheNet->findPlace("!link." + string(link->linkID->name));
	Place *p7 = TheNet->findPlace("link." + string(link->linkID->name));
        TheNet->newArc(p5, t1);
        TheNet->newArc(t1, p7, "false");
        TheNet->newArc(p5, t3);
        TheNet->newArc(t3, p7, string(link->transitionCondition->name));
	TheNet->newArc(p7, t4, READ);
	i++;
      }

      // embed activity X
      prefix = intToString(This->parentId->value) + ".internal";
      TheNet->mergePlaces(p10, TheNet->findPlace(prefix + ".negLink"));
      TheNet->mergePlaces(p11, TheNet->findPlace(prefix + ".initial"));
      TheNet->mergePlaces(p12, TheNet->findPlace(prefix + ".final"));
      TheNet->mergePlaces(p14, TheNet->findPlace(prefix + ".stop"));
      TheNet->mergePlaces(p15, TheNet->findPlace(prefix + ".stopped"));
    }
  ]
;


/*****************************************************************************/
// the printer functions

/// "fake" printer-function
void pseudoPrinter(const char *s, uview v)
{
}

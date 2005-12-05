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
 *          - last changed: \$Date: 2005/12/05 16:58:40 $
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
 * \version \$Revision: 1.50 $
 *          - 2005-11-10 (nlohmann) Added doxygen comments.
 *          - 2005-11-11 (nlohmann) Changed patterns to respect changes in
 *            \ref petrinet.c
 *          - 2005-11-15 (nlohmann) Removed prefix "empty" etc.
 *          - 2005-11-16 (nlohmann) Added pattern for <while>, <wait>, <flow>,
 *            <throw>, <terminate>
 *          - 2005-11-17 (nlohmann) Completed simplest <process>-pattern.
 *          - 2005-11-18 (nlohmann) Added pattern for <switch>.
 *          - 2005-11-20 (nlohmann) Added pattern for user-defined
 *            <faultHandlers>.
 *          - 2005-11-22 (nlohmann) Added pattern for implicit <faultHandlers>,
 *            <pick> and alarm event handler.
 *          - 2005-11-23 (nlohmann) Added <eventHandlers> (both alarm and
 *            message).
 *          - 2005-11-26 (nlohmann) Simplified code. Added implcit
 *            <compensationHandler>. Faults are now passed to the 1.
 *          - 2005-11-29 (nlohmann) Fixed several bugs such as the loop in the
 *            implicit compensation handler. Fixed order of arguments of the
 *            mergePlaces-function to assure correct labels.
 *          - 2005-11-30 (nlohmann) Added two phyla for stop patterns. Added
 *            <scope> with its stop pattern. Added support for faultHandlers
 *            in scope.
 *          - 2005-12-03 (nlohmann) Cleaned code. Fixed several bugs inside the
 *            event handler.
 *          - 2005-12-04 (nlohmann) Re-organized fault handler's catchAll
 *            branch.
 * 
 * \todo
 *          - (nlohmann) complete initial pattern database
 *          - (nlohmann) The assign-element has more attributes that have to
 *            be added to the abstract grammar as well as to the syntax file.
 *          - (nlohmann) outsource <compensate />-elements
 *
 * \bug
 *          - the child of a process should always be a scope, not a flow!
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


%uview petrinet;

/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include <string>
#include "petrinet.h"
#include "check-symbols.h"

using namespace std;

// introduced in main.c 
extern PetriNet *TheNet;

// introduced in bpel-syntax.y
extern SymbolManager symMan;

/// prefix of the current scope (read by the stop pattern)
string currentScope;
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
      
      string prefix = intToString(This->id->value) + ".";
      SymbolScope *me = symMan.getScope(this->id);

      // create push-places for the children of the process
      for (list<SymbolScope *>::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->id->value) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->id->value) + ".!push");
      }

      Place *process_p1  = TheNet->newPlace(prefix + "initial");
      Place *process_p2  = TheNet->newPlace(prefix + "ch_in");
      Place *process_p3  = TheNet->newPlace(prefix + "scopeCompensated");
      Place *process_p5  = TheNet->newPlace(prefix + "p5");
      Place *process_p6  = TheNet->newPlace(prefix + "terminate");
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
      Place *process_p43 = TheNet->newPlace(prefix + "compensated");
      Place *process_p44 = TheNet->newPlace(prefix + "compScope");
      Transition *process_t1 =  TheNet->newTransition(prefix + "t1");
      Transition *process_t2 =  TheNet->newTransition(prefix + "t2");
      Transition *process_t3 =  TheNet->newTransition(prefix + "t3");
      Transition *process_t4 =  TheNet->newTransition(prefix + "t4");
      Transition *process_t5 =  TheNet->newTransition(prefix + "t5");
      Transition *process_t6 =  TheNet->newTransition(prefix + "t6");   
      TheNet->newArc(process_p27, process_t1);
      TheNet->newArc(process_t1,  process_p34);
      TheNet->newArc(process_p1,  process_t2);
      TheNet->newArc(process_t2,  process_p7);
      TheNet->newArc(process_t2,  process_p8);
      TheNet->newArc(process_t2,  process_p10);
      TheNet->newArc(process_t2,  process_p13);
      TheNet->newArc(process_t2,  process_p15);
      TheNet->newArc(process_t2,  process_p17);
      TheNet->newArc(process_t2,  process_p19);
      TheNet->newArc(process_t2,  process_p23);      
      TheNet->newArc(process_p20, process_t3);
      TheNet->newArc(process_t3,  process_p33);
      TheNet->newArc(process_t3,  process_p24);
      TheNet->newArc(process_p7,  process_t4);
      TheNet->newArc(process_p8,  process_t4);
      TheNet->newArc(process_t4,  process_p9);
      TheNet->newArc(process_t4,  process_p11);
      TheNet->newArc(process_p25, process_t4);
      TheNet->newArc(process_p33, process_t4);
      TheNet->newArc(process_t4,  process_p39);
      TheNet->newArc(process_p22, process_t5);
      TheNet->newArc(process_t5,  process_p26);
      TheNet->newArc(process_t6,  process_p26);
      TheNet->newArc(process_p21, process_t6);
      TheNet->newArc(process_p33, process_t6);

      // special places
      Place *process_clock = TheNet->newPlace(prefix + "clock");
    }
      e // FaultHandler
      f // Compensation Handler
      g // Event Handler
      h // Stop Pattern
      i // innerActivity
    {
      // connect t2 with the push-places of the child-scopes
      for (list<SymbolScope*>::iterator child = me->children.begin(); child != me->children.end(); child++)
	TheNet->newArc(process_t2, TheNet->findPlace( intToString((*child)->id->value) + ".!push" ));

      // embed the fault handler
      TheNet->mergePlaces("1.fault",     "1.faulthandler.fault");
      TheNet->mergePlaces("1.faultSave", "1.faulthandler.faultSave");
      TheNet->mergePlaces("1.rethrow",   "1.faulthandler.rethrow");
      TheNet->mergePlaces("1.!Ended",    "1.faulthandler.!Ended");
      TheNet->mergePlaces("1.Ended",     "1.faulthandler.Ended");
      TheNet->mergePlaces("1.p41",       "1.faulthandler.ch_fh");
      TheNet->mergePlaces("1.ch_out",    "1.faulthandler.ch_out");
      TheNet->mergePlaces("1.final",     "1.faulthandler.final");      

      // embed the compensation handler
      TheNet->mergePlaces("1.ch_in",            "1.compensationHandler.ch_in");
      TheNet->mergePlaces("1.Completed",        "1.compensationHandler.Completed");
      TheNet->mergePlaces("1.!Completed",       "1.compensationHandler.!Completed");
      TheNet->mergePlaces("1.Compensated",      "1.compensationHandler.Compensated");
      TheNet->mergePlaces("1.p41",              "1.compensationHandler.fh");
      TheNet->mergePlaces("1.scopeCompensated", "1.compensationHandler.scopeCompensated");
      TheNet->mergePlaces("1.p34",              "1.compensationHandler.clean");
      TheNet->mergePlaces("1.p35",              "1.compensationHandler.cleaned");
      TheNet->mergePlaces("1.p5",               "1.compensationHandler.failed");
      TheNet->mergePlaces("1.compScope",        "1.compensationHandler.compScope");
      TheNet->mergePlaces("1.ch_out",           "1.compensationHandler.ch_out");
      TheNet->mergePlaces("1.compensated",      "1.compensationHandler.compensated");

      // embed event handler
      TheNet->mergePlaces("1.p23", "1.eventHandler.initial");
      TheNet->mergePlaces("1.p24", "1.eventHandler.finish");
      TheNet->mergePlaces("1.p25", "1.eventHandler.final");
      TheNet->mergePlaces("1.p26", "1.eventHandler.stop");
      TheNet->mergePlaces("1.p27", "1.eventHandler.stopped");
      TheNet->mergePlaces("1.p5" , "1.eventHandler.failed"); //new
   
      // embed the stop-pattern
      TheNet->mergePlaces("1.Faulted",     "1.stop.Faulted");
      TheNet->mergePlaces("1.p5",          "1.stop.fault_in");
      TheNet->mergePlaces("1.fault",       "1.stop.fault");
      TheNet->mergePlaces("1.faultSave",   "1.stop.faultSave");
      TheNet->mergePlaces("1.Active",      "1.stop.Active");
      TheNet->mergePlaces("1.!Active",     "1.stop.!Active");
      TheNet->mergePlaces("1.final",       "1.stop.final");
      TheNet->mergePlaces("1.terminate",   "1.stop.terminate");
      TheNet->mergePlaces("1.!Faulted",    "1.stop.!Faulted");
      TheNet->mergePlaces("1.rethrow",     "1.stop.rethrow");
      TheNet->mergePlaces("1.Ended",       "1.stop.Ended");
      TheNet->mergePlaces("1.Compensated", "1.stop.Compensated");
      TheNet->mergePlaces("1.!Ended",      "1.stop.!Ended");
      TheNet->mergePlaces("1.p21",         "1.stop.stop");
      TheNet->mergePlaces("1.p34",         "1.stop.stopped");
      TheNet->mergePlaces("1.p35",         "1.stop.cleanCH");
      TheNet->mergePlaces("1.p36",         "1.stop.ch_cleaned");      
      
      // embed the inner activity
      TheNet->mergePlaces(TheNet->findPlace("1.p19"), TheNet->findPlace(i, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace("1.p20"), TheNet->findPlace(i, ".final"));
      TheNet->mergePlaces(TheNet->findPlace("1.p21"), TheNet->findPlace(i, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace("1.p22"), TheNet->findPlace(i, ".stopped"));
      TheNet->mergePlaces(TheNet->findPlace("1.p5"),  TheNet->findPlace(i, ".failed"));

      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
;





/******************************************************************************
  STOP PATTERNS
******************************************************************************/

/*
 * Stop pattern embedded in a process as it is depicted in Fig. 33.
 */

StopInProcess() ->
  [petrinet:
    {
      string prefix = "1.stop.";
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
      Transition *t1 =  TheNet->newTransition(prefix + "t1");
      Transition *t2 =  TheNet->newTransition(prefix + "t2");
      Transition *t3 =  TheNet->newTransition(prefix + "t3");
      Transition *t4 =  TheNet->newTransition(prefix + "t4");
      Transition *t5 =  TheNet->newTransition(prefix + "t5");
      Transition *t6 =  TheNet->newTransition(prefix + "t6");
      Transition *t7 =  TheNet->newTransition(prefix + "t7");
      Transition *t8 =  TheNet->newTransition(prefix + "t8");
      Transition *t9 =  TheNet->newTransition(prefix + "kill");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p1, t1, READ);
      TheNet->newArc(p5, t1, "x");
      TheNet->newArc(t1, p15, "x");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p21);
      TheNet->newArc(p3, t3);
      //TheNet->newArc(p5, t3, RESET, "X");
      TheNet->newArc(p22, t3);
      TheNet->newArc(t3, p4);
      TheNet->newArc(p5, t4, "x");
      TheNet->newArc(p9, t4);
      TheNet->newArc(t4, p2);
      TheNet->newArc(t4, p6, "x");
      TheNet->newArc(t4, p10);
      TheNet->newArc(p4, t5);
      TheNet->newArc(p6, t5, "x");
      TheNet->newArc(p14, t5);
      TheNet->newArc(t5, p1);
      TheNet->newArc(t5, p7);
      TheNet->newArc(t5, p8, "x");
      TheNet->newArc(p9, t6);
      TheNet->newArc(p13, t6);
      TheNet->newArc(t6, p2);
      TheNet->newArc(t6, p10);
      TheNet->newArc(t6, p11);
      TheNet->newArc(p4, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p16);
      TheNet->newArc(p10, t8, READ);
      TheNet->newArc(p13, t8);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9, "x");
      TheNet->newArc(p5, t10, "x");
      TheNet->newArc(p17, t10, READ);
      TheNet->newArc(t10, p18, "x");
      TheNet->newArc(p18, t11, "x");
      TheNet->newArc(p20, t11);
      TheNet->newArc(t11, p19, "x");
      TheNet->newArc(t11, p23);
      TheNet->newArc(p19, t12, "x");
      TheNet->newArc(p24, t12);
      TheNet->newArc(t12, p16);
      TheNet->newArc(t12, p12);
    }
  ]
;


/*---------------------------------------------------------------------------*/


/*
 * Stop pattern embedded in a scope as it is depicted in Fig. 32.
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
      Transition *t1 =  TheNet->newTransition(prefix + "t1");
      Transition *t2 =  TheNet->newTransition(prefix + "t2");
      Transition *t3 =  TheNet->newTransition(prefix + "t3");
      Transition *t4 =  TheNet->newTransition(prefix + "t4");
      Transition *t5 =  TheNet->newTransition(prefix + "t5");
      Transition *t6 =  TheNet->newTransition(prefix + "t6");
      Transition *t7 =  TheNet->newTransition(prefix + "t7");
      Transition *t8 =  TheNet->newTransition(prefix + "t8");
      Transition *t9 =  TheNet->newTransition(prefix + "t9");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      Transition *t12 = TheNet->newTransition(prefix + "t12");
      Transition *t13 = TheNet->newTransition(prefix + "kill");
      Transition *t14 = TheNet->newTransition(prefix + "faultInCH");
      Transition *t15 = TheNet->newTransition(prefix + "t15");
      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p6);      
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p4);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p8, t2);
      TheNet->newArc(p9, t2);
      TheNet->newArc(p5, t3, READ);
      TheNet->newArc(p10, t3, "x");
      TheNet->newArc(t3, p23, "x");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(t4, p29);
      TheNet->newArc(p7, t5);
      TheNet->newArc(t5, p8);
      //TheNet->newArc(p10, t5, RESET, "X");
      TheNet->newArc(p30, t5);
      TheNet->newArc(t6, p6);
      TheNet->newArc(p10, t6, "x");
      TheNet->newArc(t6, p11, "x");
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p18);
      TheNet->newArc(t7, p5);
      TheNet->newArc(p8, t7);
      TheNet->newArc(p9, t7);
      TheNet->newArc(p11, t7, "x");
      TheNet->newArc(t7, p12);
      TheNet->newArc(t7, p13, "x");
      TheNet->newArc(t8, p6);
      TheNet->newArc(p15, t8);
      TheNet->newArc(t8, p16);
      TheNet->newArc(p8, t9);
      TheNet->newArc(p16, t9);
      TheNet->newArc(t9, p17);
      TheNet->newArc(t9, p22);
      TheNet->newArc(p28, t9);
      TheNet->newArc(t10, p6);
      TheNet->newArc(p14, t10);
      TheNet->newArc(t10, p19);
      TheNet->newArc(t10, p18);
      TheNet->newArc(p21, t10);
      TheNet->newArc(p8, t11);
      TheNet->newArc(p19, t11);
      TheNet->newArc(t11, p20);
      TheNet->newArc(t11, p22);
      TheNet->newArc(p28, t11);
      TheNet->newArc(p18, t12, READ);
      TheNet->newArc(p21, t12);
      TheNet->newArc(t12, p20);
      TheNet->newArc(p22, t13, READ);
      TheNet->newArc(p25, t13, "x");
      TheNet->newArc(p10, t14, "x");
      TheNet->newArc(p24, t14, READ);
      TheNet->newArc(t14, p25, "x");
      TheNet->newArc(p25, t15, "x");
      TheNet->newArc(t15, p26, "x");
      TheNet->newArc(p28, t15);
      TheNet->newArc(t15, p31);
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
      string prefix = "1.faulthandler.";
      
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
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p7, t5, READ, "x");
      TheNet->newArc(t5, p8);
      TheNet->newArc(t6, p3);
      TheNet->newArc(p7, t6, "x");
      TheNet->newArc(p9, t6);
      TheNet->newArc(t6, p15);
      TheNet->newArc(p16, t6);
      TheNet->newArc(p7, t7, "x");
      TheNet->newArc(t7, p11);
      TheNet->newArc(p13, t7, "y");
      TheNet->newArc(t7, p14, "y");
      TheNet->newArc(t8, p3);
      TheNet->newArc(p12, t8);
      TheNet->newArc(p14, t8, "y");
      TheNet->newArc(t8, p15);
      TheNet->newArc(p16, t8);
      TheNet->newArc(p13, t9, "y");
      TheNet->newArc(p15, t9, READ);
      
      /*---------------------------------------------------------------------*/
      // the <compensate />-element
      string new_prefix = "1.faulthandler.compensate.";

      Place *compensate_p1 = TheNet->newPlace(new_prefix + "initial");
      Place *compensate_p2 = TheNet->newPlace(new_prefix + "running");
      Place *compensate_p3 = TheNet->newPlace(new_prefix + "final");
      Place *compensate_p4 = TheNet->newPlace(new_prefix + "stop");
      Place *compensate_p5 = TheNet->newPlace(new_prefix + "ch_fh");
      Place *compensate_p6 = TheNet->newPlace(new_prefix + "ch_out");
      Place *compensate_p7 = TheNet->newPlace(new_prefix + "stopped");
      Transition *compensate_t1  = TheNet->newTransition(new_prefix + "t1");
      Transition *compensate_t2  = TheNet->newTransition(new_prefix + "t2");
      Transition *compensate_t3  = TheNet->newTransition(new_prefix + "t3");
      Transition *compensate_t4  = TheNet->newTransition(new_prefix + "t4");
      Transition *compensate_t5  = TheNet->newTransition(new_prefix + "t5");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);
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
	Transition *t1 = TheNet->newTransition(prefix + "t1." + faultToCatch, faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	// embed innerActivity_1 to innerActivity_n-1
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace("1.faulthandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace("1.faulthandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));

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
	  TheNet->mergePlaces(TheNet->findPlace("1.faulthandler.p11"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace("1.faulthandler.p12"), TheNet->findPlace(innerActivity, ".stopped"));
	  // TODO merge failed-places
	  
	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p7, t3, READ, "x");
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(p7, t4, "x");
	}
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
      string prefix = "1.faulthandler.";      
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
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1, "x");
      TheNet->newArc(t1, p9);
      TheNet->newArc(p11, t1);
      TheNet->newArc(t1, p12);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p7, t2, "x");
      TheNet->newArc(t2, p8, "y");
      TheNet->newArc(p10, t2, "y");
      TheNet->newArc(p6, t3);
      TheNet->newArc(t3, p9);
      TheNet->newArc(p8, t3, "y");
      TheNet->newArc(p11, t3);
      TheNet->newArc(t3, p12);
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
      Transition *compensate_t2  = TheNet->newTransition(prefix + "t2");
      Transition *compensate_t3  = TheNet->newTransition(prefix + "t3");
      Transition *compensate_t4  = TheNet->newTransition(prefix + "t4");
      Transition *compensate_t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);
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
      string prefix = intToString(This->parentScopeId->value) + ".faulthandler.";    
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
      Transition *t5 = TheNet->newTransition(prefix + "t5", "{rethrowComp}");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      Transition *t9 = TheNet->newTransition(prefix + "t9");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p9, t5, "x");
      TheNet->newArc(t5, p10);
      TheNet->newArc(t6, p8);
      TheNet->newArc(p9, t6, "x");
      TheNet->newArc(p11, t6);
      TheNet->newArc(t6, p12);
      TheNet->newArc(t6, p17, "x");
      TheNet->newArc(t6, p18);
      TheNet->newArc(p24, t6);
      TheNet->newArc(p9, t7, "x");
      TheNet->newArc(t7, p13);
      TheNet->newArc(p15, t7, "y");
      TheNet->newArc(t7, p16, "y");
      TheNet->newArc(t8, p8);
      TheNet->newArc(t8, p12);
      TheNet->newArc(p14, t8);
      TheNet->newArc(p16, t8, "y");
      TheNet->newArc(t8, p17, "y");
      TheNet->newArc(t8, p18);
      TheNet->newArc(p24, t8);
      TheNet->newArc(p15, t9, "y");
      TheNet->newArc(p18, t9, READ);
      TheNet->newArc(p15, t10, "y");
      TheNet->newArc(p19, t10, READ);
      TheNet->newArc(t11, p7);
      TheNet->newArc(t11, p8);
      TheNet->newArc(p19, t11);
      TheNet->newArc(p22, t11);
      TheNet->newArc(t11, p23);

      /*---------------------------------------------------------------------*/
      // the left <compensate />-element
      string new_prefix = prefix + "compensate.left.";
      Place *compensate1_p1 = TheNet->newPlace(new_prefix + "initial");
      Place *compensate1_p2 = TheNet->newPlace(new_prefix + "running");
      Place *compensate1_p3 = TheNet->newPlace(new_prefix + "final");
      Place *compensate1_p4 = TheNet->newPlace(new_prefix + "stop");
      Place *compensate1_p5 = TheNet->newPlace(new_prefix + "ch_fh");
      Place *compensate1_p6 = TheNet->newPlace(new_prefix + "ch_out");
      Place *compensate1_p7 = TheNet->newPlace(new_prefix + "stopped");
      Transition *compensate1_t1  = TheNet->newTransition(new_prefix + "t1");
      Transition *compensate1_t2  = TheNet->newTransition(new_prefix + "t2");
      Transition *compensate1_t3  = TheNet->newTransition(new_prefix + "t3");
      Transition *compensate1_t4  = TheNet->newTransition(new_prefix + "t4");
      Transition *compensate1_t5  = TheNet->newTransition(new_prefix + "t5");
      TheNet->newArc(compensate1_p1, compensate1_t1);
      TheNet->newArc(compensate1_t1, compensate1_p2);
      TheNet->newArc(compensate1_t1, compensate1_p5);
      TheNet->newArc(compensate1_p2, compensate1_t2);
      TheNet->newArc(compensate1_t2, compensate1_p3);
      TheNet->newArc(compensate1_p6, compensate1_t2);
      TheNet->newArc(compensate1_p1, compensate1_t3);
      TheNet->newArc(compensate1_p4, compensate1_t3);
      TheNet->newArc(compensate1_t3, compensate1_p7);
      TheNet->newArc(compensate1_p2, compensate1_t4);
      TheNet->newArc(compensate1_p4, compensate1_t4);
      TheNet->newArc(compensate1_t4, compensate1_p7);
      TheNet->newArc(compensate1_p2, compensate1_t5);
      TheNet->newArc(compensate1_p3, compensate1_t5);
      TheNet->newArc(compensate1_t5, compensate1_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p10"), compensate1_p1);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p11"), compensate1_p3);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), compensate1_p4);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p25"), compensate1_p5);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p26"), compensate1_p6);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), compensate1_p7);

      // the right <compensate />-element
      new_prefix = prefix + "compensate.right.";
      Place *compensate2_p1 = TheNet->newPlace(new_prefix + "initial");
      Place *compensate2_p2 = TheNet->newPlace(new_prefix + "running");
      Place *compensate2_p3 = TheNet->newPlace(new_prefix + "final");
      Place *compensate2_p4 = TheNet->newPlace(new_prefix + "stop");
      Place *compensate2_p5 = TheNet->newPlace(new_prefix + "ch_fh");
      Place *compensate2_p6 = TheNet->newPlace(new_prefix + "ch_out");
      Place *compensate2_p7 = TheNet->newPlace(new_prefix + "stopped");
      Transition *compensate2_t1  = TheNet->newTransition(new_prefix + "t1");
      Transition *compensate2_t2  = TheNet->newTransition(new_prefix + "t2");
      Transition *compensate2_t3  = TheNet->newTransition(new_prefix + "t3");
      Transition *compensate2_t4  = TheNet->newTransition(new_prefix + "t4");
      Transition *compensate2_t5  = TheNet->newTransition(new_prefix + "t5");
      TheNet->newArc(compensate2_p1, compensate2_t1);
      TheNet->newArc(compensate2_t1, compensate2_p2);
      TheNet->newArc(compensate2_t1, compensate2_p5);
      TheNet->newArc(compensate2_p2, compensate2_t2);
      TheNet->newArc(compensate2_t2, compensate2_p3);
      TheNet->newArc(compensate2_p6, compensate2_t2);
      TheNet->newArc(compensate2_p1, compensate2_t3);
      TheNet->newArc(compensate2_p4, compensate2_t3);
      TheNet->newArc(compensate2_t3, compensate2_p7);
      TheNet->newArc(compensate2_p2, compensate2_t4);
      TheNet->newArc(compensate2_p4, compensate2_t4);
      TheNet->newArc(compensate2_t4, compensate2_p7);
      TheNet->newArc(compensate2_p2, compensate2_t5);
      TheNet->newArc(compensate2_p3, compensate2_t5);
      TheNet->newArc(compensate2_t5, compensate2_p7);

      // embed the <compensate />-element
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), compensate2_p1);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), compensate2_p3);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p25"), compensate2_p5);
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p26"), compensate2_p6);
    }
    a // catch branches
    b // catchAll branch
    {
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = catchBranch->faultName->name;
	Transition *t1 = TheNet->newTransition(prefix + "t1." + faultToCatch, faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	// embed innerActivity_1 to innerActivity_1-n
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
	
	TheNet->newArc(p4, t1);
	TheNet->newArc(p9, t1, READ, "x");
	TheNet->newArc(t2, p3);
	TheNet->newArc(t2, p7);
	TheNet->newArc(t2, p8);
	TheNet->newArc(p9, t2, "x");
      }

      with(b) {
	CatchAll(innerActivity):
	{
	  Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll", "catchAll");
	  Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");
	  
	  // embed innerActivity_n
	  TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p13"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "p14"), TheNet->findPlace(innerActivity, ".stopped"));
	  // TODO merge failed-places
	  
	  TheNet->newArc(p4, t3);
	  TheNet->newArc(p9, t9, READ, "x");	
	  TheNet->newArc(t4, p3);
	  TheNet->newArc(t4, p7);
	  TheNet->newArc(t4, p8);
	  TheNet->newArc(p9, t4, "x");
	}
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
      string prefix = intToString(This->parentScopeId->value) + ".faulthandler.";      
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
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      Transition *t6  = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p2, t1);
      TheNet->newArc(p7, t1, "x");
      TheNet->newArc(t1, p9, "x");
      TheNet->newArc(t1, p10);
      TheNet->newArc(t1, p11);
      TheNet->newArc(p16, t1);
      TheNet->newArc(t1, p17);
      TheNet->newArc(p2, t2);
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p14);
      TheNet->newArc(t3, p3);
      TheNet->newArc(p7, t3, "x");
      TheNet->newArc(t3, p12, "y");
      TheNet->newArc(p13, t3, "y");
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p9, "y");
      TheNet->newArc(t4, p11);
      TheNet->newArc(p12, t4, "y");
      TheNet->newArc(t4, p15);
      TheNet->newArc(p16, t4);
      TheNet->newArc(t4, p17);
      TheNet->newArc(p8, t5, READ);
      TheNet->newArc(p13, t5, "y");
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
      Transition *compensate_t2  = TheNet->newTransition(compensate_prefix + "t2");
      Transition *compensate_t3  = TheNet->newTransition(compensate_prefix + "t3");
      Transition *compensate_t4  = TheNet->newTransition(compensate_prefix + "t4");
      Transition *compensate_t5  = TheNet->newTransition(compensate_prefix + "t5");
      TheNet->newArc(compensate_p1, compensate_t1);
      TheNet->newArc(compensate_t1, compensate_p2);
      TheNet->newArc(compensate_t1, compensate_p5);
      TheNet->newArc(compensate_p2, compensate_t2);
      TheNet->newArc(compensate_t2, compensate_p3);
      TheNet->newArc(compensate_p6, compensate_t2);
      TheNet->newArc(compensate_p1, compensate_t3);
      TheNet->newArc(compensate_p4, compensate_t3);
      TheNet->newArc(compensate_t3, compensate_p7);
      TheNet->newArc(compensate_p2, compensate_t4);
      TheNet->newArc(compensate_p4, compensate_t4);
      TheNet->newArc(compensate_t4, compensate_p7);
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
      string prefix = intToString(This->parentScopeId->value) + ".eventHandler.";
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");
      Place *p28 = TheNet->newPlace(prefix + "failed");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      Transition *t7  = TheNet->newTransition(prefix + "allFinished");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      Transition *t16 = TheNet->newTransition(prefix + "allStopped");
      Transition *t17 = TheNet->newTransition(prefix + "normalStop");
      Transition *t18 = TheNet->newTransition(prefix + "t18");
      Transition *t19 = TheNet->newTransition(prefix + "stop+finish");
      Transition *t20 = TheNet->newTransition(prefix + "t20");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p17);
      TheNet->newArc(TheNet->findPlace("1.clock"), t5, READ, "x");
      TheNet->newArc(t7, p8);
      TheNet->newArc(p18, t7);
      TheNet->newArc(p13, t11);
      TheNet->newArc(p17, t11);
      TheNet->newArc(t11, p18);
      TheNet->newArc(t16, p23);
      TheNet->newArc(p18, t17);      
      TheNet->newArc(p22, t17);
      TheNet->newArc(p6, t18);
      TheNet->newArc(p22, t18);
      TheNet->newArc(t18, p23);
      TheNet->newArc(p17, t19);
      TheNet->newArc(p22, t19);
      TheNet->newArc(p8, t20);
      TheNet->newArc(p22, t20);
      TheNet->newArc(t20, p23);
    }
    a // onMessage branches
    b // onAlarm branches
    {

      int onMessageCount = 1;
      foreach (OnMessage(innerActivity); tOnMessage_list a)
      {
	string message = intToString(onMessageCount);
	string newprefix = prefix + "onMessage" + message + ".";
	Place *onMessage_p1  = TheNet->newPlace(newprefix + "p1");
	Place *onMessage_p2  = TheNet->newPlace(newprefix + "p2");
	Place *onMessage_p3  = TheNet->newPlace(newprefix + "p3");
	Place *onMessage_p4  = TheNet->newPlace(newprefix + "p4");
	Place *onMessage_p5  = TheNet->newPlace(newprefix + "wait" + message );
	Place *onMessage_p6  = TheNet->newPlace(newprefix + "p6");
	Place *onMessage_p7  = TheNet->newPlace(newprefix + "finish" + message );
	Place *onMessage_p9  = TheNet->newPlace(newprefix + "finish+stop" + message );
	Place *onMessage_p11 = TheNet->newPlace(newprefix + "stop" + message );
	Place *onMessage_p12 = TheNet->newPlace(newprefix + "p12");
	Transition *onMessage_t1  = TheNet->newTransition(newprefix + "t1", "guard" + message );
	Transition *onMessage_t2  = TheNet->newTransition(newprefix + "t2");
	Transition *onMessage_t3  = TheNet->newTransition(newprefix + "t3", "!guard" + message );
	Transition *onMessage_t4  = TheNet->newTransition(newprefix + "t4");
	Transition *onMessage_t5  = TheNet->newTransition(newprefix + "t5");
	Transition *onMessage_t7  = TheNet->newTransition(newprefix + "t7");
	Transition *onMessage_t8  = TheNet->newTransition(newprefix + "t8");
	Transition *onMessage_t10 = TheNet->newTransition(newprefix + "t10");
	Transition *onMessage_t11 = TheNet->newTransition(newprefix + "t11");
	Transition *onMessage_t12 = TheNet->newTransition(newprefix + "t12");
	TheNet->newArc(onMessage_p2, onMessage_t1, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_t1, onMessage_p1);
	TheNet->newArc(onMessage_t1, TheNet->findPlace(intToString(This->parentScopeId->value) + ".eventHandler.failed"), "fault" + message);
	TheNet->newArc(onMessage_t2, onMessage_p2, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_p5, onMessage_t2);
	TheNet->newArc(onMessage_p2, onMessage_t3, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_t3, onMessage_p3);
	TheNet->newArc(onMessage_p4, onMessage_t4);
	TheNet->newArc(onMessage_t4, onMessage_p5);
	TheNet->newArc(onMessage_p5, onMessage_t5);
	TheNet->newArc(onMessage_t5, onMessage_p6);
	TheNet->newArc(onMessage_p7, onMessage_t5);
	TheNet->newArc(onMessage_p7, onMessage_t7);
	TheNet->newArc(onMessage_p9, onMessage_t7);
	TheNet->newArc(onMessage_t7, onMessage_p11);
	TheNet->newArc(onMessage_p6, onMessage_t8);
	TheNet->newArc(onMessage_p9, onMessage_t8);
	TheNet->newArc(onMessage_t8, onMessage_p12);
	TheNet->newArc(onMessage_p1, onMessage_t10);
	TheNet->newArc(onMessage_p11, onMessage_t10);
	TheNet->newArc(onMessage_t10, onMessage_p12);
	TheNet->newArc(onMessage_p2, onMessage_t11, "(M" + message + ",CS" + message + ")" );
	TheNet->newArc(onMessage_p11, onMessage_t11);
	TheNet->newArc(onMessage_t11, onMessage_p12);
	TheNet->newArc(onMessage_p5, onMessage_t12);
	TheNet->newArc(onMessage_p11, onMessage_t12);
	TheNet->newArc(onMessage_t12, onMessage_p12);

	// arcs from the EH
	TheNet->newArc(t11, onMessage_p7);  // t11 in AEH is t13 in MEH
	TheNet->newArc(t5,  onMessage_p5);  // t5  in AEH is t6  in MEH
	TheNet->newArc(t17, onMessage_p9);  // t17 in AEH is t18 in MEH (stop+finish)
	TheNet->newArc(t19, onMessage_p11); // t19 in AEH is t27 in MEH (normalStop)
	
	// arcs to the EH
	TheNet->newArc(onMessage_p6, t7);   // "allFinished" in AEH is "finishAll" in MEH
	TheNet->newArc(onMessage_p12, t16);

	// embed the innerActivity
	TheNet->mergePlaces(onMessage_p3,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onMessage_p11, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onMessage_p12, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onMessage_p4,  TheNet->findPlace(innerActivity, ".final"));
	TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) + ".eventHandler.failed"),  TheNet->findPlace(innerActivity, ".failed")); //new	
	
	onMessageCount++;
      }


      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
	string onAlarmGuard;
	string newprefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y";  // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y";  // "<=" is escaped
	
	Place *onAlarm_p1  = TheNet->newPlace(newprefix + "p1");
	Place *onAlarm_p2  = TheNet->newPlace(newprefix + "p2");
	Place *onAlarm_p3  = TheNet->newPlace(newprefix + "wait" + intToString(onAlarmCount));
	Place *onAlarm_p4  = TheNet->newPlace(newprefix + "p4");
	Place *onAlarm_p5  = TheNet->newPlace(newprefix + "finish" + intToString(onAlarmCount));
	Place *onAlarm_p7  = TheNet->newPlace(newprefix + "stop+finish" + intToString(onAlarmCount));
	Place *onAlarm_p9  = TheNet->newPlace(newprefix + "stop" + intToString(onAlarmCount));
	Place *onAlarm_p10 = TheNet->newPlace(newprefix + "p10");
	Transition *onAlarm_t1 = TheNet->newTransition(newprefix + "t1", onAlarmGuard);
	Transition *onAlarm_t2 = TheNet->newTransition(newprefix + "t2");
	Transition *onAlarm_t3 = TheNet->newTransition(newprefix + "t3");
	Transition *onAlarm_t4 = TheNet->newTransition(newprefix + "t4");
	Transition *onAlarm_t6 = TheNet->newTransition(newprefix + "t6");
	Transition *onAlarm_t8 = TheNet->newTransition(newprefix + "t8");
	TheNet->newArc(onAlarm_p3, onAlarm_t1, "x");
	TheNet->newArc(onAlarm_t1, onAlarm_p1);
	TheNet->newArc(TheNet->findPlace("1.clock"), onAlarm_t1, READ, "y");
	TheNet->newArc(onAlarm_p2, onAlarm_t2);
	TheNet->newArc(onAlarm_t2, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t2);
	TheNet->newArc(onAlarm_p3, onAlarm_t3, "x");
	TheNet->newArc(onAlarm_t3, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t3);
	TheNet->newArc(onAlarm_p5, onAlarm_t4);
	TheNet->newArc(onAlarm_p7, onAlarm_t4);
	TheNet->newArc(onAlarm_t4, onAlarm_p9);
	TheNet->newArc(onAlarm_p4, onAlarm_t6);
	TheNet->newArc(onAlarm_p7, onAlarm_t6);
	TheNet->newArc(onAlarm_t6, onAlarm_p10);
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
	
	onMessageCount++;
      }
    }
  ]
;





/*
 * This is an implicit eventhandler, i.e. a pattern that is inserted if no
 * event handler is given. This pattern is not inside the semantics of
 * [Sta04]. It is a "stub" with the only use of moving the tokes so that the
 * surrounding process or scope functions correctly.
 */

This=implicitEventHandler() ->
  [petrinet:
    {
      string prefix = intToString(This->parentScopeId->value) + ".eventHandler.";
      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "finish");
      Place *p3  = TheNet->newPlace(prefix + "final");
      Place *p4  = TheNet->newPlace(prefix + "stop");
      Place *p5  = TheNet->newPlace(prefix + "stopped");
      Place *p6  = TheNet->newPlace(prefix + "running");
      Place *p7  = TheNet->newPlace(prefix + "finishing");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      Transition *t3 = TheNet->newTransition(prefix + "allFinished");
      Transition *t4 = TheNet->newTransition(prefix + "stop+finish");
      Transition *t5 = TheNet->newTransition(prefix + "normalStop");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p6);
      TheNet->newArc(p2, t2);
      TheNet->newArc(p6, t2);
      TheNet->newArc(t2, p7);
      TheNet->newArc(t3, p3);
      TheNet->newArc(p7, t3);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      TheNet->newArc(p6, t4);
      TheNet->newArc(p4, t5);
      TheNet->newArc(t5, p5);
      TheNet->newArc(p6, t5);
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
      string prefix = intToString(This->parentScopeId->value) + ".compensationHandler.";     
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
      Place *p19 = TheNet->newPlace(prefix + "compScope");
      Place *p24 = TheNet->newPlace(prefix + "clean");
      Place *p25 = TheNet->newPlace(prefix + "failed");
      Transition *t1  = TheNet->newTransition(prefix + "t1", "scope=s");
      Transition *t2  = TheNet->newTransition(prefix + "t2", "scope=s");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t2", "scope=s");
      Transition *t6  = TheNet->newTransition(prefix + "t6");
      Transition *t7  = TheNet->newTransition(prefix + "t7");
      Transition *t8  = TheNet->newTransition(prefix + "t8");
      Transition *t9  = TheNet->newTransition(prefix + "t9");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      Transition *t12 = TheNet->newTransition(prefix + "t12");
      Transition *t15 = TheNet->newTransition(prefix + "t15");
      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(t1, p1);      
      TheNet->newArc(p2, t1, "s");
      TheNet->newArc(p3, t1, READ);
      TheNet->newArc(t1, p25, "fault");
      TheNet->newArc(p2, t2, "s");
      TheNet->newArc(p4, t2, READ);
      TheNet->newArc(p5, t2, READ);
      TheNet->newArc(t2, p9, "scopeName");
      TheNet->newArc(p1, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p24, t3);
      TheNet->newArc(p2, t4);
      TheNet->newArc(t4, p3);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(t4, p11);
      TheNet->newArc(t4, p15);
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p11);
      TheNet->newArc(t6, p16);
      TheNet->newArc(t7, p9, "scopeName");
      TheNet->newArc(p11, t7);
      TheNet->newArc(p15, t7);
      TheNet->newArc(p11, t8);
      TheNet->newArc(p16, t8);
      TheNet->newArc(t8, p17);
      TheNet->newArc(t9, p11);
      TheNet->newArc(p16, t9, READ);
      TheNet->newArc(p18, t9, "copeScopeName");
      TheNet->newArc(t10, p11);
      TheNet->newArc(p15, t10, READ);
      TheNet->newArc(p18, t10, "compScopeName");
      TheNet->newArc(t11, p13);
      TheNet->newArc(p15, t11);
      TheNet->newArc(p24, t11);
      TheNet->newArc(t12, p13);
      TheNet->newArc(p16, t12);
      TheNet->newArc(p24, t12);
      TheNet->newArc(p9, t15, "scopeName");
      TheNet->newArc(t15, p13);
      TheNet->newArc(p24, t15);
      TheNet->newArc(t16, p13);
      TheNet->newArc(p17, t16);
      TheNet->newArc(p24, t16);

      // generate transitions t13 for the child scopes
      SymbolScope *me = symMan.getScope(This->parentScopeId);      
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *p20 = TheNet->findPlace(intToString((*child)->id->value) + ".push");
	Place *p21 = TheNet->findPlace(intToString((*child)->id->value) + ".!push");
	Transition *t13 = TheNet->newTransition(prefix + "t13." + intToString((*child)->id->value));
	TheNet->newArc(p20, t7);
	TheNet->newArc(p20, t8);
	TheNet->newArc(p11, t13);
	TheNet->newArc(t13, p19, "s");
	TheNet->newArc(t13, p20);
	TheNet->newArc(p21, t13, "s");
      }
    }
  ]
;






/******************************************************************************
  EMPTY
******************************************************************************/

This=Empty(a) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";     
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t1);
      TheNet->newArc(p1, t2);
      TheNet->newArc(p2, t3);
      TheNet->newArc(p3, t2);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t2, p4);
      TheNet->newArc(t3, p4);
    }
    a // standardElements
  ]
;





/******************************************************************************
  INVOKE
******************************************************************************/

// asynchronous invoke (case initiate="no")
This=Invoke(a,b) provided (This->outputVariable == mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");
      Place *p7 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Transition *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "(X,CS)");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);
      TheNet->newArc(t3, p4);
      TheNet->newArc(t3, p7, "fault");
      TheNet->newArc(p2, t3, "(X,CS)");
      TheNet->newArc(p1, t4);
      TheNet->newArc(p5, t4);
      TheNet->newArc(t4, p6);
      TheNet->newArc(p2, t5, "(X,CS)");
      TheNet->newArc(p5, t5);
      TheNet->newArc(t5, p6);
      TheNet->newArc(p4, t6);
      TheNet->newArc(p5, t6);
      TheNet->newArc(t6, p6);
      TheNet->newArc(p3, t7);
      TheNet->newArc(p5, t7);
      TheNet->newArc(t7, p6);
    }
    a // standardElements
  ]
;

// synchronous invoke (case initiate="no")
This=Invoke(a,b) provided (This->outputVariable != mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running1");
      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "running2");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "p6");
      Place *p7 = TheNet->newPlace(prefix + "stop");
      Place *p8 = TheNet->newPlace(prefix + "stopped");
      Place *p9 = TheNet->newPlace(prefix + "failed");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", "!guard1");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4", "!guard2");
      Transition *t5  = TheNet->newTransition(prefix + "t5", "guard2");
      Transition *t6  = TheNet->newTransition(prefix + "t6", "guard1");
      Transition *t7  = TheNet->newTransition(prefix + "t7");
      Transition *t8  = TheNet->newTransition(prefix + "t8");
      Transition *t9  = TheNet->newTransition(prefix + "t9");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");
      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "(X,CS1)");
      TheNet->newArc(p2, t2, "(X,CS1)");
      TheNet->newArc(t2, p3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4, "(Y,CS2)");
      TheNet->newArc(p4, t4, "(Y,CS2)");
      TheNet->newArc(t4, p5);
      TheNet->newArc(p4, t5, "(Y,CS2)");
      TheNet->newArc(t5, p6);
      TheNet->newArc(t5, p9, "fault2");
      TheNet->newArc(p2, t6, "(X,CS1)");
      TheNet->newArc(t6, p6);
      TheNet->newArc(t6, p9, "fault1");
      TheNet->newArc(p1, t7);
      TheNet->newArc(p7, t7);
      TheNet->newArc(t7, p8);
      TheNet->newArc(p2, t8, "(X,CS1)");
      TheNet->newArc(p7, t8);
      TheNet->newArc(t8, p8);
      TheNet->newArc(p3, t9);
      TheNet->newArc(p7, t9);
      TheNet->newArc(t9, p8);
      TheNet->newArc(p4, t10, "(Y,CS2)");
      TheNet->newArc(p7, t10);
      TheNet->newArc(t10, p8);
      TheNet->newArc(p6, t11);
      TheNet->newArc(p7, t11);
      TheNet->newArc(t11, p8);
      TheNet->newArc(p5, t12);
      TheNet->newArc(p7, t12);
      TheNet->newArc(t12, p8);
    }
    a // standardElements
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

// receive in case of initiate="no"
This=Receive(a,b) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";     
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");
      Place *p7 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Transition *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "(X,CS)");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);
      TheNet->newArc(t3, p4);
      TheNet->newArc(t3, p7, "fault");
      TheNet->newArc(p2, t3, "(X,CS)");
      TheNet->newArc(p1, t4);
      TheNet->newArc(p5, t4);
      TheNet->newArc(t4, p6);
      TheNet->newArc(p2, t5, "(X,CS)");
      TheNet->newArc(p5, t5);
      TheNet->newArc(t5, p6);
      TheNet->newArc(p4, t6);
      TheNet->newArc(p5, t6);
      TheNet->newArc(t6, p6);
      TheNet->newArc(p3, t7);
      TheNet->newArc(p5, t7);
      TheNet->newArc(t7, p6);
    }
    a // standardElements
  ]
;





/******************************************************************************
  REPLY
******************************************************************************/

// reply in case of initiate="no"
This=Reply(a,b) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";      
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");
      Place *p7 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Transition *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "(X,CS)");
      TheNet->newArc(p2, t2, "(X,CS)");
      TheNet->newArc(t2, p3);
      TheNet->newArc(t3, p4);
      TheNet->newArc(t3, p7, "fault");
      TheNet->newArc(p2, t3, "(X,CS)");
      TheNet->newArc(p1, t4);
      TheNet->newArc(p5, t4);
      TheNet->newArc(t4, p6);
      TheNet->newArc(p2, t5, "(X,CS)");
      TheNet->newArc(p5, t5);
      TheNet->newArc(t5, p6);
      TheNet->newArc(p4, t6);
      TheNet->newArc(p5, t6);
      TheNet->newArc(t6, p6);
      TheNet->newArc(p3, t7);
      TheNet->newArc(p5, t7);
      TheNet->newArc(t7, p6);
    }
    a // standardElements
  ]
;





/******************************************************************************
  ASSIGN
******************************************************************************/

This=Assign(a,b) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";     
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "stop");
      Place *p6 = TheNet->newPlace(prefix + "stopped");
      Place *p7 = TheNet->newPlace(prefix + "failed");

      Transition* t4 = TheNet->newTransition(prefix + "t1");
      Transition* t7 = TheNet->newTransition(prefix + "t7");
    } a b
  ]
;


Copy(a,b) provided (a->variable != mkcasestring("")) ->
  [petrinet:

  ]
;

Copy(a,b) provided (a->variable == mkcasestring("")) ->
  [petrinet:

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
      string prefix = intToString(This->id->value) + ".";  
      string guard = "(" + string(This->until->name) + " \\<= x)"; // "<=" is escaped
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1", guard);
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(TheNet->findPlace("1.clock"), t1, READ, "x");
      TheNet->newArc(p1, t2);
      TheNet->newArc(p3, t2);
      TheNet->newArc(t2, p4);
      TheNet->newArc(p2, t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);
    }
    a // standardElements
  ]
;

This=Wait(a) provided (This->For != mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";     
      string guard = "(x + " + string(This->For->name) + " \\<= y)"; // "<=" is escaped
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", guard);
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2, "x");
      TheNet->newArc(TheNet->findPlace("1.clock"), t1, READ, "x");
      TheNet->newArc(p2, t2, "x");
      TheNet->newArc(t2, p3);
      TheNet->newArc(TheNet->findPlace("1.clock"), t2, READ, "y");
      TheNet->newArc(p1, t3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p5);
      TheNet->newArc(p2, t4, "x");
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);
      TheNet->newArc(p3, t5);
      TheNet->newArc(p4, t5);
      TheNet->newArc(t5, p5);
    }
    a // standardElements
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
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");
      Place *p5 = TheNet->newPlace(prefix + "failed");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, This->faultName);
      TheNet->newArc(p1, t2);
      TheNet->newArc(p3, t2);
      TheNet->newArc(t2, p4);
      TheNet->newArc(p2, t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);
    }
  a // standardElements
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
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "comp");
      Place *p6 = TheNet->newPlace(prefix + "done");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);
      TheNet->newArc(p1, t3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p7);
      TheNet->newArc(p2, t4);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(p2, t5);
      TheNet->newArc(p3, t5);
      TheNet->newArc(t5, p7);
    }
  a // standardElements
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *p6 = TheNet->newPlace(prefix + "ch_out");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5);
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2);
      TheNet->newArc(p1, t3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p7);
      TheNet->newArc(p2, t4);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(p2, t5);
      TheNet->newArc(p3, t5);
      TheNet->newArc(t5, p7);
    }
  a // standardElements
  ]
;

This=Compensate(a) provided (!This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "compScope");
      Place *p6 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", string(This->scope->name) + "=scopeName");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, string(This->scope->name));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2, "scopeName");
      TheNet->newArc(p1, t3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p7);
      TheNet->newArc(p2, t4);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(p2, t5);
      TheNet->newArc(p3, t5);
      TheNet->newArc(t5, p7);
    }
  a // standardElements
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *p6 = TheNet->newPlace(prefix + "compensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", string(This->scope->name) + "=scopeName");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, string(This->scope->name));
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(p6, t2, "scopeName");
      TheNet->newArc(p1, t3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p7);
      TheNet->newArc(p2, t4);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p7);
      TheNet->newArc(p2, t5);
      TheNet->newArc(p3, t5);
      TheNet->newArc(t5, p7);
    }
  a // standardElements
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
      string prefix = intToString(This->id->value) + ".";
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "!Terminated");
      Place *p5 = TheNet->newPlace(prefix + "Terminated");
      Place *p6 = TheNet->newPlace(prefix + "terminate");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");      
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(p5, t1, READ);
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p6);
      TheNet->newArc(p1, t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p7);
      TheNet->newArc(p2, t4);
      TheNet->newArc(p3, t4);
      TheNet->newArc(t4, p7);
      TheNet->mergePlaces(TheNet->findPlace("1.!Terminated"), p4);
      TheNet->mergePlaces(TheNet->findPlace("1.Terminated"), p5);
      TheNet->mergePlaces(TheNet->findPlace("1.terminate"), p6);
    }
  a // standardElements
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
      string prefix = intToString(This->id->value) + ".";
      Place *p7  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p9  = TheNet->newPlace(prefix + "running");
      Place *p15 = TheNet->newPlace(prefix + "stop");
      Place *p16 = TheNet->newPlace(prefix + "stopped");
      Place *p17 = TheNet->newPlace(prefix + "failed"); // new
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      Transition *t6  = TheNet->newTransition(prefix + "innerStopped");
      Transition *t7  = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p7, t2);
      TheNet->newArc(t2, p9);
      TheNet->newArc(t3, p8);
      TheNet->newArc(p9, t3);
      TheNet->newArc(p7, t4);
      TheNet->newArc(p15, t4);
      TheNet->newArc(t4, p16);
      TheNet->newArc(p9, t5);
      TheNet->newArc(p15, t5);
      TheNet->newArc(t6, p16);
      TheNet->newArc(p8, t7);
      TheNet->newArc(p15, t7);
      TheNet->newArc(t7, p16);
    }
    a // standardElements
    c // inner activities
    {
      foreach (innerActivity; activity_list c)
      {
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(t5, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".stopped"), t6);
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t3);
	TheNet->mergePlaces(TheActivity, ".failed", innerActivity, ".failed");
      }	
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
      string prefix = intToString(This->id->value) + ".";
      Place *switch_p4 =  TheNet->newPlace(prefix + "final");
      Place *switch_p5 =  TheNet->newPlace(prefix + "initial");
      Place *switch_p6 =  TheNet->newPlace(prefix + "running");
      Place *switch_p10 = TheNet->newPlace(prefix + "p10");
      Place *switch_p11 = TheNet->newPlace(prefix + "stop");
      Place *switch_p12 = TheNet->newPlace(prefix + "stopped");
      Place *switch_p13 = TheNet->newPlace(prefix + "failed");
      Transition *switch_t3 =  TheNet->newTransition(prefix + "t3");
      Transition *switch_t7 =  TheNet->newTransition(prefix + "t7", "guard");
      Transition *switch_t8 =  TheNet->newTransition(prefix + "t8");
      Transition *switch_t9 =  TheNet->newTransition(prefix + "t9");
      Transition *switch_t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(switch_p5, switch_t3);
      TheNet->newArc(switch_t3, switch_p6, "x");
      TheNet->newArc(switch_p6, switch_t7, "x");
      TheNet->newArc(switch_t7, switch_p10);
      TheNet->newArc(switch_t7, switch_p13, "fault");
      TheNet->newArc(switch_p5, switch_t8);
      TheNet->newArc(switch_p11, switch_t8);
      TheNet->newArc(switch_t8, switch_p12);
      TheNet->newArc(switch_p6, switch_t9, "x");
      TheNet->newArc(switch_p11, switch_t9);
      TheNet->newArc(switch_t9, switch_p12);
      TheNet->newArc(switch_p10, switch_t10);
      TheNet->newArc(switch_p11, switch_t10);
      TheNet->newArc(switch_t10, switch_p12);
    }
    a // standardElements
    c // otherwise branch
    {
      int currentCase = 1;
      Transition *last;
      foreach (caseBranch=Case(innerActivity); tCase_list b)
      {
	string condition = caseBranch->condition->name;
	
	if (currentCase == 1)
	{
	  // two transitions: guard fulfilled or not
	  Transition *t2 = TheNet->newTransition(prefix + "t2.case1", "!guard & (" + condition + ")");
	  Transition *t4 = TheNet->newTransition(prefix + "t4.case1", "!guard & !(" + condition + ")");
	  
	  // arcs connecting "running"-place
	  TheNet->newArc(TheNet->findPlace(This, ".running"), t2, "x");
	  TheNet->newArc(TheNet->findPlace(This, ".running"), t4, "x");
	
	  // embed the innerActivity
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->mergePlaces(This, ".final",   innerActivity, ".final");
	  TheNet->mergePlaces(This, ".stop",    innerActivity, ".stop");
	  TheNet->mergePlaces(This, ".stopped", innerActivity, ".stopped");
	  TheNet->mergePlaces(This, ".failed",  innerActivity, ".failed");
	  
	  last = t4;
	}
	else
	{
	  // two transitions: guard fulfilled or not
	  string caseNumber = intToString(currentCase);
	  Transition *t2 = TheNet->newTransition(prefix + "t2.case" + caseNumber, condition);
	  Transition *t4 = TheNet->newTransition(prefix + "t4.case" + caseNumber, "!(" + condition + ")");

	  // "running"-place
	  Place *running =  TheNet->newPlace(prefix + "running.case" + caseNumber);

	  // arcs connecting "running"-place
	  TheNet->newArc(last, running, "x");
	  TheNet->newArc(running, t2, "x");
	  TheNet->newArc(running, t4, "x");

	  // move token from "running"-place to stop
	  Transition *t11 = TheNet->newTransition(prefix + "t11." + caseNumber);
	  TheNet->newArc(running, t11, "x");
	  TheNet->newArc(TheNet->findPlace(This, ".stop"), t11);
	  TheNet->newArc(t11, TheNet->findPlace(This, ".stopped"));

	  // embed the innerActivity
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->mergePlaces(This, ".final",   innerActivity, ".final");
	  TheNet->mergePlaces(This, ".stop",    innerActivity, ".stop");
	  TheNet->mergePlaces(This, ".stopped", innerActivity, ".stopped");
	  TheNet->mergePlaces(This, ".failed",  innerActivity, ".failed");

	  last = t4;
	}
	currentCase++;
      }
      
      Transition *t2 = TheNet->newTransition(prefix + "t2.otherwise");
      
      // "running"-place
      Place *running = TheNet->newPlace(prefix + "running.otherwise");
      
      // arcs connecting "running"-place
      TheNet->newArc(running, t2, "x");
      TheNet->newArc(last, running, "x");
      
      // remove token from "running"-place to stop
      Transition *t11 = TheNet->newTransition(prefix + "t11.o");
      TheNet->newArc(running, t11, "x");
      TheNet->newArc(TheNet->findPlace(This, ".stop"), t11);
      TheNet->newArc(t11, TheNet->findPlace(This, ".stopped"));	
      
      // embed inner activity
      TheNet->newArc(t2, TheNet->findPlace(c, ".initial"));
      TheNet->mergePlaces(This, ".final",   c, ".final");
      TheNet->mergePlaces(This, ".stop",    c, ".stop");
      TheNet->mergePlaces(This, ".stopped", c, ".stopped");
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
      string prefix = intToString(TheWhile->id->value) + ".";
      string condition = string(TheWhile->condition->name);
      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "p5");
      Place *p6 = TheNet->newPlace(prefix + "p6");
      Place *p7 = TheNet->newPlace(prefix + "stop");
      Place *p8 = TheNet->newPlace(prefix + "stopped");
      Place *p9 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1", "!(" + condition + ") & !guard");
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      Transition *t3 = TheNet->newTransition(prefix + "t3", "(" + condition + ") & !guard");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5", "guard");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(p2, t1, "x");
      TheNet->newArc(t1, p3);
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p2, "x");
      TheNet->newArc(p2, t3, "x");
      TheNet->newArc(t3, p4);
      TheNet->newArc(t4, p1);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p2, t5, "x");
      TheNet->newArc(t5, p6);
      TheNet->newArc(t5, p9, "fault");
      //TheNet->newArc(TheNet->findPlace("1.Active"), t5, READ); // added to test new idea of stop pattern
      TheNet->newArc(p1, t6);
      TheNet->newArc(p7, t6);
      TheNet->newArc(t6, p8);
      TheNet->newArc(p2, t7, "x");
      TheNet->newArc(p7, t7);
      TheNet->newArc(t7, p8);
      TheNet->newArc(p6, t8);
      TheNet->newArc(p7, t8);
      TheNet->newArc(t8, p8);
      TheNet->newArc(p3, t9);
      TheNet->newArc(p7, t9);
      TheNet->newArc(t9, p8);
    }
    a // standardElements
    b // innerActivity
    {
      // embed innerActivity
      TheNet->mergePlaces(This, ".p4",      b, ".initial");
      TheNet->mergePlaces(This, ".p5",      b, ".final");
      TheNet->mergePlaces(This, ".stop",    b, ".stop");
      TheNet->mergePlaces(This, ".stopped", b, ".stopped");
      TheNet->mergePlaces(This, ".failed",  b, ".failed");
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
      string prefix = intToString(This->id->value) + ".";
      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p9 = TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "stopped");
      Place *p12 = TheNet->newPlace(prefix + "failed"); // new
    }
    a // standardElements
    b // inner activities
    {
      int activityCount = 1;
      kc::impl_activity* lastActivity;
      
      foreach (innerActivity; activity_list b)
      {
	if (activityCount == 1)
	  TheNet->mergePlaces(This, ".initial", innerActivity, ".initial");
	else
	  TheNet->mergePlaces(lastActivity, ".final", innerActivity, ".initial");
	
	if (activityCount == b->length())
	  TheNet->mergePlaces(This, ".final", innerActivity, ".final");
	
	lastActivity = innerActivity;
	activityCount++;

	TheNet->mergePlaces(This, ".stop", innerActivity, ".stop");
	TheNet->mergePlaces(This, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(This, ".failed", innerActivity, ".failed");
      }
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
      string prefix = intToString(This->id->value) + ".";
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");
      Place *p13 = TheNet->newPlace(prefix + "failed");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(TheNet->findPlace("1.clock"), t5, READ, "x");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7, "x");
      TheNet->newArc(p6, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(p7, t8, "x");
      TheNet->newArc(p11, t8);
      TheNet->newArc(t8, p12);      
    }
    a // standardElements
    b // onMessage branches
    c // onAlarm branches
    {
      int onMessageCount = 1;      
      foreach (OnMessage(innerActivity); tOnMessage_list b)
      {
	string message = intToString(onMessageCount);
	string newprefix = prefix + "onMessage" + message + ".";
	Place *p3 = TheNet->newPlace(newprefix + "p3");
	Place *p10 = TheNet->newPlace(newprefix + "p10");
	Transition *t2 = TheNet->newTransition(newprefix + "t2");
	Transition *t3 = TheNet->newTransition(newprefix + "t3", "!guard" + message);
	Transition *t4 = TheNet->newTransition(newprefix + "t4", "guard" + message);
	Transition *t9 = TheNet->newTransition(newprefix + "t9");
	Transition *t10 = TheNet->newTransition(newprefix + "t10");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".TimeStamp"), t2, "x");
	TheNet->newArc(t2, p3, "M" + message );
	TheNet->newArc(p3, t3, "M" + message );
	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(p3, t4, "M" + message );
	TheNet->newArc(t4, p10);
	TheNet->newArc(t4, TheNet->findPlace(TheActivity, ".failed"), "fault");
	TheNet->newArc(p3, t9, "M" + message );
	TheNet->newArc(TheNet->findPlace(TheActivity, ".stop"), t9);
	TheNet->newArc(t9, TheNet->findPlace(TheActivity, ".stopped"));
	TheNet->newArc(p10, t10);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".stop"), t10);
	TheNet->newArc(t10, TheNet->findPlace(TheActivity, ".stopped"));

	// embed the inner activity
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");
	TheNet->mergePlaces(TheActivity, ".failed",  innerActivity, ".failed");

	onMessageCount++;
      }


      int onAlarmCount = 1;      
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	string newprefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";
	string onAlarmGuard;

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y";  // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y";  // "<=" is escaped

	Transition *t6 = TheNet->newTransition(newprefix + "t6", onAlarmGuard);
	TheNet->newArc(TheNet->findPlace("1.clock"), t6, READ, "y");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".TimeStamp"), t6, "x");
	TheNet->newArc(t6, TheNet->findPlace(innerActivity, ".initial"));

	// embed the inner activity	
	TheNet->mergePlaces(TheActivity, ".stop",    innerActivity, ".stop");
	TheNet->mergePlaces(TheActivity, ".stopped", innerActivity, ".stopped");
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");
	TheNet->mergePlaces(TheActivity, ".failed",  innerActivity, ".failed");

	onMessageCount++;
      }
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
      string prefix = intToString(This->id->value) + ".";
      currentScope = prefix;
      SymbolScope *me = symMan.getScope(this->id);      

      // create push-places for the children of the scope
      for (list< SymbolScope * >::iterator child = me->children.begin(); child != me->children.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->id->value) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->id->value) + ".!push");
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
      Place *p57 = TheNet->newPlace(prefix + "compScope");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      Transition *t6  = TheNet->newTransition(prefix + "t6");
      Transition *t7  = TheNet->newTransition(prefix + "t7");
      Transition *t8  = TheNet->newTransition(prefix + "t8");
      Transition *t9  = TheNet->newTransition(prefix + "t9");
      Transition *t10 = TheNet->newTransition(prefix + "t10");
      Transition *t11 = TheNet->newTransition(prefix + "t11");      
      TheNet->newArc(p29, t1);
      TheNet->newArc(t1, p38);     
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p13);
      TheNet->newArc(t2, p16);
      TheNet->newArc(t2, p18);
      TheNet->newArc(t2, p21);
      TheNet->newArc(t2, p25);
      TheNet->newArc(p22, t3);
      TheNet->newArc(t3, p26);
      TheNet->newArc(t3, p35);
      TheNet->newArc(p10, t4);
      TheNet->newArc(p11, t4);
      TheNet->newArc(t4, p12);
      TheNet->newArc(t4, p14);
      TheNet->newArc(TheNet->findPlace(prefix + "!push"), t4);
      TheNet->newArc(t4, TheNet->findPlace(prefix + "push"));
      TheNet->newArc(p27, t4);
      TheNet->newArc(p35, t4);
      TheNet->newArc(t4, p51);
      TheNet->newArc(p24, t5);
      TheNet->newArc(t5, p28);
      TheNet->newArc(p23, t6);
      TheNet->newArc(t6, p28);
      TheNet->newArc(p35, t6);
      TheNet->newArc(p8, t7, READ);
      TheNet->newArc(p10, t7);
      TheNet->newArc(t7, p14);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p36);      
      TheNet->newArc(p9, t8, READ);
      TheNet->newArc(p10, t8);
      TheNet->newArc(t8, p14);
      TheNet->newArc(p20, t8);
      TheNet->newArc(t8, p37);
      TheNet->newArc(p2, t9);
      TheNet->newArc(p20, t9);
      TheNet->newArc(t9, p45);
      TheNet->newArc(p17, t10, READ);
      TheNet->newArc(p20, t10);
      TheNet->newArc(t10, p45);
      TheNet->newArc(p20, t11);
      TheNet->newArc(t11, p45);
      TheNet->newArc(p51, t11);
    }
    a // standardElements
    c // Fault Handler
    d // Compensation Handler
    e // Event Handler
    f // Stop Pattern
    g // innerActivity
    {
      // embed fault handler
      TheNet->mergePlaces(prefix + "p43",    prefix + "faulthandler.fault");
      TheNet->mergePlaces(prefix + "p44",    prefix + "faulthandler.faultSave");
      TheNet->mergePlaces(prefix + "p47",    prefix + "faulthandler.rethrow");
      TheNet->mergePlaces(prefix + "p41",    prefix + "faulthandler.ft");
      TheNet->mergePlaces(prefix + "p42",    prefix + "faulthandler.ftSave");
      TheNet->mergePlaces(prefix + "!Ended", prefix + "faulthandler.!Ended");
      TheNet->mergePlaces(prefix + "Ended",  prefix + "faulthandler.Ended");
      TheNet->mergePlaces(prefix + "p49",    prefix + "faulthandler.ch_fh");
      TheNet->mergePlaces(prefix + "p50",    prefix + "faulthandler.ch_out");
      TheNet->mergePlaces(prefix + "final",  prefix + "faulthandler.out");
      TheNet->mergePlaces(prefix + "p52",    prefix + "faulthandler.trueOut");
      TheNet->mergePlaces(prefix + "p53",    prefix + "faulthandler.sourceFalse");
      TheNet->mergePlaces(prefix + "p54",    prefix + "faulthandler.falseOut");
      TheNet->mergePlaces(prefix + "p48",    prefix + "faulthandler.upperFH");
      TheNet->mergePlaces(prefix + "p46",    prefix + "faulthandler.stopped");

      // embed the compensation handler
      TheNet->mergePlaces(prefix + "ch_in",            prefix + "compensationHandler.ch_in");
      TheNet->mergePlaces(prefix + "Completed",        prefix + "compensationHandler.Completed");
      TheNet->mergePlaces(prefix + "!Completed",       prefix + "compensationHandler.!Completed");
      TheNet->mergePlaces(prefix + "Compensated",      prefix + "compensationHandler.Compensated");
      TheNet->mergePlaces(prefix + "p49",              prefix + "compensationHandler.fh");
      TheNet->mergePlaces(prefix + "scopeCompensated", prefix + "compensationHandler.scopeCompensated");
      TheNet->mergePlaces(prefix + "p39",              prefix + "compensationHandler.clean");
      TheNet->mergePlaces(prefix + "p40",              prefix + "compensationHandler.cleaned");
      TheNet->mergePlaces(prefix + "p6",               prefix + "compensationHandler.failed");
      TheNet->mergePlaces(prefix + "compScope",        prefix + "compensationHandler.compScope");
      TheNet->mergePlaces(prefix + "p50",              prefix + "compensationHandler.ch_out");
      TheNet->mergePlaces(prefix + "compensated",      prefix + "compensationHandler.compensated");

      // embed event handler
      TheNet->mergePlaces(prefix + "p25", prefix + "eventHandler.initial");
      TheNet->mergePlaces(prefix + "p26", prefix + "eventHandler.finish");
      TheNet->mergePlaces(prefix + "p27", prefix + "eventHandler.final");
      TheNet->mergePlaces(prefix + "p28", prefix + "eventHandler.stop");
      TheNet->mergePlaces(prefix + "p29", prefix + "eventHandler.stopped");
      TheNet->mergePlaces(prefix + "p6",  prefix + "eventHandler.failed"); //new      
      
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
      TheNet->mergePlaces(prefix + "p41",            prefix + "stop.ftSave");
      TheNet->mergePlaces(prefix + "p42",            prefix + "stop.ft");
      TheNet->mergePlaces(prefix + "p43",            prefix + "stop.fault");
      TheNet->mergePlaces(prefix + "p44",            prefix + "stop.faultSave");
      TheNet->mergePlaces(prefix + "stopped",        prefix + "stop.stopped_s");
      TheNet->mergePlaces(prefix + "p46",            prefix + "stop.upperTerminate");
      TheNet->mergePlaces(prefix + "p47",            prefix + "stop.rethrow");
      TheNet->mergePlaces(prefix + "p48",            prefix + "stop.upperFH");      
      
      // embed the inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(g, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(g, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(g, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(g, ".stopped"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p6"),  TheNet->findPlace(g, ".failed"));
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(NiltTarget_list(), NiltSource_list()) ->
  [petrinet: /* do nothing */ ]
;

This=StandardElements(NiltTarget_list(), a) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".link."; 
      Place *p1 = TheNet->newPlace(prefix + "negLink");
      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p4 = TheNet->newPlace(prefix + "p4");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "stop");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p3);
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p5);
      TheNet->newArc(p5, t4);
      TheNet->newArc(p6, t4);
      TheNet->newArc(t4, p7);

      foreach (link; tSource_list a)
      {
	Place *outLink = TheNet->findPlace(string(link->linkName->name));
	Place *NoutLink = TheNet->findPlace("!" + string(link->linkName->name));
	TheNet->newArc(NoutLink, t1);
	TheNet->newArc(t1, outLink, "false");
	TheNet->newArc(NoutLink, t2);
	TheNet->newArc(t2, outLink, string(link->transitionCondition->name));
	TheNet->newArc(outLink, t3, READ);
      }
   }
  ]
;

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("no")) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".link.";
      Place *p5  = TheNet->newPlace(prefix + "negLink");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1", string(This->joinCondition->name));
      Transition *t2 = TheNet->newTransition(prefix + "t2", "!(" + string(This->joinCondition->name) + ")");
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1);      
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p12, "fault");
      TheNet->newArc(p8, t3);
      TheNet->newArc(p9, t3);
      TheNet->newArc(t3, p10);
      TheNet->newArc(p9, t4);
      TheNet->newArc(t4, p10);
      TheNet->newArc(p11, t4);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *inLink = TheNet->findPlace(string(link->linkName->name));
	Place *NinLink = TheNet->findPlace("!" + string(link->linkName->name));
	TheNet->newArc(t1, NinLink);
	TheNet->newArc(inLink, t1, "b" + intToString(i));
	TheNet->newArc(inLink, t2, "b" + intToString(i));
	i++;
      }
    }
  ]
;

This=StandardElements(a, NiltSource_list()) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".link.";
      Place *p5  = TheNet->newPlace(prefix + "negLink");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "final");
      Place *p8  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "stop");
      Place *p10 = TheNet->newPlace(prefix + "stopped");
      Transition *t1 = TheNet->newTransition(prefix + "t1", string(This->joinCondition->name));
      Transition *t2 = TheNet->newTransition(prefix + "t2", "!(" + string(This->joinCondition->name) + ")");
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(t1, p6);
      TheNet->newArc(p8, t1);
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p5);
      TheNet->newArc(t2, p7);
      TheNet->newArc(p8, t3);
      TheNet->newArc(p9, t3);
      TheNet->newArc(t3, p10);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *inLink = TheNet->findPlace(string(link->linkName->name));
	Place *NinLink = TheNet->findPlace("!" + string(link->linkName->name));
	TheNet->newArc(t1, NinLink);
	TheNet->newArc(inLink, t1, "b" + intToString(i));
	TheNet->newArc(inLink, t2, "b" + intToString(i));
	i++;
      }      
    }
  ]
;

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("no")) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".link.";
      Place *p9  = TheNet->newPlace(prefix + "negLink");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2", string(This->joinCondition->name));
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5", "!(" + string(This->joinCondition->name) + ")");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p9, t1);
      TheNet->newArc(t1, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);
      TheNet->newArc(t5, p10);
      TheNet->newArc(t5, p13);
      TheNet->newArc(p16, t5);
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p15);
      TheNet->newArc(p16, t6);
      TheNet->newArc(p13, t7);
      TheNet->newArc(p14, t7);
      TheNet->newArc(t7, p15);

      int i = 1;
      foreach (link; tTarget_list a)
      {
	Place *inLink = TheNet->findPlace(string(link->linkName->name));
	Place *NinLink = TheNet->findPlace("!" + string(link->linkName->name));
        TheNet->newArc(t2, NinLink);	
	TheNet->newArc(inLink, t2, "b" + intToString(i));
        TheNet->newArc(t5, NinLink);
	TheNet->newArc(inLink, t5, "b" + intToString(i));
	i++;
      }

      i = 1;
      foreach (link; tSource_list b)
      {
	Place *outLink = TheNet->findPlace(string(link->linkName->name));
	Place *NoutLink = TheNet->findPlace("!" + string(link->linkName->name));
        TheNet->newArc(NoutLink, t1);
        TheNet->newArc(t1, outLink, "false");
        TheNet->newArc(NoutLink, t3);
        TheNet->newArc(t3, outLink, string(link->transitionCondition->name));
        TheNet->newArc(NoutLink, t5);
        TheNet->newArc(t5, outLink, "false");
	TheNet->newArc(outLink, t4, READ);
	i++;
      }
    }
  ]
;

This=StandardElements(a,b) provided (This->suppressJoinFailure == mkcasestring("yes")) ->
  [petrinet:
    {
      string prefix = intToString(This->parentId->value) + ".link.";
      Place *p9  = TheNet->newPlace(prefix + "negLink");
      Place *p10 = TheNet->newPlace(prefix + "p10");
      Place *p11 = TheNet->newPlace(prefix + "p11");
      Place *p12 = TheNet->newPlace(prefix + "p12");
      Place *p13 = TheNet->newPlace(prefix + "final");
      Place *p14 = TheNet->newPlace(prefix + "stop");
      Place *p15 = TheNet->newPlace(prefix + "stopped");
      Place *p16 = TheNet->newPlace(prefix + "initial");
      Place *p17 = TheNet->newPlace(prefix + "initial");
      Place *p18 = TheNet->newPlace(prefix + "failed");
      Transition *t1 = TheNet->newTransition(prefix + "t1");
      Transition *t2 = TheNet->newTransition(prefix + "t2", string(This->joinCondition->name));
      Transition *t3 = TheNet->newTransition(prefix + "t3");
      Transition *t4 = TheNet->newTransition(prefix + "t4");
      Transition *t5 = TheNet->newTransition(prefix + "t5", "!(" + string(This->joinCondition->name) + ")");
      Transition *t6 = TheNet->newTransition(prefix + "t6");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p9, t1);
      TheNet->newArc(t1, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(p16, t2);
      TheNet->newArc(p12, t3);
      TheNet->newArc(t3, p13);
      TheNet->newArc(p12, t4);
      TheNet->newArc(t4, p13);
      TheNet->newArc(t5, p10);
      TheNet->newArc(t5, p17);
      TheNet->newArc(t5, p18, "fault");
      TheNet->newArc(p16, t5);
      TheNet->newArc(p14, t6);
      TheNet->newArc(t6, p15);
      TheNet->newArc(p16, t6);
      TheNet->newArc(p17, t7);
      TheNet->newArc(p14, t7);
      TheNet->newArc(t7, p15);
      TheNet->newArc(p13, t8);
      TheNet->newArc(p14, t8);
      TheNet->newArc(t8, p15);
    }
  ]
;


/*****************************************************************************/
// the printer functions

/// "fake" printer-function
void pseudoPrinter(const char *s, uview v)
{
}

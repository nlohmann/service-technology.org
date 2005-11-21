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
 *          - last changed: \$Date: 2005/11/21 12:52:38 $
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
 * \version
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
 * 
 * \todo
 *          - find a simple "merge-language" to ease the definition of new
 *            patterns
 *          - complete initial pattern database
 *          - special treatment of <invoke>
 *          - tidy up case discrimination of <switch>
 *          - tidy up patterns <faultHandlers> and <process>
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


%{
/*!
 * \class kc::petrinet_class
 * \brief Unparse rules: Petri net
 */
%}

%uview petrinet;

/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include <string>
#include "petrinet.h"

// introduced in main.c 
extern PetriNet *TheNet;
%}





/******************************************************************************/

// All the includes, variables and structs defined here can be used in the
// printers below.
%{
#include <iostream>
%}

/******************************************************************************/





Process(a,b,c,d,e,f,g,h) ->
  [petrinet:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
      
      /*---------------------------------------------------------------------*/
      std::string prefix = "process.";
      Place *process_p1  = TheNet->newPlace(prefix + "initial");
      Place *process_p5  = TheNet->newPlace(prefix + "fault_in");
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
      Place *process_p21 = TheNet->newPlace(prefix + "stop");
      Place *process_p33 = TheNet->newPlace(prefix + "p33");
      Place *process_p34 = TheNet->newPlace(prefix + "stopped");
      Place *process_p35 = TheNet->newPlace(prefix + "cleanCH");
      Place *process_p36 = TheNet->newPlace(prefix + "ch_cleaned");
      Place *process_p37 = TheNet->newPlace(prefix + "fault");
      Place *process_p38 = TheNet->newPlace(prefix + "faultSave");
      Place *process_p39 = TheNet->newPlace(prefix + "final");
      Place *process_p40 = TheNet->newPlace(prefix + "rethrow");

      Transition *process_t2 =  TheNet->newTransition(prefix + "t2");
      Transition *process_t3 =  TheNet->newTransition(prefix + "t3");
      Transition *process_t4 =  TheNet->newTransition(prefix + "t4");
      
      TheNet->newArc(process_p1, process_t2);
      TheNet->newArc(process_t2, process_p7);
      TheNet->newArc(process_t2, process_p8);
      TheNet->newArc(process_t2, process_p10);
      TheNet->newArc(process_t2, process_p13);
      TheNet->newArc(process_t2, process_p15);
      TheNet->newArc(process_t2, process_p17);
      TheNet->newArc(process_t3, process_p33);
      TheNet->newArc(process_p7, process_t4);
      TheNet->newArc(process_p8, process_t4);
      TheNet->newArc(process_t4, process_p9);
      TheNet->newArc(process_t4, process_p11);
      TheNet->newArc(process_p33, process_t4);
      TheNet->newArc(process_t4, process_p39);
      /*---------------------------------------------------------------------*/



      
      /*---------------------------------------------------------------------*/
      // special places
      Place *process_clock = TheNet->newPlace(prefix + "clock");
      /*---------------------------------------------------------------------*/


      
      
      /*---------------------------------------------------------------------*/
      // the stop-pattern
      prefix = "process.stop.";
      Place *stop_p1  = TheNet->newPlace(prefix + "Faulted");
      Place *stop_p2  = TheNet->newPlace(prefix + "p2");
      Place *stop_p3  = TheNet->newPlace(prefix + "p3");
      Place *stop_p4  = TheNet->newPlace(prefix + "p4");
      Place *stop_p5  = TheNet->newPlace(prefix + "fault_in");
      Place *stop_p6  = TheNet->newPlace(prefix + "p6");
      Place *stop_p7  = TheNet->newPlace(prefix + "fault");
      Place *stop_p8  = TheNet->newPlace(prefix + "faultSave");
      Place *stop_p9  = TheNet->newPlace(prefix + "Active");
      Place *stop_p10 = TheNet->newPlace(prefix + "!Active");
      Place *stop_p11 = TheNet->newPlace(prefix + "p11");
      Place *stop_p12 = TheNet->newPlace(prefix + "final");
      Place *stop_p13 = TheNet->newPlace(prefix + "terminate");
      Place *stop_p14 = TheNet->newPlace(prefix + "!Faulted");
      Place *stop_p15 = TheNet->newPlace(prefix + "rethrow");
      Place *stop_p16 = TheNet->newPlace(prefix + "Ended");
      Place *stop_p17 = TheNet->newPlace(prefix + "Compensated");
      Place *stop_p18 = TheNet->newPlace(prefix + "p18");
      Place *stop_p19 = TheNet->newPlace(prefix + "p19");
      Place *stop_p20 = TheNet->newPlace(prefix + "!Ended");
      Place *stop_p21 = TheNet->newPlace(prefix + "stop");
      Place *stop_p22 = TheNet->newPlace(prefix + "stopped");
      Place *stop_p23 = TheNet->newPlace(prefix + "cleanCH");
      Place *stop_p24 = TheNet->newPlace(prefix + "ch_cleaned");
      Transition *stop_t1 =  TheNet->newTransition(prefix + "t1");
      Transition *stop_t2 =  TheNet->newTransition(prefix + "t2");
      Transition *stop_t3 =  TheNet->newTransition(prefix + "t3");
      Transition *stop_t4 =  TheNet->newTransition(prefix + "t4");
      Transition *stop_t5 =  TheNet->newTransition(prefix + "t5");
      Transition *stop_t6 =  TheNet->newTransition(prefix + "t6");
      Transition *stop_t7 =  TheNet->newTransition(prefix + "t7");
      Transition *stop_t8 =  TheNet->newTransition(prefix + "t8");
      Transition *stop_t9 =  TheNet->newTransition(prefix + "kill");
      Transition *stop_t10 = TheNet->newTransition(prefix + "t10");
      Transition *stop_t11 = TheNet->newTransition(prefix + "t11");
      Transition *stop_t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(stop_p1, stop_t1, READ);
      TheNet->newArc(stop_p5, stop_t1, "x");
      TheNet->newArc(stop_t1, stop_p15, "x");
      TheNet->newArc(stop_p2, stop_t2);
      TheNet->newArc(stop_t2, stop_p3);
      TheNet->newArc(stop_t2, stop_p21);
      TheNet->newArc(stop_p3, stop_t3);
      TheNet->newArc(stop_p5, stop_t3, RESET, "X");
      TheNet->newArc(stop_p22, stop_t3);
      TheNet->newArc(stop_t3, stop_p4);
      TheNet->newArc(stop_p5, stop_t4, "x");
      TheNet->newArc(stop_p9, stop_t4);
      TheNet->newArc(stop_t4, stop_p2);
      TheNet->newArc(stop_t4, stop_p6, "x");
      TheNet->newArc(stop_t4, stop_p10);
      TheNet->newArc(stop_p4, stop_t5);
      TheNet->newArc(stop_p6, stop_t5, "x");
      TheNet->newArc(stop_p14, stop_t5);
      TheNet->newArc(stop_t5, stop_p1);
      TheNet->newArc(stop_t5, stop_p7);
      TheNet->newArc(stop_t5, stop_p8, "x");
      TheNet->newArc(stop_p9, stop_t6);
      TheNet->newArc(stop_p13, stop_t6);
      TheNet->newArc(stop_t6, stop_p2);
      TheNet->newArc(stop_t6, stop_p10);
      TheNet->newArc(stop_t6, stop_p11);
      TheNet->newArc(stop_p4, stop_t7);
      TheNet->newArc(stop_p11, stop_t7);
      TheNet->newArc(stop_p20, stop_t7);
      TheNet->newArc(stop_t7, stop_p12);
      TheNet->newArc(stop_t7, stop_p16);
      TheNet->newArc(stop_p10, stop_t8, READ);
      TheNet->newArc(stop_p13, stop_t8);
      TheNet->newArc(stop_p16, stop_t9, READ);
      TheNet->newArc(stop_p18, stop_t9, "x");
      TheNet->newArc(stop_p5, stop_t10, "x");
      TheNet->newArc(stop_p17, stop_t10, READ);
      TheNet->newArc(stop_t10, stop_p18, "x");
      TheNet->newArc(stop_p18, stop_t11, "x");
      TheNet->newArc(stop_p20, stop_t11);
      TheNet->newArc(stop_t11, stop_p19, "x");
      TheNet->newArc(stop_t11, stop_p23);
      TheNet->newArc(stop_p19, stop_t12, "x");
      TheNet->newArc(stop_p24, stop_t12);
      TheNet->newArc(stop_t12, stop_p16);
      TheNet->newArc(stop_t12, stop_p12);

      // embed stop-pattern in process
      TheNet->mergePlaces("process.stop.Faulted",     "process.Faulted");
      TheNet->mergePlaces("process.stop.fault_in",    "process.fault_in");
      TheNet->mergePlaces("process.stop.fault",       "process.fault");
      TheNet->mergePlaces("process.stop.faultSave",   "process.faultSave");
      TheNet->mergePlaces("process.stop.Active",      "process.Active");
      TheNet->mergePlaces("process.stop.!Active",     "process.!Active");
      TheNet->mergePlaces("process.stop.final",       "process.final");
      TheNet->mergePlaces("process.stop.terminate",   "process.terminate");
      TheNet->mergePlaces("process.stop.!Faulted",    "process.!Faulted");
      TheNet->mergePlaces("process.stop.rethrow",     "process.rethrow");
      TheNet->mergePlaces("process.stop.Ended",       "process.Ended");
      TheNet->mergePlaces("process.stop.Compensated", "process.Compensated");
      TheNet->mergePlaces("process.stop.!Ended",      "process.!Ended");
      TheNet->mergePlaces("process.stop.stop",        "process.stop");
      TheNet->mergePlaces("process.stop.stopped",     "process.stopped");
      TheNet->mergePlaces("process.stop.cleanCH",     "process.cleanCH");
      TheNet->mergePlaces("process.stop.ch_cleaned",  "process.ch_cleaned");
      /*---------------------------------------------------------------------*/
    }
      a b c d e f g h
    {
      TheNet->newArc(process_t2, TheNet->findPlace(h, ".initial"));
      TheNet->newArc(TheNet->findPlace(h, ".final"), process_t3);
      TheNet->mergePlaces(TheNet->findPlace(h, ".stop"),
		          TheNet->findPlace("process.stop"));

      TheNet->mergePlaces(TheNet->findPlace(h, ".failed"),
		          TheNet->findPlace("process.fault_in"));
      
      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
  ;

/*---------------------------------------------------------------------------*/




/******************************************************************************
  PARTNER LINKS
******************************************************************************/


/******************************************************************************
  PARTNERS
******************************************************************************/


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

FaultHandlers(a,b) ->
  [petrinet:
    {
      // inside a <process>
      std::string prefix = "process.faulthandler.";
      
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
      prefix = "process.faulthandler.compensate.";

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
      TheNet->mergePlaces(compensate_p1, p8);
      TheNet->mergePlaces(compensate_p3, p9);
      TheNet->mergePlaces(compensate_p4, p11);
      TheNet->mergePlaces(compensate_p5, p17);
      TheNet->mergePlaces(compensate_p6, p18);
      TheNet->mergePlaces(compensate_p7, p12);
      /*---------------------------------------------------------------------*/
    }
    a b
    {
      /*---------------------------------------------------------------------*/
      prefix = "process.faulthandler.";
      
      foreach (catchLeaf=Catch(catchActivity=activity); tCatch_list a)
      {
	string faultToCatch = catchLeaf->faultName->name;
	Transition *t1 = TheNet->newTransition(prefix + "t1." + faultToCatch, faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	TheNet->newArc(p4, t1);
	TheNet->newArc(p7, t1, READ, "x");

	TheNet->newArc(t1, TheNet->findPlace(catchActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(catchActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace(catchActivity, ".stop"),
			    TheNet->findPlace("process.faulthandler.p11"));
	TheNet->mergePlaces(TheNet->findPlace(catchActivity, ".stopped"),
			    TheNet->findPlace("process.faulthandler.p12"));
	
	TheNet->newArc(t2, p3);
	TheNet->newArc(p7, t2, "x");
      }

      foreach (catchAllLeaf=CatchAll(catchAllActivity=activity); tCatchAll_list b)
      {
	Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll", "catchAll");
	Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");

	TheNet->newArc(p4, t3);
	TheNet->newArc(p7, t3, READ, "x");

	TheNet->newArc(t3, TheNet->findPlace(catchAllActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(catchAllActivity, ".final"), t4);
	TheNet->mergePlaces(TheNet->findPlace(catchAllActivity, ".stop"),
			    TheNet->findPlace("process.faulthandler.p11"));
	TheNet->mergePlaces(TheNet->findPlace(catchAllActivity, ".stopped"),
			    TheNet->findPlace("process.faulthandler.p12"));

	TheNet->newArc(t4, p3);
	TheNet->newArc(p7, t4, "x");
      }
      /*---------------------------------------------------------------------*/
      
      TheNet->mergePlaces(TheNet->findPlace("process.fault"),
		          TheNet->findPlace("process.faulthandler.fault"));
      TheNet->mergePlaces(TheNet->findPlace("process.faultSave"),
		          TheNet->findPlace("process.faulthandler.faultSave"));
      TheNet->mergePlaces(TheNet->findPlace("process.rethrow"),
		          TheNet->findPlace("process.faulthandler.rethrow"));
      TheNet->mergePlaces(TheNet->findPlace("process.!Ended"),
		          TheNet->findPlace("process.faulthandler.!Ended"));
      TheNet->mergePlaces(TheNet->findPlace("process.Ended"),
		          TheNet->findPlace("process.faulthandler.Ended"));
      TheNet->mergePlaces(TheNet->findPlace("process.ch_fh"),
		          TheNet->findPlace("process.faulthandler.ch_fh"));
      TheNet->mergePlaces(TheNet->findPlace("process.ch_out"),
		          TheNet->findPlace("process.faulthandler.ch_out"));
      TheNet->mergePlaces(TheNet->findPlace("process.final"),
		          TheNet->findPlace("process.faulthandler.final"));
    }
  ]
;

/******************************************************************************
  EVENT HANDLERS
******************************************************************************/


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/


/******************************************************************************
  VARIABLES
******************************************************************************/


/******************************************************************************
  CORRELATION SETS
******************************************************************************/


/******************************************************************************
  CORRELATIONS
******************************************************************************/


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
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
    } a
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

// asynchronous invoke in case of initiate="no"
Invoke(a,b,c,d,e) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
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
    } a b
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

// receive in case of initiate="no"
Receive(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
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
    } a b
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

// reply in case of initiate="no"
Reply(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
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
    } a b
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

// dummy assign! (in fact an <empty>)

Assign(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
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
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
     
      if ( ($0->For) != mkcasestring("") )
      {
	std::string guard = "(x + " + std::string($0->For->name) + " \\<= y)"; // "<=" is escaped
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
	TheNet->newArc(TheNet->findPlace("process.clock"), t1, READ, "x");
	TheNet->newArc(p2, t2, "x");
	TheNet->newArc(t2, p3);
	TheNet->newArc(TheNet->findPlace("process.clock"), t2, READ, "y");
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
      else if ( ($0->until) != mkcasestring("") )
      {
	std::string guard = "(" + std::string($0->until->name) + " \\<= x)"; // "<=" is escaped
  	Place *p1 = TheNet->newPlace(prefix + "initial");
      	Place *p2 = TheNet->newPlace(prefix + "final");
	Place *p3 = TheNet->newPlace(prefix + "stop");
	Place *p4 = TheNet->newPlace(prefix + "stopped");
  	Transition *t1  = TheNet->newTransition(prefix + "t1", guard);
    	Transition *t2  = TheNet->newTransition(prefix + "t2");
      	Transition *t3  = TheNet->newTransition(prefix + "t3");
	TheNet->newArc(p1, t1);
	TheNet->newArc(t1, p2);
	TheNet->newArc(TheNet->findPlace("process.clock"), t1, READ, "x");
	TheNet->newArc(p1, t2);
	TheNet->newArc(p3, t2);
	TheNet->newArc(t2, p4);
	TheNet->newArc(p2, t3);
	TheNet->newArc(p3, t3);
	TheNet->newArc(t3, p4);
      }
    }
  a
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

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
      TheNet->newArc(t1, p5, $0->faultName);
      TheNet->newArc(p1, t2);
      TheNet->newArc(p3, t2);
      TheNet->newArc(t2, p4);
      TheNet->newArc(p2, t3);
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);
    }
  a
  ]
;

/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5;
      Place *p6;
      Place *p7 = TheNet->newPlace(prefix + "stopped");

      if ($0->inFaultHandler)
      {
	p5 = TheNet->newPlace(prefix + "ch_fh");
	p6 = TheNet->newPlace(prefix + "ch_out");
      }
      else
      {
	p5 = TheNet->newPlace(prefix + "comp");
	p6 = TheNet->newPlace(prefix + "done");
      }
      
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
  a
  ]
;

/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "!Terminate");
      Place *p5 = TheNet->newPlace(prefix + "Terminate");
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
    }
  a
  ]
;

/******************************************************************************
  FLOW
******************************************************************************/

activityFlow(Flow(a,b,c)) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Place *p7  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p9  = TheNet->newPlace(prefix + "running");
      Place *p15 = TheNet->newPlace(prefix + "stop");
      Place *p16 = TheNet->newPlace(prefix + "stopped");
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
    a b c
    {
      foreach (e; activity_list c)
      {
	Place *initial = TheNet->findPlace(e, ".initial");
	Place *stop    = TheNet->findPlace(e, ".stop");
	Place *stopped = TheNet->findPlace(e, ".stopped");
	Place *final   = TheNet->findPlace(e, ".final");
	TheNet->newArc(t2, initial);
	TheNet->newArc(t5, stop);
	TheNet->newArc(stopped, t6);
	TheNet->newArc(final, t3);
      }	
    }
  ]  
;

/******************************************************************************
  SWITCH
******************************************************************************/

s=activitySwitch(Switch(a,b,c)) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

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
    a b c
    {
      int currentCase = 1;
      
      Transition *last;

      foreach (o=Case(p=activity); tCase_list b)
      {
	std::string condition = o->condition->name;
	
	if (currentCase == 1)
	{
	  // two transitions: guard fulfilled or not
	  Transition *t2 = TheNet->newTransition(prefix + "t2.case1", "!guard & (" + condition + ")");
	  Transition *t4 = TheNet->newTransition(prefix + "t4.case1", "!guard & !(" + condition + ")");
	  
	  // arcs connecting "running"-place
	  TheNet->newArc(TheNet->findPlace(s, ".running"), t2, "x");
	  TheNet->newArc(TheNet->findPlace(s, ".running"), t4, "x");
	
	  // embed the innerActivity
	  TheNet->newArc(t2, TheNet->findPlace(p, ".initial"));
	  TheNet->mergePlaces(p, ".final", s, ".final");
	  TheNet->mergePlaces(p, ".stop", s, ".stop");
	  TheNet->mergePlaces(p, ".stopped", s, ".stopped");
	  
	  last = t4;
	}
	else
	{
	  // two transitions: guard fulfilled or not
	  std::string caseNumber = intToString(currentCase);
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
	  TheNet->newArc(TheNet->findPlace(s, ".stop"), t11);
	  TheNet->newArc(t11, TheNet->findPlace(s, ".stopped"));

	  // embed the innerActivity
	  TheNet->newArc(t2, TheNet->findPlace(p, ".initial"));
	  TheNet->mergePlaces(p, ".final", s, ".final");
	  TheNet->mergePlaces(p, ".stop", s, ".stop");
	  TheNet->mergePlaces(p, ".stopped", s, ".stopped");

	  last = t4;
	}		

	currentCase++;
      }


      foreach (Otherwise(p=activity); tOtherwise_list c)
      {
	Transition *t2 = TheNet->newTransition(prefix + "t2.otherwise");
	
	// "running"-place
	Place *running = TheNet->newPlace(prefix + "running.otherwise");

	// arcs connecting "running"-place
	TheNet->newArc(running, t2, "x");
	TheNet->newArc(last, running, "x");

	// remove token from "running"-place to stop
	Transition *t11 = TheNet->newTransition(prefix + "t11.o");
	TheNet->newArc(running, t11, "x");
	TheNet->newArc(TheNet->findPlace(s, ".stop"), t11);
	TheNet->newArc(t11, TheNet->findPlace(s, ".stopped"));	
	
	// embed inner activity
	TheNet->newArc(t2, TheNet->findPlace(p, ".initial"));
	TheNet->mergePlaces(p, ".final", s, ".final");
	TheNet->mergePlaces(p, ".stop", s, ".stop");
	TheNet->mergePlaces(p, ".stopped", s, ".stopped");
      }
    }
  ]
;

/******************************************************************************
  WHILE
******************************************************************************/

activityWhile(While(a,b)) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      std::string condition = $1->condition->name;

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
    a b
    {
      TheNet->mergePlaces(b, ".initial", $0, ".p4");
      TheNet->mergePlaces(b, ".final", $0, ".p5");
      TheNet->mergePlaces(b, ".stop", $0, ".stop");
      TheNet->mergePlaces(b, ".stopped", $0, ".stopped");
    }
  ]  
;

/******************************************************************************
  SEQUENCE
******************************************************************************/

activitySequence(Sequence(a,b)) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p9 = TheNet->newPlace(prefix + "final");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "stopped");
    }
    a b
    {
      // merge places
      int i = 0;
      kc::impl_activity* last;
      
      foreach (e; activity_list b)
      {
	if (i == 0)
	  TheNet->mergePlaces(e, ".initial", $0, ".initial");
	else
	  TheNet->mergePlaces(e, ".initial", last, ".final");
	
	if (e == b->last())
	  TheNet->mergePlaces(e, ".final", $0, ".final");
	
	last = e;
	i++;

	TheNet->mergePlaces(e, ".stopped", $0, ".stopped");
	TheNet->mergePlaces(e, ".stop", $0, ".stop");
      }
    }
  ]  
;


/******************************************************************************
  PICK
******************************************************************************/

activityPick(Pick(a,b,c)) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Place *p3 = TheNet->newPlace(prefix + "p3");
      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");
      Place *p13 = TheNet->newPlace(prefix + "failed");

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      Transition *t9 = TheNet->newTransition(prefix + "t9");

      TheNet->newArc(p7, t2, "x");
      TheNet->newArc(t2, p3, "(M1,CS1)");

      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7, "x");

      TheNet->newArc(p6, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(t7, p12);

      TheNet->newArc(p7, t8, "x");
      TheNet->newArc(p11, t8);
      TheNet->newArc(t8, p12);
      
      TheNet->newArc(p3, t9, "(M1,CS1)");
      TheNet->newArc(p11, t9);
      TheNet->newArc(t9, p12);
    }
    a b c
  ]
;

/******************************************************************************
  SCOPE
******************************************************************************/


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/






/*****************************************************************************/
// the printer functions

/// "fake" printer-function
void pseudoPrinter(const char *s, uview v)
{
}

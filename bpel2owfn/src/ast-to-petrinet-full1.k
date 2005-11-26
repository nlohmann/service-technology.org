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
 *          - last changed: \$Date: 2005/11/26 13:29:13 $
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
 * \version \$Revision: 1.25 $
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
 * 
 * \todo
 *          - find a simple "merge-language" to ease the definition of new
 *            patterns
 *          - complete initial pattern database
 *          - special treatment of <invoke>
 *          - tidy up case discrimination of <switch>
 *          - tidy up patterns <faultHandlers> and <process>
 *          - What about the fault-places: are they always merged with the FH?
 *          - When do inner activities should be merged: upon creation or
 *            later?
 *          - The assign-element has more attributes that have to be added to
 *            the abstract grammar as well as to the syntax file.
 *          - The attribute "initiate" is missing.
 *          - All faults have to be merged (failed-places).
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
      Place *process_p19 = TheNet->newPlace(prefix + "p19");
      Place *process_p20 = TheNet->newPlace(prefix + "p20");
      Place *process_p21 = TheNet->newPlace(prefix + "stop");
      Place *process_p22 = TheNet->newPlace(prefix + "p22");
      Place *process_p23 = TheNet->newPlace(prefix + "p23");
      Place *process_p24 = TheNet->newPlace(prefix + "p24");
      Place *process_p25 = TheNet->newPlace(prefix + "p25");
      Place *process_p26 = TheNet->newPlace(prefix + "p26");
      Place *process_p27 = TheNet->newPlace(prefix + "p27");
      Place *process_p33 = TheNet->newPlace(prefix + "p33");
      Place *process_p34 = TheNet->newPlace(prefix + "stopped");
      Place *process_p35 = TheNet->newPlace(prefix + "cleanCH");
      Place *process_p36 = TheNet->newPlace(prefix + "ch_cleaned");
      Place *process_p37 = TheNet->newPlace(prefix + "fault");
      Place *process_p38 = TheNet->newPlace(prefix + "faultSave");
      Place *process_p39 = TheNet->newPlace(prefix + "final");
      Place *process_p40 = TheNet->newPlace(prefix + "rethrow");
      Transition *process_t1 =  TheNet->newTransition(prefix + "t1");
      Transition *process_t2 =  TheNet->newTransition(prefix + "t2");
      Transition *process_t3 =  TheNet->newTransition(prefix + "t3");
      Transition *process_t4 =  TheNet->newTransition(prefix + "t4");
      Transition *process_t5 =  TheNet->newTransition(prefix + "t5");
      Transition *process_t6 =  TheNet->newTransition(prefix + "t6");   
      TheNet->newArc(process_p27, process_t1);
      TheNet->newArc(process_t1, process_p34);
      TheNet->newArc(process_p1, process_t2);
      TheNet->newArc(process_t2, process_p7);
      TheNet->newArc(process_t2, process_p8);
      TheNet->newArc(process_t2, process_p10);
      TheNet->newArc(process_t2, process_p13);
      TheNet->newArc(process_t2, process_p15);
      TheNet->newArc(process_t2, process_p17);
      TheNet->newArc(process_t2, process_p19);
      TheNet->newArc(process_t2, process_p23);      
      TheNet->newArc(process_p20, process_t3);
      TheNet->newArc(process_t3, process_p33);
      TheNet->newArc(process_t3, process_p24);
      TheNet->newArc(process_p7, process_t4);
      TheNet->newArc(process_p8, process_t4);
      TheNet->newArc(process_t4, process_p9);
      TheNet->newArc(process_t4, process_p11);
      TheNet->newArc(process_p25, process_t4);
      TheNet->newArc(process_p33, process_t4);
      TheNet->newArc(process_t4, process_p39);
      TheNet->newArc(process_p22, process_t5);
      TheNet->newArc(process_t5, process_p26);
      TheNet->newArc(process_t6, process_p26);
      TheNet->newArc(process_p21, process_t6);
      TheNet->newArc(process_p33, process_t6);

      
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
      TheNet->newArc(stop_p5, stop_t3, "X"); // here was the RESET arc (now of type STANDARD)
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
      // embed the inner activity
      TheNet->mergePlaces(TheNet->findPlace("process.p19"), TheNet->findPlace(h, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace("process.p20"), TheNet->findPlace(h, ".final"));
      TheNet->mergePlaces(TheNet->findPlace("process.p21"), TheNet->findPlace(h, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace("process.p22"), TheNet->findPlace(h, ".stopped"));

      TheNet->mergePlaces("process.p23", "process.eventHandler.initial");
      TheNet->mergePlaces("process.p24", "process.eventHandler.finish");
      TheNet->mergePlaces("process.p25", "process.eventHandler.final");
      TheNet->mergePlaces("process.p26", "process.eventHandler.stop");
      TheNet->mergePlaces("process.p27", "process.eventHandler.stopped");
      
      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
  ;





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

/*
 * The user-defined fault handler inside a process. See Fig. 36 for details.
 */
  
ConstFaultHandlers_opt(FaultHandlers(a,b), NiltFaultHandlers_opt()) ->
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
      std::string new_prefix = "process.faulthandler.compensate.";

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
      foreach (catchLeaf=Catch(innerActivity); tCatch_list a)
      {
	string faultToCatch = catchLeaf->faultName->name;
	Transition *t1 = TheNet->newTransition(prefix + "t1." + faultToCatch, faultToCatch);
	Transition *t2 = TheNet->newTransition(prefix + "t2." + faultToCatch);

	TheNet->newArc(p4, t1);
	TheNet->newArc(p7, t1, READ, "x");

	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stop"),
			    TheNet->findPlace("process.faulthandler.p11"));
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stopped"),
			    TheNet->findPlace("process.faulthandler.p12"));
	
	TheNet->newArc(t2, p3);
	TheNet->newArc(p7, t2, "x");
      }

      foreach (CatchAll(innerActivity); tCatchAll_list b)
      {
	Transition *t3 = TheNet->newTransition(prefix + "t3.catchAll", "catchAll");
	Transition *t4 = TheNet->newTransition(prefix + "t4.catchAll");

	TheNet->newArc(p4, t3);
	TheNet->newArc(p7, t3, READ, "x");

	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t4);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stop"),
			    TheNet->findPlace("process.faulthandler.p11"));
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stopped"),
			    TheNet->findPlace("process.faulthandler.p12"));

	TheNet->newArc(t4, p3);
	TheNet->newArc(p7, t4, "x");
      }
      /*---------------------------------------------------------------------*/
      
      // embed the fault handler
      TheNet->mergePlaces("process.fault",     "process.faulthandler.fault");
      TheNet->mergePlaces("process.faultSave", "process.faulthandler.faultSave");
      TheNet->mergePlaces("process.rethrow",   "process.faulthandler.rethrow");
      TheNet->mergePlaces("process.!Ended",    "process.faulthandler.!Ended");
      TheNet->mergePlaces("process.Ended",     "process.faulthandler.Ended");
      TheNet->mergePlaces("process.ch_fh",     "process.faulthandler.ch_fh");
      TheNet->mergePlaces("process.ch_out",    "process.faulthandler.ch_out");
      TheNet->mergePlaces("process.final",     "process.faulthandler.final");      
    }
  ]
;





/*
 * The implicit fault handler inside a process. See Fig. 35 for details.
 */

NiltFaultHandlers_opt() ->
  [petrinet:
    {
      std::string prefix = "process.faulthandler.";
      
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
      TheNet->mergePlaces(compensate_p1, p1);
      TheNet->mergePlaces(compensate_p3, p2);
      TheNet->mergePlaces(compensate_p4, p3);
      TheNet->mergePlaces(compensate_p5, p4);
      TheNet->mergePlaces(compensate_p6, p5);
      TheNet->mergePlaces(compensate_p7, p6);
      /*---------------------------------------------------------------------*/

      // embed the fault handler
      TheNet->mergePlaces("process.fault",     "process.faulthandler.fault");
      TheNet->mergePlaces("process.faultSave", "process.faulthandler.faultSave");
      TheNet->mergePlaces("process.rethrow",   "process.faulthandler.rethrow");
      TheNet->mergePlaces("process.!Ended",    "process.faulthandler.!Ended");
      TheNet->mergePlaces("process.Ended",     "process.faulthandler.Ended");
      TheNet->mergePlaces("process.ch_fh",     "process.faulthandler.ch_fh");
      TheNet->mergePlaces("process.ch_out",    "process.faulthandler.ch_out");
      TheNet->mergePlaces("process.final",     "process.faulthandler.final");      
    }
  ]
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

ConstEventHandlers_opt(EventHandlers(a,b), NiltEventHandlers_opt()) ->
  [petrinet:
    {
      std::string prefix = "process.eventHandler.";
      
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");
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
      TheNet->newArc(TheNet->findPlace("process.clock"), t5, READ, "x");
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
    a b
    {
      // the <onMessage>s
      int onMessageCount = 1;

      foreach (OnMessage(innerActivity); tOnMessage_list a)
      {
	std::string newprefix = prefix + "onMessage" + intToString(onMessageCount) + ".";

	Place *onMessage_p1  = TheNet->newPlace(newprefix + "p1");
	Place *onMessage_p2  = TheNet->newPlace(newprefix + "p2");
	Place *onMessage_p3  = TheNet->newPlace(newprefix + "p3");
	Place *onMessage_p4  = TheNet->newPlace(newprefix + "p4");
	Place *onMessage_p5  = TheNet->newPlace(newprefix + "wait" + intToString(onMessageCount) );
	Place *onMessage_p6  = TheNet->newPlace(newprefix + "p6");
	Place *onMessage_p7  = TheNet->newPlace(newprefix + "finish" + intToString(onMessageCount) );
	Place *onMessage_p9  = TheNet->newPlace(newprefix + "finish+stop" + intToString(onMessageCount) );
	Place *onMessage_p11 = TheNet->newPlace(newprefix + "stop" + intToString(onMessageCount) );
	Place *onMessage_p12 = TheNet->newPlace(newprefix + "p12");
	Transition *onMessage_t1  = TheNet->newTransition(newprefix + "t1", "guard" + intToString(onMessageCount) );
	Transition *onMessage_t2  = TheNet->newTransition(newprefix + "t2");
	Transition *onMessage_t3  = TheNet->newTransition(newprefix + "t3", "!guard" + intToString(onMessageCount) );
	Transition *onMessage_t4  = TheNet->newTransition(newprefix + "t4");
	Transition *onMessage_t5  = TheNet->newTransition(newprefix + "t5");
	Transition *onMessage_t7  = TheNet->newTransition(newprefix + "t7");
	Transition *onMessage_t8  = TheNet->newTransition(newprefix + "t8");
	Transition *onMessage_t10 = TheNet->newTransition(newprefix + "t10");
	Transition *onMessage_t11 = TheNet->newTransition(newprefix + "t11");
	Transition *onMessage_t12 = TheNet->newTransition(newprefix + "t12");
	TheNet->newArc(onMessage_p2, onMessage_t1, "(M" + intToString(onMessageCount) + ",CS" + intToString(onMessageCount) + ")" );
	TheNet->newArc(onMessage_t1, onMessage_p1);
	TheNet->newArc(onMessage_t2, onMessage_p2, "(M" + intToString(onMessageCount) + ",CS" + intToString(onMessageCount) + ")" );
	TheNet->newArc(onMessage_p5, onMessage_t2);
	TheNet->newArc(onMessage_p2, onMessage_t3, "(M" + intToString(onMessageCount) + ",CS" + intToString(onMessageCount) + ")" );
	TheNet->newArc(onMessage_t3, onMessage_p3);
	TheNet->newArc(onMessage_t4, onMessage_p5);
	TheNet->newArc(onMessage_p6, onMessage_t4);
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
	TheNet->newArc(onMessage_p2, onMessage_t11, "(M" + intToString(onMessageCount) + ",CS" + intToString(onMessageCount) + ")" );
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
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".initial"), onMessage_p3);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stop"), onMessage_p11);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stopped"), onMessage_p12);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".final"), onMessage_p4);
	
	onMessageCount++;
      }


      // the <onAlarm>s
      int onAlarmCount = 1;
      foreach (o=OnAlarm(innerActivity); tOnAlarm_list b)
      {
	std::string onAlarmGuard;
	std::string newprefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";

	if (o->For != mkcasestring(""))
	  onAlarmGuard = "x + " + std::string(o->For->name) + " \\<= y";  // "<=" is escaped
	else
	  onAlarmGuard = std::string(o->until->name) + " \\<= y";  // "<=" is escaped
	
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
	TheNet->newArc(TheNet->findPlace("process.clock"), onAlarm_t1, READ, "y");
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
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".initial"), onAlarm_p1);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stop"), onAlarm_p9);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".stopped"), onAlarm_p10);
	TheNet->mergePlaces(TheNet->findPlace(innerActivity, ".final"), onAlarm_p2);
	
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

NiltEventHandlers_opt() ->
  [petrinet:
    {
      std::string prefix = "process.eventHandler.";
      
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





/******************************************************************************
  EMPTY
******************************************************************************/

This=Empty(a) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
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

// asynchronous invoke (case initiate="no")
This=Invoke(a,b,c,d,e) provided (This->outputVariable == mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

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

// synchronous invoke (case initiate="no")
This=Invoke(a,b,c,d,e) provided (This->outputVariable != mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

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
    } a b
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

// receive in case of initiate="no"
This=Receive(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
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
This=Reply(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
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

This=Assign(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
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

/*
 * The two patterns for a wait element:
 * - with an until condition (Fig. 4)
 * - with a for expression (Fig. 5)
 */

This=Wait(a) provided (This->until != mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
      std::string guard = "(" + std::string(This->until->name) + " \\<= x)"; // "<=" is escaped
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
    a
  ]
;

This=Wait(a) provided (This->For != mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";
      
      std::string guard = "(x + " + std::string(This->For->name) + " \\<= y)"; // "<=" is escaped
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
      std::string prefix = intToString(This->id->value) + ".";

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
  a
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
      std::string prefix = intToString(This->id->value) + ".";

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
  a
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope == mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

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
  a
  ]
;

This=Compensate(a) provided (!This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "compScope");
      Place *p6 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", std::string(This->scope->name) + "=scopeName");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, std::string(This->scope->name));
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
  a
  ]
;

This=Compensate(a) provided (This->inFaultHandler && This->scope != mkcasestring("")) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "stop");
      Place *p5 = TheNet->newPlace(prefix + "ch_fh");
      Place *p6 = TheNet->newPlace(prefix + "compensated");
      Place *p7 = TheNet->newPlace(prefix + "stopped");
      Transition *t1  = TheNet->newTransition(prefix + "t1");
      Transition *t2  = TheNet->newTransition(prefix + "t2", std::string(This->scope->name) + "=scopeName");
      Transition *t3  = TheNet->newTransition(prefix + "t3");
      Transition *t4  = TheNet->newTransition(prefix + "t4");
      Transition *t5  = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, p5, std::string(This->scope->name));
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
  a
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
      std::string prefix = intToString(This->id->value) + ".";

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

      TheNet->mergePlaces(p4, TheNet->findPlace("process.!Terminated"));
      TheNet->mergePlaces(p5, TheNet->findPlace("process.Terminated"));
      TheNet->mergePlaces(p6, TheNet->findPlace("process.terminate"));
    }
  a
  ]
;





/******************************************************************************
  FLOW
******************************************************************************/

/*
 * Flow as it is depicted in Fig. 17.
 */

This=Flow(a,b,c) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

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
      foreach (innerActivity; activity_list c)
      {
	Place *innerActivity_initial = TheNet->findPlace(innerActivity, ".initial");
	Place *innerActivity_stop    = TheNet->findPlace(innerActivity, ".stop");
	Place *innerActivity_stopped = TheNet->findPlace(innerActivity, ".stopped");
	Place *innerActivity_final   = TheNet->findPlace(innerActivity, ".final");
	TheNet->newArc(t2, innerActivity_initial);
	TheNet->newArc(t5, innerActivity_stop);
	TheNet->newArc(innerActivity_stopped, t6);
	TheNet->newArc(innerActivity_final, t3);
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

This=activitySwitch(Switch(a,b,c)) ->
  [petrinet:
    {
      std::string prefix = intToString(This->id->value) + ".";

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
      // the case branches
      int currentCase = 1;
      Transition *last;
      foreach (o=Case(innerActivity); tCase_list b)
      {
	std::string condition = o->condition->name;
	
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
	  TheNet->mergePlaces(innerActivity, ".final",   This, ".final");
	  TheNet->mergePlaces(innerActivity, ".stop",    This, ".stop");
	  TheNet->mergePlaces(innerActivity, ".stopped", This, ".stopped");
	  
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
	  TheNet->newArc(TheNet->findPlace(This, ".stop"), t11);
	  TheNet->newArc(t11, TheNet->findPlace(This, ".stopped"));

	  // embed the innerActivity
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	  TheNet->mergePlaces(innerActivity, ".final",   This, ".final");
	  TheNet->mergePlaces(innerActivity, ".stop",    This, ".stop");
	  TheNet->mergePlaces(innerActivity, ".stopped", This, ".stopped");

	  last = t4;
	}
	currentCase++;
      }

      // the otherwise branch
      foreach (Otherwise(innerActivity); tOtherwise_list c)
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
	TheNet->newArc(TheNet->findPlace(This, ".stop"), t11);
	TheNet->newArc(t11, TheNet->findPlace(This, ".stopped"));	
	
	// embed inner activity
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(innerActivity, ".final",   This, ".final");
	TheNet->mergePlaces(innerActivity, ".stop",    This, ".stop");
	TheNet->mergePlaces(innerActivity, ".stopped", This, ".stopped");
      }
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

/*
 * While as it is depicted in Fig. 18.
 */

TheActivity=activityWhile(TheWhile=While(a,b)) ->
  [petrinet:
    {
      std::string prefix = intToString(TheWhile->id->value) + ".";
      std::string condition = std::string(TheWhile->condition->name);

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
      TheNet->mergePlaces(b, ".initial", TheActivity, ".p4");
      TheNet->mergePlaces(b, ".final", TheActivity, ".p5");
      TheNet->mergePlaces(b, ".stop", TheActivity, ".stop");
      TheNet->mergePlaces(b, ".stopped", TheActivity, ".stopped");
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
      std::string prefix = intToString(This->id->value) + ".";

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
      
      foreach (innerActivity; activity_list b)
      {
	if (i == 0)
	  TheNet->mergePlaces(innerActivity, ".initial", This, ".initial");
	else
	  TheNet->mergePlaces(innerActivity, ".initial", last, ".final");
	
	if (innerActivity == b->last())
	  TheNet->mergePlaces(innerActivity, ".final", This, ".final");
	
	last = innerActivity;
	i++;

	TheNet->mergePlaces(innerActivity, ".stopped", This, ".stopped");
	TheNet->mergePlaces(innerActivity, ".stop", This, ".stop");
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
      std::string prefix = intToString(This->id->value) + ".";

      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
      Place *p7 = TheNet->newPlace(prefix + "TimeStamp");
      Place *p11 = TheNet->newPlace(prefix + "stop");
      Place *p12 = TheNet->newPlace(prefix + "stopped");
      Place *p13 = TheNet->newPlace(prefix + "failed");
      Transition *t5 = TheNet->newTransition(prefix + "t5");
      Transition *t7 = TheNet->newTransition(prefix + "t7");
      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(TheNet->findPlace("process.clock"), t5, READ, "x");
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p7, "x");
      TheNet->newArc(p6, t7);
      TheNet->newArc(p11, t7);
      TheNet->newArc(t7, p12);
      TheNet->newArc(p7, t8, "x");
      TheNet->newArc(p11, t8);
      TheNet->newArc(t8, p12);      
    }
    a b c
    {
      // the onMessage branches
      int onMessageCount = 1;      
      foreach (OnMessage(innerActivity); tOnMessage_list b)
      {
	std::string newprefix = prefix + "onMessage" + intToString(onMessageCount) + ".";

	Place *p3 = TheNet->newPlace(newprefix + "p3");
	Place *p10 = TheNet->newPlace(newprefix + "p10");
	Transition *t2 = TheNet->newTransition(newprefix + "t2");
	Transition *t3 = TheNet->newTransition(newprefix + "t3", "!guard" + intToString(onMessageCount));
	Transition *t4 = TheNet->newTransition(newprefix + "t4", "guard" + intToString(onMessageCount));
	Transition *t9 = TheNet->newTransition(newprefix + "t9");
	Transition *t10 = TheNet->newTransition(newprefix + "t10");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".TimeStamp"), t2, "x");
	TheNet->newArc(t2, p3, "M" + intToString(onMessageCount) );
	TheNet->newArc(p3, t3, "M" + intToString(onMessageCount) );
	TheNet->newArc(t3, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(p3, t4, "M" + intToString(onMessageCount) );
	TheNet->newArc(t4, p10);
	TheNet->newArc(t4, TheNet->findPlace(TheActivity, ".failed"), "fault");
	TheNet->newArc(p3, t9, "M" + intToString(onMessageCount) );
	TheNet->newArc(TheNet->findPlace(TheActivity, ".stop"), t9);
	TheNet->newArc(t9, TheNet->findPlace(TheActivity, ".stopped"));
	TheNet->newArc(p10, t10);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".stop"), t10);
	TheNet->newArc(t10, TheNet->findPlace(TheActivity, ".stopped"));

	TheNet->mergePlaces(innerActivity, ".stop", TheActivity, ".stop");
	TheNet->mergePlaces(innerActivity, ".stopped", TheActivity, ".stopped");
	TheNet->mergePlaces(innerActivity, ".final", TheActivity, ".final");

	onMessageCount++;
      }

      // the onAlarm branches
      int onAlarmCount = 1;      
      foreach (o=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	std::string newprefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";
	std::string onAlarmGuard;

	if (o->For != mkcasestring(""))
	  onAlarmGuard = "x + " + std::string(o->For->name) + " \\<= y";  // "<=" is escaped
	else
	  onAlarmGuard = std::string(o->until->name) + " \\<= y";  // "<=" is escaped

	Transition *t6 = TheNet->newTransition(newprefix + "t6", onAlarmGuard);
	TheNet->newArc(TheNet->findPlace("process.clock"), t6, READ, "y");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".TimeStamp"), t6, "x");
	TheNet->newArc(t6, TheNet->findPlace(innerActivity, ".initial"));
	
	TheNet->mergePlaces(innerActivity, ".stop", TheActivity, ".stop");
	TheNet->mergePlaces(innerActivity, ".stopped", TheActivity, ".stopped");
	TheNet->mergePlaces(innerActivity, ".final", TheActivity, ".final");

	onMessageCount++;
      }
    }
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

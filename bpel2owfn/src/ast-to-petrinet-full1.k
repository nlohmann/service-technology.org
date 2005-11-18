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
 *          - last changed: \$Date: 2005/11/18 16:54:30 $
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
 * 
 * \todo
 *          - find a simple "merge-language" to ease the definition of new
 *            patterns
 *          - complete initial pattern database
 *          - special treatment of <invoke>
 *          - tidy up case discrimination of <switch>
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
      Node *process_p1  = TheNet->newPlace(prefix + "initial");
      Node *process_p5  = TheNet->newPlace(prefix + "fault_in", DATA);
      Node *process_p6  = TheNet->newPlace(prefix + "terminate");
      Node *process_p7  = TheNet->newPlace(prefix + "Active");
      Node *process_p8  = TheNet->newPlace(prefix + "!Completed");
      Node *process_p9  = TheNet->newPlace(prefix + "Completed");
      Node *process_p10 = TheNet->newPlace(prefix + "!Compensated");
      Node *process_p11 = TheNet->newPlace(prefix + "!Active");
      Node *process_p12 = TheNet->newPlace(prefix + "Compensated");
      Node *process_p13 = TheNet->newPlace(prefix + "!Ended");
      Node *process_p14 = TheNet->newPlace(prefix + "Ended");
      Node *process_p15 = TheNet->newPlace(prefix + "!Faulted");
      Node *process_p16 = TheNet->newPlace(prefix + "Faulted");
      Node *process_p17 = TheNet->newPlace(prefix + "!Terminated");
      Node *process_p18 = TheNet->newPlace(prefix + "Terminated");
      Node *process_p21 = TheNet->newPlace(prefix + "stop");
      Node *process_p33 = TheNet->newPlace(prefix + "p33");
      Node *process_p34 = TheNet->newPlace(prefix + "stopped");
      Node *process_p35 = TheNet->newPlace(prefix + "cleanCH");
      Node *process_p36 = TheNet->newPlace(prefix + "ch_cleaned");
      Node *process_p37 = TheNet->newPlace(prefix + "fault");
      Node *process_p38 = TheNet->newPlace(prefix + "faultSave", DATA);
      Node *process_p39 = TheNet->newPlace(prefix + "final");
      Node *process_p40 = TheNet->newPlace(prefix + "rethrow", DATA);

      Node *process_clock = TheNet->newPlace(prefix + "clock", DATA);

      Node *process_t2 =  TheNet->newTransition(prefix + "t2");
      Node *process_t3 =  TheNet->newTransition(prefix + "t3");
      Node *process_t4 =  TheNet->newTransition(prefix + "t4");
      
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

      
      // the stop pattern
      prefix = "process.stop.";
      Node *stop_p1  = process_p16;
      Node *stop_p2  = TheNet->newPlace(prefix + "p2");
      Node *stop_p3  = TheNet->newPlace(prefix + "p3");
      Node *stop_p4  = TheNet->newPlace(prefix + "p4");
      Node *stop_p5  = process_p5;
      Node *stop_p6  = TheNet->newPlace(prefix + "p6", DATA);
      Node *stop_p7  = process_p37;
      Node *stop_p8  = process_p38;
      Node *stop_p9  = process_p7;
      Node *stop_p10 = process_p11;
      Node *stop_p11 = TheNet->newPlace(prefix + "p11");
      Node *stop_p12 = process_p39;
      Node *stop_p13 = process_p6;
      Node *stop_p14 = process_p15;
      Node *stop_p15 = process_p40;
      Node *stop_p16 = process_p14;
      Node *stop_p17 = process_p12;
      Node *stop_p18 = TheNet->newPlace(prefix + "p18", DATA);
      Node *stop_p19 = TheNet->newPlace(prefix + "p19", DATA);
      Node *stop_p20 = process_p13;
      Node *stop_p21 = process_p21;
      Node *stop_p22 = process_p34;
      Node *stop_p23 = process_p35;
      Node *stop_p24 = process_p36;
      Node *stop_t1 =  TheNet->newTransition(prefix + "t1");
      Node *stop_t2 =  TheNet->newTransition(prefix + "t2");
      Node *stop_t3 =  TheNet->newTransition(prefix + "t3");
      Node *stop_t4 =  TheNet->newTransition(prefix + "t4");
      Node *stop_t5 =  TheNet->newTransition(prefix + "t5");
      Node *stop_t6 =  TheNet->newTransition(prefix + "t6");
      Node *stop_t7 =  TheNet->newTransition(prefix + "t7");
      Node *stop_t8 =  TheNet->newTransition(prefix + "t8");
      Node *stop_t9 =  TheNet->newTransition(prefix + "kill");
      Node *stop_t10 = TheNet->newTransition(prefix + "t10");
      Node *stop_t11 = TheNet->newTransition(prefix + "t11");
      Node *stop_t12 = TheNet->newTransition(prefix + "t12");
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
    }
      a b c d e f g h
    {
      Place *innerActivityInitial = TheNet->findPlace(h, ".initial");
      Place *innerActivityFinal   = TheNet->findPlace(h, ".final");
      Place *innerActivityStop    = TheNet->findPlace(h, ".stop");
      TheNet->newArc(process_t2, innerActivityInitial);
      TheNet->newArc(innerActivityFinal, process_t3);
      TheNet->mergePlaces(innerActivityStop, (Place *)process_p21);
      
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
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "final");
      Node *p3 = TheNet->newPlace(prefix + "stop");
      Node *p4 = TheNet->newPlace(prefix + "stopped");
      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2");
      Node *t3 = TheNet->newTransition(prefix + "t3");
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
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "running", DATA);
      Node *p3 = TheNet->newPlace(prefix + "final");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "stop");
      Node *p6 = TheNet->newPlace(prefix + "stopped");
      Node *p7 = TheNet->newPlace(prefix + "failed", DATA);
      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Node *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Node *t4 = TheNet->newTransition(prefix + "t4");
      Node *t5 = TheNet->newTransition(prefix + "t5");
      Node *t6 = TheNet->newTransition(prefix + "t6");
      Node *t7 = TheNet->newTransition(prefix + "t7");

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
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "running", DATA);
      Node *p3 = TheNet->newPlace(prefix + "final");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "stop");
      Node *p6 = TheNet->newPlace(prefix + "stopped");
      Node *p7 = TheNet->newPlace(prefix + "failed", DATA);
      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Node *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Node *t4 = TheNet->newTransition(prefix + "t4");
      Node *t5 = TheNet->newTransition(prefix + "t5");
      Node *t6 = TheNet->newTransition(prefix + "t6");
      Node *t7 = TheNet->newTransition(prefix + "t7");

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
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "running", DATA);
      Node *p3 = TheNet->newPlace(prefix + "final");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "stop");
      Node *p6 = TheNet->newPlace(prefix + "stopped");
      Node *p7 = TheNet->newPlace(prefix + "failed", DATA);
      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2", "!guard");
      Node *t3 = TheNet->newTransition(prefix + "t3", "guard");
      Node *t4 = TheNet->newTransition(prefix + "t4");
      Node *t5 = TheNet->newTransition(prefix + "t5");
      Node *t6 = TheNet->newTransition(prefix + "t6");
      Node *t7 = TheNet->newTransition(prefix + "t7");

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
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "final");
      Node *p3 = TheNet->newPlace(prefix + "stop");
      Node *p4 = TheNet->newPlace(prefix + "stopped");
      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2");
      Node *t3 = TheNet->newTransition(prefix + "t3");
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
  	Node *p1 = TheNet->newPlace(prefix + "initial");
    	Node *p2 = TheNet->newPlace(prefix + "TimeStamp", DATA);
      	Node *p3 = TheNet->newPlace(prefix + "final");
	Node *p4 = TheNet->newPlace(prefix + "stop");
	Node *p5 = TheNet->newPlace(prefix + "stopped");
  	Node *t1  = TheNet->newTransition(prefix + "t1");
    	Node *t2  = TheNet->newTransition(prefix + "t2", guard);
      	Node *t3  = TheNet->newTransition(prefix + "t3");
      	Node *t4  = TheNet->newTransition(prefix + "t4");
      	Node *t5  = TheNet->newTransition(prefix + "t5");
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
  	Node *p1 = TheNet->newPlace(prefix + "initial");
      	Node *p2 = TheNet->newPlace(prefix + "final");
	Node *p3 = TheNet->newPlace(prefix + "stop");
	Node *p4 = TheNet->newPlace(prefix + "stopped");
  	Node *t1  = TheNet->newTransition(prefix + "t1", guard);
    	Node *t2  = TheNet->newTransition(prefix + "t2");
      	Node *t3  = TheNet->newTransition(prefix + "t3");
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

      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "p2");
      Node *p3 = TheNet->newPlace(prefix + "stop");
      Node *p4 = TheNet->newPlace(prefix + "stopped");
      Node *p5 = TheNet->newPlace(prefix + "failed");
      Node *t1  = TheNet->newTransition(prefix + "t1");
      Node *t2  = TheNet->newTransition(prefix + "t2");
      Node *t3  = TheNet->newTransition(prefix + "t3");
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


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";

      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "final");
      Node *p3 = TheNet->newPlace(prefix + "stop");
      Node *p4 = TheNet->newPlace(prefix + "!Terminate");
      Node *p5 = TheNet->newPlace(prefix + "Terminate");
      Node *p6 = TheNet->newPlace(prefix + "terminate");
      Node *p7 = TheNet->newPlace(prefix + "stopped");
      Node *t1  = TheNet->newTransition(prefix + "t1");
      Node *t2  = TheNet->newTransition(prefix + "t2");
      Node *t3  = TheNet->newTransition(prefix + "t3");
      Node *t4  = TheNet->newTransition(prefix + "t4");
      
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

      Node *p7  = TheNet->newPlace(prefix + "initial");
      Node *p8  = TheNet->newPlace(prefix + "final");
      Node *p9  = TheNet->newPlace(prefix + "running");
      Node *p15 = TheNet->newPlace(prefix + "stop");
      Node *p16 = TheNet->newPlace(prefix + "stopped");
      Node *t2  = TheNet->newTransition(prefix + "t2");
      Node *t3  = TheNet->newTransition(prefix + "t3");
      Node *t4  = TheNet->newTransition(prefix + "t4");
      Node *t5  = TheNet->newTransition(prefix + "t5");
      Node *t6  = TheNet->newTransition(prefix + "innerStopped");
      Node *t7  = TheNet->newTransition(prefix + "t7");

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

      Node *switch_p4 =  TheNet->newPlace(prefix + "final");
      Node *switch_p5 =  TheNet->newPlace(prefix + "initial");
      Node *switch_p6 =  TheNet->newPlace(prefix + "running", DATA);
      Node *switch_p10 = TheNet->newPlace(prefix + "p10");
      Node *switch_p11 = TheNet->newPlace(prefix + "stop");
      Node *switch_p12 = TheNet->newPlace(prefix + "stopped");
      Node *switch_p13 = TheNet->newPlace(prefix + "failed", DATA);

      Node *switch_t3 =  TheNet->newTransition(prefix + "t3");
      Node *switch_t7 =  TheNet->newTransition(prefix + "t7", "guard");
      Node *switch_t8 =  TheNet->newTransition(prefix + "t8");
      Node *switch_t9 =  TheNet->newTransition(prefix + "t9");
      Node *switch_t10 = TheNet->newTransition(prefix + "t10");
      
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
      int caseNumber = 1;
      
      Node *last;

      foreach (o=Case(p=activity); tCase_list b)
      {
	std::string condition = o->condition->name;
	
	if (caseNumber == 1)
	{
	  // two transitions: guard fulfilled or not
	  Node *t2 = TheNet->newTransition(prefix + "t2.1", "!guard & (" + condition + ")");
	  Node *t4 = TheNet->newTransition(prefix + "t4.1", "!guard & !(" + condition + ")");
	  
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
	  Node *t2 = TheNet->newTransition(prefix + "t2." + intToString(caseNumber), condition);
	  Node *t4 = TheNet->newTransition(prefix + "t4." + intToString(caseNumber), "!(" + condition + ")");

	  // "running"-place
	  Node *running =  TheNet->newPlace(prefix + "running." + intToString(caseNumber), DATA);

	  // arcs connecting "running"-place
	  TheNet->newArc(last, running, "x");
	  TheNet->newArc(running, t2, "x");
	  TheNet->newArc(running, t4, "x");

	  // move token from "running"-place to stop
	  Node *t11 = TheNet->newTransition(prefix + "t11." + intToString(caseNumber));
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

	caseNumber++;
      }


      foreach (Otherwise(p=activity); tOtherwise_list c)
      {
	Node *t2 = TheNet->newTransition(prefix + "t2", "otherwise");
	
	// "running"-place
	Node *running = TheNet->newPlace(prefix + "running.o", DATA);

	// arcs connecting "running"-place
	TheNet->newArc(running, t2, "x");
	TheNet->newArc(last, running, "x");

	// remove token from "running"-place to stop
	Node *t11 = TheNet->newTransition(prefix + "t11.o");
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

      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "running", DATA);
      Node *p3 = TheNet->newPlace(prefix + "final");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "p5");
      Node *p6 = TheNet->newPlace(prefix + "p6");
      Node *p7 = TheNet->newPlace(prefix + "stop");
      Node *p8 = TheNet->newPlace(prefix + "stopped");
      Node *p9 = TheNet->newPlace(prefix + "failed", DATA);
      Node *t1 = TheNet->newTransition(prefix + "t1", "!(" + condition + ") & !guard");
      Node *t2 = TheNet->newTransition(prefix + "t2");
      Node *t3 = TheNet->newTransition(prefix + "t3", "(" + condition + ") & !guard");
      Node *t4 = TheNet->newTransition(prefix + "t4");
      Node *t5 = TheNet->newTransition(prefix + "t5", "guard");
      Node *t6 = TheNet->newTransition(prefix + "t6");
      Node *t7 = TheNet->newTransition(prefix + "t7");
      Node *t8 = TheNet->newTransition(prefix + "t8");
      Node *t9 = TheNet->newTransition(prefix + "t9");
      
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

      Node *p2 = TheNet->newPlace(prefix + "initial");
      Node *p9 = TheNet->newPlace(prefix + "final");
      Node *p10 = TheNet->newPlace(prefix + "stop");
      Node *p11 = TheNet->newPlace(prefix + "stopped");
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

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
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-11
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
 * 
 * \todo
 *          - find a simple "merge-language" to ease the definition of new
 *            patterns
 *          - complete initial pattern database
 *
 * \bug
 *          - conversion crashes if unimplemented patterns are used inside a
 *            sequence
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
      
      /* the stop pattern */
      Node *p1 = TheNet->newPlace(prefix + "Faulted");
      Node *p2 = TheNet->newPlace(prefix + "p2");
      Node *p3 = TheNet->newPlace(prefix + "p3");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "fault_in");
      Node *p6 = TheNet->newPlace(prefix + "p6");
      Node *p7 = TheNet->newPlace(prefix + "fault");
      Node *p8 = TheNet->newPlace(prefix + "faultSave");
      Node *p9 = TheNet->newPlace(prefix + "Active");
      Node *p10 = TheNet->newPlace(prefix + "!Active");
      Node *p11 = TheNet->newPlace(prefix + "p11");
      Node *p12 = TheNet->newPlace(prefix + "final");
      Node *p13 = TheNet->newPlace(prefix + "terminate");
      Node *p14 = TheNet->newPlace(prefix + "!Faulted");
      Node *p15 = TheNet->newPlace(prefix + "rethrow");
      Node *p16 = TheNet->newPlace(prefix + "Ended");
      Node *p17 = TheNet->newPlace(prefix + "Compensated");
      Node *p18 = TheNet->newPlace(prefix + "p18");
      Node *p19 = TheNet->newPlace(prefix + "p19");
      Node *p20 = TheNet->newPlace(prefix + "!Ended");
      Node *p21 = TheNet->newPlace(prefix + "stop");
      Node *p22 = TheNet->newPlace(prefix + "stopped");
      Node *p23 = TheNet->newPlace(prefix + "cleanCH");
      Node *p24 = TheNet->newPlace(prefix + "ch_cleaned");

      Node *t1 = TheNet->newTransition(prefix + "t1");
      Node *t2 = TheNet->newTransition(prefix + "t2");
      Node *t3 = TheNet->newTransition(prefix + "t3");
      Node *t4 = TheNet->newTransition(prefix + "t4");
      Node *t5 = TheNet->newTransition(prefix + "t5");
      Node *t6 = TheNet->newTransition(prefix + "t6");
      Node *t7 = TheNet->newTransition(prefix + "t7");
      Node *t8 = TheNet->newTransition(prefix + "t8");
      Node *t9 = TheNet->newTransition(prefix + "t9");
      Node *t10 = TheNet->newTransition(prefix + "t10");
      Node *t11 = TheNet->newTransition(prefix + "t11");
      Node *t12 = TheNet->newTransition(prefix + "t12");

      TheNet->newArc(p1, t1, READ);
      TheNet->newArc(p5, t1, "x");
      TheNet->newArc(t1, p15, "x");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      TheNet->newArc(t2, p21);
      TheNet->newArc(p3, t3);
      TheNet->newArc(p5, t3, RESET, "X");
      TheNet->newArc(p22, t3);
      TheNet->newArc(t3, p4);
      TheNet->newArc(p5, t4, "x");
      TheNet->newArc(p9, t4, "x");
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
      a b c d e f g h
    {
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
    {
      trace(TRACE_DEBUG, "[PNU]\tCreated pattern for <empty> " + intToString($0->id->value) + ".\n");
    }
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/


/******************************************************************************
  RECEIVE
******************************************************************************/


Receive(a,b) ->
  [petrinet:
    {
      std::string prefix = intToString($0->id->value) + ".";
      
      Node *p1 = TheNet->newPlace(prefix + "initial");
      Node *p2 = TheNet->newPlace(prefix + "running");
      Node *p3 = TheNet->newPlace(prefix + "final");
      Node *p4 = TheNet->newPlace(prefix + "p4");
      Node *p5 = TheNet->newPlace(prefix + "stop");
      Node *p6 = TheNet->newPlace(prefix + "stopped");
      Node *p7 = TheNet->newPlace(prefix + "failed");
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


/******************************************************************************
  ASSIGN
******************************************************************************/


/******************************************************************************
  WAIT
******************************************************************************/


/******************************************************************************
  THROW
******************************************************************************/


/******************************************************************************
  COMPENSATE
******************************************************************************/


/******************************************************************************
  TERMINATE
******************************************************************************/


/******************************************************************************
  FLOW
******************************************************************************/


/******************************************************************************
  SWITCH
******************************************************************************/


/******************************************************************************
  WHILE
******************************************************************************/


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

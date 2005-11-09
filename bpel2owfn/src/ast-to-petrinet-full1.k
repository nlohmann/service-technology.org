%uview petrinet;

/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include "petrinet.h"

extern PetriNet *TheNet;


%}

/******************************************************************************/

// All the includes, variables and structs defined here can be used in the
// printers below.
%{
#include <iostream>
#include <vector>
#include <string>
%}

/******************************************************************************/

Process(a,b,c,d,e,f,g,h) ->
  [petrinet:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
	    
      std::string prefix = "process.";
      std::string id = "";
      
      /* the stop pattern */
      Node *p1 = TheNet->newPlace("n"+id+"p1", prefix + "Faulted");
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "p2");
      Node *p3 = TheNet->newPlace("n"+id+"p3", prefix + "p3");
      Node *p4 = TheNet->newPlace("n"+id+"p4", prefix + "p4");
      Node *p5 = TheNet->newPlace("n"+id+"p5", prefix + "fault_in");
      Node *p6 = TheNet->newPlace("n"+id+"p6", prefix + "p6");
      Node *p7 = TheNet->newPlace("n"+id+"p7", prefix + "fault");
      Node *p8 = TheNet->newPlace("n"+id+"p8", prefix + "faultSave");
      Node *p9 = TheNet->newPlace("n"+id+"p9", prefix + "Active");
      Node *p10 = TheNet->newPlace("n"+id+"p10", prefix + "!Active");
      Node *p11 = TheNet->newPlace("n"+id+"p11", prefix + "p11");
      Node *p12 = TheNet->newPlace("n"+id+"p12", prefix + "final");
      Node *p13 = TheNet->newPlace("n"+id+"p13", prefix + "terminate");
      Node *p14 = TheNet->newPlace("n"+id+"p14", prefix + "!Faulted");
      Node *p15 = TheNet->newPlace("n"+id+"p15", prefix + "rethrow");
      Node *p16 = TheNet->newPlace("n"+id+"p16", prefix + "Ended");
      Node *p17 = TheNet->newPlace("n"+id+"p17", prefix + "Compensated");
      Node *p18 = TheNet->newPlace("n"+id+"p18", prefix + "p18");
      Node *p19 = TheNet->newPlace("n"+id+"p19", prefix + "p19");
      Node *p20 = TheNet->newPlace("n"+id+"p20", prefix + "!Ended");
      Node *p21 = TheNet->newPlace("n"+id+"p21", prefix + "stop");
      Node *p22 = TheNet->newPlace("n"+id+"p22", prefix + "stopped");
      Node *p23 = TheNet->newPlace("n"+id+"p23", prefix + "cleanCH");
      Node *p24 = TheNet->newPlace("n"+id+"p24", prefix + "ch_cleaned");

      Node *t1 = TheNet->newTransition("n"+id+"t1", prefix + "t1");
      Node *t2 = TheNet->newTransition("n"+id+"t2", prefix + "t2");
      Node *t3 = TheNet->newTransition("n"+id+"t3", prefix + "t3");
      Node *t4 = TheNet->newTransition("n"+id+"t4", prefix + "t4");
      Node *t5 = TheNet->newTransition("n"+id+"t5", prefix + "t5");
      Node *t6 = TheNet->newTransition("n"+id+"t6", prefix + "t6");
      Node *t7 = TheNet->newTransition("n"+id+"t7", prefix + "t7");
      Node *t8 = TheNet->newTransition("n"+id+"t8", prefix + "t8");
      Node *t9 = TheNet->newTransition("n"+id+"t9", prefix + "t9");
      Node *t10 = TheNet->newTransition("n"+id+"t10", prefix + "t10");
      Node *t11 = TheNet->newTransition("n"+id+"t11", prefix + "t11");
      Node *t12 = TheNet->newTransition("n"+id+"t12", prefix + "t12");

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

    } a b c d e f g h
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
      std::string prefix = "empty." + intToString($0->id->value) + ".";
      std::string id = intToString($0->id->value);
      
      Node *p1 = TheNet->newPlace("n"+id+"p1", prefix + "initial");
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "final");
      Node *p3 = TheNet->newPlace("n"+id+"p3", prefix + "stop");
      Node *p4 = TheNet->newPlace("n"+id+"p4", prefix + "stopped");
      Node *t1 = TheNet->newTransition("n"+id+"t1", prefix + "t1");
      Node *t2 = TheNet->newTransition("n"+id+"t2", prefix + "t2");
      Node *t3 = TheNet->newTransition("n"+id+"t3", prefix + "t3");
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
      std::string prefix = "receive." + intToString($0->id->value) + ".";
      std::string id = intToString($0->id->value);
      
      Node *p1 = TheNet->newPlace("n"+id+"p1", prefix + "initial");
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "running");
      Node *p3 = TheNet->newPlace("n"+id+"p3", prefix + "final");
      Node *p4 = TheNet->newPlace("n"+id+"p4", prefix + "p4");
      Node *p5 = TheNet->newPlace("n"+id+"p5", prefix + "stop");
      Node *p6 = TheNet->newPlace("n"+id+"p6", prefix + "stopped");
      Node *p7 = TheNet->newPlace("n"+id+"p7", prefix + "failed");
      Node *t1 = TheNet->newTransition("n"+id+"t1", prefix + "t1");
      Node *t2 = TheNet->newTransition("n"+id+"t2", prefix + "t2", "!guard");
      Node *t3 = TheNet->newTransition("n"+id+"t3", prefix + "t3", "guard");
      Node *t4 = TheNet->newTransition("n"+id+"t4", prefix + "t4");
      Node *t5 = TheNet->newTransition("n"+id+"t5", prefix + "t5");
      Node *t6 = TheNet->newTransition("n"+id+"t6", prefix + "t6");
      Node *t7 = TheNet->newTransition("n"+id+"t7", prefix + "t7");
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
      std::string prefix = "sequence." + intToString($0->id->value) + ".";
      std::string id = intToString($0->id->value);
      
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "initial");
      Node *p9 = TheNet->newPlace("n"+id+"p9", prefix + "final");
      Node *p10 = TheNet->newPlace("n"+id+"p10", prefix + "stop");
      Node *p11 = TheNet->newPlace("n"+id+"p11", prefix + "stopped");
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

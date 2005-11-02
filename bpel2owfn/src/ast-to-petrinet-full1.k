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
      std::string prefix = "process.";
      std::string id = "";
      
      /* the stop pattern */
      Node *p1 = TheNet->newPlace("n"+id+"p1", prefix + "Faulted", 0);
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "p2", 0);
      Node *p3 = TheNet->newPlace("n"+id+"p3", prefix + "p3", 0);
      Node *p4 = TheNet->newPlace("n"+id+"p4", prefix + "p4", 0);
      Node *p5 = TheNet->newPlace("n"+id+"p5", prefix + "fault_in", 0);
      Node *p6 = TheNet->newPlace("n"+id+"p6", prefix + "p6", 0);
      Node *p7 = TheNet->newPlace("n"+id+"p7", prefix + "fault", 0);
      Node *p8 = TheNet->newPlace("n"+id+"p8", prefix + "faultSave", 0);
      Node *p9 = TheNet->newPlace("n"+id+"p9", prefix + "Active", 0);
      Node *p10 = TheNet->newPlace("n"+id+"p10", prefix + "!Active", 0);
      Node *p11 = TheNet->newPlace("n"+id+"p11", prefix + "p11", 0);
      Node *p12 = TheNet->newPlace("n"+id+"p12", prefix + "final", 0);
      Node *p13 = TheNet->newPlace("n"+id+"p13", prefix + "terminate", 0);
      Node *p14 = TheNet->newPlace("n"+id+"p14", prefix + "!Faulted", 0);
      Node *p15 = TheNet->newPlace("n"+id+"p15", prefix + "rethrow", 0);
      Node *p16 = TheNet->newPlace("n"+id+"p16", prefix + "Ended", 0);
      Node *p17 = TheNet->newPlace("n"+id+"p17", prefix + "Compensated", 0);
      Node *p18 = TheNet->newPlace("n"+id+"p18", prefix + "p18", 0);
      Node *p19 = TheNet->newPlace("n"+id+"p19", prefix + "p19", 0);
      Node *p20 = TheNet->newPlace("n"+id+"p20", prefix + "!Ended", 0);
      Node *p21 = TheNet->newPlace("n"+id+"p21", prefix + "stop", 0);
      Node *p22 = TheNet->newPlace("n"+id+"p22", prefix + "stopped", 0);
      Node *p23 = TheNet->newPlace("n"+id+"p23", prefix + "cleanCH", 0);
      Node *p24 = TheNet->newPlace("n"+id+"p24", prefix + "ch_cleaned", 0);

      Node *t1 = TheNet->newTransition("n"+id+"t1", prefix + "t1", 0);
      Node *t2 = TheNet->newTransition("n"+id+"t2", prefix + "t2", 0);
      Node *t3 = TheNet->newTransition("n"+id+"t3", prefix + "t3", 0);
      Node *t4 = TheNet->newTransition("n"+id+"t4", prefix + "t4", 0);
      Node *t5 = TheNet->newTransition("n"+id+"t5", prefix + "t5", 0);
      Node *t6 = TheNet->newTransition("n"+id+"t6", prefix + "t6", 0);
      Node *t7 = TheNet->newTransition("n"+id+"t7", prefix + "t7", 0);
      Node *t8 = TheNet->newTransition("n"+id+"t8", prefix + "t8", 0);
      Node *t9 = TheNet->newTransition("n"+id+"t9", prefix + "t9", 0);
      Node *t10 = TheNet->newTransition("n"+id+"t10", prefix + "t10", 0);
      Node *t11 = TheNet->newTransition("n"+id+"t11", prefix + "t11", 0);
      Node *t12 = TheNet->newTransition("n"+id+"t12", prefix + "t12", 0);

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
      
      Node *p1 = TheNet->newPlace("n"+id+"p1", prefix + "initial", 0);
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "final", 0);
      Node *p3 = TheNet->newPlace("n"+id+"p3", prefix + "stop", 0);
      Node *p4 = TheNet->newPlace("n"+id+"p4", prefix + "stopped", 0);
      Node *t1 = TheNet->newTransition("n"+id+"t1", prefix + "t1", 0);
      Node *t2 = TheNet->newTransition("n"+id+"t2", prefix + "t2", 0);
      Node *t3 = TheNet->newTransition("n"+id+"t3", prefix + "t3", 0);
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


/******************************************************************************
  RECEIVE
******************************************************************************/


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
      
      Node *p2 = TheNet->newPlace("n"+id+"p2", prefix + "initial", 0);
      Node *p9 = TheNet->newPlace("n"+id+"p9", prefix + "final", 0);
      Node *p10 = TheNet->newPlace("n"+id+"p10", prefix + "stop", 0);
      Node *p11 = TheNet->newPlace("n"+id+"p11", prefix + "stopped", 0);
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

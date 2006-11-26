%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet *TheNet;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;
%}





/******************************************************************************
  WHILE
******************************************************************************/

void PWhile(const impl_tWhile* abstract_phylum, uview current_view)
{
  impl_tWhile_While* phylum = (impl_tWhile_While*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = TheNet->newPlace(prefix + "initial");
	Place *p2 = TheNet->newPlace(prefix + "running");
	Place *p3 = TheNet->newPlace(prefix + "final");
	Place *p4 = TheNet->newPlace(prefix + "p4");
	Place *p5 = TheNet->newPlace(prefix + "p5");
	Place *p6 = TheNet->newPlace(prefix + "p6");
	Place *p7 = TheNet->newPlace(prefix + "stop");
	Place *p8 = TheNet->newPlace(prefix + "stopped");
	
	Transition *t2 = TheNet->newTransition(prefix + "init");
	TheNet->newArc(p1, t2);
	TheNet->newArc(t2, p2);
	
	Transition *t3 = TheNet->newTransition(prefix + "start");
	TheNet->newArc(p2, t3);
	TheNet->newArc(t3, p4);

	// the single loop/skip transition is only needed when loops are not counted
	if (!parameters[P_LOOPCOUNT])
	{
	  Transition *t1 = TheNet->newTransition(prefix + "skip");
	  TheNet->newArc(p2, t1);
	  TheNet->newArc(t1, p3);
	  
	  Transition *t4 = TheNet->newTransition(prefix + "loop");
	  TheNet->newArc(t4, p1);
	  TheNet->newArc(p5, t4);
	}
	
	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t5 = throwFault(p2, p6, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t6 = stop(p1, "initial", prefix);
	Transition *t7 = stop(p2, "running", prefix);
	Transition *t8 = stop(p6, "p6", prefix);
	Transition *t9 = stop(p3, "final", prefix);

	// the subnet if loops are counted
	if (parameters[P_LOOPCOUNT])
	{
	  unsigned int max_loops = ASTEmap[phylum->id]->max_loops;
	  if (max_loops == UINT_MAX)
	    cerr << "no max loop provided!" << endl;
	  else
	  {
	    Place *p9_old = NULL;
	    Place *p9_first = NULL;
	    for (unsigned i = 1; i <= max_loops; i++)
	    {
	      Place *p9 = TheNet->newPlace(prefix + "count." + toString(i));

	      if (i == 1)
	      {
		p9->mark();
		p9_first = p9;
	      }
	      
	      if (i > 1)
	      {
		assert(p9_old != NULL);
		Transition *t4 = TheNet->newTransition(prefix + "loop" + toString(i-1));
		TheNet->newArc(t4, p1);
		TheNet->newArc(p5, t4);
		TheNet->newArc(p9_old, t4);
		TheNet->newArc(t4, p9);
	      }
	      
	      assert(p9_first != NULL);
	      Transition *t1 = TheNet->newTransition(prefix + "skip" + toString(i));
	      TheNet->newArc(p2, t1);
	      TheNet->newArc(t1, p3);
	      TheNet->newArc(t1, p9_first);
	      TheNet->newArc(p9, t1);
	      
	      p9_old = p9;
	    }
	    
	    Transition *t10 = TheNet->newTransition(prefix + "finish");
	    TheNet->newArc(p9_old, t10);
	    TheNet->newArc(p5, t10);
	    TheNet->newArc(t10, p3);
	    TheNet->newArc(t10, p9_first);
	  }
	}
	
	
	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);
	
	
	// Embed the inner activity.
	TheNet->mergePlaces(phylum->id, ".internal.p4",      phylum->activity_1->id, ".initial");
	TheNet->mergePlaces(phylum->id, ".internal.p5",      phylum->activity_1->id, ".final");
	TheNet->mergePlaces(phylum->id, ".internal.stop",    phylum->activity_1->id, ".stop");
	TheNet->mergePlaces(phylum->id, ".internal.stopped", phylum->activity_1->id, ".stopped");
	
	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = TheNet->newPlace(prefix + "initial");
	Place *p2 = TheNet->newPlace(prefix + "running");
	Place *p3 = TheNet->newPlace(prefix + "final");
	Place *p4 = TheNet->newPlace(prefix + "done");
	
	Transition *t1 = TheNet->newTransition(prefix + "enter");
	TheNet->newArc(p1, t1);
	TheNet->newArc(t1, p2);
	
	Transition *t2 = TheNet->newTransition(prefix + "leave");
	TheNet->newArc(p1, t2);
	TheNet->newArc(t2, p3);

	Transition *t3 = TheNet->newTransition(prefix + "loop");
	TheNet->newArc(p4, t3);
	TheNet->newArc(t3, p1);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	// Embed the inner activity.
	TheNet->mergePlaces(phylum->id, ".internal.running", phylum->activity_1->id, ".initial");
	TheNet->mergePlaces(phylum->id, ".internal.done", phylum->activity_1->id, ".final");	

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}





/******************************************************************************
  REPEAT UNTIL
******************************************************************************/

void PRepeatUntil(const impl_tRepeatUntil* abstract_phylum, uview current_view)
{
  impl_tRepeatUntil_RepeatUntil* phylum = (impl_tRepeatUntil_RepeatUntil*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = TheNet->newPlace(prefix + "initial");
	Place *p2 = TheNet->newPlace(prefix + "p2");
	Place *p3 = TheNet->newPlace(prefix + "running");
	Place *p4 = TheNet->newPlace(prefix + "final");
	Place *p5 = TheNet->newPlace(prefix + "p5");
	Place *p6 = TheNet->newPlace(prefix + "stop");
	Place *p7 = TheNet->newPlace(prefix + "stopped");
	
	Transition *t1 = TheNet->newTransition(prefix + "t1"); /* read variable */
	TheNet->newArc(p2, t1);
	TheNet->newArc(t1, p3);

	Transition *t2 = TheNet->newTransition(prefix + "t2"); /* end */
	TheNet->newArc(p3, t2);
	TheNet->newArc(t2, p4);
	
	Transition *t3 = TheNet->newTransition(prefix + "t3"); /* repeat */
	TheNet->newArc(p3, t3);
	TheNet->newArc(t3, p1);

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t4 = throwFault(p3, p6, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t5 = stop(p1, "initial", prefix);
	Transition *t6 = stop(p2, "running", prefix);
	Transition *t7 = stop(p5, "p5", prefix);
	Transition *t8 = stop(p4, "final", prefix);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// Embed the inner activity.
	TheNet->mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
	TheNet->mergePlaces(phylum->id, ".internal.p2",      phylum->activity_1->id, ".final");
	TheNet->mergePlaces(phylum->id, ".internal.stop",    phylum->activity_1->id, ".stop");
	TheNet->mergePlaces(phylum->id, ".internal.stopped", phylum->activity_1->id, ".stopped");

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = TheNet->newPlace(prefix + "initial");
	Place *p2 = TheNet->newPlace(prefix + "p2");
	Place *p3 = TheNet->newPlace(prefix + "final");

	Transition *t1 = TheNet->newTransition(prefix + "end");
	TheNet->newArc(p2, t1);
	TheNet->newArc(t1, p3);

	Transition *t2 = TheNet->newTransition(prefix + "repeat");
	TheNet->newArc(p2, t2);
	TheNet->newArc(t2, p1);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// Embed the inner activity.
	TheNet->mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
	TheNet->mergePlaces(phylum->id, ".internal.p2", phylum->activity_1->id, ".final");

	break;
      }

    default: { /* no pattern in this case */ }
  }
 
  footer(phylum->id);
}





/******************************************************************************
  SEQUENCE
******************************************************************************/

void PSequence(const impl_tSequence* abstract_phylum, uview current_view)
{
  impl_tSequence_Sequence* phylum = (impl_tSequence_Sequence*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  Place *p2  = TheNet->newPlace(prefix + "initial");
  Place *p9  = TheNet->newPlace(prefix + "final");

  if (current_view != petrinetsmall_enum)
  {
    Place *p10 = TheNet->newPlace(prefix + "stop");
    Place *p11 = TheNet->newPlace(prefix + "stopped");
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  // unparse the embedded activity
  phylum->activity_list_1->unparse(pseudoPrinter, current_view);


  int activityCount = 1;
  impl_activity* lastActivity;

  foreach (innerActivity; activity_list phylum->activity_list_1)
  {
    if (activityCount == 1)
      TheNet->mergePlaces(phylum->id, ".internal.initial", innerActivity->id, ".initial");
    else
      TheNet->mergePlaces(lastActivity->id, ".final", innerActivity->id, ".initial");

    if (activityCount == phylum->activity_list_1->length())
      TheNet->mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");

    if (current_view != petrinetsmall_enum)
    {
      TheNet->mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
      TheNet->mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
    }

    lastActivity = innerActivity;
    activityCount++;
  }


  footer(phylum->id);
}





/******************************************************************************
  FLOW
******************************************************************************/

void PFlow(const impl_tFlow* abstract_phylum, uview current_view)
{
  impl_tFlow_Flow* phylum = (impl_tFlow_Flow*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
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

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_list_1->unparse(pseudoPrinter, current_view);

	// embed the inner activity 
	foreach (innerActivity; activity_list phylum->activity_list_1)
	{
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity->id, ".initial"));
	  TheNet->newArc(t5, TheNet->findPlace(innerActivity->id, ".stop"));
	  TheNet->newArc(TheNet->findPlace(innerActivity->id, ".stopped"), t6);
	  TheNet->newArc(TheNet->findPlace(innerActivity->id, ".final"), t3);
	}

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1  = TheNet->newPlace(prefix + "initial");
	Place *p2  = TheNet->newPlace(prefix + "final");

	Transition *t1 = TheNet->newTransition(prefix + "split");
	TheNet->newArc(p1, t1);

	Transition *t2 = TheNet->newTransition(prefix + "join");
	TheNet->newArc(t2, p2);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_list_1->unparse(pseudoPrinter, current_view);

	foreach (innerActivity; activity_list phylum->activity_list_1)
	{
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity->id, ".initial"));
	  TheNet->newArc(TheNet->findPlace(innerActivity->id, ".final"), t2);
	}
	
	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);

}





/******************************************************************************
  SWITCH
******************************************************************************/

void PSwitch(const impl_tSwitch* abstract_phylum, uview current_view)
{
  impl_tSwitch_Switch* phylum = (impl_tSwitch_Switch*) abstract_phylum;
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
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
	  Transition *t7 = throwFault(p6, p10, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t8  = stop(p5, "initial", prefix);
	Transition *t9  = stop(p6, "running", prefix);
	Transition *t10 = stop(p10, "p10", prefix);


	impl_tOtherwise_Otherwise *otherwise = (impl_tOtherwise_Otherwise*) phylum->tOtherwise_1;

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the case branches
	phylum->tCase_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the otherwise activity
	otherwise->activity_1->unparse(pseudoPrinter, current_view);

	

	int currentCase = 1;
	Transition *last;
	foreach (caseBranch=Case(innerActivity); tCase_list phylum->tCase_list_1)
	{
	  string caseNumber = toString(currentCase);
	  
	  trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding case branch " + caseNumber + "...\n");

	  Transition *t2 = NULL;
	  Transition *t4 = NULL;
	  
	  if (currentCase == 1)
	  {
	    // two transitions: guard fulfilled or not
	    t2 = TheNet->newTransition(prefix + "t2.case1");
	    t4 = TheNet->newTransition(prefix + "t4.case1");

	    // arcs connecting "running"-place
	    TheNet->newArc(TheNet->findPlace(phylum->id, ".internal.running"), t2);
	    TheNet->newArc(TheNet->findPlace(phylum->id, ".internal.running"), t4);

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
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity->id, ".initial"));
	  TheNet->mergePlaces(phylum->id, ".internal.final",   innerActivity->id, ".final");
	  TheNet->mergePlaces(phylum->id, ".internal.stop",    innerActivity->id, ".stop");
	  TheNet->mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
	  
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
	TheNet->newArc(t2, TheNet->findPlace(otherwise->activity_1->id, ".initial"));
	TheNet->mergePlaces(phylum->id, ".internal.final",   otherwise->activity_1->id, ".final");
	TheNet->mergePlaces(phylum->id, ".internal.stop",    otherwise->activity_1->id, ".stop");
	TheNet->mergePlaces(phylum->id, ".internal.stopped", otherwise->activity_1->id, ".stopped");

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 =  TheNet->newPlace(prefix + "final");
	Place *p2 =  TheNet->newPlace(prefix + "initial");


	impl_tOtherwise_Otherwise *otherwise = (impl_tOtherwise_Otherwise*) phylum->tOtherwise_1;

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the case branches
	phylum->tCase_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the otherwise activity
	otherwise->activity_1->unparse(pseudoPrinter, current_view);
	

	// the <case> branches
	int currentCase = 1;
	foreach (caseBranch=Case(innerActivity,_id2); tCase_list phylum->tCase_list_1)
	{
	  string caseNumber = toString(currentCase++);
	  Transition* t1 = TheNet->newTransition(prefix + "case" + caseNumber);
	  TheNet->newArc(TheNet->findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here
	  
	  // Embed the inner activity.
	  TheNet->newArc(t1, TheNet->findPlace(innerActivity->id, ".initial"));
	  TheNet->mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
	}

	// the <otherwise> branch
	Transition *t1 = TheNet->newTransition(prefix + "caseo");
	TheNet->newArc(TheNet->findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here

	// embed inner activity
	TheNet->newArc(t1, TheNet->findPlace(otherwise->activity_1->id, ".initial"));
	TheNet->mergePlaces(phylum->id, ".internal.final", otherwise->activity_1->id, ".final");


	// Create the arcs for the Dead Path elimination: each chosen activity
	// (i.e. firing of the respective t2-transition) has set the links of all
	// other branches to false.
	currentCase = 1;
	foreach (Case(innerActivity1); tCase_list phylum->tCase_list_1)
	{
	  foreach (Case(innerActivity2); tCase_list phylum->tCase_list_1)
	  {
	    if (innerActivity1 != innerActivity2)
            dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), innerActivity2->id); // case -> case'
	  }

	  dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), otherwise->activity_1->id); // case -> otherwise
	  dpeLinks(TheNet->findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
	  currentCase++;
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id);
}

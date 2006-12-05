/*****************************************************************************\
 * Copyright 2006 Niels Lohmann                                              *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

%{
/*!
 * \file    petrinet-patterns-structured.cc
 *
 * \brief   Petri net patterns for BPEL's structured activities
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $ 
 * 
 * \since   2006-11-26
 *
 * \date    \$Date: 2006/12/05 13:58:01 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-structured.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.11 $
 */
%}


%{ HEADER
/*!
 * \file petrinet-patterns-structured.h
 * \brief Petri net patterns for BPEL's structured activities
 *
 * See \ref petrinet-patterns-structured.cc for more information.
 */
%}

%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;
%}





/******************************************************************************
  WHILE
******************************************************************************/

void PWhile(impl_tWhile* abstract_phylum, uview current_view)
{
  impl_tWhile_While* phylum = static_cast<impl_tWhile_While*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "p5");
	Place *p6 = PN.newPlace(prefix + "p6");
	Place *p7 = PN.newPlace(prefix + "stop");
	Place *p8 = PN.newPlace(prefix + "stopped");
	
	Transition *t2 = PN.newTransition(prefix + "init");
	PN.newArc(p1, t2);
	PN.newArc(t2, p2);
	
	Transition *t3 = PN.newTransition(prefix + "start");
	PN.newArc(p2, t3);
	PN.newArc(t3, p4);

	// the single loop/skip transition is only needed when loops are not counted
	if (!parameters[P_LOOPCOUNT])
	{
	  Transition *t1 = PN.newTransition(prefix + "skip");
	  PN.newArc(p2, t1);
	  PN.newArc(t1, p3);
	  
	  Transition *t4 = PN.newTransition(prefix + "loop");
	  PN.newArc(t4, p1);
	  PN.newArc(p5, t4);
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
	      Place *p9 = PN.newPlace(prefix + "count." + toString(i));

	      if (i == 1)
	      {
		p9->mark();
		p9_first = p9;
	      }
	      
	      if (i > 1)
	      {
		assert(p9_old != NULL);
		Transition *t4 = PN.newTransition(prefix + "loop" + toString(i-1));
		PN.newArc(t4, p1);
		PN.newArc(p5, t4);
		PN.newArc(p9_old, t4);
		PN.newArc(t4, p9);
	      }
	      
	      assert(p9_first != NULL);
	      Transition *t1 = PN.newTransition(prefix + "skip" + toString(i));
	      PN.newArc(p2, t1);
	      PN.newArc(t1, p3);
	      PN.newArc(t1, p9_first);
	      PN.newArc(p9, t1);
	      
	      p9_old = p9;
	    }
	    
	    Transition *t10 = PN.newTransition(prefix + "finish");
	    PN.newArc(p9_old, t10);
	    PN.newArc(p5, t10);
	    PN.newArc(t10, p3);
	    PN.newArc(t10, p9_first);
	  }
	}
	
	
	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);
	
	
	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.p4",      phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.p5",      phylum->activity_1->id, ".final");
	PN.mergePlaces(phylum->id, ".internal.stop",    phylum->activity_1->id, ".stop");
	PN.mergePlaces(phylum->id, ".internal.stopped", phylum->activity_1->id, ".stopped");
	
	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "done");
	
	Transition *t1 = PN.newTransition(prefix + "enter");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	
	Transition *t2 = PN.newTransition(prefix + "leave");
	PN.newArc(p1, t2);
	PN.newArc(t2, p3);

	Transition *t3 = PN.newTransition(prefix + "loop");
	PN.newArc(p4, t3);
	PN.newArc(t3, p1);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.running", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.done", phylum->activity_1->id, ".final");	

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}





/******************************************************************************
  REPEAT UNTIL
******************************************************************************/

void PRepeatUntil(impl_tRepeatUntil* abstract_phylum, uview current_view)
{
  impl_tRepeatUntil_RepeatUntil* phylum = static_cast<impl_tRepeatUntil_RepeatUntil*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  switch (current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "p2");
	Place *p3 = PN.newPlace(prefix + "running");
	Place *p4 = PN.newPlace(prefix + "final");
	Place *p5 = PN.newPlace(prefix + "p5");
	Place *p6 = PN.newPlace(prefix + "stop");
	Place *p7 = PN.newPlace(prefix + "stopped");
	
	Transition *t1 = PN.newTransition(prefix + "t1"); /* read variable */
	PN.newArc(p2, t1);
	PN.newArc(t1, p3);

	Transition *t2 = PN.newTransition(prefix + "t2"); /* end */
	PN.newArc(p3, t2);
	PN.newArc(t2, p4);
	
	Transition *t3 = PN.newTransition(prefix + "t3"); /* repeat */
	PN.newArc(p3, t3);
	PN.newArc(t3, p1);

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
	PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.p2",      phylum->activity_1->id, ".final");
	PN.mergePlaces(phylum->id, ".internal.stop",    phylum->activity_1->id, ".stop");
	PN.mergePlaces(phylum->id, ".internal.stopped", phylum->activity_1->id, ".stopped");

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "p2");
	Place *p3 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "end");
	PN.newArc(p2, t1);
	PN.newArc(t1, p3);

	Transition *t2 = PN.newTransition(prefix + "repeat");
	PN.newArc(p2, t2);
	PN.newArc(t2, p1);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.p2", phylum->activity_1->id, ".final");

	break;
      }

    default: { /* no pattern in this case */ }
  }
 
  footer(phylum->id, true);
}





/******************************************************************************
  SEQUENCE
******************************************************************************/

void PSequence(impl_tSequence* abstract_phylum, uview current_view)
{
  impl_tSequence_Sequence* phylum = static_cast<impl_tSequence_Sequence*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  Place *p2  = PN.newPlace(prefix + "initial");
  Place *p9  = PN.newPlace(prefix + "final");

  if (current_view != petrinetsmall_enum)
  {
    Place *p10 = PN.newPlace(prefix + "stop");
    Place *p11 = PN.newPlace(prefix + "stopped");
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
      PN.mergePlaces(phylum->id, ".internal.initial", innerActivity->id, ".initial");
    else
      PN.mergePlaces(lastActivity->id, ".final", innerActivity->id, ".initial");

    if (activityCount == phylum->activity_list_1->length())
      PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");

    if (current_view != petrinetsmall_enum)
    {
      PN.mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
      PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
    }

    lastActivity = innerActivity;
    activityCount++;
  }


  footer(phylum->id, true);
}





/******************************************************************************
  FLOW
******************************************************************************/

void PFlow(impl_tFlow* abstract_phylum, uview current_view)
{
  impl_tFlow_Flow* phylum = static_cast<impl_tFlow_Flow*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p7  = PN.newPlace(prefix + "initial");
	Place *p8  = PN.newPlace(prefix + "final");
	Place *p9  = PN.newPlace(prefix + "running");
	Place *p15 = PN.newPlace(prefix + "stop");
	Place *p16 = PN.newPlace(prefix + "stopped");

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p7, t2);
	PN.newArc(t2, p9);

	Transition *t3 = PN.newTransition(prefix + "t3");
	PN.newArc(t3, p8);
	PN.newArc(p9, t3);

	Transition *t5 = PN.newTransition(prefix + "t5");
	PN.newArc(p9, t5);
	PN.newArc(p15, t5);

	Transition *t6 = PN.newTransition(prefix + "innerStopped");
	PN.newArc(t6, p16);

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
	  PN.newArc(t2, PN.findPlace(innerActivity->id, ".initial"));
	  PN.newArc(t5, PN.findPlace(innerActivity->id, ".stop"));
	  PN.newArc(PN.findPlace(innerActivity->id, ".stopped"), t6);
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t3);
	}

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1  = PN.newPlace(prefix + "initial");
	Place *p2  = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "split");
	PN.newArc(p1, t1);

	Transition *t2 = PN.newTransition(prefix + "join");
	PN.newArc(t2, p2);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_list_1->unparse(pseudoPrinter, current_view);

	foreach (innerActivity; activity_list phylum->activity_list_1)
	{
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t2);
	}
	
	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);

}





/******************************************************************************
  SWITCH
******************************************************************************/

void PSwitch(impl_tSwitch* abstract_phylum, uview current_view)
{
  impl_tSwitch_Switch* phylum = static_cast<impl_tSwitch_Switch*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p4 =  PN.newPlace(prefix + "final");
	Place *p5 =  PN.newPlace(prefix + "initial");
	Place *p6 =  PN.newPlace(prefix + "running");
	Place *p10 = PN.newPlace(prefix + "p10");
	Place *p11 = PN.newPlace(prefix + "stop");
	Place *p12 = PN.newPlace(prefix + "stopped");

	Transition *t3 = PN.newTransition(prefix + "t3");
	PN.newArc(p5, t3);
	PN.newArc(t3, p6);

	// Transitions for the negative control flow.
	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t7 = throwFault(p6, p10, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t8  = stop(p5, "initial", prefix);
	Transition *t9  = stop(p6, "running", prefix);
	Transition *t10 = stop(p10, "p10", prefix);


	impl_tOtherwise_Otherwise *otherwise = static_cast<impl_tOtherwise_Otherwise*>(phylum->tOtherwise_1);

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
	    t2 = PN.newTransition(prefix + "t2.case1");
	    t4 = PN.newTransition(prefix + "t4.case1");

	    // arcs connecting "running"-place
	    PN.newArc(PN.findPlace(phylum->id, ".internal.running"), t2);
	    PN.newArc(PN.findPlace(phylum->id, ".internal.running"), t4);

	    last = t4;
	  }
	  else
	  {
	    // two transitions: guard fulfilled or not
	    t2 = PN.newTransition(prefix + "t2.case" + caseNumber);
	    t4 = PN.newTransition(prefix + "t4.case" + caseNumber);

	    // "running"-place and connecting arcs
	    Place *running =  PN.newPlace(prefix + "running.case" + caseNumber);
	    PN.newArc(last, running);
	    PN.newArc(running, t2);
	    PN.newArc(running, t4);
	    
	    Transition *t11 = stop(running, "runningCase" + caseNumber, prefix);
	    
	    last = t4;
	  }
	  
	  // Embed the inner activity.
	  PN.newArc(t2, PN.findPlace(innerActivity->id, ".initial"));
	  PN.mergePlaces(phylum->id, ".internal.final",   innerActivity->id, ".final");
	  PN.mergePlaces(phylum->id, ".internal.stop",    innerActivity->id, ".stop");
	  PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
	  
	  currentCase++;
	}
	
	trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding otherwise branch...\n");
	// The <otherwise> branch.
	Transition *t2 = PN.newTransition(prefix + "t2.otherwise");

	// "running"-place and connecting arcs
	Place *running = PN.newPlace(prefix + "running.otherwise");
	PN.newArc(running, t2);
	PN.newArc(last, running);
	
	Transition *tnoname = stop(running, "runningOtherwise", prefix);
	
	// embed inner activity
	PN.newArc(t2, PN.findPlace(otherwise->activity_1->id, ".initial"));
	PN.mergePlaces(phylum->id, ".internal.final",   otherwise->activity_1->id, ".final");
	PN.mergePlaces(phylum->id, ".internal.stop",    otherwise->activity_1->id, ".stop");
	PN.mergePlaces(phylum->id, ".internal.stopped", otherwise->activity_1->id, ".stopped");

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 =  PN.newPlace(prefix + "final");
	Place *p2 =  PN.newPlace(prefix + "initial");


	impl_tOtherwise_Otherwise *otherwise = static_cast<impl_tOtherwise_Otherwise*>(phylum->tOtherwise_1);

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
	  Transition* t1 = PN.newTransition(prefix + "case" + caseNumber);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here
	  
	  // Embed the inner activity.
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	  PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
	}

	// the <otherwise> branch
	Transition *t1 = PN.newTransition(prefix + "caseo");
	PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here

	// embed inner activity
	PN.newArc(t1, PN.findPlace(otherwise->activity_1->id, ".initial"));
	PN.mergePlaces(phylum->id, ".internal.final", otherwise->activity_1->id, ".final");


	// Create the arcs for the Dead Path elimination: each chosen activity
	// (i.e. firing of the respective t2-transition) has set the links of all
	// other branches to false.
	currentCase = 1;
	foreach (Case(innerActivity1); tCase_list phylum->tCase_list_1)
	{
	  foreach (Case(innerActivity2); tCase_list phylum->tCase_list_1)
	  {
	    if (innerActivity1 != innerActivity2)
            dpeLinks(PN.findTransition(prefix + "case" + toString(currentCase)), innerActivity2->id); // case -> case'
	  }

	  dpeLinks(PN.findTransition(prefix + "case" + toString(currentCase)), otherwise->activity_1->id); // case -> otherwise
	  dpeLinks(PN.findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
	  currentCase++;
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}





/******************************************************************************
  PICK
******************************************************************************/

void PPick(impl_tPick* abstract_phylum, uview current_view)
{
  impl_tPick_Pick* phylum = static_cast<impl_tPick_Pick*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p5 = PN.newPlace(prefix + "final");
	Place *p6 = PN.newPlace(prefix + "initial");
	Place *p7 = PN.newPlace(prefix + "TimeStamp");
	Place *p11 = PN.newPlace(prefix + "stop");
	Place *p12 = PN.newPlace(prefix + "stopped");

	Transition *t5 = PN.newTransition(prefix + "t5");
	PN.newArc(p6, t5);
	PN.newArc(t5, p7);
        if (!parameters[P_NOVARIABLES])
	  PN.newArc(PN.findPlace("1.internal.clock"), t5, READ);

  	// Transitions for the negative control flow.
    	Transition *t7 = stop(p6, "initial", prefix);
      	Transition *t8 = stop(p7, "TimeStamp", prefix);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the onMessage branches
	phylum->tOnMessage_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the onAlarm branches
	phylum->tOnAlarm_list_1->unparse(pseudoPrinter, current_view);


  	// Traverse the <onMessage> branches.
	int onMessageCount = 1;
	foreach (ThisBranch = OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list phylum->tOnMessage_list_1)
	{
	  trace(TRACE_DEBUG, "[PNU]\t\t<onMessage>\n");
	  string message = toString(onMessageCount++);
	  string newPrefix = prefix + "onMessage" + message + ".";
	  
	  Place *p3 = PN.newPlace(newPrefix + "p3");
	  Place *p10 = PN.newPlace(newPrefix + "p10");

	  Transition *t2 = PN.newTransition(newPrefix + "t2");
	  PN.newArc(PN.findPlace("in." + ASTEmap[ThisBranch->id]->channelName), t2);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.TimeStamp"), t2);
	  PN.newArc(t2, p3);

	  Transition *t3 = PN.newTransition(newPrefix + "t3");
	  PN.newArc(p3, t3);
	  PN.newArc(t3, PN.findPlace(innerActivity->id, ".initial"));
          if (!parameters[P_NOVARIABLES])
          {
	    PN.newArc(PN.findPlace("variable." + ASTEmap[ThisBranch->id]->variableName), t3);
	    PN.newArc(t3, PN.findPlace("variable." + ASTEmap[ThisBranch->id]->variableName));
          }

	  // Transitions for the negative control flow.
	  if (parameters[P_NOSTANDARDFAULTS] == false)
	    Transition *t4 = throwFault(p3, p10, "p3", newPrefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	  Transition *t9  = stop(p3, "p3at" + message, prefix);
	  Transition *t10 = stop(p10, "p10at" + message, prefix);

	  
	  // Embed the inner activity.
	  PN.mergePlaces(phylum->id, ".stop",    innerActivity->id, ".stop");
	  PN.mergePlaces(phylum->id, ".stopped", innerActivity->id, ".stopped");
	  PN.mergePlaces(phylum->id, ".final",   innerActivity->id, ".final");

	  trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
	}

	
	// Traverse the <onAlarm> branches.
	int onAlarmCount = 1;
	foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list phylum->tOnAlarm_list_1)
	{
	  trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
	  string newPrefix = prefix + "onAlarm" + toString(onAlarmCount++) + ".";

	  Transition *t6 = PN.newTransition(newPrefix + "t6");
	  PN.newArc(PN.findPlace(phylum->id, ".internal.TimeStamp"), t6);
	  PN.newArc(t6, PN.findPlace(innerActivity->id, ".initial"));
          if(!parameters[P_NOVARIABLES])
	    PN.newArc(PN.findPlace("1.internal.clock"), t6, READ);
	  
	  // Embed the inner activity.
	  PN.mergePlaces(phylum->id, ".stop",    innerActivity->id, ".stop");
	  PN.mergePlaces(phylum->id, ".stopped", innerActivity->id, ".stopped");
	  PN.mergePlaces(phylum->id, ".final",   innerActivity->id, ".final");

	  trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
	}

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "final");
	Place *p2 = PN.newPlace(prefix + "initial");

  
	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the onMessage branches
	phylum->tOnMessage_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the onAlarm branches
	phylum->tOnAlarm_list_1->unparse(pseudoPrinter, current_view);


	// Traverse the <onMessage> branches.
	int onMessageCount = 1;
	foreach (ThisBranch = OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list phylum->tOnMessage_list_1)
	{
	  trace(TRACE_DEBUG, "[PNU]\t\t<onMessage>\n");
	  string message = toString(onMessageCount++);
	  
	  Transition *t1 = PN.newTransition(prefix + "pickM" + message);
	  PN.newArc(PN.findPlace("in." + ASTEmap[ThisBranch->id]->channelName), t1);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1);
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));

	  // Embed the inner activity.
	  PN.mergePlaces(phylum->id, ".final", innerActivity->id, ".final");
	  
	  trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
	}

	// Traverse the <onAlarm> branches.
	int onAlarmCount = 1;
	foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list phylum->tOnAlarm_list_1)
	{
	  trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
	  string alarm = toString(onAlarmCount++);

	  Transition *t1 = PN.newTransition(prefix + "pickA" + alarm);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1);
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));

	  // Embed the inner activity.
	  PN.mergePlaces(phylum->id, ".final", innerActivity->id, ".final");

	  trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
	}


	// Create the arcs for the Dead Path elimination: each chosen activity
	// (i.e. firing of the respective t2/t6-transition) has set the links of
	// all other branches to false.
	int currentMessage = 1;
	foreach (OnMessage(corr_1, from_1, innerActivity1, _id1); tOnMessage_list phylum->tOnMessage_list_1) {
	  foreach (OnMessage(corr_2, from_2, innerActivity2, _id2); tOnMessage_list phylum->tOnMessage_list_1) {
	    if ((innerActivity1 != innerActivity2))
	      dpeLinks(PN.findTransition(prefix + "pickM" + toString(currentMessage)), innerActivity2->id); // onMessage -> onMessage'
	  }

	  foreach (OnAlarm(innerActivity2); tOnAlarm_list phylum->tOnAlarm_list_1) {
	    dpeLinks(PN.findTransition(prefix + "pickM" + toString(currentMessage)), innerActivity2->id); // onMessage -> onAlarm
	  }
	  
	  currentMessage++;
	}

	int currentAlarm = 1;
	foreach (OnAlarm(innerActivity1); tOnAlarm_list phylum->tOnAlarm_list_1) {
	  foreach (otheronalarm=OnAlarm(innerActivity2); tOnAlarm_list phylum->tOnAlarm_list_1) {
	    if ((innerActivity1 != innerActivity2))
	      dpeLinks(PN.findTransition(prefix + "pickA" + toString(currentAlarm)), innerActivity2->id); // onAlarm -> onAlarm'
	  }

	  foreach (OnMessage(_corr2, _from2, innerActivity2, _id2); tOnMessage_list phylum->tOnMessage_list_1) {
	    dpeLinks(PN.findTransition(prefix + "pickA" + toString(currentAlarm)), innerActivity2->id); // onAlarm -> onMessage
	  }
	  
	  currentAlarm++;
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}





/******************************************************************************
  IF
******************************************************************************/

void PIf(impl_tIf* abstract_phylum, uview current_view)
{
  impl_tIf_If* phylum = static_cast<impl_tIf_If*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  /*
   * Ad hoc and not quite correct version of the pattern: the branches are not
   * checked sequentially, but in parallel. FIXME!
   */
  switch(current_view)
  {
    case(petrinet_enum):
    case(petrinetnew_enum):
      {
	Place *p1 =  PN.newPlace(prefix + "initial");
	Place *p2 =  PN.newPlace(prefix + "final");
	Place *p3 =  PN.newPlace(prefix + "stop");
	Place *p4 =  PN.newPlace(prefix + "stopped");

    	Transition *t9 = stop(p1, "initial", prefix);
    	Transition *t10 = stop(p2, "final", prefix);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the elseIf branches
	phylum->tElseIf_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the else branch
	phylum->tElse_1->unparse(pseudoPrinter, current_view);


	int currentCase = 1;
	foreach (caseBranch=ElseIf(innerActivity, _id2); tElseIf_list phylum->tElseIf_list_1)
	{
	  string caseNumber = toString(currentCase++);
	  Transition* t1 = PN.newTransition(prefix + "case" + caseNumber);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here
	  
	  // Embed the inner activity.
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	  PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
	  PN.mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
	  PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
	}
	
	// The <else> branch.
	Transition *t3 = PN.newTransition(prefix + "caseo");
	PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t3); // added ".internal" here

	impl_tElse *elseBranch = phylum->tElse_1;

	with (elseBranch)
	{
	  elseBranch=Else(innerActivity,_id2):
	  {
	    // embed inner activity
            PN.newArc(t3, PN.findPlace(innerActivity->id, ".initial"));
	    PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
	    PN.mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
	    PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
	  }
	  
	  elseBranch=NoElse(_id2):
	  {
	    PN.newArc(t3, PN.findPlace(phylum->id, ".internal.final"));
	  }
	  
	  default: { /* do nothing */ }
	}

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 =  PN.newPlace(prefix + "initial");
	Place *p2 =  PN.newPlace(prefix + "final");


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the elseIf branches
	phylum->tElseIf_list_1->unparse(pseudoPrinter, current_view);
	
	// unparse the else branch
	phylum->tElse_1->unparse(pseudoPrinter, current_view);


	int currentCase = 1;
	foreach (caseBranch=ElseIf(innerActivity, _id2); tElseIf_list phylum->tElseIf_list_1)
	{
	  string caseNumber = toString(currentCase++);
	  Transition* t1 = PN.newTransition(prefix + "case" + caseNumber);
	  PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here
	  
	  // Embed the inner activity.
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	  PN.mergePlaces(phylum->id, ".internal.final", _id2->value, ".final");
	}
	
	// The <else> branch.
	Transition *t1 = PN.newTransition(prefix + "caseo");
	PN.newArc(PN.findPlace(phylum->id, ".internal.initial"), t1); // added ".internal" here

	impl_tElse *elseBranch = phylum->tElse_1;

	with (elseBranch)
	{
	  elseBranch=Else(innerActivity,_id2):
	  {
	    // embed inner activity
            PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
	    PN.mergePlaces(phylum->id, ".internal.final", _id2->value, ".final");
	  }
	  
	  elseBranch=NoElse(_id2):
	  {
	    PN.newArc(t1, PN.findPlace(phylum->id, ".internal.final"));
	  }
	  
	  default: { /* do nothing */ }
	}
	
	
	// Create the arcs for the Dead Path elimination: each chosen activity
	// (i.e. firing of the respective t2-transition) has set the links of all
	// other branches to false.
	currentCase = 1;
	foreach (ElseIf(innerActivity1); tElseIf_list phylum->tElseIf_list_1)
	{
	  foreach (ElseIf(innerActivity2); tElseIf_list phylum->tElseIf_list_1)
	  {
	    if (innerActivity1 != innerActivity2)
	    {
	      dpeLinks(PN.findTransition(prefix + "case" + toString(currentCase)), innerActivity2->id); // case -> case'
	    }
	  }
	  with (elseBranch)
	  {
	    elseBranch=Else(innerActivity3,_id2):
	    {
	      dpeLinks(PN.findTransition(prefix + "case" + toString(currentCase)), innerActivity3->id); // case -> otherwise
	      dpeLinks(PN.findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
	    }
	    default: { /* do nothing */ }
	  }
	  currentCase++;
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}      

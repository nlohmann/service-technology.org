/*****************************************************************************\
 * Copyright 2006, 2007 Niels Lohmann, Christian Gierds                      *
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
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 * 
 * \since   2006-11-26
 *
 * \date    \$Date: 2007/04/18 12:05:13 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-structured.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.48 $
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
#include "globals.h"
#include "petrinet-patterns-shortcuts.h"

using std::cerr;
using std::endl;

extern PetriNet PN;
%}





/******************************************************************************
  WHILE
******************************************************************************/

%{
/*!
 * \fn void kc::PWhile(impl_tWhile* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <while> activity.
 *
 *          \image html "while.png" "pattern"
 *
 * \ingroup patterns
 */
%}
void PWhile(impl_tWhile* abstract_phylum, uview current_view)
{
  impl_tWhile_While* phylum = static_cast<impl_tWhile_While*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  assert(globals::ASTEmap[phylum->id] != NULL);
  ASTE *myself = globals::ASTEmap[phylum->id];
  
  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "p3");
	Place *p4 = PN.newPlace(prefix + "final");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");
        Place *p7 = PN.newPlace(prefix + "p7");	

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
	  Place *p5l = PN.newPlace(prefix + "stop_loop");
	  Place *p5a = PN.newPlace(prefix + "stop_activity");
	  Place *p6l = PN.newPlace(prefix + "stopped_loop");
	  Place *p6a = PN.newPlace(prefix + "stopped_activity");

	  Transition *stop_split = PN.newTransition(prefix + "stop_split");
	  PN.newArc(p5, stop_split);
	  PN.newArc(stop_split, p5l);
	  PN.newArc(stop_split, p5a);
        
	  Transition *stopped_merge = PN.newTransition(prefix + "stopped_merge");
	  PN.newArc(p6l, stopped_merge);
	  PN.newArc(p6a, stopped_merge);
	  PN.newArc(stopped_merge, p6);
	}

	// external control of loop condition	
	Place *control_enter = NULL;
	Place *control_leave = NULL;
        if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	{
	  control_enter = PN.newPlace("in.enterloop." + toString(phylum->id), IN);
	  control_leave = PN.newPlace("in.leaveloop." + toString(phylum->id), IN);
        }

	Transition *t1 = PN.newTransition(prefix + "read_condition");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	
        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
          looppattern(myself->attributes["condition"], myself->max_loops, prefix, phylum->id,
                      p2, p3, p4);
        }
        else
        {
          // the single loop/skip transition is only needed when loops are not counted
          if (!globals::parameters[P_LOOPCOUNT])
	  {
	    Transition *t2 = PN.newTransition(prefix + "start");
	    PN.newArc(p2, t2);
	    PN.newArc(t2, p3);
            if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	    {
              PN.newArc(control_enter, t2);
            }
	  
            if (myself->attributes["condition"] != "true")
            {
	      Transition *t3 = PN.newTransition(prefix + "end");
	      PN.newArc(p2, t3);
	      PN.newArc(t3, p4);
              if (globals::parameters[P_LOOPCONTROL])
	      {
                PN.newArc(control_leave, t3);
              }
            }
	  }
	}
	
	// Transitions for the negative control flow.
	if (globals::parameters[P_STANDARDFAULTS])
	  Transition *t5 = throwFault(p2, p7, "running", prefix, mkinteger(phylum->id), globals::ASTEmap[phylum->id]->controlFlow);

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
	  Transition *t6 = stop_activity(p2, "running", prefix);
	  Transition *t7 = stop_activity(p4, "final", prefix);
	  Transition *t8 = stop_activity(p7, "p7", prefix);
	}
	else
	{
	  Transition *t6 = stop(p2, "running", prefix);
	  Transition *t7 = stop(p4, "final", prefix);
	  Transition *t8 = stop(p7, "p7", prefix);  
	}

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);
	
	
	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.p3",      phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".final");
	PN.mergePlaces(phylum->id, ".internal.stop",    phylum->activity_1->id, ".stop");
	PN.mergePlaces(phylum->id, ".internal.stopped", phylum->activity_1->id, ".stopped");
	
	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	

	// external control of loop condition	
	Place *control_enter = NULL;
	Place *control_leave = NULL;
        if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	{
	  control_enter = PN.newPlace("in.enterloop." + toString(phylum->id), IN);
	  control_leave = PN.newPlace("in.leaveloop." + toString(phylum->id), IN);
        }

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
          looppattern(myself->attributes["condition"], myself->max_loops, prefix, phylum->id,
                      p1, p2, p3);
        }
        else
        {
          if(myself->attributes["condition"] != "true")
          {
	    Transition *t1 = PN.newTransition(prefix + "leave");
	    PN.newArc(p1, t1);
	    PN.newArc(t1, p3);
	    if (globals::parameters[P_LOOPCONTROL])
	      PN.newArc(control_leave, t1);
	  }

	  Transition *t2 = PN.newTransition(prefix + "loop");
	  PN.newArc(p1, t2);
	  PN.newArc(t2, p2);
	  if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	    PN.newArc(control_enter, t2);
	}


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.running", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".final");	

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}





/******************************************************************************
  REPEAT UNTIL
******************************************************************************/

%{
/*!
 * \fn void kc::PRepeatUntil(impl_tRepeatUntil* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <repeatUntil> activity.
 * 
 *          \image html "repeatuntil.png" "pattern"
 *
 * \ingroup patterns
 */
%}
void PRepeatUntil(impl_tRepeatUntil* abstract_phylum, uview current_view)
{
  impl_tRepeatUntil_RepeatUntil* phylum = static_cast<impl_tRepeatUntil_RepeatUntil*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  assert(globals::ASTEmap[phylum->id] != NULL);
  ASTE *myself = globals::ASTEmap[phylum->id];

  switch (current_view)
  {
    case(petrinetnew_enum):
      {

        Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "p2"); // activity end
	Place *p3 = PN.newPlace(prefix + "running");
	Place *p4 = PN.newPlace(prefix + "final");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");
        Place *p7 = PN.newPlace(prefix + "faulted");


        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
	  Place *p5l = PN.newPlace(prefix + "stop_loop");
	  Place *p5a = PN.newPlace(prefix + "stop_activity");
	  Place *p6l = PN.newPlace(prefix + "stopped_loop");
	  Place *p6a = PN.newPlace(prefix + "stopped_activity");

	  Transition *stop_split = PN.newTransition(prefix + "stop_split");
	  PN.newArc(p5, stop_split);
	  PN.newArc(stop_split, p5l);
	  PN.newArc(stop_split, p5a);
        
	  Transition *stopped_merge = PN.newTransition(prefix + "stopped_merge");
	  PN.newArc(p6l, stopped_merge);
	  PN.newArc(p6a, stopped_merge);
	  PN.newArc(stopped_merge, p6);
	}

	// external control of loop condition	
	Place *control_enter = NULL;
	Place *control_leave = NULL;
        if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	{
	  control_enter = PN.newPlace("in.enterloop." + toString(phylum->id), IN);
	  control_leave = PN.newPlace("in.leaveloop." + toString(phylum->id), IN);
        }

	Transition *t1 = PN.newTransition(prefix + "read_condition");
	PN.newArc(p2, t1);
	PN.newArc(t1, p3);

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
          looppattern(myself->attributes["condition"], (myself->max_loops-1), prefix, phylum->id,
                      p3, p1, p4);
        }
        else
        {
          if(myself->attributes["condition"] != "true")
          {
            Transition *t2 = PN.newTransition(prefix + "end");
            PN.newArc(p3, t2);
            PN.newArc(t2, p4);
            if( globals::parameters[P_LOOPCONTROL])
              PN.newArc(control_leave, t2);
          }

          Transition *t3 = PN.newTransition(prefix + "repeat");
          PN.newArc(p3, t3);
          PN.newArc(t3, p1);
          if( globals::parameters[P_LOOPCONTROL])
            PN.newArc(control_enter, t3);	
        }

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
	  Transition *t4 = stop_activity(p3, "running", prefix);
	  Transition *t5 = stop_activity(p4, "final", prefix);
	  Transition *t6 = stop_activity(p7, "faulted", prefix);
	}
	else
	{
	  Transition *t4 = stop(p3, "running", prefix);
	  Transition *t5 = stop(p4, "final", prefix);
	  Transition *t6 = stop(p7, "faulted", prefix);
	}

	if (globals::parameters[P_STANDARDFAULTS])
	  Transition *t7 = throwFault(p3, p7, "running", prefix, mkinteger(phylum->id), globals::ASTEmap[phylum->id]->controlFlow);
	

	// the single loop/skip transition is only needed when loops are not counted
        /*
	if (!globals::parameters[P_LOOPCOUNT])
        {
	  Transition *t2 = PN.newTransition(prefix + "end");
	  PN.newArc(p3, t2);
	  PN.newArc(t2, p4);
	  if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	    PN.newArc(control_leave, t2);
	
	  Transition *t3 = PN.newTransition(prefix + "repeat");
	  PN.newArc(p3, t3);
	  PN.newArc(t3, p1);
	  if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	    PN.newArc(control_enter, t3);
	}

	// the subnet if loops are counted
	if (globals::parameters[P_LOOPCOUNT])
	{
	  unsigned int max_loops = globals::ASTEmap[phylum->id]->max_loops;
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
		Transition *t4 = PN.newTransition(prefix + "repeat" + toString(i-1));
		PN.newArc(t4, p1);
		PN.newArc(p3, t4);
		PN.newArc(p9_old, t4);
		PN.newArc(t4, p9);
		if (globals::parameters[P_LOOPCONTROL])
		  PN.newArc(control_enter, t4);
	      }
	      
	      assert(p9_first != NULL);
	      Transition *t1 = PN.newTransition(prefix + "end" + toString(i));
	      PN.newArc(p3, t1);
	      PN.newArc(t1, p4);
	      PN.newArc(t1, p9_first);
	      PN.newArc(p9, t1);
    	      if (globals::parameters[P_LOOPCONTROL])
    		PN.newArc(control_leave, t1);
	      
	      p9_old = p9;
	    }
	  }
	}
        */


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


	// external control of loop condition
	Place *control_enter = NULL;
	Place *control_leave = NULL;
        if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	{
	  control_enter = PN.newPlace("in.enterloop." + toString(phylum->id), IN);
	  control_leave = PN.newPlace("in.leaveloop." + toString(phylum->id), IN);
        }

        if((myself->max_loops != 0) && (myself->max_loops != UINT_MAX))
        {
          looppattern(myself->attributes["condition"], (myself->max_loops-1), prefix, phylum->id,
                      p2, p1, p3);
        }
        else
        {
          if(myself->attributes["condition"] != "true")
	  {
	    Transition *t1 = PN.newTransition(prefix + "end");
	    PN.newArc(p2, t1);
	    PN.newArc(t1, p3);
	    if (globals::parameters[P_LOOPCONTROL])
	      PN.newArc(control_leave, t1);
	  }

	  Transition *t2 = PN.newTransition(prefix + "repeat");
	  PN.newArc(p2, t2);
	  PN.newArc(t2, p1);
	  if (globals::parameters[P_LOOPCONTROL] && myself->attributes["condition"] != "true")
	    PN.newArc(control_enter, t2);
	}


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

%{
/*!
 * \fn      void kc::PSequence(impl_tSequence* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <sequence> activity.
 * 
 *          \image html "sequence.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PSequence(impl_tSequence* abstract_phylum, uview current_view)
{
  impl_tSequence_Sequence* phylum = static_cast<impl_tSequence_Sequence*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  // Places.
  Place *p1  = PN.newPlace(prefix + "initial");
  Place *p2  = PN.newPlace(prefix + "final");

  // Stopping places.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  // Unparse the embedded activities.
  phylum->activity_list_1->unparse(pseudoPrinter, current_view);

  // Connect the embedded activities.
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

%{
/*!
 * \fn void kc::PFlow(impl_tFlow* abstract_phylum, uview current_view)
 * \brief Pattern for the <flow> activity.
 * \image html "flow.png" "pattern"
 *
 * \ingroup patterns
 */
%}
void PFlow(impl_tFlow* abstract_phylum, uview current_view)
{
  impl_tFlow_Flow* phylum = static_cast<impl_tFlow_Flow*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");
	Place *p5 = PN.newPlace(prefix + "running");

	Transition *t1 = PN.newTransition(prefix + "split");
	PN.newArc(p1, t1);
	PN.newArc(t1, p5);

	Transition *t2 = PN.newTransition(prefix + "join");
	PN.newArc(p5, t2);
	PN.newArc(t2, p2);

	Transition *t3 = PN.newTransition(prefix + "stop_all");
	PN.newArc(p3, t3);
	PN.newArc(p5, t3);

	Transition *t4 = PN.newTransition(prefix + "all_stopped");
	PN.newArc(t4, p4);

	// Transitions for the negative control flow.
	Transition *t5 = stop(p1, "initial", prefix);
	Transition *t6 = stop(p2, "final", prefix);

	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_list_1->unparse(pseudoPrinter, current_view);

	// embed the inner activity 
	foreach (innerActivity; activity_list phylum->activity_list_1)
	{
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
       	  PN.newArc(t3, PN.findPlace(innerActivity->id, ".stop"));
       	  PN.newArc(PN.findPlace(innerActivity->id, ".stopped"), t4);
       	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t2);
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
  PICK
******************************************************************************/

%{
/*!
 * \fn      void kc::PPick(impl_tPick* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <pick> activity.
 *
 *          \image html "pick.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 * 
 * \todo    Use "receivingTransition" function in <onMessage> branches.
 */
%}
void PPick(impl_tPick* abstract_phylum, uview current_view)
{
  impl_tPick_Pick* phylum = static_cast<impl_tPick_Pick*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  
  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");

  // Stop places and stopping transition.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");

    Transition *t4 = stop(p1, "initial", prefix);
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  // Traverse the <onMessage> branches.
  unsigned int branch_count = 1;
  foreach (ThisBranch = OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list phylum->tOnMessage_list_1)
  {
    header(_id2, true);
    string branch = toString(branch_count++);

    // Unparse the embedded activity.
    innerActivity->unparse(pseudoPrinter, current_view);

    // Message-receiving transition.
    Transition *t1 = PN.newTransition(prefix + "onMessage_" + branch);
    PN.newArc(PN.findPlace(prefix + "initial"), t1);
    PN.newArc(PN.findPlace("in." + globals::ASTEmap[ThisBranch->id]->channelName), t1);
    PN.newArc(t1, PN.findPlace(toString(innerActivity->id) + ".initial"));

    // Embed the inner activity.
    PN.mergePlaces(phylum->id, ".final", innerActivity->id, ".final");

    if (current_view != petrinetsmall_enum)
    {
      PN.mergePlaces(phylum->id, ".stop",        innerActivity->id, ".stop");
      PN.mergePlaces(phylum->id, ".stopped",     innerActivity->id, ".stopped");
    }

    footer(_id2, true);
  }


  // Traverse the <onAlarm> branches.
  foreach (onAlarmBranch=OnAlarm(*,innerActivity,_id2); tOnAlarm_list phylum->tOnAlarm_list_1)
  {
    header(_id2, true);
    string branch = toString(branch_count++);

    // Unparse the embedded activity.
    innerActivity->unparse(pseudoPrinter, current_view);

    // The <onAlarm> transition.
    Transition *t3 = PN.newTransition(prefix + "onAlarm_" + branch);
    PN.newArc(PN.findPlace(prefix + "initial"), t3);
    PN.newArc(t3, PN.findPlace(toString(innerActivity->id) + ".initial"));

    // Embed the inner activity.
    PN.mergePlaces(phylum->id, ".final", innerActivity->id, ".final");

    if (current_view != petrinetsmall_enum)
    {
      PN.mergePlaces(phylum->id, ".stop",        innerActivity->id, ".stop");
      PN.mergePlaces(phylum->id, ".stopped",     innerActivity->id, ".stopped");
    }

    footer(_id2, true);
  }
  

  // Create the arcs for the Dead Path elimination: each chosen activity (i.e.
  // firing of the respective t1/t3-transition) has set the links of all other
  // branches to false.
  branch_count = 1;
  foreach (OnMessage(corr_1, from_1, innerActivity1, _id1); tOnMessage_list phylum->tOnMessage_list_1) {
    foreach (OnMessage(corr_2, from_2, innerActivity2, _id2); tOnMessage_list phylum->tOnMessage_list_1) {
      if ((innerActivity1 != innerActivity2))
	dpeLinks(PN.findTransition(prefix + "onMessage_" + toString(branch_count)), innerActivity2->id); // onMessage -> onMessage'
    }

    foreach (OnAlarm(*,innerActivity2); tOnAlarm_list phylum->tOnAlarm_list_1) {
      dpeLinks(PN.findTransition(prefix + "onMessage_" + toString(branch_count)), innerActivity2->id); // onMessage -> onAlarm
    }
    
    branch_count++;
  }

  foreach (OnAlarm(*,innerActivity1); tOnAlarm_list phylum->tOnAlarm_list_1) {
    foreach (otheronalarm=OnAlarm(*,innerActivity2); tOnAlarm_list phylum->tOnAlarm_list_1) {
      if ((innerActivity1 != innerActivity2))
	dpeLinks(PN.findTransition(prefix + "onAlarm_" + toString(branch_count)), innerActivity2->id); // onAlarm -> onAlarm'
    }
    
    foreach (OnMessage(_corr2, _from2, innerActivity2, _id2); tOnMessage_list phylum->tOnMessage_list_1) {
      dpeLinks(PN.findTransition(prefix + "onAlarm_" + toString(branch_count)), innerActivity2->id); // onAlarm -> onMessage
    }
    
    branch_count++;
  }

  footer(phylum->id, true);
}





/******************************************************************************
  IF / SWITCH
******************************************************************************/

%{
/*!
 * \fn      void kc::PIf(impl_tIf* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <if> and <switch> activity.
 *
 *          \image html "if.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    The branches are not checked sequentially, but in parallel. This
 *          would make a difference in colored Petri nets, but as we are
 *          modeling data-driven decisions by nondeterminism, the <if> can be
 *          modeled by a single conflicting place.
 *
 * \note    The <switch> activity of BPEL4WS 1.1 is modeled with the same
 *          pattern as the <if> activity as the two activity have similar
 *          semantics. The only difference is that an absent <otherwise> branch
 *          in the <switch> activity has to be filled with an <empty> activity
 *          rather than with a shortcut transition as an absent <else> branch.
 *          This implicit <empty> activity is added by the parser.
 *
 * \ingroup patterns
 */
%}
void PIf(impl_tIf* abstract_phylum, uview current_view)
{
  impl_tIf_If* phylum = static_cast<impl_tIf_If*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  // Places.
  Place *p1 =  PN.newPlace(prefix + "initial");
  Place *p2 =  PN.newPlace(prefix + "final");

  // Stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 =  PN.newPlace(prefix + "stop");
    Place *p4 =  PN.newPlace(prefix + "stopped");

    Transition *t9 = stop(p1, "initial", prefix);
    Transition *t10 = stop(p2, "final", prefix);
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  // Traverse the <elseif> branches (the first activity is also an <elseif>
  // branch).
  unsigned int currentCase = 1;
  foreach (caseBranch=ElseIf(innerActivity, _id2); tElseIf_list phylum->tElseIf_list_1)
  {
    header(_id2, true);
    string branch = toString(currentCase++);

    // Unparse the embedded activity.
    innerActivity->unparse(pseudoPrinter, current_view);

    // Transition chosing this branch.
    Transition* t1 = PN.newTransition(prefix + "case" + branch);
    PN.newArc(PN.findPlace(prefix + "initial"), t1);

    // Embed the inner activity.
    PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));
    PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
    PN.mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
    PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");

    footer(_id2, true);
  }


  // The <else> branch.
  Transition *t3 = PN.newTransition(prefix + "caseo");
  PN.newArc(PN.findPlace(prefix + "initial"), t3);

  impl_tElse *elseBranch = phylum->tElse_1;
  with (elseBranch)
  {
    elseBranch=Else(innerActivity,_id2):
    {
      header(_id2, true);
      
      // Unparse the embedded activity.
      phylum->tElse_1->unparse(pseudoPrinter, current_view);

      // Embed inner activity.
      PN.newArc(t3, PN.findPlace(innerActivity->id, ".initial"));
      PN.mergePlaces(phylum->id, ".internal.final", innerActivity->id, ".final");
  
      if (current_view != petrinetsmall_enum)
      {
	PN.mergePlaces(phylum->id, ".internal.stop", innerActivity->id, ".stop");
	PN.mergePlaces(phylum->id, ".internal.stopped", innerActivity->id, ".stopped");
      }

      footer(_id2, true);
    }

    elseBranch=NoElse(_id2):
    {
      PN.newArc(t3, PN.findPlace(phylum->id, ".internal.final"));
    }

    default: { /* do nothing */ }
  }


  // Create the arcs for the Dead Path elimination: each chosen activity (i.e.
  // firing of the respective t2-transition) has set the links of all other
  // branches to false.
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
      
      default:
      {
	dpeLinks(PN.findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
      }
    }
    
    currentCase++;
  }

  footer(phylum->id, true);
}





/******************************************************************************
  FOREACH
******************************************************************************/

void PForEach(impl_tForEach* abstract_phylum, uview current_view)
{
  impl_tForEach_ForEach* phylum = static_cast<impl_tForEach_ForEach*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  // the patterns for the parallel <forEach>
  if (globals::ASTEmap[phylum->id]->attributes["parallel"] == "no")
  {
    switch(current_view)
    {
      case(petrinetnew_enum):
      case(petrinetsmall_enum):
    	{
  	  Place *p1 = PN.newPlace(prefix + "initial");
  	  Place *p2 = PN.newPlace(prefix + "final");
  	  Place *p3 = PN.newPlace(prefix + "stop");
  	  Place *p4 = PN.newPlace(prefix + "stopped");
  	  Place *p5 = PN.newPlace(prefix + "running");

  	  Transition *t1 = PN.newTransition(prefix + "split");
  	  PN.newArc(p1, t1);
  	  PN.newArc(t1, p5);

  	  Transition *t2 = PN.newTransition(prefix + "join");
  	  PN.newArc(t2, p2);
  	  PN.newArc(p5, t2);

  	  Transition *t3 = PN.newTransition(prefix + "stop_foreach");
  	  PN.newArc(p3, t3);
  	  PN.newArc(p5, t3);

  	  Transition *t4 = PN.newTransition(prefix + "foreach_stopped");
  	  PN.newArc(t4, p4);

  	  // unparse the standard elements
  	  phylum->standardElements_1->unparse(pseudoPrinter, current_view);



          assert (globals::ASTEmap[phylum->tScope_1->id] != NULL);
          if (globals::ASTEmap[phylum->tScope_1->id]->max_occurrences != UINT_MAX)
          {
            for (unsigned int scope_number = 1; scope_number <= globals::ASTEmap[phylum->tScope_1->id]->max_occurrences; scope_number++)
            {
              globals::forEach_suffix = "_foreach_" + toString(scope_number);
              cerr << globals::forEach_suffix << endl;
//              phylum->tScope_1->unparse(pseudoPrinter, current_view);
            }
          }


  	  // unparse the embedded scope
  	  phylum->tScope_1->unparse(pseudoPrinter, current_view);


  	  // embedd scope
  	  PN.newArc(t1, PN.findPlace(toString(phylum->tScope_1->id) + ".initial"));
  	  PN.newArc(PN.findPlace(toString(phylum->tScope_1->id) + ".final"), t2);
  	  PN.newArc(t3, PN.findPlace(toString(phylum->tScope_1->id) + ".stop"));
  	  PN.newArc(PN.findPlace(toString(phylum->tScope_1->id) + ".stopped"), t4);

	  break;
	}

      default: { /* no pattern in this case */ }
    }
  }
  else
  {
    assert( false == "No pattern for parallel FOREACH, yet!" );
  }


  footer(phylum->id, true);
}

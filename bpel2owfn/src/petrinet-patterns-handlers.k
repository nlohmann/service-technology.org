/*****************************************************************************\
 * Copyright 2007 Niels Lohmann                                              *
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
 * \file    petrinet-patterns-handlers.cc
 *
 * \brief   Petri net patterns for the handlers
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2007-03-04
 *
 * \date    \$Date: 2007/03/04 15:10:58 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-scopes.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.3 $
 */
%}


%{ HEADER
/*!
 * \file petrinet-patterns-handlers.h
 * \brief Petri net patterns for the handlers
 *
 * See \ref petrinet-patterns-handlers.cc for more information.
 */
%}

%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"

using std::cerr;
using std::endl;

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
extern string currentScope;

extern set<string> ASTE_inputChannels;
extern set<string> ASTE_outputChannels;
extern set<string> ASTE_linkNames;
extern set<string> ASTE_variableNames;
%}





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

void PFaultHandlers(impl_tFaultHandlers* abstract_phylum, uview current_view)
{
  impl_tFaultHandlers_userDefinedFaultHandler* phylum = static_cast<impl_tFaultHandlers_userDefinedFaultHandler*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinetnew_enum):
      {
        prefix = toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.faultHandler.";

  	Place *p1 =  PN.newPlace(prefix + "initial");
    	Place *p2 =  PN.newPlace(prefix + "final");
      	Place *p3 =  PN.newPlace(prefix + "fault");
	Place *p4 =  PN.newPlace(prefix + "stop");
	Place *p5 =  PN.newPlace(prefix + "stopped");
	Place *p7 =  PN.newPlace(prefix + "running");
	Place *p8 =  PN.newPlace(prefix + "inner_stop");

	Transition *t4 = PN.newTransition(prefix + "stopped.initial");
	PN.newArc(p1, t4);
	PN.newArc(p3, t4);
	PN.newArc(p4, t4);
	PN.newArc(t4, p5);

	Transition *t5 = PN.newTransition(prefix + "stop_fh");
	PN.newArc(p4, t5);
	PN.newArc(p7, t5);
	PN.newArc(t5, p8);

	Transition *t9 = stop(p2, "final", prefix);

  	int catchBranches = 1;
    	foreach (catchBranch=Catch(innerActivity); tCatch_list phylum->tCatch_list_1)
      	{
	  // unparse the activity embedded to the catch branch
	  innerActivity->unparse(pseudoPrinter, current_view);

	  Transition *t1 = PN.newTransition(prefix + "catch" + toString(catchBranches));
	  PN.newArc(p1, t1);
	  PN.newArc(p3, t1);
	  PN.newArc(t1, p7);
	  PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));

	  Transition *t6 = PN.newTransition(prefix + "finish" + toString(catchBranches++));
	  PN.newArc(p7, t6);
	  PN.newArc(t6, p2);
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t6);

	  PN.mergePlaces(prefix + "inner_stop", toString(innerActivity->id) + ".stop");
	  PN.mergePlaces(prefix + "stopped", toString(innerActivity->id) + ".stopped");
	}

	with(phylum->tCatchAll_1)
	{
	  CatchAll(innerActivity, _id2):
	  {
  	    // unparse the activity embedded to the catchAll branch
  	    innerActivity->unparse(pseudoPrinter, current_view);

	    Transition *t2 = PN.newTransition(prefix + "catchAll");
	    PN.newArc(p1, t2);
	    PN.newArc(p3, t2);
	    PN.newArc(t2, p7);
	    PN.newArc(t2, PN.findPlace(innerActivity->id, ".initial"));

	    Transition *t7 = PN.newTransition(prefix + "finishCatchAll");
	    PN.newArc(p7, t7);
	    PN.newArc(t7, p2);
	    PN.newArc(PN.findPlace(innerActivity->id, ".final"), t7);

	    PN.mergePlaces(prefix + "inner_stop", toString(innerActivity->id) + ".stop");
	    PN.mergePlaces(prefix + "stopped", toString(innerActivity->id) + ".stopped");
	  }

	  NoCatchAll():
	  {
	    Place *p6 = PN.newPlace(prefix + "faulted");
	    Transition *t1 = throwFault(p1, p6, "initial", prefix, mkinteger(phylum->id), 4);
	    Transition *t2 = stop(p6, "faulted", prefix);
	  }

	  default:
	  {
	    /* this should never happen */
	  }
	}
	break;
      }

    default: { /* no pattern in this case */ }
  }

  footer(phylum->id, true);
}





/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

void PTerminationHandler(impl_tTerminationHandler* abstract_phylum, uview current_view)
{
  impl_tTerminationHandler_TerminationHandler* phylum = static_cast<impl_tTerminationHandler_TerminationHandler*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	prefix = toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.terminationHandler.";

  	Place *p1 = PN.newPlace(prefix + "initial");
    	Place *p2 = PN.newPlace(prefix + "final");
      	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "running");
	Place *p5 = PN.newPlace(prefix + "stopping");
	Place *p6 = PN.newPlace(prefix + "inner_initial");
	Place *p7 = PN.newPlace(prefix + "inner_final");
	Place *p8 = PN.newPlace(prefix + "inner_fault");

	Transition *t1 = PN.newTransition(prefix + "begin");
	PN.newArc(p1, t1);
	PN.newArc(t1, p4);
	PN.newArc(t1, p6);

	Transition *t2 = PN.newTransition(prefix + "end");
	PN.newArc(t2, p2);
	PN.newArc(p4, t2);
	PN.newArc(p7, t2);

	Transition *t3 = PN.newTransition(prefix + "abort");
	PN.newArc(t3, p3);
	PN.newArc(p4, t3);
	PN.newArc(t3, p5);
	PN.newArc(p8, t3);

	Transition *t4 = PN.newTransition(prefix + "ignore_fault");
	PN.newArc(p5, t4, READ);
	PN.newArc(p8, t4);

	Transition *t5 = PN.newTransition(prefix + "aborted");
	PN.newArc(t5, p2);
	PN.newArc(p5, t5);
	PN.newArc(p7, t5);

	// unparse inner activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);
  
	// embed inner activity
	PN.mergePlaces(prefix + "inner_initial", toString(phylum->activity_1->id) + ".initial");
	PN.mergePlaces(prefix + "inner_final",   toString(phylum->activity_1->id) + ".final");
	PN.mergePlaces(prefix + "stop",          toString(phylum->activity_1->id) + ".stop");
	PN.mergePlaces(prefix + "inner_final",   toString(phylum->activity_1->id) + ".stopped");
   
	break;
      }

    default: { /* no pattern in this case */ }
  }

  footer(phylum->id, true);
}





/******************************************************************************
  COMPENSATION HANDLER
******************************************************************************/

userDefinedCompensationHandler(a, _id) ->
  [petrinetnew:
    { header(_id->value, true); }
    a
    { footer(_id->value, true); }
  ]
;





/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

void PEventHandlers(impl_tEventHandlers* abstract_phylum, uview current_view)
{
  impl_tEventHandlers_userDefinedEventHandler* phylum = static_cast<impl_tEventHandlers_userDefinedEventHandler*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "finishing");
	Place *p4 = PN.newPlace(prefix + "final");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");
	Place *p11 = PN.newPlace(prefix + "stopping");
        Place *p12 = PN.newPlace(prefix + "fault");

	Transition *t1 = PN.newTransition(prefix + "start_eh");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t4 = PN.newTransition(prefix + "stop_running");
	PN.newArc(p2, t4);
	PN.newArc(p5, t4);
	PN.newArc(t4, p11);

	Transition *t5 = PN.newTransition(prefix + "stop_finishing");
        PN.newArc(p3, t5);
        PN.newArc(p5, t5);
	PN.newArc(t5, p11);

	Transition *t6 = PN.newTransition(prefix + "eh_stopped");
        PN.newArc(p11, t6);
	PN.newArc(t6, p6);

	Transition *t7 = PN.newTransition(prefix + "finish_eh");
        PN.newArc(p3, t7);
	PN.newArc(t7, p4);


	// traverse the <onMessage> branches
	unsigned int event_number = 1;
	foreach(branch=OnMessage(my_corr, my_from, my_activity, my_integer); tOnMessage_list phylum->tOnMessage_list_1)
	{
	  assert(ASTEmap[my_integer->value] != NULL);
	  my_activity->unparse(pseudoPrinter, current_view);

	  Place *p7 = PN.newPlace(prefix + "pending." + toString(event_number));
	  Place *p8 = PN.newPlace(prefix + "p8." + toString(event_number));
	  Place *p9 = PN.newPlace(prefix + "p9." + toString(event_number));
	  Place *p10 = PN.newPlace(prefix + "finished." + toString(event_number));

          PN.newArc(t1, PN.findPlace(prefix + "pending." + toString(event_number)));
          PN.newArc(t4, PN.findPlace(prefix + "p9." + toString(event_number)));
          PN.newArc(t5, PN.findPlace(prefix + "p9." + toString(event_number)));
          PN.newArc(PN.findPlace(prefix + "finished." + toString(event_number)), t6);
          PN.newArc(PN.findPlace(prefix + "finished." + toString(event_number)), t7);

          Transition *t2 = PN.newTransition(prefix + "event." + toString(event_number));
          PN.newArc(PN.findPlace(prefix + "pending." + toString(event_number)), t2);
          PN.newArc(p2, t2, READ);
          PN.newArc(PN.findPlace("in." + ASTEmap[my_integer->value]->channelName), t2);
          PN.newArc(t2, PN.findPlace(prefix + "p8." + toString(event_number)));

          Transition *t3 = PN.newTransition(prefix + "finish." + toString(event_number));
          PN.newArc(PN.findPlace(prefix + "pending." + toString(event_number)), t3);
          PN.newArc(p3, t3, READ);
          PN.newArc(t3, PN.findPlace(prefix + "finished." + toString(event_number)));

	  PN.mergePlaces((prefix + "p8." + toString(event_number)),       (toString(my_activity->id) + ".internal.initial"));
	  PN.mergePlaces((prefix + "p9." + toString(event_number)),       (toString(my_activity->id) + ".internal.stop"));
	  PN.mergePlaces((prefix + "finished." + toString(event_number)), (toString(my_activity->id) + ".internal.stopped"));
	  PN.mergePlaces((prefix + "pending." + toString(event_number)),  (toString(my_activity->id) + ".internal.final"));
//	  PN.mergePlaces((prefix + "fault"),                              (toString(my_activity->id) + ".internal.fault")); // should be obsolete...

	  event_number++;
	}


	// traverse the <onAlarm> branches
	foreach(branch=OnAlarm(my_activity, my_integer); tOnAlarm_list phylum->tOnAlarm_list_1)
	{
	  assert(ASTEmap[my_integer->value] != NULL);
	  my_activity->unparse(pseudoPrinter, current_view);

	  Place *p7 = PN.newPlace(prefix + "pending." + toString(event_number));
	  Place *p8 = PN.newPlace(prefix + "p8." + toString(event_number));
	  Place *p9 = PN.newPlace(prefix + "p9." + toString(event_number));
	  Place *p10 = PN.newPlace(prefix + "finished." + toString(event_number));

          PN.newArc(t1, PN.findPlace(prefix + "pending." + toString(event_number)));
          PN.newArc(t4, PN.findPlace(prefix + "p9." + toString(event_number)));
          PN.newArc(t5, PN.findPlace(prefix + "p9." + toString(event_number)));
          PN.newArc(PN.findPlace(prefix + "finished." + toString(event_number)), t6);
          PN.newArc(PN.findPlace(prefix + "finished." + toString(event_number)), t7);

          Transition *t2 = PN.newTransition(prefix + "event." + toString(event_number));
          PN.newArc(PN.findPlace(prefix + "pending." + toString(event_number)), t2);
          PN.newArc(p2, t2, READ);
          PN.newArc(t2, PN.findPlace(prefix + "p8." + toString(event_number)));

          Transition *t3 = PN.newTransition(prefix + "finish." + toString(event_number));
          PN.newArc(PN.findPlace(prefix + "pending." + toString(event_number)), t3);
          PN.newArc(p3, t3, READ);
          PN.newArc(t3, PN.findPlace(prefix + "finished." + toString(event_number)));

	  PN.mergePlaces((prefix + "p8." + toString(event_number)),       (toString(my_activity->id) + ".internal.initial"));
	  PN.mergePlaces((prefix + "p9." + toString(event_number)),       (toString(my_activity->id) + ".internal.stop"));
	  PN.mergePlaces((prefix + "finished." + toString(event_number)), (toString(my_activity->id) + ".internal.stopped"));
	  PN.mergePlaces((prefix + "pending." + toString(event_number)),  (toString(my_activity->id) + ".internal.final"));
//	  PN.mergePlaces((prefix + "fault"),                              (toString(my_activity->id) + ".internal.fault")); // should be obsolete

          // \TODO check for <repeatEvery> 

	  event_number++;
	}

	break;
      }

    default: { /* no pattern in this case */ }
  }

  footer(phylum->id, true);
}

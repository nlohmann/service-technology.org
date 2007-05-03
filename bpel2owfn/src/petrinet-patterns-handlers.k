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
 *          last changes of: \$Author: gierds $ 
 * 
 * \since   2007-03-04
 *
 * \date    \$Date: 2007/05/03 12:50:07 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-scopes.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.18 $
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
#include <cassert>

#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"
#include "globals.h"
#include "petrinet-patterns-shortcuts.h"

using std::cerr;
using std::endl;

extern PetriNet PN;
%}





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

%{
/*!
 * \fn      void kc::PFaultHandlers(impl_tFaultHandlers* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <faultHandlers> construct.
 *
 *          \image html "fh.png" "pattern"
 *
 * \note    This function embeds the <faultHandlers> to the parent scope.
 *
 * \todo    
 *          - Replace NoCatchAll by a CatchAll phylum with a <rethrow> activity?
 *          - Implement the new <faultHandlers> pattern.
 *
 * \ingroup patterns
 */
%}
void PFaultHandlers(impl_tFaultHandlers* abstract_phylum, uview current_view)
{
  impl_tFaultHandlers_FaultHandlers* phylum = static_cast<impl_tFaultHandlers_FaultHandlers*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".faultHandlers.";


  // Places.
  Place *p1 =  PN.newPlace(prefix + "initial");
  Place *p2 =  PN.newPlace(prefix + "final");
  Place *p3 =  PN.newPlace(prefix + "fault");
  Place *p4 =  PN.newPlace(prefix + "stop");
  Place *p5 =  PN.newPlace(prefix + "stopped");
  Place *p7 =  PN.newPlace(prefix + "running");
  Place *p8 =  PN.newPlace(prefix + "inner_stop");

  if ( globals::ASTEmap[phylum->id]->callable )
  {

  Transition *t4 = PN.newTransition(prefix + "stopped.initial");
  PN.newArc(p1, t4);
  PN.newArc(p3, t4);
  PN.newArc(p4, t4);
  PN.newArc(t4, p5);

  Transition *t5 = PN.newTransition(prefix + "stop_fh");
  PN.newArc(p4, t5);
  PN.newArc(p7, t5);
  PN.newArc(p3, t5);
  PN.newArc(t5, p8);

  Transition *t9 = stop(p2, "final", prefix);


  // Traverse the <catch> branches.  
  unsigned int catchBranches = 1;
  foreach (catchBranch=Catch(innerActivity); tCatch_list phylum->tCatch_list_1)
  {
    // unparse the activity embedded to the catch branch
    innerActivity->unparse(pseudoPrinter, current_view);

    Transition *t1 = PN.newTransition(prefix + "catch" + toString(catchBranches));
    PN.newArc(p1, t1);
    PN.newArc(p3, t1, READ);
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
      header(_id2, true);

      // Unparse the activity embedded to the <catchAll> branch
      innerActivity->unparse(pseudoPrinter, current_view);

      Transition *t2 = PN.newTransition(prefix + "catchAll");
      PN.newArc(p1, t2);
      PN.newArc(p3, t2, READ);
      PN.newArc(t2, p7);
      PN.newArc(t2, PN.findPlace(innerActivity->id, ".initial"));

      Transition *t7 = PN.newTransition(prefix + "finishCatchAll");
      PN.newArc(p7, t7);
      PN.newArc(t7, p2);
      PN.newArc(PN.findPlace(innerActivity->id, ".final"), t7);

      PN.mergePlaces(prefix + "inner_stop", toString(innerActivity->id) + ".stop");
      PN.mergePlaces(prefix + "stopped", toString(innerActivity->id) + ".stopped");

      footer(_id2, true);
    }

    NoCatchAll():
    {
      Place *p6 = PN.newPlace(prefix + "faulted");
      Transition *t1 = throwFault(p1, p6, "initial", prefix, mkinteger(phylum->id), 4);
      Transition *t2 = stop(p6, "faulted", prefix);
    }

    default: { /* this should never happen */ }
  }


  // Embedd fault handlers to parent scope.
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.inner_stopped"), (prefix + "initial"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.fh_fault"),      (prefix + "fault"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.final"),         (prefix + "final"));
  if (globals::ASTEmap[phylum->id]->parentScopeId != 1)
  {
    PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.stop"),        (prefix + "stop"));
    PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.stopped"),     (prefix + "stopped"));
  }
  else
  {
    /* für das Prozesspattern hier andere Plätze verschmelzen */
    PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.inner_stop"),  (prefix + "stop"));
    PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.final"),       (prefix + "stopped"));
  }

  }
  footer(phylum->id, true);
}





/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

%{
/*!
 * \fn      void kc::PTerminationHandler(impl_tTerminationHandler* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <terminationHandler> construct.
 *
 *          \image html "th.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This function embeds the <terminationHandler> to the parent scope.
 *
 * \ingroup patterns
 */
%}
void PTerminationHandler(impl_tTerminationHandler* abstract_phylum, uview current_view)
{
  impl_tTerminationHandler_TerminationHandler* phylum = static_cast<impl_tTerminationHandler_TerminationHandler*>(abstract_phylum);
  assert(phylum != NULL);
  assert(globals::ASTEmap[phylum->id] != NULL);

  header(phylum->id, true);
  string prefix = toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".terminationHandler.";

  // Places.
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


  // Unparse inner activity.
  phylum->activity_1->unparse(pseudoPrinter, current_view);
  
  // Embed inner activity to termination handler.
  PN.mergePlaces(prefix + "inner_initial", toString(phylum->activity_1->id) + ".initial");
  PN.mergePlaces(prefix + "inner_final",   toString(phylum->activity_1->id) + ".final");
  PN.mergePlaces(prefix + "stop",          toString(phylum->activity_1->id) + ".stop");
  PN.mergePlaces(prefix + "inner_final",   toString(phylum->activity_1->id) + ".stopped");

  // Embedd termination handler to parent scope.
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.th_initial"), (prefix + "initial"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.stopped"),    (prefix + "final"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.stop"),       (prefix + "stop"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.stopped"),    (prefix + "final"));

   
  footer(phylum->id, true);
}





/******************************************************************************
  COMPENSATION HANDLER
******************************************************************************/

%{
/*!
 * \fn      void kc::PCompensationHandler(impl_tCompensationHandler* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <compensationHandler> construct.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This function embeds the <compensationHandler> to the parent scope.
 *
 * \ingroup patterns
 *
 * \todo    Remove or implement code snippet for loop count parameter.
 */
%}
void PCompensationHandler(impl_tCompensationHandler* abstract_phylum, uview current_view)
{
  impl_tCompensationHandler_CompensationHandler* phylum = static_cast<impl_tCompensationHandler_CompensationHandler*>(abstract_phylum);
  assert(phylum != NULL);
  assert(globals::ASTEmap[phylum->id] != NULL);

  header(phylum->id, true);

  // Unparse inner activity.
  phylum->activity_1->unparse(pseudoPrinter, current_view);


  // Embed compensation handler (i.e., the compensation handler's inner
  // activity) to parent scope.
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.ch_initial"), (toString(phylum->activity_1->id) + ".initial"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.compensated"), (toString(phylum->activity_1->id) + ".final"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.ch_stop"),    (toString(phylum->activity_1->id) + ".stop"));
  PN.mergePlaces((toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.ch_stopped"), (toString(phylum->activity_1->id) + ".stopped"));


// OLD CODE FOR LOOPCOUNT
//	    if (globals::ASTEmap[phylum->id]->ancestorLoops().size() != 0 && parameters[P_LOOPCOUNT])
//	      PN.mergePlaces((prefix + "compensated1"), (toString(ch_activity->id) + ".final"));
//	    else
//	      PN.mergePlaces((prefix + "compensated"),  (toString(ch_activity->id) + ".final"));

  footer(phylum->id, true);
}





/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

%{
/*!
 * \fn      void kc::PEventHandlers(impl_tEventHandlers* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <eventHandlers> construct.
 *
 *          \image html "eh.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 * 
 * \todo 
 *          - Use "receivingTransition" function in <onEvent> branches.
 *          - Implement <onAlarm> pattern with "repeatEvery" attribute.
 *          - Check the pattern with stopping constructs.
 */
%}
void PEventHandlers(impl_tEventHandlers* abstract_phylum, uview current_view)
{
  impl_tEventHandlers_EventHandlers* phylum = static_cast<impl_tEventHandlers_EventHandlers*>(abstract_phylum);
  assert(phylum != NULL);
  assert(globals::ASTEmap[phylum->id] != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + "."; // was ".internal."

  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "disabled");
  Place *p3 = PN.newPlace(prefix + "start");
  Place *p4 = PN.newPlace(prefix + "active");
  Place *p7 = PN.newPlace(prefix + "disabling");

  // Initialize the event handlers.
  Transition *t1 = PN.newTransition(prefix + "start_eh");
  PN.newArc(p1, t1);
  PN.newArc(t1, p3);
  PN.newArc(t1, p4);

  // Disable the event handlers.
  Transition *t2 = PN.newTransition(prefix + "disable_eh");
  PN.newArc(p7, t2);
  PN.newArc(t2, p2);


  // Stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p5 = PN.newPlace(prefix + "stop");
    Place *p6 = PN.newPlace(prefix + "stopped");

    // Stop event handlers when active.
    Transition *t7 = PN.newTransition(prefix + "stop_active");
    PN.newArc(p4, t7);
    PN.newArc(p5, t7);

    // Stop event handlers when disabling.
    Transition *t8 = PN.newTransition(prefix + "stop_disabling");
    PN.newArc(p5, t8);
    PN.newArc(p7, t8);

    // Event handlers stopped.
    Transition *t11 = PN.newTransition(prefix + "eh_stopped");
    PN.newArc(t11, p6);
  }


  // Traverse the <onEvent> branches.
  unsigned int event_number = 1;
  foreach(branch=OnMessage(my_corr, my_from, my_activity, my_integer); tOnMessage_list phylum->tOnMessage_list_1)
  {
    header(my_integer, true);
    assert(globals::ASTEmap[branch->id] != NULL);
    string event = toString(event_number++);
//    prefix = toString(my_integer->value);

    // Unparse the embedded activity.
    my_activity->unparse(pseudoPrinter, current_view);

    // Places.
    Place *p8 = PN.newPlace(prefix + "active." + event);
    Place *p9 = PN.newPlace(prefix + "disabled." + event);
    Place *p10 = PN.findPlace(my_activity->id, ".initial");
    Place *p13 = PN.findPlace(my_activity->id, ".final");

    // Start embedded activity.
    Transition *t4 = PN.newTransition(toString(my_integer->value) + "." + "onEvent." + event);
    PN.newArc(p8, t4);
    PN.newArc(t4, p10);
    PN.newArc(PN.findPlace("in." + globals::ASTEmap[branch->id]->channelName), t4);

    // Re-activate the <onEvent> handler.
    Transition *t5 = PN.newTransition(prefix + "loop." + event);
    PN.newArc(t5, p8);
    PN.newArc(p13, t5);
    PN.newArc(PN.findPlace(prefix + "active"), t5, READ);

    // Disable <onEvent> handler (activity finished).
    Transition *t6 = PN.newTransition(prefix + "disable_finished." + event);
    PN.newArc(t6, p9);
    PN.newArc(p13, t6);
    PN.newArc(PN.findPlace(prefix + "disabling"), t6, READ);

    // Disable <onEvent> handler (activity not started).
    Transition *t12 = PN.newTransition(prefix + "disable_active." + event);
    PN.newArc(t12, p9);
    PN.newArc(p8, t12);
    PN.newArc(PN.findPlace(prefix + "disabling"), t12, READ);

    // Connect <onEvent> handler.
    PN.newArc(t1, p8);
    PN.newArc(p9, t2);


    // Stop places and stopping transitions.
    if (current_view != petrinetsmall_enum)
    {
      Place *p11 = PN.findPlace(my_activity->id, ".stop");
      Place *p12 = PN.findPlace(my_activity->id, ".stopped");

      Transition *t9 = PN.newTransition(prefix + "stop_active." + event);
      PN.newArc(p8, t9);
      PN.newArc(p11, t9);
      PN.newArc(t9, p12);

      Transition *t10 = PN.newTransition(prefix + "stop_disabled." + event);
      PN.newArc(p9, t10);
      PN.newArc(p11, t10);
      PN.newArc(t10, p12);

      Transition *t7 = PN.findTransition(prefix + "stop_active");
      Transition *t8 = PN.findTransition(prefix + "stop_disabling");
      Transition *t11 = PN.findTransition(prefix + "eh_stopped");    
      PN.newArc(t7, p11);
      PN.newArc(t8, p11);
      PN.newArc(p12, t11);
    }

    footer(my_integer, true);
  }


  // Traverse the <onAlarm> branches.
  foreach(branch=OnAlarm(my_RepeatEvery,my_activity, my_integer); tOnAlarm_list phylum->tOnAlarm_list_1)
  {
    header(my_integer, true);
    string event = toString(event_number++);

    // Unparse the embedded activity.
    my_activity->unparse(pseudoPrinter, current_view);

    // Places.
    Place *p8 = PN.newPlace(prefix + "active." + event);
    Place *p9 = PN.newPlace(prefix + "disabled." + event);
    Place *p10 = PN.findPlace(my_activity->id, ".initial");
    Place *p13 = PN.findPlace(my_activity->id, ".final");

    // Start embedded activity.
    Transition *t4 = PN.newTransition(prefix + "onAlarm." + event);
    PN.newArc(p8, t4);
    PN.newArc(t4, p10);

    // Disable <onAlarm> handler (activity not started).
    Transition *t12 = PN.newTransition(prefix + "disable_active." + event);
    PN.newArc(t12, p9);
    PN.newArc(p8, t12);
    PN.newArc(PN.findPlace(prefix + "disabling"), t12, READ);

    // Connect <onAlarm> handler.
    PN.newArc(t1, p8);
    PN.newArc(p9, t2);

    // Extend net in case of RepeatEvery
    if(string(my_RepeatEvery->op_name())=="RepeatEvery")
    {
      // Insertion of loop Transition
      Transition *t5 = PN.newTransition(prefix + "loop." + event);
      PN.newArc(p13, t5);
      PN.newArc(t5, p8);      
      PN.newArc(PN.findPlace(prefix + "active"), t5, READ);

      // Extended Disable
      Transition *t6 = PN.newTransition(prefix + "disable_final." + event);
      PN.newArc(p13, t6);
      PN.newArc(t6, p9);      
      PN.newArc(PN.findPlace(prefix + "disabling"), t12, READ);

    }
    else
    {
    // Merge activity's final place with the disabled place.
    PN.mergePlaces(p9, p13);
    }


    // Stop places and stopping transitions.
    if (current_view != petrinetsmall_enum)
    {
      Place *p11 = PN.findPlace(my_activity->id, ".stop");
      Place *p12 = PN.findPlace(my_activity->id, ".stopped");

      Transition *t9 = PN.newTransition(prefix + "stop_active." + event);
      PN.newArc(p8, t9);
      PN.newArc(p11, t9);
      PN.newArc(t9, p12);

      Transition *t7 = PN.findTransition(prefix + "stop_active");
      Transition *t8 = PN.findTransition(prefix + "stop_disabling");
      Transition *t11 = PN.findTransition(prefix + "eh_stopped");    
      PN.newArc(t7, p11);
      PN.newArc(t8, p11);
      PN.newArc(p12, t11);

      if(string(my_RepeatEvery->op_name())=="RepeatEvery")
      {
        // Extended Stop
        Transition *t10 = PN.newTransition(prefix + "stop_disable." + event);
        PN.newArc(p9, t10);
        PN.newArc(p11, t10);
        PN.newArc(t10, p12);
      }
    }

    footer(my_integer, true);
  }


  footer(phylum->id, true);
}

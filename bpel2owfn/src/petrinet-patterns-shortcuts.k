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
 * \file    petrinet-patterns-shortcuts.cc
 *
 * \brief   Petri net patterns for shortcuts
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2007-03-06
 *
 * \date    \$Date: 2007/03/06 15:08:28 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-shortcuts.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.8 $
*/
%}


%{ HEADER
/*!
 * \file petrinet-patterns-shortcuts.h
 * \brief Petri net patterns for shortcuts
 *
 * See \ref petrinet-patterns-shortcuts.cc for more information.
 */
%}


%{ HEADER
#include "petrinet.h"
%}

%{
#include <iostream>
#include <cmath>

#include "ast-details.h"
#include "options.h"
#include "globals.h"
#include "petrinet.h"

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;

using std::cerr;
using std::endl;
%}





%{
/*!
 * \fn      Transition *kc::throwFault(Place *p1, Place *p2, string p1name, string prefix, integer id, int negativeControlFlow, bool preventFurtherFaults = true)
 *
 * \brief   generates transition and places to throw a fault
 *
 *          This functions generates a subnet to model the throwing of a fault.
 *          It models faults in any control flow of the BPEL process as well as
 *          in any patterns used.
 *
 * \param   p1  the place in positive control flow from which the control flow
 *              enters the negative control flow
 *
 * \param   p2  the place of the pattern on which a token shall be produced which
 *              can only be removed by stopping the pattern
 *
 * \param   p1name  name of place p1 which is used to label the generated
 *                  transitions (e.g. if p1 is called "running" generated
 *                  transitions end with this name (e.g. "throwFault.running")
 *
 * \param   prefix  prefix of the pattern to label generated places and
 *                  transitions
 *
 * \param   id  identifier of the caller activity
 *
 * \param   negativeControlFlow  signals where the activity is located:
 *                                - 0: inside a scope or the process
 *                                - 1: inside a fault handler
 *                                - 2: inside a compensation handler
 *
 * \param   preventFurtherFaults  controls what happens to further faults
 *                                 - true: these faults are prevented (standard)
 *                                 - false: these faults are suppressed
 *
 * \return  a pointer to the (first) generated fault transition
*/
%}
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, integer id,
    int negativeControlFlow, bool preventFurtherFaults = true)
{
  assert(p1 != NULL);
  assert(p2 != NULL);


  // no fault transitions in case of "communicationonly" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;


  // if attribute "exitOnStandardFault" is set to "yes", the process should
  // terminate rather than handling the fault
  if (ASTEmap[ASTEmap[id->value]->parentScopeId]->attributes["exitOnStandardFault"] == "yes")
  {
    Transition *t1 = PN.newTransition(prefix + "exitOnStandardFault." + p1name);
    PN.newArc(p1, t1);
    PN.newArc(t1, p2);
    PN.newArc(t1, PN.findPlace("1.internal.exit"));

    return t1;
  }


  // fault handling for the `new' patterns
    switch (negativeControlFlow)
    {
      case(0): // activity in scope or process
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(1): // activity in fault handlers
      case(4): // <rethrow> activity
	{
	  // The parent scope is just the scope of the handler or the
	  // activity. Thus, we are interested in the parent's parent.
          unsigned int parentId = ASTEmap[ASTEmap[id->value]->parentScopeId]->parentScopeId;

	  Transition *t1;
	  if (negativeControlFlow == 4)
	    t1 = PN.newTransition(prefix + "rethrow");
	  else
	    t1 = PN.newTransition(prefix + "rethrowFault." + p1name);

	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);

	  if (ASTEmap[id->value]->parentScopeId == 1)
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.exit"));
	  else
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(2): // activity in compensation handler
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

          // parent scope of a compensation handler shouldn't be the process :)
	  assert(parentId != 1);

	  Transition *t1 = PN.newTransition(prefix + "throwCHFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.ch_fault_in"));

	  return t1;
	}

      case(3): // activity in termination handler
        {
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "signalFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.terminationHandler.inner_fault"));
        }
    }
    return NULL;
}





%{
/*!
 * \fn      Transition *kc::stop(Place *p, string p_name, string prefix)
 *
 * \brief   generates a subnet to stop
 *
 *          Generates a transition and places to stop the activity, i.e. a
 *          transition moving a token on the "stop" place to "stopped".
 *
 * \param   p  the place in control flow from which the token is move to
 *             "stop"
 *
 * \param   p_name  name of place p which is used to label the generated
 *                  transition (e.g. if p1 is called "running" generated
 *                  transition end with this name (e.g. "stop.running")
 *
 * \param   prefix  prefix of the pattern to label generated transition
 *
 * \return  a pointer to the stop transition
 */
%}
Transition *stop(Place *p, string p_name, string prefix)
{
  assert(p != NULL);

  // no stop transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
  Transition *stopTransition = PN.newTransition(prefix + "stopped." + p_name);
  PN.newArc(PN.findPlace(prefix + "stop"), stopTransition);
  PN.newArc(stopTransition, PN.findPlace(prefix + "stopped"));
  PN.newArc(p, stopTransition);

  return stopTransition;
}





%{
/*!
 * \fn      void kc::dpeLinks(Transition *t, int id)
 *
 * \brief   add a subnet for dead-path elimination
 *
 *          Creates arcs to set links on dead paths to false.
 *
 * \param   t   transition that invokes DPE
 * \param   id  the identifier of the AST node
 */
%}
void dpeLinks(Transition *t, int id)
{
  ENTER("[ASTT]");

  assert(t != NULL);
  assert(ASTEmap[id] != NULL);

  for (set<unsigned int>::iterator linkID = ASTEmap[id]->enclosedSourceLinks.begin();
      linkID != ASTEmap[id]->enclosedSourceLinks.end();
      linkID++)
  {
    assert(ASTEmap[*linkID] != NULL);
    string linkName = ASTEmap[*linkID]->linkName;
    PN.newArc(t, PN.findPlace("!link." + linkName));
  }

  LEAVE("[ASTT]");
}





%{
/*!
 * \fn void kc::P_transitionCondition(string prefix, tSource_list source_links)
 *
 * \brief   Pattern to model the transition condition of an activity
 *
 * \param   prefix        the prefix of the activity's pattern
 * \param   source_links  the list of outgoing links of the activity
 *
 * \todo    Implement nondeterministic transition condition and add respective
 *          parameter.
 */
%}
void P_transitionCondition(string prefix, tSource_list source_links)
{
  // Find the places of the wrapper pattern. The "done" place is marked when
  // the activity completes. The "final" place is marked after the outgoing
  // links are set.
  Place *p_done = PN.findPlace(prefix + "done");
  Place *p_final = PN.findPlace(prefix + "final");

  assert(p_done != NULL);
  assert(p_final != NULL);


  /*
   * case 1: use XOR for the links (outgoing links are set to true mutually
   *         exclusive, i.e. "1 out of n")
   */
  if (parameters[P_XOR] && (source_links->length() > 1))
  {
    // Traverse the source links.
    foreach (source; tSource_list source_links)
    {
      unsigned int linkCounter = 1;

      // Add a transition.
      Transition *t1 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
      PN.newArc(p_done, t1);
      PN.newArc(t1, p_final);

      // Set one link to true.
      Place *link_true = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t1, link_true);

      // Set the other links to false.
      foreach (source2; tSource_list source_links)
      {
	if (ASTEmap[source2->id]->linkName != ASTEmap[source->id]->linkName)
	{
	  Place *link_false = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	  PN.newArc(t1, link_false);
	}
      }

      // User-defined transition conditions will be ignored.
      string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
      if ((givenTransitionCondition != "") && (givenTransitionCondition != "true"))
      {
        cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
        cerr << "Using XOR instead." << endl << endl;
      }
    }
  }

  /*
   * case 2: use the standard transition condition (all outgoing links are set
   *         to true, i.e. "n out of n")
   */
  else
  {
    // Add a transition for all links.
    Transition *t1 = PN.newTransition(prefix + "setLinks");
    PN.newArc(p_done, t1);
    PN.newArc(t1, p_final);

    // Set all links to true.
    foreach (source; tSource_list source_links)
    {
      Place *link_true = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t1, link_true);

      // User-defined transition conditions will be ignored.
      string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
      if (givenTransitionCondition != "" && givenTransitionCondition != "true")
      {
        cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
        cerr << "Using standard transition \"true\" instead." << endl << endl;
      }
    }
  }
}





%{
/*!
 * \fn void kc::P_joinCondition(string prefix, tTarget_list target_links, joinCondition jc)
 *
 * \brief   Pattern to model the join condition of an activity
 *
 * \param   prefix        the prefix of the activity's pattern
 * \param   target_links  the list of incoming links of the activity
 * \param   jc            the join condition
 */
%}
void P_joinCondition(string prefix, tTarget_list target_links, joinCondition jc)
{
  // Containers for the join transitions and the incoming links.
  map<unsigned int, Transition*> joinTransitions;
  map<unsigned int, Place*> trueLinks;
  map<unsigned int, Place*> falseLinks;
  unsigned int transitionNumber = (unsigned int)pow(2.0, target_links->length());

  Place *p5 = PN.findPlace(prefix + "joinFalse");
  Place *p6 = PN.findPlace(prefix + "joinTrue");

  assert(p5 != NULL);
  assert(p6 != NULL);


  // Fill containers with places modelling links set to true (trueLinks) and
  // false (falseLinks).
  unsigned int linkCount = 0;
  foreach (target; tTarget_list target_links)
  {
    Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
    Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);
    
    trueLinks[linkCount] = linkT;
    falseLinks[linkCount] = linkF;

    linkCount++;
  }


  // Add transitions and connect them with the links.
  for (unsigned int i = 0; i < transitionNumber; i++)
  {
    joinTransitions[i] = PN.newTransition(prefix + "join" + toString(i));

    for (unsigned int j = 0; j < linkCount; j++)
    {
      if (((i >> j) % 2) == 0) // don't ask me why this is working :-|
	PN.newArc(trueLinks[j], joinTransitions[i]);
      else
	PN.newArc(falseLinks[j], joinTransitions[i]);  
    }
  }

  /*
   * case 1: No join condition was specified. In this case, use the standard
   *         join condition. This join condition evaluates to true if at least
   *         one incoming link is set to true ("1 out of n", OR).
   */
  if (string(jc->op_name()) == "standardJoinCondition")
  {
    // Traverse the different cases.
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      // Only one transition makes the join fail. All others set the join
      // transition to true.
      if (i == transitionNumber-1)
	PN.newArc(joinTransitions[i], p5); // p5=joinFalse
      else
	PN.newArc(joinTransitions[i], p6); // p6=joinTrue
    }
  }

  /*
   * case 2: A join condition is specified. The results for the different cases
   *         have been calculated in the postprocessing and stored in the
   *         global variable "globals::joinCondition_result".
   */
  else
  {
    // Traverse the different cases.
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      // Connect transitions according to the stored result.
      if (globals::joinCondition_result[i])
	PN.newArc(joinTransitions[i], p5);
      else
	PN.newArc(joinTransitions[i], p6);
    }  
  }
}

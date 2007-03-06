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
 * \date    \$Date: 2007/03/06 12:45:58 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-shortcuts.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.4 $
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
#include "ast-details.h"
#include "options.h"
#include "globals.h"
#include "petrinet.h"

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
%}





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

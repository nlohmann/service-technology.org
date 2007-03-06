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
 * \file    petrinet-patterns-links.cc
 *
 * \brief   Petri net patterns for the link wrappers
 *
 *          This file implements the wrapper patterns necessary to model
 *          incoming and outgoing links of an activity:
 *           - link wrapper 0: no incoming or outgoing links
 *           - link wrapper 1: outgoing links
 *           - link wrapper 2: incoming links, suppressJoinFailure="yes"
 *           - link wrapper 3: incoming links, suppressJoinFailure="no"
 *           - link wrapper 4: incoming and outgoing links,
 *             suppressJoinFailure="yes"
 *           - link wrapper 5: incoming and outgoing links,
 *             suppressJoinFailure="no"
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2007-03-04
 *
 * \date    \$Date: 2007/03/06 10:36:49 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-links.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.9 $
 */
%}


%{ HEADER
/*!
 * \file petrinet-patterns-links.h
 * \brief Petri net patterns for the link wrappers
 *
 * See \ref petrinet-patterns-links.cc for more information.
 */
%}

%{
#include <cmath>
#include <set>
#include <map>
#include <string>

#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"
#include "globals.h"

using std::cerr;
using std::endl;
using std::set;
using std::string;
using std::map;

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
%}





/*****************************************************************************
 * WRAPPER PATTERN 0
 *****************************************************************************/

%{
/*!
 * \fn void kc::PLinkWrapper0(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief Wrapper Pattern 0
 *
 * Wrapper pattern for an activity that has neither incoming nor outgoing
 * links.
 *
 * \param abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param current_view     the current unparse view
 */
%}
void PLinkWrapper0(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 0: no links\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";



  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = NULL;
  Place *p4 = NULL;
  if (current_view != petrinetsmall_enum)
  {
    p3 = PN.newPlace(prefix + "stop");
    p4 = PN.newPlace(prefix + "stopped");
  }


  // Embed the inner activity.
  PN.mergePlaces(p1, newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");
  if (current_view != petrinetsmall_enum)
  {
    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 1
 *****************************************************************************/

%{
/*!
 * \fn      void kc::PLinkWrapper1(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 1
 *
 *          Wrapper pattern for an activity that has only outgoing links.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 */
%}
void PLinkWrapper1(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 1\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";



  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = NULL;
  Place *p4 = NULL;
  Place *p5 = PN.newPlace(prefix + "done");
  if (current_view != petrinetsmall_enum)
  {
    p3 = PN.newPlace(prefix + "stop");
    p4 = PN.newPlace(prefix + "stopped");
  }


  // The activity can be stopped at the final place.
  if (current_view != petrinetsmall_enum)
    Transition *t = stop(p2, "final", prefix);


  // Set the outgoing links according to the transition condition.
  unsigned int linkCounter = 1;
  if (parameters[P_XOR] && (phylum->tSource_list_1->length() > 1))
  {
    // Use XOR for the links.
    foreach (source; tSource_list phylum->tSource_list_1)
    {
      // Add a transition for each link.
      Transition *t1 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
      PN.newArc(p2, t1);
      PN.newArc(t1, p3);

      // Set one link to true.
      Place *p4 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t1, p4);

      foreach (source2; tSource_list phylum->tSource_list_1)
      {
        // Set the other links to false.
	if (ASTEmap[source2->id]->linkName != ASTEmap[source->id]->linkName)
	{
	  Place *p5 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	  PN.newArc(t1, p5);
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
  else
  {
    Transition *t1 = PN.newTransition(prefix + "setLinks");
    PN.newArc(p2, t1);
    PN.newArc(t1, p3);

    // Use standard transition condition (true).
    foreach (source; tSource_list phylum->tSource_list_1)
    {
      Place *p4 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t1, p4);

      // User-defined transition conditions will be ignored.
      string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
      if (givenTransitionCondition != "" && givenTransitionCondition != "true")
      {
        cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
        cerr << "Using standard transition \"true\" instead." << endl << endl;
      }
    }
  }


  // Embed the inner activity.
  PN.mergePlaces(p1, newPrefix + ".initial");
  PN.mergePlaces(p5, newPrefix + ".final");
  if (current_view != petrinetsmall_enum)
  {
    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 2
 *****************************************************************************/

%{
/*!
 * \fn      void kc::PLinkWrapper2(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 2
 *
 *          Wrapper pattern for an activity that has incoming links only, with
 *          attribute "suppressJoinFailure" set to "yes".
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 */
%}
void PLinkWrapper2(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 2\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";



  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = PN.newPlace(prefix + "joinTrue");
  Place *p4 = PN.newPlace(prefix + "joinFalse");
  Place *p5 = PN.newPlace(prefix + "begin");
  Place *p6 = NULL;
  Place *p7 = NULL;
  if (current_view != petrinetsmall_enum)
  {
    p6 = PN.newPlace(prefix + "stop");
    p7 = PN.newPlace(prefix + "stopped");
  }


  // Join condition evalates to true: start activity.
  Transition *t1 = PN.newTransition(prefix + "start");
  PN.newArc(p1, t1);
  PN.newArc(p3, t1);
  PN.newArc(t1, p5);

  // Join condition evalates to false: skip activity.
  Transition *t2 = PN.newTransition(prefix + "skip");
  PN.newArc(p1, t2);
  PN.newArc(p4, t2);
  PN.newArc(t2, p2);

  // When activity is skipped, sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);



  /*
   * pattern for the join condition
   */

  // containers for the join transitions and the incoming links
  map<unsigned int, Transition*> joinTransitions;
  map<unsigned int, Place*> trueLinks;
  map<unsigned int, Place*> falseLinks;
  unsigned int transitionNumber = (unsigned int)pow(2.0, phylum->tTarget_list_1->length());

  // fill link containers
  unsigned int linkCount = 0;
  foreach (target; tTarget_list phylum->tTarget_list_1)
  {
    Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
    Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

    trueLinks[linkCount] = linkT;
    falseLinks[linkCount] = linkF;

    linkCount++;
  }

  // add transitions and connect them with the links
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

  // case 1: standard join condition
  if (string(phylum->joinCondition_1->op_name()) == "standardJoinCondition")
  {
    // create OR-join
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      // only one transition makes the join fail
      if (i == transitionNumber-1)
        PN.newArc(joinTransitions[i], p4); // p4=joinFalse
      else
        PN.newArc(joinTransitions[i], p3); // p3=joinTrue
    }
  }
  else // case 2: user-defined join condition
  {
    // create pattern for join condition
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      if (globals::joinCondition_result[i])
        PN.newArc(joinTransitions[i], p3);
      else
        PN.newArc(joinTransitions[i], p4);
    }
  }



  // Embed the inner activity.
  PN.mergePlaces(p5, newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");
  if (current_view != petrinetsmall_enum)
  {
    PN.mergePlaces(p6, newPrefix + ".stop");
    PN.mergePlaces(p7, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 3
 *****************************************************************************/

%{
/*!
 * \fn      void kc::PLinkWrapper3(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 3
 *
 *          Wrapper pattern for an activity that has incoming links only, with
 *          attribute "suppressJoinFailure" set to "false".
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This pattern is not implemented for the mode "communication only".
 */
%}
void PLinkWrapper3(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 3\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";




  if (petrinetsmall_enum == current_view)
  {
    cerr << "no patterns allowed with suppressJoinFailure=\"no\"" << endl;
    assert(false);
    /* in fact, this should not be an error -- just a warning */
    return;
  }


  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = PN.newPlace(prefix + "joinTrue");
  Place *p4 = PN.newPlace(prefix + "joinFalse");
  Place *p5 = PN.newPlace(prefix + "begin");
  Place *p6 = PN.newPlace(prefix + "stop");
  Place *p7 = PN.newPlace(prefix + "stopped");
  Place *p8 = PN.newPlace(prefix + "faulted");

 // Join condition evalates to true: start activity.
   Transition *t1 = PN.newTransition(prefix + "start");
  PN.newArc(p1, t1);
  PN.newArc(p3, t1);
  PN.newArc(t1, p5);

  // Join condition evalates to false: skip activity and throw fault.
  Transition *t2 = PN.newTransition(prefix + "skip");
  PN.newArc(p1, t2);
  PN.newArc(p4, t2);
//  PN.newArc(t2, p2);
  PN.newArc(t2, p8);
  // the fault is not thrown!

  // The activity can be stopped as several places. (todo: check this)
  Transition *t3 = stop(p1, "initial", prefix);
  Transition *t4 = stop(p3, "joinTrue", prefix);
  Transition *t5 = stop(p4, "joinFalse", prefix);
  Transition *t6 = stop(p8, "faulted", prefix);

  // When activity is skipped, sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);



  /*
   * pattern for the join condition
   */

  // containers for the join transitions and the incoming links
  map<unsigned int, Transition*> joinTransitions;
  map<unsigned int, Place*> trueLinks;
  map<unsigned int, Place*> falseLinks;
  unsigned int transitionNumber = (unsigned int)pow(2.0, phylum->tTarget_list_1->length());

  // fill link containers
  unsigned int linkCount = 0;
  foreach (target; tTarget_list phylum->tTarget_list_1)
  {
    Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
    Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

    trueLinks[linkCount] = linkT;
    falseLinks[linkCount] = linkF;

    linkCount++;
  }

  // add transitions and connect them with the links
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

  // case 1: standard join condition
  if (string(phylum->joinCondition_1->op_name()) == "standardJoinCondition")
  {
    // create OR-join
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      // only one transition makes the join fail
      if (i == transitionNumber-1)
        PN.newArc(joinTransitions[i], p4); // p4=joinFalse
      else
        PN.newArc(joinTransitions[i], p3); // p3=joinTrue
    }
  }
  else // case 2: user-defined join condition
  {
    // create pattern for join condition
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      if (globals::joinCondition_result[i])
        PN.newArc(joinTransitions[i], p3);
      else
        PN.newArc(joinTransitions[i], p4);
    }
  }



  // Embed the inner activity.
  PN.mergePlaces(p5, newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");
  PN.mergePlaces(p6, newPrefix + ".stop");
  PN.mergePlaces(p7, newPrefix + ".stopped");
}






/*****************************************************************************
 * WRAPPER PATTERN 4
 *****************************************************************************/

%{
/*!
 * \fn      void kc::PLinkWrapper4(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 4
 *
 *          Wrapper pattern for an activity that has incoming and outgoing
 *          links, with attribute suppressJoinFailure" set to "yes".
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 */
%}
void PLinkWrapper4(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 4\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";



  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "begin");
  Place *p3 = PN.newPlace(prefix + "done");
  Place *p4 = PN.newPlace(prefix + "final");
  Place *p5 = PN.newPlace(prefix + "joinFalse");
  Place *p6 = PN.newPlace(prefix + "joinTrue");
  Place *p7 = NULL;
  Place *p8 = NULL;
  if (current_view != petrinetsmall_enum)
  {
    p7 = PN.newPlace(prefix + "stop");
    p8 = PN.newPlace(prefix + "stopped");
  }

  Transition *t1 = PN.newTransition(prefix + "start");
  PN.newArc(p1, t1);
  PN.newArc(p6, t1);
  PN.newArc(t1, p2);

  Transition *t2 = PN.newTransition(prefix + "skip");
  PN.newArc(p1, t2);
  PN.newArc(p5, t2);
  PN.newArc(t2, p4);

  // sets enclosed source links to false
  dpeLinks(t2, phylum->parentId);

  unsigned int linkCounter = 1;
  if (parameters[P_XOR] && (phylum->tSource_list_1->length() > 1))
  {
    // Set the outgoing links (XOR given).
    foreach (source; tSource_list phylum->tSource_list_1)
    {
      Transition *t3 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
      PN.newArc(p3, t3);
      PN.newArc(t3, p4);

      Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t3, p7);

      foreach (source2; tSource_list phylum->tSource_list_1)
      {
	if (source2 != source)
	{
	  Place *p8 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	  PN.newArc(t3, p8);
	}
      }

      string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
      if (givenTransitionCondition != "" && givenTransitionCondition != "true")
      {
	cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
	cerr << "Using XOR instead." << endl << endl;
      }
    }
  }
  else
  {
    Transition *t3 = PN.newTransition(prefix + "setLinks");
    PN.newArc(p3, t3);
    PN.newArc(t3, p4);
      
    // Set the outgoing links (standard transition condition expected).
    foreach (source; tSource_list phylum->tSource_list_1)
    {
      Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
      PN.newArc(t3, p7);
      
      string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
      if (givenTransitionCondition != "" && givenTransitionCondition != "true")
      {
	cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
	cerr << "Using standard transition \"true\" instead." << endl << endl;
      }
    }
  }

  /************************************************************************
   * pattern for the join condition
   ***********************************************************************/

  // containers for the join transitions and the incoming links
  map<unsigned int, Transition*> joinTransitions;
  map<unsigned int, Place*> trueLinks;
  map<unsigned int, Place*> falseLinks;
  unsigned int transitionNumber = (unsigned int)pow(2.0, phylum->tTarget_list_1->length());

  // fill link containers
  unsigned int linkCount = 0;
  foreach (target; tTarget_list phylum->tTarget_list_1)
  {
    Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
    Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

    trueLinks[linkCount] = linkT;
    falseLinks[linkCount] = linkF;

    linkCount++;
  }

  // add transitions and connect them with the links
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

  // case 1: standard join condition
  if (string(phylum->joinCondition_1->op_name()) == "standardJoinCondition")
  {
    // create OR-join
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      // only one transition makes the join fail
      if (i == transitionNumber-1)
	PN.newArc(joinTransitions[i], p5); // p5=joinFalse
      else
	PN.newArc(joinTransitions[i], p6); // p6=joinTrue
    }
  }
  else // case 2: user-defined join condition
  {
    // create pattern for join condition
    for (unsigned int i = 0; i < transitionNumber; i++)
    {
      if (globals::joinCondition_result[i])
	PN.newArc(joinTransitions[i], p5);
      else
	PN.newArc(joinTransitions[i], p6);
    }
  }


  // Embed the inner activity.
  PN.mergePlaces(p2, newPrefix + ".initial");
  PN.mergePlaces(p3, newPrefix + ".final"); 
  if (current_view != petrinetsmall_enum)
  {
    PN.mergePlaces(p7, newPrefix + ".stop");
    PN.mergePlaces(p8, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 5
 *****************************************************************************/

%{
/*!
 * \fn      void kc::PLinkWrapper5(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 5
 *
 *          Wrapper pattern for an activity that has incoming and outgoing
 *          links, with attribute suppressJoinFailure" set to "no".
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This pattern is not implemented for the mode "communication only".
 */
%}
void PLinkWrapper5(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 5\n");

  impl_standardElements_StandardElements* phylum = static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";


  if (petrinetsmall_enum == current_view)
  {
    cerr << "no patterns allowed with suppressJoinFailure=\"no\"" << endl;
    assert(false);
    /* in fact, this should not be an error -- just a warning */
    return;
  }



      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "begin");
      Place *p3 = PN.newPlace(prefix + "done");
      Place *p4 = PN.newPlace(prefix + "final");
      Place *p5 = PN.newPlace(prefix + "joinFalse");
      Place *p6 = PN.newPlace(prefix + "joinTrue");
      Place *p7 = PN.newPlace(prefix + "stop");
      Place *p8 = PN.newPlace(prefix + "stopped");
      Place *p9 = PN.newPlace(prefix + "faulted");

      Transition *t1 = PN.newTransition(prefix + "start");
      PN.newArc(p1, t1);
      PN.newArc(p6, t1);
      PN.newArc(t1, p2);

      Transition *t2 = PN.newTransition(prefix + "skip");
      PN.newArc(p1, t2);
      PN.newArc(p5, t2);
      PN.newArc(t2, p4);
      PN.newArc(t2, p9);

      Transition *t6 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p4, "final", prefix);
      Transition *t8 = stop(p5, "joinTrue", prefix);
      Transition *t9 = stop(p6, "joinFalse", prefix);
      Transition *t10 = stop(p9, "faulted", prefix);


      // sets enclosed source links to false
      dpeLinks(t2, phylum->parentId);

      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (phylum->tSource_list_1->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list phylum->tSource_list_1)
	{
  	  Transition *t3 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  PN.newArc(p3, t3);
      	  PN.newArc(t3, p4);

          Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
	  PN.newArc(t3, p7);

          foreach (source2; tSource_list phylum->tSource_list_1)
          {
            if (source2 != source)
            {
              Place *p8 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	      PN.newArc(t3, p8);
            }
          }

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using XOR instead." << endl << endl;
          }
        }
      }
      else
      {
  	Transition *t3 = PN.newTransition(prefix + "setLinks");
    	PN.newArc(p3, t3);
      	PN.newArc(t3, p4);
      
	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list phylum->tSource_list_1)
        {
	  Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
          PN.newArc(t3, p7);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }





      /************************************************************************
       * pattern for the join condition
       ***********************************************************************/

      // containers for the join transitions and the incoming links
      map<unsigned int, Transition*> joinTransitions;
      map<unsigned int, Place*> trueLinks;
      map<unsigned int, Place*> falseLinks;
      unsigned int transitionNumber = (unsigned int)pow(2.0, phylum->tTarget_list_1->length());

      // fill link containers
      unsigned int linkCount = 0;
      foreach (target; tTarget_list phylum->tTarget_list_1)
      {
        Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

        trueLinks[linkCount] = linkT;
        falseLinks[linkCount] = linkF;

        linkCount++;
      }

      // add transitions and connect them with the links
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

      // case 1: standard join condition
      if (string(phylum->joinCondition_1->op_name()) == "standardJoinCondition")
      {
        // create OR-join
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          // only one transition makes the join fail
          if (i == transitionNumber-1)
            PN.newArc(joinTransitions[i], p5); // p5=joinFalse
          else
            PN.newArc(joinTransitions[i], p6); // p6=joinTrue
        }
      }
      else // case 2: user-defined join condition
      {
        // create pattern for join condition
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          if (globals::joinCondition_result[i])
            PN.newArc(joinTransitions[i], p5);
          else
            PN.newArc(joinTransitions[i], p6);
        }
      }




      // Embed the inner activity.
      PN.mergePlaces(p2, newPrefix + ".initial");
      PN.mergePlaces(p3, newPrefix + ".final");
      PN.mergePlaces(p7, newPrefix + ".stop");
      PN.mergePlaces(p8, newPrefix + ".stopped");
}

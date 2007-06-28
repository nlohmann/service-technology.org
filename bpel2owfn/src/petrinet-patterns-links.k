/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
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
 * \date    \$Date: 2007/06/28 07:38:17 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-links.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.25 $
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
#include "petrinet-patterns-shortcuts.h"

using std::set;
using std::string;
using std::map;

extern PetriNet PN;
%}





/*****************************************************************************
 * WRAPPER PATTERN 0
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper0(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 0
 *
 *          Wrapper pattern for an activity that has neither incoming nor
 *          outgoing links.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void P_LinkWrapper0(impl_standardElements* abstract_phylum, uview current_view)
{
  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");

  // Embed the inner activity.
  PN.mergePlaces(p1, newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");

  // Add stop places.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");

    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 1
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper1(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 1
 *
 *          Wrapper pattern for an activity that has only outgoing links.
 *
 *          \image   html "wrapper1.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void P_LinkWrapper1(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 1\n");

  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);
  
  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p5 = PN.newPlace(prefix + "done");
  
  // Model the transition condition.
  P_transitionCondition(prefix, phylum->tSource_list_1);

  // Embed the inner activity.
  PN.mergePlaces(p1, newPrefix + ".initial");
  PN.mergePlaces(p5, newPrefix + ".final");

  // Add stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");

    Transition *t = stop(p2, "final", prefix);

    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 2
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper2(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 2
 *
 *          Wrapper pattern for an activity that has incoming links only, with
 *          attribute "suppressJoinFailure" set to "yes".
 *
 *          \image html "wrapper2.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void P_LinkWrapper2(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 2\n");

  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p5 = PN.newPlace(prefix + "jc_true");
  Place *p6 = PN.newPlace(prefix + "jc_false");
  Place *p7 = PN.newPlace(prefix + "begin");

  // Join condition evalates to true: start activity.
//NEW//  Transition *t1 = PN.newTransition(prefix + "start");
//NEW//  PN.newArc(p1, t1);
//NEW//  PN.newArc(p5, t1);
//NEW//  PN.newArc(t1, p7);
  PN.mergePlaces(p5, p7);

  // Join condition evalates to false: skip activity.
  Transition *t2 = PN.newTransition(prefix + "skip");
//NEW//  PN.newArc(p1, t2);
  PN.newArc(p6, t2);
  PN.newArc(t2, p2);

  // When activity is skipped, sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);

  // Model the join condition
  P_joinCondition(prefix, phylum->tTarget_list_1, phylum->joinCondition_1);

  // Embed the inner activity.
  PN.mergePlaces(prefix + "begin", newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");

  // Add stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");

    Transition *t = stop(p1, "initial", prefix);

    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 3
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper3(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 3
 *
 *          Wrapper pattern for an activity that has incoming links only, with
 *          attribute "suppressJoinFailure" set to "false".
 *
 *          \image html "wrapper3.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    In mode "communication only", wrapper pattern 2 is used instead.
 *
 * \ingroup patterns
 */
%}
void P_LinkWrapper3(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 3\n");

  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Display warning message if small patterns are used. Then use wrapper
  // pattern which models the case where join failures are suppressed.
  if (petrinetsmall_enum == current_view)
  {
    assert(globals::ASTEmap[phylum->parentId] != NULL);
    genericError(117, globals::ASTEmap[phylum->parentId]->activityTypeName(), globals::ASTEmap[phylum->parentId]->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    P_LinkWrapper2(abstract_phylum, current_view);
    return;
  }

  // Add new interface places (p5 is not modeled).
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = PN.newPlace(prefix + "stop");
  Place *p4 = PN.newPlace(prefix + "stopped");
  Place *p6 = PN.newPlace(prefix + "jc_true");
  Place *p7 = PN.newPlace(prefix + "jc_false");
  Place *p8 = PN.newPlace(prefix + "begin");
  Place *p9 = PN.newPlace(prefix + "faulted");

  // Join condition evalates to true: start activity.
//NEW//  Transition *t1 = PN.newTransition(prefix + "start");
//NEW//  PN.newArc(p1, t1);
//NEW//  PN.newArc(p6, t1);
//NEW//  PN.newArc(t1, p8);
  PN.mergePlaces(p6, p8);

  // Join condition evalates to false: skip activity and throw fault. The
  // transition is not labeled "skip", but "throwFault.joinFailure".
  Transition *t2 = throwFault(p7, p9, "joinFailure", prefix,
                              mkinteger(phylum->parentId),
                              globals::ASTEmap[phylum->parentId]->controlFlow);
//NEW//  PN.newArc(p1, t2);
  PN.newArc(p7, t2);
  PN.newArc(t2, p9);

  // When activity is skipped, sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);

  // The activity can be stopped as several places. (todo: check this)
  Transition *t3 = stop(p1, "initial", prefix);
  Transition *t4 = stop(p9, "faulted", prefix);

  // Model the join condition.
  P_joinCondition(prefix, phylum->tTarget_list_1, phylum->joinCondition_1);

  // Embed the inner activity.
  PN.mergePlaces(prefix + "begin", newPrefix + ".initial");
  PN.mergePlaces(p2, newPrefix + ".final");
  PN.mergePlaces(p3, newPrefix + ".stop");
  PN.mergePlaces(p4, newPrefix + ".stopped");
}






/*****************************************************************************
 * WRAPPER PATTERN 4
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper4(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 4
 *
 *          Wrapper pattern for an activity that has incoming and outgoing
 *          links, with attribute suppressJoinFailure" set to "yes".
 *
 *          \image html "wrapper4.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 *
 * \todo    Check this pattern.
 */
%}
void P_LinkWrapper4(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 4\n");

  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p5 = PN.newPlace(prefix + "jc_true");
  Place *p6 = PN.newPlace(prefix + "jc_false");
  Place *p7 = PN.newPlace(prefix + "begin");
  Place *p8 = PN.newPlace(prefix + "done");

  // Join condition evalates to true: start activity.
//NEW//  Transition *t1 = PN.newTransition(prefix + "start");
//NEW//  PN.newArc(p1, t1);
//NEW//  PN.newArc(p5, t1);
//NEW//  PN.newArc(t1, p7);
  PN.mergePlaces(p5, p7);

  // Join condition evalates to false: skip activity.
  Transition *t2 = PN.newTransition(prefix + "skip");
//NEW//  PN.newArc(p1, t2);
  PN.newArc(p6, t2);
  PN.newArc(t2, p2);

  // When activity is skipped, sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);

  // Model the transition condition.
  P_transitionCondition(prefix, phylum->tSource_list_1);

  // Model the join condition.
  P_joinCondition(prefix, phylum->tTarget_list_1, phylum->joinCondition_1);

  // Embed the inner activity.
  PN.mergePlaces(prefix + "begin", newPrefix + ".initial");
  PN.mergePlaces(p8, newPrefix + ".final"); 

  // Add stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");

    Transition *t3 = stop(p1, "initial", prefix);
    Transition *t4 = stop(p2, "final", prefix);

    PN.mergePlaces(p3, newPrefix + ".stop");
    PN.mergePlaces(p4, newPrefix + ".stopped");
  }
}





/*****************************************************************************
 * WRAPPER PATTERN 5
 *****************************************************************************/

%{
/*!
 * \fn      void kc::P_LinkWrapper5(impl_standardElements* abstract_phylum, uview current_view)
 *
 * \brief   Wrapper Pattern 5
 *
 *          Wrapper pattern for an activity that has incoming and outgoing
 *          links, with attribute suppressJoinFailure" set to "no".
 *
 *          \image html "wrapper5.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    In mode "communication only", wrapper pattern 4 is used instead.
 *
 * \ingroup patterns
 *
 * \todo    Check this pattern.
 */
%}
void P_LinkWrapper5(impl_standardElements* abstract_phylum, uview current_view)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "  wrapper-pattern 5\n");

  impl_standardElements_StandardElements* phylum =
    static_cast<impl_standardElements_StandardElements*>(abstract_phylum);
  assert(phylum != NULL);

  // Organize prefixes.
  string prefix = toString(phylum->parentId) + ".";
  string newPrefix = toString(phylum->parentId) + ".internal";

  // Display warning message if small patterns are used. Then use wrapper
  // pattern which models the case where join failures are suppressed.
  if (petrinetsmall_enum == current_view)
  {
    assert(globals::ASTEmap[phylum->parentId] != NULL);
    genericError(117, globals::ASTEmap[phylum->parentId]->activityTypeName(), globals::ASTEmap[phylum->parentId]->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    P_LinkWrapper4(abstract_phylum, current_view);
    return;
  }

  // Add new interface places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");
  Place *p3 = PN.newPlace(prefix + "stop");
  Place *p4 = PN.newPlace(prefix + "stopped");
  Place *p6 = PN.newPlace(prefix + "jc_true");
  Place *p7 = PN.newPlace(prefix + "jc_false");
  Place *p8 = PN.newPlace(prefix + "begin");
  Place *p9 = PN.newPlace(prefix + "done");
  Place *p10 = PN.newPlace(prefix + "faulted");

  // Join condition evalates to true: start activity.
//NEW//  Transition *t1 = PN.newTransition(prefix + "start");
//NEW//  PN.newArc(p1, t1);
//NEW//  PN.newArc(p6, t1);
//NEW//  PN.newArc(t1, p8);
  PN.mergePlaces(p6, p8);

  // Join condition evalates to false: skip activity and throw fault. The
  // transition is not labeled "skip", but "throwFault.joinFailure".
  Transition *t2 = throwFault(p7, p10, "joinFailure", prefix,
                              mkinteger(phylum->parentId),
                              globals::ASTEmap[phylum->parentId]->controlFlow);
//NEW//  PN.newArc(p1, t2);
  PN.newArc(p7, t2);
  PN.newArc(t2, p10);

  // Sets enclosed source links to false.
  dpeLinks(t2, phylum->parentId);

  // The activity can be stopped as several places. (todo: check this)
  Transition *t3 = stop(p1, "initial", prefix);
  Transition *t4 = stop(p2, "final", prefix);
  Transition *t5 = stop(p10, "faulted", prefix);

  // Model the transition condition.
  P_transitionCondition(prefix, phylum->tSource_list_1);

  // Model the join condition.
  P_joinCondition(prefix, phylum->tTarget_list_1, phylum->joinCondition_1);

  // Embed the inner activity.
  PN.mergePlaces(prefix + "begin", newPrefix + ".initial");
  PN.mergePlaces(p9, newPrefix + ".final");
  PN.mergePlaces(p3, newPrefix + ".stop");
  PN.mergePlaces(p4, newPrefix + ".stopped");
}

/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
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

/*!
 * \file    ast-tools.cc
 *
 * \brief   unparse helper tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2006/12/30 00:52:23 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.52 $
 *
 * \ingroup debug
 * \ingroup creation
 */




#include <cmath>
#include <cassert>

#include "options.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"

using namespace std;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// number of spaces to be added as indention
unsigned int indent = 0;

/// number of spaces to be added when indention is increased
unsigned int indentStep = 4;





/******************************************************************************
 * Functions for the Petri net unparsers
 *****************************************************************************/

/*!
 * \brief generates transition and places to throw a fault
 *
 * This functions generates a subnet to model the throwing of a fault. It
 * models faults in any control flow of the BPEL process as well as in any
 * patterns used.
 *
 * \param p1  the place in positive control flow from which the control flow
 *            enters the negative control flow
 *
 * \param p2  the place of the pattern on which a token shall be produced which
 *            can only be removed by stopping the pattern
 *
 * \param p1name  name of place p1 which is used to label the generated
 *                transitions (e.g. if p1 is called "running" generated
 *                transitions end with this name (e.g. "throwFault.running")
 *
 * \param prefix  prefix of the pattern to label generated places and
 *                transitions
 *
 * \param id  identifier of the caller activity
 *
 * \param negativeControlFlow  signals where the activity is located:
 *                             - 0: inside a scope or the process
 *                             - 1: inside a fault handler
 *                             - 2: inside a compensation handler
 *
 * \param preventFurtherFaults  controls what happens to further faults
 *                               - true: these faults are prevented (standard)
 *                               - false: these faults are suppressed
 *
 * \return a pointer to the (first) generated fault transition
 *
 * \ingroup creation
*/
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, kc::integer id,
    int negativeControlFlow, bool preventFurtherFaults)
{
  extern string currentScope;	// introduced in bpel-unparse.k
  extern PetriNet PN;		// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

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
  if (parameters[P_WSBPEL])
  {
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


  // fault handling for the `old' patterns
  switch (negativeControlFlow)
  {
    case(0): // activity in scope or process
      {
	Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
	PN.newArc(PN.findPlace(currentScope + "Active"), t1);
	PN.newArc(t1, PN.findPlace(currentScope + "!Active"));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace(currentScope + "stop.fault_in"));

	if (!preventFurtherFaults)
	{
	  Transition *t2 = PN.newTransition(prefix + "ignoreFault." + p1name);
	  PN.newArc(PN.findPlace(currentScope + "!Active"), t2, READ);
	  PN.newArc(p1, t2);
	  PN.newArc(t2, p2);
	}
	return t1;
      }

    case(1): // activity in fault handler
      {
	// No transition is added if the parameter "nofhfaults" is set, since
	// then it is not allowed for activities in the fault handler to throw
	// fault.s
	if (!parameters[P_FHFAULTS])
	  return NULL;

	Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
	PN.newArc(PN.findPlace(currentScope + "!FHFaulted"), t1);
	PN.newArc(t1, PN.findPlace(currentScope + "FHFaulted"));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace(currentScope + "stop.fh_fault_in"));

	if (!preventFurtherFaults)
	{
	  Transition *t2 = PN.newTransition(prefix + "ignoreFault." + p1name);
	  PN.newArc(PN.findPlace(currentScope + "FHFaulted"), t2, READ);
	  PN.newArc(p1, t2);
	  PN.newArc(t2, p2);
	}
	return t1;
      }

    case(2): // activity in compensation handler
      {
       	Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
    	PN.newArc(PN.findPlace(currentScope + "!CHFaulted"), t1);
      	PN.newArc(t1, PN.findPlace(currentScope + "CHFaulted"));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace(currentScope + "stop.ch_fault_in"));

	if (!preventFurtherFaults)
	{
	  Transition *t2 = PN.newTransition(prefix + "ignoreFault." + p1name);
	  PN.newArc(PN.findPlace(currentScope + "CHFaulted"), t2, READ);
	  PN.newArc(p1, t2);
	  PN.newArc(t2, p2);
	}
	return t1;
      }

    default:
      {
	return NULL; /* should never happen */
      }
  }
}





/*!
 * \brief generates a subnet to stop
 *
 * Generates a transition and places to stop the activity, i.e. a
 * transition moving a token on the "stop" place to "stopped".
 *
 * \param p  the place in control flow from which the token is move to
 *           "stop"
 *
 * \param p_name  name of place p which is used to label the generated
 *                transition (e.g. if p1 is called "running" generated
 *                transition end with this name (e.g. "stop.running")
 *
 * \param prefix  prefix of the pattern to label generated transition
 *
 * \return a pointer to the stop transition
 *
 * \ingroup creation
 */
Transition *stop(Place *p, string p_name, string prefix)
{
  extern PetriNet PN;	// introduced in main.c

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
 * \brief   returns a string used for identation.
 * \ingroup debug
 */
string inString()
{
  string result = "";

  for(unsigned int i=0; i<indent; i++)
    result += " ";

  return result;
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (opening) tag name of the
 * activity of the given id. If myindent is set to true, the enclosed
 * elements are indented.
 *
 * \see #footer
 *
 * \ingroup debug
*/
void header(int id, bool myindent)
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  trace(TRACE_DEBUG, "[PNU]" + inString() + "<" + ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");

  if (myindent)
    indent += indentStep;
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see header(int id, bool myindent)
 * \ingroup debug
*/
void header(kc::integer id, bool myindent)
{
  header(id->value, myindent);
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (closing) tag name of the
 * activity of the given id. If myindent is set to true, the indentation is
 * reduced.
 *
 * \see #header

 * \ingroup debug
 */
void footer(int id, bool myindent)
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  if (myindent)
    indent -= indentStep;

  trace(TRACE_DEBUG, "[PNU]" + inString() + "</" + ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see footer(int id, bool myindent)
 * \ingroup debug
 */
void footer(kc::integer id, bool myindent)
{
  footer(id->value, myindent);
}





/*!
 * \brief add a subnet for dead-path elimination
 *
 * Creates arcs to set links on dead paths to false.
 *
 * \param t	transition that invokes DPE
 * \param id	the identifier of the AST node
 *
 * \ingroup creation
 */
void dpeLinks(Transition *t, int id)
{
  ENTER("[ASTT]");

  extern PetriNet PN;	// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

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





/*!
 * \param loop_bounds	    a vector holding the maximal indices of the ancestor loops
 * \param loop_identifiers  a vector holding the identifiers of the ancestor loops
 * \param prefix            the prefix of the calling <scope>
 * \param my_max	    the maximal index of the direct parent loop
 */
void process_loop_bounds(const vector<unsigned int> &loop_bounds, const vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max)
{
  extern PetriNet PN;	// introduced in main.c

  vector<unsigned int> current_index(loop_bounds.size(), 1);
  unsigned int number = 1;

  // count the possible permutations of indices
  for (unsigned int i = 0; i < loop_bounds.size(); i++)
    number *= loop_bounds[i];

  // create transitions, places and arcs
  for (unsigned int i = 1; i <= number; i++)
  {
    Place *p1 = PN.newPlace(prefix + "c" + toString(current_index));
    Place *p2 = PN.newPlace(prefix + "!c" + toString(current_index));
    p2->mark(my_max);

    Transition *t14 = PN.newTransition(prefix + "t14." + toString(i));
    PN.newArc(PN.findPlace(prefix + "final1"), t14);
    PN.newArc(PN.findPlace(prefix + "!Successful"), t14, READ);
    PN.newArc(t14, PN.findPlace(prefix + "final"));

    Transition *t15 = PN.newTransition(prefix + "t15." + toString(i));
    PN.newArc(t15, p1);
    PN.newArc(p2, t15);
    PN.newArc(PN.findPlace(prefix + "final1"), t15);
    PN.newArc(PN.findPlace(prefix + "Successful"), t15);
    PN.newArc(t15, PN.findPlace(prefix + "!Successful"));
    PN.newArc(t15, PN.findPlace(prefix + "final"));

    Transition *t16 = PN.newTransition(prefix + "t16." + toString(i));
    PN.newArc(p2, t16, READ, my_max);
    PN.newArc(PN.findPlace(prefix + "compensated1"), t16);
    PN.newArc(t16, PN.findPlace(prefix + "compensated"));

    Transition *t17 = PN.newTransition(prefix + "t17." + toString(i));
    PN.newArc(p1, t17);
    PN.newArc(t17, p2);
    PN.newArc(PN.findPlace(prefix + "compensate"), t17);
    PN.newArc(t17, PN.findPlace(prefix + "ch_initial"));

    PN.mergePlaces(PN.findPlace(prefix + "compensated1"), PN.findPlace(prefix + "compensate"));

    // connect transitions with counters of ancestor loops
    for (unsigned i = 0; i < loop_identifiers.size(); i++)
    {
      Place *p3 = PN.findPlace(toString(loop_identifiers[i]) + ".internal.count." + toString(current_index[i]));
      assert(p3 != NULL);

      PN.newArc(p3, t15, READ);
      PN.newArc(p3, t16, READ);
      PN.newArc(p3, t17, READ);
    }

    // generate next index
    next_index(current_index, loop_bounds);
  }
}





/******************************************************************************
 * ACTIVITY RELATIONS
 *****************************************************************************/

namespace {
    map< pair< unsigned int, unsigned int >, activityRelationType > activityRelationMap;
}

activityRelationType activityRelation(unsigned int a, unsigned int b)
{
    ENTER("activityRelation");
    return activityRelationMap[pair<unsigned int,unsigned int>(a,b)];
    LEAVE("activityRelation");
}

void conflictingActivities(unsigned int a, unsigned int b)
{
    ENTER("conflictingActivities");
    // cerr << " # conflicting activities " << a << " <-> " << b << endl;
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_CONFLICT;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_CONFLICT;
    LEAVE("conflictingActivities");
}

void enterConflictingActivities( set< unsigned int > a, set< unsigned int > b )
{
  ENTER("enterConflictingActivities");
  for ( set< unsigned int >::iterator id1 = a.begin();
        id1 != a.end();
        id1++ )
  {
    for ( set< unsigned int >::iterator id2 = b.begin();
          id2 != b.end();
          id2++ )
    {
      conflictingActivities( *id1, *id2 );
    }
  }
  LEAVE("enterConflictingActivities");
}

void enclosedActivities( unsigned int a, unsigned int b )
{
    ENTER("enclosedActivities");
    // cerr << " # enclosed activities " << a << " <-> " << b << endl;
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_ENCLOSES;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_DESCENDS;
    LEAVE("enclosedActivities");
}

void enterEnclosedActivities( unsigned int a, set< unsigned int > b )
{
  ENTER("enterEnclosedActivities");
  for ( set< unsigned int >::iterator id = b.begin();
        id != b.end();
        id++ )
  {
    enclosedActivities( a, *id );
  }
  LEAVE("enterEnclosedActivities");
}


/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

/*!
 * \brief adds whitespace to indent output
 * \see #inup #indown
 * \ingroup debug
 */
void in()
{
  for(unsigned int i=0; i<indent; i++)
    *output << " ";
}





/*!
 * \brief increases indention#
 * \see #indown #in
 * \ingroup debug
 */
void inup()
{
  in();
  indent += indentStep;
}





/*!
 * \brief decreases indention
 * \see #inup #in
 * \ingroup debug
 */
void indown()
{
  indent -= indentStep;
  in();
}





/*!
 * \todo comment me
 */
void listAttributes ( unsigned int id )
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  string result = "";
  for (map< string, string >::iterator attribute = ASTEmap[ id ]->attributes.begin(); attribute != ASTEmap[ id ]->attributes.end(); attribute++ )
  {
    if ( attribute->second != "" &&
         attribute->first != "referenceLine" &&
         !( (attribute->first == "exitOnStandardFaults" ||
             attribute->first == "createInstance" ||
             attribute->first == "initializePartnerRole" ||
             attribute->first == "initiate" ||
             attribute->first == "isolated" ||
             attribute->first == "surpressJoinFailure" ||
             attribute->first == "validate")
            && attribute->second == "no" )
      )
    {
      result += " " + attribute->first + "=\""+ attribute->second +"\"";
    }
  }
  *output << result;
}

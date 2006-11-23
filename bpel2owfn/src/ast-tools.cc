/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file    ast-tools.cc
 *
 * \brief   unparse helper tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2006/11/23 14:33:39 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.36 $
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





/******************************************************************************
 * External variables
 *****************************************************************************/

// introduced in main.c
extern PetriNet *TheNet;

// introduced in bpel-unparse.k
extern string currentScope;

// introduced in bpel-unparse-tools.h
extern map<unsigned int, ASTE*> ASTEmap;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// number of spaces to be added as indention
unsigned int indent = 0;

/// number of spaces to be added when indention is increased
unsigned int indentStep = 4;





/******************************************************************************
 * Functions for the Petri net unparser defined in bpel-unparse-petri.k
 *****************************************************************************/

/*!
 * \brief generates transition and places to throw a fault
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
 * \post case 0 (preventFurtherFaults = false):
 * \dot
   digraph D
   {
     node [shape=circle fixedsize]
     p1
     p2
     A1 [label="Active"]
     A2 [label="!Active"]
     fault_in
     node [shape=box regular=true fixedsize]
     t1 [label="t1\nthrowFault.p1name"]
     t2 [label="t1\nignoreFault.p1name"]
     
     p1 -> t1
     t1 -> p2
     A1 -> t1
     t1 -> A2
     t1 -> fault_in

     p1 -> t2
     t2 -> p2
     A2 -> t2
     t2 -> A2
   }
   \enddot
 * \post case 0 (preventFurtherFaults = true):
 * \dot
   digraph D
   {
     node [shape=circle fixedsize]
     p1
     p2
     A1 [label="Active"]
     A2 [label="!Active"]
     fault_in
     node [shape=box regular=true fixedsize]
     t1 [label="t1\nthrowFault.p1name"]
     
     p1 -> t1
     t1 -> p2
     A1 -> t1
     t1 -> A2
     t1 -> fault_in
   }
   \enddot
 * 
 * \todo set scope to "Faulted" in case of "exitOnStandardFault"
 *
 * \ingroup creation
*/
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, kc::integer id,
    int negativeControlFlow, bool preventFurtherFaults)
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
    Transition *t1 = TheNet->newTransition(prefix + "exitOnStandardFault." + p1name);
    TheNet->newArc(p1, t1);
    TheNet->newArc(t1, p2);
    TheNet->newArc(t1, TheNet->findPlace("1.internal.exit"));

    return t1;
  }

  if (parameters[P_NEW])
  {
    switch (negativeControlFlow)
    {
      case(0): // activity in scope or process
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = TheNet->newTransition(prefix + "throwFault." + p1name);
	  TheNet->newArc(p1, t1);
	  TheNet->newArc(t1, p2);
	  TheNet->newArc(t1, TheNet->findPlace(toString(parentId) + ".internal.fault_in"));

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
	    t1 = TheNet->newTransition(prefix + "rethrow");
	  else
	    t1 = TheNet->newTransition(prefix + "rethrowFault." + p1name);

	  TheNet->newArc(p1, t1);
	  TheNet->newArc(t1, p2);

	  if (ASTEmap[id->value]->parentScopeId == 1)
	    TheNet->newArc(t1, TheNet->findPlace(toString(parentId) + ".internal.exit"));
	  else
	    TheNet->newArc(t1, TheNet->findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(2): // activity in compensation handler
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

          // parent scope of a compensation handler shouldn't be the process :)
	  assert(parentId != 1);

	  Transition *t1 = TheNet->newTransition(prefix + "throwCHFault." + p1name);
	  TheNet->newArc(p1, t1);
	  TheNet->newArc(t1, p2);
	  TheNet->newArc(t1, TheNet->findPlace(toString(parentId) + ".internal.ch_fault_in"));

	  return t1;
	}

      case(3): // activity in termination handler
        {
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = TheNet->newTransition(prefix + "signalFault." + p1name);
	  TheNet->newArc(p1, t1);
	  TheNet->newArc(t1, p2);
	  TheNet->newArc(t1, TheNet->findPlace(toString(parentId) + ".internal.terminationHandler.inner_fault"));
        }
    }
    return NULL;
  }



  switch (negativeControlFlow)
  {
    case(0): // activity in scope or process
    {
      Transition *t1 = TheNet->newTransition(prefix + "throwFault." + p1name);
      TheNet->newArc(TheNet->findPlace(currentScope + "Active"), t1);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "!Active"));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "stop.fault_in"));

      if (!preventFurtherFaults)
      {
	Transition *t2 = TheNet->newTransition(prefix + "ignoreFault." + p1name);
	TheNet->newArc(TheNet->findPlace(currentScope + "!Active"), t2, READ);
	TheNet->newArc(p1, t2);
	TheNet->newArc(t2, p2);
      }
      return t1;
    }

    case(1): // activity in fault handler
    {
      // No transition is added if the parameter "nofhfaults" is set, since
      // then it is not allowed for activities in the fault handler to throw
      // fault.s
      if (parameters[P_NOFHFAULTS])
	return NULL;

      Transition *t1 = TheNet->newTransition(prefix + "throwFault." + p1name);
      TheNet->newArc(TheNet->findPlace(currentScope + "!FHFaulted"), t1);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "FHFaulted"));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "stop.fh_fault_in"));

      if (!preventFurtherFaults)
      {
	Transition *t2 = TheNet->newTransition(prefix + "ignoreFault." + p1name);
	TheNet->newArc(TheNet->findPlace(currentScope + "FHFaulted"), t2, READ);
	TheNet->newArc(p1, t2);
	TheNet->newArc(t2, p2);
      }
      return t1;
    }

    case(2): // activity in compensation handler
    {
      Transition *t1 = TheNet->newTransition(prefix + "throwFault." + p1name);
      TheNet->newArc(TheNet->findPlace(currentScope + "!CHFaulted"), t1);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "CHFaulted"));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, TheNet->findPlace(currentScope + "stop.ch_fault_in"));

      if (!preventFurtherFaults)
      {
	Transition *t2 = TheNet->newTransition(prefix + "ignoreFault." + p1name);
	TheNet->newArc(TheNet->findPlace(currentScope + "CHFaulted"), t2, READ);
	TheNet->newArc(p1, t2);
	TheNet->newArc(t2, p2);
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
 * \post Generated Petri net
 * \dot
   digraph D
   {
     node [shape=circle fixedsize]
     p [label="p_name"]
     stop
     stopped

     node [shape=box regular=true fixedsize]
     t [label="stoppedAt.p_name"]
     
     stop -> t
     p -> t
     t -> stopped
   }
   \enddot
 *
 * \ingroup creation
 */
Transition *stop(Place *p, string p_name, string prefix)
{
  assert(p != NULL);

  // no stop transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
  Transition *stopTransition = TheNet->newTransition(prefix + "stopped." + p_name);
  TheNet->newArc(TheNet->findPlace(prefix + "stop"), stopTransition);
  TheNet->newArc(stopTransition, TheNet->findPlace(prefix + "stopped"));
  TheNet->newArc(p, stopTransition);

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
 * \post
 * \dot
   digraph D
   {
     node [shape=circle fixedsize]
     link1 [label="!link1"]
     link2 [label="!link2"]

     node [shape=box regular=true fixedsize]
     t
     
     t -> link1
     t -> link2
   }
   \enddot

 * \ingroup creation
 */
void dpeLinks(Transition *t, int id)
{
  ENTER("[ASTT]");

  assert(t != NULL);
  cerr << "id: " << id << endl;
  assert(ASTEmap[id] != NULL);

  for (set<int>::iterator linkID = ASTEmap[id]->enclosedSourceLinks.begin();
      linkID != ASTEmap[id]->enclosedSourceLinks.end();
      linkID++)
  {
    assert(ASTEmap[*linkID] != NULL);
    string linkName = ASTEmap[*linkID]->attributes["name"];
    TheNet->newArc(t, TheNet->findPlace("!link." + linkName));
  }

  LEAVE("[ASTT]");
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


void listAttributes ( unsigned int id )
{
  std::string result = "";
  for (map< std::string, std::string >::iterator attribute = ASTEmap[ id ]->attributes.begin(); attribute != ASTEmap[ id ]->attributes.end(); attribute++ )
  {
    if ( attribute->second != "" )
    {
      result += " " + attribute->first + "=\""+ attribute->second +"\"";
    }
  }
  *output << result;
}


void next_index(vector<unsigned int> &current_index, vector<unsigned int> &max_index)
{
  assert(current_index.size() == max_index.size());

  for (unsigned int i = 0; i < current_index.size(); i++)
  {
    if (current_index[i] < max_index[i])
    {
      current_index[i]++;
      break;
    }
    else
      current_index[i] = 1;
  }
}





/*!
 * converts a vector to a C++ string
 */
string print_vector(vector<unsigned int> &v)
{
  string result;

  for (unsigned int i = 0; i < v.size(); i++)
  {
    if (i != 0)
      result += ".";

    result += toString(v[i]);
  }

  return result;
}





/*!
 * \param loop_bounds	a vector holding the maximal indices of the ancestor
 *                      loops
 * \param loop_bounds	a vector holding the identifiers of the ancestor loops
 * \param prefix	the prefix of the calling <scope>
 * \param my_max	the maximal index of the direct parent loop
 */
void process_loop_bounds(vector<unsigned int> &loop_bounds, vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max)
{
  vector<unsigned int> current_index(loop_bounds.size(), 1);
  unsigned int number = 1;

  // count the possible permutations of indices
  for (unsigned int i = 0; i < loop_bounds.size(); i++)
    number *= loop_bounds[i];

  // create transitions, places and arcs
  for (unsigned int i = 1; i <= number; i++)
  {
    Place *p1 = TheNet->newPlace(prefix + "c" + print_vector(current_index));
    Place *p2 = TheNet->newPlace(prefix + "!c" + print_vector(current_index));
    p2->mark(my_max);

    Transition *t14 = TheNet->newTransition(prefix + "t14." + toString(i));
    TheNet->newArc(TheNet->findPlace(prefix + "final1"), t14);
    TheNet->newArc(TheNet->findPlace(prefix + "!Successful"), t14, READ);
    TheNet->newArc(t14, TheNet->findPlace(prefix + "final"));

    Transition *t15 = TheNet->newTransition(prefix + "t15." + toString(i));
    TheNet->newArc(t15, p1);
    TheNet->newArc(p2, t15);
    TheNet->newArc(TheNet->findPlace(prefix + "final1"), t15);
    TheNet->newArc(TheNet->findPlace(prefix + "Successful"), t15);
    TheNet->newArc(t15, TheNet->findPlace(prefix + "!Successful"));
    TheNet->newArc(t15, TheNet->findPlace(prefix + "final"));

    Transition *t16 = TheNet->newTransition(prefix + "t16." + toString(i));
    TheNet->newArc(p2, t16, READ, my_max);
    TheNet->newArc(TheNet->findPlace(prefix + "compensated1"), t16);
    TheNet->newArc(t16, TheNet->findPlace(prefix + "compensated"));

    Transition *t17 = TheNet->newTransition(prefix + "t17." + toString(i));
    TheNet->newArc(p1, t17);
    TheNet->newArc(t17, p2);
    TheNet->newArc(TheNet->findPlace(prefix + "compensate"), t17);
    TheNet->newArc(t17, TheNet->findPlace(prefix + "ch_initial"));

    TheNet->mergePlaces(TheNet->findPlace(prefix + "compensated1"), TheNet->findPlace(prefix + "compensate"));

    // connect transitions with counters of ancestor loops
    for (unsigned i = 0; i < loop_identifiers.size(); i++)
    {
      Place *p3 = TheNet->findPlace(toString(loop_identifiers[i]) + ".internal.count." + toString(current_index[i]));
      assert(p3 != NULL);

      TheNet->newArc(p3, t15, READ);
      TheNet->newArc(p3, t16, READ);
      TheNet->newArc(p3, t17, READ);
    }

    // generate next index
    next_index(current_index, loop_bounds);
  }
}



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
 * \file bpel-unparse-tools.cc
 *
 * \brief Unparse helper tools (implementation)
 *
 * This file implements several helpe functions used during the unparsing
 * of the abstract syntax tree.
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2006/02/08
 *          - last changed: \$Date: 2006/07/11 20:47:51 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.10 $
 */




#include <cmath>
#include "bpel-unparse-tools.h"
#include "options.h"
#include "helpers.h"
#include <assert.h>
#include "ast-details.h"





/******************************************************************************
 * External variables
 *****************************************************************************/

// introduced in bpel-syntax.y
//extern SymbolTable symTab;

// introduced in main.c
extern PetriNet *TheNet;

// introduced in bpel-unparse.k
extern string currentScope;





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

/**
 * Generates transition and places to throw a fault.
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
*/
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix,
    int negativeControlFlow, bool preventFurtherFaults)
{
  assert(p1 != NULL);
  assert(p2 != NULL);

  // no fault transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
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





/**
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
 */
Transition *stop(Place *p, string p_name, string prefix)
{
  assert(p != NULL);

  // no stop transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
  Transition *stopTransition = TheNet->newTransition(prefix + "stoppedAt." + p_name);
  TheNet->newArc(TheNet->findPlace(prefix + "stop"), stopTransition);
  TheNet->newArc(stopTransition, TheNet->findPlace(prefix + "stopped"));
  TheNet->newArc(p, stopTransition);

  return stopTransition;
}





/**
 * Returns a string used for identation.
 */
string inString()
{
  string result = "";

  for(unsigned int i=0; i<indent; i++)
    result += " ";

  return result;
}





/**
 * Prints a debug trace message containing the (opening) tag name of the
 * activity of the given id. If myindent is set to true, the enclosed
 * elements are indented.
 */
void header(int id, bool myindent)
{
//  trace(TRACE_DEBUG, "[PNU]" + inString() + symTab.getInformation(id) + "\n");
  trace(TRACE_DEBUG, "[PNU]" + inString() + "<id=" + intToString(id) + ">\n");

  if (myindent)
    indent += indentStep;
}





/**
 * Prints a debug trace message containing the (closing) tag name of the
 * activity of the given id. If myindent is set to true, the indentation is
 * reduced.
 */
void footer(int id, bool myindent)
{
  if (myindent)
    indent -= indentStep;

//  trace(TRACE_DEBUG, "[PNU]" + inString() + symTab.getInformation(id, true) + "\n");
  trace(TRACE_DEBUG, "[PNU]" + inString() + "</id=" + intToString(id) + ">\n");
}




/**
 * Creates arcs to set links on dead paths to false.
 */
void dpeLinks(Transition *t, int id)
{
  extern map<unsigned int, ASTE*> ASTEmap;  

  assert(t != NULL);

//  STActivity *branch = dynamic_cast<STActivity*>(symTab.lookup(id));
//  assert(branch != NULL);
  
//  for (set<STLink*>::iterator link = branch->enclosedSourceLinks.begin();
//      link != branch->enclosedSourceLinks.end();
//      link++)
//
  assert(ASTEmap[id] != NULL);

  // TODO Overwork and TEST this function -- don't have a good feeling about it...

  for (list<int>::iterator linkID = ASTEmap[id]->enclosedSourceLinks.begin();
      linkID != ASTEmap[id]->enclosedSourceLinks.end();
      linkID++)
  {
    assert(ASTEmap[*linkID] != NULL);
    string linkName = ASTEmap[*linkID]->attributes["name"];
    TheNet->newArc(t, TheNet->findPlace("!link." + linkName));
  }
}




/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

/// Adds whitespace to indent output
void in()
{
  for(unsigned int i=0; i<indent; i++)
    *output << " ";
}





/// Increases indention
void inup()
{
  in();
  indent += indentStep;
}





/// Decreases indention
void indown()
{
  indent -= indentStep;
  in();
}

/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or(at your    *
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
\*****************************************************************************/

/*!
 * \file petrinet-output.cc
 *
 * \brief Output Functions for Petri nets (implementation)
 * 
 * This file implements the classes and functions defined in petrinet.h.
 *   - Transition::dotOut()
 *   - Place::dotOut()
 *   - Arc::dotOut()
 *   - PetriNet::information()
 *   - PetriNet::printInformation()
 *   - PetriNet::dotOut()
 *   - PetriNet::pnmlOut()
 *   - PetriNet::pepOut()
 *   - PetriNet::apnnOut()
 *   - PetriNet::lolaOut()
 *   - PetriNet::owfnOut()
 *
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2006-03-16
 *          - last changed: \$Date: 2006/07/01 21:58:08 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.22 $
 */





#include "petrinet.h"
#include "debug.h"		// debugging help
#include "helpers.h"		// helper functions
#include "bpel2owfn.h"
#include "options.h"




/******************************************************************************
 * Functions to print information of the net and its nodes
 *****************************************************************************/

/*!
 * \return the name of the node type
 */
string Node::nodeTypeName()
{
  return "";
}





/*!
 * \return the name of the node type
 */
string Place::nodeTypeName()
{
  return "place";
}





/*!
 * \return the name of the node type
 */
string Transition::nodeTypeName()
{
  return "transition";
}





/*!
 * dummy-implementation of virtual function
 */
string Node::nodeShortName()
{
  return "";
}




/*!
 * \return the (nice) name of the node for DOT output
 */
string Node::nodeName()
{
  string result = history[0];
  result = result.substr(result.find_last_of(".")+1,result.length());
  result = prefix + result;
  return result;
}






/*!
 * \return the short name of the place, e.g. for LoLA output
 */
string Place::nodeShortName()
{
  if (type == INTERNAL)
    return ("p" + intToString(id));
  else
    return history[0];
}





/*!
 * \return the short name of the transition, e.g. for LoLA output
 */
string Transition::nodeShortName()
{
  return ("t" + intToString(id));
}





/*!
 * \return string containing information about the net
 */
string PetriNet::information()
{
  string result = "|P|=" + intToString(P.size() + P_in.size() + P_out.size());
  result += ", |P_in|= " + intToString(P_in.size());
  result += ", |P_out|= " + intToString(P_out.size());
  result += ", |T|=" + intToString(T.size());
  result += ", |F|=" + intToString(F.size());
  return result;
}





/*!
 * Prints information about the generated Petri net. In particular, for each
 * place and transition all roles of the history are printed to understand
 * the net and maybe LoLA's witness pathes later.
 *
 * \todo put this to the nodes
 */
void PetriNet::printInformation()
{
  // the places
  (*output) << "PLACES:\nID\tTYPE\t\tROLES\n";

  // the internal places
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\tinternal";

    for (vector<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
      if (role == (*p)->history.begin())
	(*output) << "\t" + (*p)->prefix + *role + "\n";
      else
	(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
  }

  // the input places
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\tinput   ";

    for (vector<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
      if (role == (*p)->history.begin())
	(*output) << "\t" + (*p)->prefix + *role + "\n";
      else
	(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
  }

  // the output places
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\toutput  ";

    for (vector<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
      if (role == (*p)->history.begin())
	(*output) << "\t" + (*p)->prefix + *role + "\n";
      else
	(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
  }

  // the transitions
  (*output) << "\nTRANSITIONS:\n";
  (*output) << "ID\tROLES\n";

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << (*t)->nodeShortName() + "\t";

    for (vector<string>::iterator role = (*t)->history.begin(); role != (*t)->history.end(); role++)
      if (role == (*t)->history.begin())
	(*output) << (*t)->prefix + *role + "\n";
      else
	(*output) << "\t" + (*t)->prefix + *role + "\n";
  }
}





/******************************************************************************
 * DOT output of the net
 *****************************************************************************/


/*!
 * DOT-output of the arc.
*/
string Arc::dotOut()
{
  string result = " " + intToString(source->id) + " -> " + intToString(target->id); 
  result += ";\n";

  return result;
}





/*!
 * DOT-output of the transition. Transitions are colored corresponding to their
 * initial role.
*/
string Transition::dotOut()
{
  string result;
  result += " " + intToString(id) + "\t[label=\"" + nodeShortName();
 
  if (parameters[P_COMMUNICATIONONLY])
  {
    result += "\\n";
    result += nodeName();
  }

  result += "\"";

  if (type == IN)
    result += " style=filled fillcolor=gold";
  if (type == OUT)
    result += " style=filled fillcolor=yellow";
  
  result += "];\n";
  return result;
}





/*!
 * DOT-output of the place. Places are colored corresponding to their initial
 * role.
*/
string Place::dotOut()
{
  string result;
  result += " " + intToString(id) + "\t[label=\"" + nodeShortName() + "\"";


  if (firstMemberIs("in."))
    result += " style=filled fillcolor=gold shape=ellipse";
  else if (firstMemberIs("out."))
    result += " style=filled fillcolor=yellow shape=ellipse";
  else if (firstMemberIs("!link."))
    result += " style=filled fillcolor=deeppink";
  else if (firstMemberIs("link."))
    result += " style=filled fillcolor=cornflowerblue";
  else if (firstMemberIs("variable."))
    result += " style=filled fillcolor=cyan";
  else if (firstMemberIs("1.internal.initial")
	   || firstMemberIs("1.internal.final"))
    result += " style=filled fillcolor=green";
  else if (firstMemberIs("1.internal.clock"))
    result += " style=filled fillcolor=seagreen";

  result += "];\n";
  return result;
}





/*!
 * Creates a DOT output(see http://www.graphviz.org) of the Petri net. It uses
 * the digraph-statement and adds labels to transitions, places and arcs if
 * neccessary. It also distinguishes the three arc types of #arc_type.
 */
void PetriNet::dotOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating DOT-output.\n");

  (*output) << "digraph N {" << endl;
  (*output) << " graph [fontname=\"Helvetica\", label=\"";

  if (parameters[P_SIMPLIFY])
    (*output) << "structural simplified ";

  (*output) << "Petri net generated from " << filename << "\"];" << endl;
  (*output) <<
    " node [fontname=\"Helvetica\" fontsize=10 fixedsize];" << endl;
  (*output) << " edge [fontname=\"Helvetica\" fontsize=10];" <<
    endl << endl;

  // list the places
  (*output) << endl << " node [shape=circle];" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*output) << (*p)->dotOut();

  if (!hasNoInterface)
  {
    for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
      (*output) << (*p)->dotOut();
    for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
      (*output) << (*p)->dotOut();
  }

  // list the transitions
  (*output) << endl << " node [shape=box regular=true];" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) << (*t)->dotOut();

  // list the arcs
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    (*output) << (*f)->dotOut();

  (*output) << "}" << endl;
}





/******************************************************************************
 * Petri net file formats
 *****************************************************************************/

/*!
 * Outputs the net in PNML (Petri Net Markup Language).
 */
void PetriNet::pnmlOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating PNML-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "<!-- Petri net created by " << PACKAGE_STRING << " reading file " << filename << " -->" << endl << endl;

  (*output) << "<pnml>" << endl;
  (*output) << "  <net id=\"" << filename << "\" type=\"\">" << endl << endl;

  // places(only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "    <place id=\"" << (*p)->nodeShortName() << "\">" << endl;
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->history[0] << "</text>" << endl;
    (*output) << "      </name>" << endl;
    if ((*p)->marked)
    {
      (*output) << "      <initialMarking>" << endl;
      (*output) << "        <text>1</text>" << endl;
      (*output) << "      </initialMarking>" << endl;
    }
    (*output) << "    </place>" << endl << endl;
  }

  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "    <transition id=\"" << (*t)->nodeShortName() << "\">" << endl;
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*t)->history[0] << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "    </transition>" << endl << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++, arcNumber++)
  {
    (*output) << "    <arc id=\"a" << arcNumber << "\" ";
    (*output) << "source=\"" << (*f)->source->nodeShortName() << "\" ";
    (*output) << "target=\"" << (*f)->source->nodeShortName() << "\" />" << endl;
  }
  (*output) << endl;
  (*output) << "  </net>" << endl;
  (*output) << "</pnml>" << endl;
  (*output) << endl << "<!-- END OF FILE -->" << endl;
}





/*!
 * Outputs the net in low-level PEP notation.
 */
void PetriNet::pepOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating PEP-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  // header
  (*output) << "PEP" << endl << "PTNet" << endl << "FORMAT_N" << endl;

  // places(only internal)
  (*output) << "PL" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << (*p)->id << "\"" << (*p)->nodeShortName() << "\"80@40";
    if ((*p)->marked)
      (*output) << "M1";
    (*output) << "k1" << endl;
  }

  // transitions
  (*output) << "TR" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) << (*t)->id << "\"" << (*t)->nodeShortName() << "\"80@40" << endl;

  // arcs from transitions to places
  (*output) << "TP" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == TRANSITION)
      (*output) << (*f)->source->id << "<" << (*f)->target->id << endl;

  // arcs from places to transitions
  (*output) << "PT" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == PLACE)
      (*output) << (*f)->source->id << ">" << (*f)->target->id << endl;
}





/*!
 * Outputs the net in APNN (Abstract Petri Net Notation).
 */
void PetriNet::apnnOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating APNN-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "\\beginnet{" << filename << "}" << endl << endl;

  // places(only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "  \\place{" << (*p)->nodeShortName() << "}{";
    if ((*p)->marked)
      (*output) << "\\init{1}";
    (*output) << "}" << endl;
  }
  (*output) << endl;

  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "  \\transition{" << (*t)->nodeShortName() << "}{}" << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++, arcNumber++)
  {
    (*output) << "  \\arc{a" << arcNumber << "}{ ";
    (*output) << "\\from{" << (*f)->source->nodeShortName() << "} ";
    (*output) << "\\to{" << (*f)->target->nodeShortName() << "} }" << endl;
  }
  (*output) << endl;

  (*output) << "\\endnet" << endl;
}





/*!
 * Outputs the net in LoLA-format.
 */
void PetriNet::lolaOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating LoLA-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "{ Petri net created by " << PACKAGE_STRING << " reading " << filename << " }" << endl << endl;

  
  // places (only internal)
  (*output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
    (*output) << "  " << (*p)->nodeShortName();
    
    if (count < P.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl << endl;


  // initial marking
  (*output) << "MARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->marked)
    {
      if (count++ != 1)
	(*output) << "," << endl;
      
      (*output) << "  " << (*p)->nodeShortName() << ":\t1";
    }
  }
  (*output) << endl << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->nodeShortName() << endl;
    set<Node *> consume = preset(*t);
    set<Node *> produce = postset(*t);
    
    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = consume.begin(); pre != consume.end(); count++, pre++)
    {
      (*output) << "  " << (*pre)->nodeShortName() << ":\t1";
      
      if (count < consume.size())
	(*output) << "," << endl;
    }
    (*output) << ";" << endl;
    
    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = produce.begin(); post != produce.end(); count++, post++)
    {
      (*output) << "  " << (*post)->nodeShortName() << ":\t1";
      
      if (count < produce.size())
	(*output) << "," << endl;
    }
    
    (*output) << ";" << endl << endl;
  }  
  (*output) << "{ END OF FILE }" << endl;
}





/******************************************************************************
 * oWFN file format
 *****************************************************************************/

/*!
 * Outputs the net in oWFN-format.
 */
void PetriNet::owfnOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating oWFN-output.\n");

  (*output) << "{ oWFN created by " << PACKAGE_STRING << " reading " << filename << " }" << endl << endl;

  // places
  (*output) << "PLACE" << endl;

  // internal places
  (*output) << "  INTERNAL" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
    (*output) << "    " << (*p)->nodeShortName();
    
    if (count < P.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;


  // input places
  (*output) << "  INPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); count++, p++)
  {
    (*output) << "    " << (*p)->nodeShortName();
    
    if (count < P_in.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;


  // output places
  (*output) << "  OUTPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); count++, p++)
  {
    (*output) << "    " << (*p)->nodeShortName();
    
    if (count < P_out.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;
  

  // initial marking
  (*output) << "INITIALMARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->marked)
    {
      if (count++ != 1)
	(*output) << "," << endl;
      
      (*output) << "  " << (*p)->nodeShortName() << ":\t1";
    }
  }
  (*output) << endl << ";" << endl << endl;  

/*
  // final condition
  (*output) << "FINALCONDITION" << endl << "  (";
  
  for (set<Place*>::iterator p_in = P_in.begin();
      p_in != P_in.end();
      p_in++)
  {
    (*output) << "(" << (*p_in)->nodeShortName() << " = 0) AND ";
  }

  (*output) << endl << "   ";
  
  for (set<Place*>::iterator p_out = P_out.begin();
      p_out != P_out.end();
      p_out++)
  {
    (*output) << "(" << (*p_out)->nodeShortName() << " = 0) AND ";
  }

  (*output) << endl << "   (" << findPlace("1.internal.final")->nodeShortName() << " > 0));" << endl;
  (*output) << endl << endl << endl;
*/

  // final marking
  (*output) << "FINALMARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->history[0] == ((*p)->prefix + "1.internal.final") ) // was: ((*p)->nodeName() == ((*p)->prefix + "1.internal.final") )
    {
      if (count++ != 1)
	(*output) << "," << endl;
      
      (*output) << "  " << (*p)->nodeShortName() << ":\t1";
    }
  }
  (*output) << endl << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->nodeShortName() << endl;
    set<Node *> consume = preset(*t);
    set<Node *> produce = postset(*t);
    
    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = consume.begin(); pre != consume.end(); count++, pre++)
    {
      (*output) << "  " << (*pre)->nodeShortName() << ":\t1";
      
      if (count < consume.size())
	(*output) << "," << endl;
    }
    (*output) << ";" << endl;
    
    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = produce.begin(); post != produce.end(); count++, post++)
    {
      (*output) << "  " << (*post)->nodeShortName() << ":\t1";
      
      if (count < produce.size())
	(*output) << "," << endl;
    }
    
    (*output) << ";" << endl << endl;
  }  
  (*output) << "{ END OF FILE }" << endl;
}

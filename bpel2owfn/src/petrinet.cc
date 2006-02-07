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
\*****************************************************************************/

/*!
 * \file petrinet.cc
 *
 * \brief Functions for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in petrinet.h.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005-10-18
 *          - last changed: \$Date: 2006/02/07 07:47:01 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.85 $
 */





#include "petrinet.h"

#include "options.h"


extern SymbolManager symMan;	// defined in bpel-syntax.yy





/*!
 * \todo
 *       - (nlohmann) comment me!
 *       - (nlohmann) put me into helpers.cc
 */
set < pair < Node *, arc_type > >setUnion (set < pair < Node *, arc_type > >a,
					   set < pair < Node *, arc_type > >b)
{
  set < pair < Node *, arc_type > >result;
  insert_iterator < set < pair < Node *, arc_type >, less < pair < Node *,
    arc_type > > > >res_ins (result, result.begin ());
  set_union (a.begin (), a.end (), b.begin (), b.end (), res_ins);

  return result;
}





/*****************************************************************************/


/*!
 * \param role a role of a node
 * \return true, if the node's first history entry contains the given role
 */
bool Node::firstMemberAs (string role)
{
  string
    firstEntry = (*history.begin ());
  return (firstEntry.find (role, 0) == firstEntry.find_first_of (".") + 1);
}





/*!
 * \param role a role of a node
 * \return true, if the node's first history entry begins with the given role
 */
bool Node::firstMemberIs (string role)
{
  string
    firstEntry = (*history.begin ());
  return (firstEntry.find (role, 0) == 0);
}





/*!
 * \return the name of the node type
 */
string Node::nodeTypeName ()
{
  switch (nodeType)
    {
    case (PLACE):
      return "place";
    case (TRANSITION):
      return "transition";
    default:
      return "uninitialized node";	// should never happen
    }
}





/*!
 * \return the short name of the node, e.g. for LoLA output
 */
string Node::nodeShortName ()
{
  if (nodeType == PLACE)
    return ("p" + intToString(id));
  if (nodeType == TRANSITION)
    return ("t" + intToString(id));
  else
    return NULL;
}




/*****************************************************************************/


/*!
 * \param mysource      the source-node of the arc
 * \param mytarget      the target-node of the arc
 * \param mytype        the type of the arc (as defined in #arc_type)
 * \param myinscription the inscription of the arc (empty string for none)
 */
Arc::Arc (Node * mysource, Node * mytarget, arc_type mytype,
	  string myinscription)
{
  source = mysource;
  target = mytarget;
  type = mytype;
  inscription = myinscription;
}





/*!
 * DOT-output of the arc.
*/
string Arc::dotOut ()
{
  string
    result;
  result +=
    " " + intToString (source->id) + " -> " + intToString (target->id) +
    "\t[";

  if (type == RESET)
    result += "arrowtail=odot ";
  else if (type == READ)
    result += "arrowhead=diamond arrowtail=tee ";

  if (inscription != "")
    result += "label=\"" + inscription + "\" ";

  result += "];\n";
  return result;
}





/*****************************************************************************/


/*!
 * \param myid    the id of the transition
 * \param role    the first role of the transition
 * \param myguard the guard of the transition
 */
Transition::Transition (unsigned int myid, string role, string myguard)
{
  guard = myguard;
  id = myid;
  nodeType = TRANSITION;

  if (role != "")
    history.push_back (role);
}





/*!
 * DOT-output of the transition. Transitions are colored corresponding to their
 * initial role.
 *
 * \todo
 *       - (nlohmann) escape guards
*/
string Transition::dotOut ()
{
  string result;
  result += " " + intToString (id) + "\t[label=\"" + nodeShortName() + "\"";

  if (guard != "")
    result +=
      " shape=record label=\"{" + nodeShortName() + "|{" + guard + "}}\"";

  if (firstMemberAs ("internal.eventHandler."))
    result += " style=filled fillcolor=plum";
  else if (firstMemberAs ("internal.compensationHandler."))
    result += " style=filled fillcolor=aquamarine";
  else if (firstMemberAs ("internal.stop."))
    result += " style=filled fillcolor=lightskyblue2";
  else if (firstMemberAs ("internal.faultHandler."))
    result += " style=filled fillcolor=pink";

  result += "];\n";
  return result;
}





/*****************************************************************************/


/*!
 * \param myid   the internal id of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #place_type)
 */
Place::Place (unsigned int myid, string role, place_type mytype)
{
  type = mytype;
  id = myid;
  nodeType = PLACE;

  if (role != "")
    history.push_back (role);
}





/*!
 * DOT-output of the place. Places are colored corresponding to their initial
 * role.
*/
string Place::dotOut ()
{
  string
    result;
  result += " " + intToString (id) + "\t[label=\"" + nodeShortName() + "\"";

  if (firstMemberAs ("eventHandler."))
    result += " style=filled fillcolor=plum";
  else if (firstMemberAs ("internal.compensationHandler."))
    result += " style=filled fillcolor=aquamarine";
  else if (firstMemberAs ("internal.stop."))
    result += " style=filled fillcolor=lightskyblue2";
  else if (firstMemberAs ("internal.faultHandler."))
    result += " style=filled fillcolor=pink";
  else if (firstMemberIs ("link.") || firstMemberIs ("!link."))
    result += " style=filled fillcolor=yellow";
  else if (firstMemberIs ("variable."))
    result += " style=filled fillcolor=cyan shape=ellipse";
  else if (firstMemberIs ("in.") || firstMemberIs ("out."))
    result += " style=filled fillcolor=gold shape=ellipse";
  else if (firstMemberAs ("internal.Active")
	   || firstMemberAs ("internal.!Active"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs ("internal.Completed")
	   || firstMemberAs ("internal.!Completed"))
    result += " style=filled fillcolor=yellowgreen ";
  else if (firstMemberAs ("internal.Compensated")
	   || firstMemberAs ("internal.!Compensated"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs ("internal.Ended")
	   || firstMemberAs ("internal.!Ended"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs ("internal.Faulted")
	   || firstMemberAs ("internal.!Faulted"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs ("internal.Terminated")
	   || firstMemberAs ("internal.!Terminated"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberIs ("1.internal.initial")
	   || firstMemberIs ("1.internal.final"))
    result += " style=filled fillcolor=green";
  else if (firstMemberIs ("1.internal.clock"))
    result += " style=filled fillcolor=seagreen";

  result += "];\n";
  return result;
}





/*****************************************************************************/


PetriNet::PetriNet ()
{
  lowLevel = false;
  nextId = 0;
}





/*!
 * Creates a low-level place without an initial role.
 * \return pointer of the created place
 */
Place *
PetriNet::newPlace ()
{
  return newPlace ("", INTERNAL);
}





/*!
 * The actual newPlace function called by all other overloaded newPlace
 * functions.
 *
 * \param role   the initial role of the place
 * \param mytype the type of the place (as defined in #place_type)
 * \return pointer of the created place
 */
Place *
PetriNet::newPlace (string role, place_type mytype)
{
  trace (TRACE_VERY_DEBUG, "[PN]\tCreating place p" + intToString (id ()) +
	 " (" + role + ") ...\n");

  Place *p = new Place (getId (), role, mytype);
  p->initialMarking = 0;
  P.insert (p);

  if (role != "")
    {
      if (roleMap[role] != NULL)
	{
	  throw Exception (DOUBLE_NODE,
			   "Place with role '" + role +
			   "' already defined.\n", pos (__FILE__, __LINE__,
							__FUNCTION__));
	}
      else
	roleMap[role] = p;
    }

  return p;
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates an unguarded transition without an initial role.
 * \return pointer of the created transition
 */
Transition *
PetriNet::newTransition ()
{
  return newTransition ("", "");
}





/*!
 * The actual newTransition function called by all other overloaded
 * newTransition functions.
 *
 * \param role  the initial role of the transition
 * \param guard guard of the transition
 * \return pointer of the created transition
 */
Transition *
PetriNet::newTransition (string role, string guard)
{
  trace (TRACE_VERY_DEBUG,
	 "[PN]\tCreating transition t" + intToString (id ()) + " (" + role +
	 ") ...\n");

  Transition *t = new Transition (getId (), role, guard);
  T.insert (t);

  if (role != "")
    {
      if (roleMap[role] != NULL)
	{
	  throw Exception (DOUBLE_NODE,
			   "Place with role '" + role +
			   "' already defined.\n", pos (__FILE__, __LINE__,
							__FUNCTION__));
	}
      else
	roleMap[role] = t;
    }

  return t;
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates an inscripted standard arc.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param inscription inscription of the arc
 * \return pointer of the created arc
 */
Arc *
PetriNet::newArc (Node * source, Node * target, string inscription)
{
  return newArc (source, target, STANDARD, inscription);
}





/*!
 * Creates an inscripted standard arc.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param inscription inscription of the arc
 * \return pointer of the created arc
 */
Arc *
PetriNet::newArc (Node * source, Node * target, kc::casestring inscription)
{
  return newArc (source, target, STANDARD, inscription->name);
}





/*!
 * Creates an arc with arbitrary inscription and type.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param type        type of the arc (as defined in #arc_type)
 * \param inscription inscription of the arc 
 * \return pointer of the created arc
 */
Arc *
PetriNet::newArc (Node * source, Node * target, arc_type type,
		  string inscription)
{
  if ((Place *) source == NULL || (Transition *) source == NULL)
    {
      string name = "unknown";
      string role = "unknown";
      if ((Place *) target != NULL)
	{
	  name = target->nodeTypeName () + " " + target->nodeShortName();
	  role = *(target->history.begin ());
	}
      if ((Transition *) target != NULL)
	{
	  name = target->nodeTypeName () + " " + target->nodeShortName();
	  role = *(target->history.begin ());
	}
      throw Exception (ARC_ERROR,
		       "Source of arc to " + name + " (" + role +
		       ") not found!\n", pos (__FILE__, __LINE__,
					      __FUNCTION__));
    }

  if ((Place *) target == NULL || (Transition *) target == NULL)
    {
      string name = "unknown";
      string role = "unknown";
      if ((Place *) source != NULL)
	{
	  name = source->nodeTypeName () + " " + source->nodeShortName();
	  role = *(source->history.begin ());
	}
      if ((Transition *) source != NULL)
	{
	  name = source->nodeTypeName () + " " + source->nodeShortName();
	  role = *(source->history.begin ());
	}
      throw Exception (ARC_ERROR,
		       "Target of arc from " + name + " (" + role +
		       ") not found!\n", pos (__FILE__, __LINE__,
					      __FUNCTION__));
    }

  trace (TRACE_VERY_DEBUG, "[PN]\tCreating arc (" + intToString (source->id) +
	 "," + intToString (target->id) + ")...\n");

  if (source->nodeType == target->nodeType)
    throw Exception (ARC_ERROR,
		     "Arc between two " + source->nodeTypeName () + "s!\n" +
		     *((source->history).begin ()) + " and " +
		     *((target->history).begin ()), pos (__FILE__, __LINE__,
							 __FUNCTION__));

  if (F.size () % 1000 == 0)
    trace (TRACE_INFORMATION, "[PN]\t" + information () + "\n");

  Arc *f = new Arc (source, target, type, inscription);
  F.insert (f);

  return f;
}





/*---------------------------------------------------------------------------*/

/*!
 * Removes all ingoing and outgoing arcs of a node, i.e. detatches the node
 * from the rest of the net.
 * 
 * \param n node to be detached
 */
void
PetriNet::detachNode (Node * n)
{
  trace (TRACE_VERY_DEBUG, "[PN]\tDetaching node " + intToString (n->id) +
	 "...\n");

  vector < Arc * >removeList;

  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    if (((*f)->source == n) || ((*f)->target == n))
      removeList.push_back (*f);

  for (unsigned int i = 0; i < removeList.size (); i++)
    removeArc (removeList[i]);
}





/*!
 * Removes a place and all arcs connected with it.
 * \param p place to be removed
 */
void
PetriNet::removePlace (Place * p)
{
  trace (TRACE_VERY_DEBUG, "[PN]\tRemoving place " + intToString (p->id) +
	 "...\n");

  detachNode (p);
  P.erase (p);
  delete p;
}





/*!
 * Removes a transition and all arcs connected with it.
 * \param t transition to be removed
 */
void
PetriNet::removeTransition (Transition * t)
{
  trace (TRACE_VERY_DEBUG,
	 "[PN]\tRemoving transition " + intToString (t->id) + "...\n");

  detachNode (t);
  T.erase (t);
  delete t;
}





/*!
 * \param f arc to be removed
 */
void
PetriNet::removeArc (Arc * f)
{
  trace (TRACE_VERY_DEBUG,
	 "[PN]\tRemoving arc (" + intToString (f->source->id) + "," +
	 intToString (f->target->id) + ")...\n");

  F.erase (f);
  delete f;
}





/*---------------------------------------------------------------------------*/


/*!
 * \return string containing information about the net
 */
string PetriNet::information ()
{
  string
    result;
  result += intToString (P.size ()) + " places, " + intToString (T.size ()) +
    " transitions, " + intToString (F.size ()) + " arcs";
  return result;
}





/*!
 * Prints information about the generated Petri net. In particular, for each
 * place and transition all roles of the history are printed to understand
 * the net and maybe LoLA's witness pathes later.
 */
void
PetriNet::printInformation ()
{
  // the places
  (*output) << "PLACES:\nID\tTYPE\t\tROLES\n";
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    {
      (*output) << (*p)->nodeShortName() << "\t";

      switch ((*p)->type)
	{
	case (INTERNAL):
	  {
	    (*output) << "internal";
	    break;
	  }
	case (IN):
	  {
	    (*output) << "input   ";
	    break;
	  }
	case (OUT):
	  {
	    (*output) << "output  ";
	    break;
	  }
	default:
	  {
	    (*output) << "other   ";
	  }			// should never happen
	}

      for (vector < string >::iterator role = (*p)->history.begin ();
	   role != (*p)->history.end (); role++)
	{
	  if (role == (*p)->history.begin ())
	    (*output) << "\t" + *role + "\n";
	  else
	    (*output) << "\t\t\t" + *role + "\n";
	}
    }


  // the transitions
  (*output) << "\nTRANSITIONS:\n";
  (*output) << "ID\tGUARD\t\tROLES\n";
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
    {
      (*output) << (*t)->nodeShortName() + "\t";

      if ((*t)->guard != "")
	(*output) << "{" + (*t)->guard + "} ";
      else
	(*output) << "\t";

      for (vector < string >::iterator role = (*t)->history.begin ();
	   role != (*t)->history.end (); role++)
	{
	  if (role == (*t)->history.begin ())
	    (*output) << "\t" + *role + "\n";
	  else
	    (*output) << "\t\t\t" + *role + "\n";
	}
    }
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates a DOT output (see http://www.graphviz.org) of the Petri net. It uses
 * the digraph-statement and adds labels to transitions, places and arcs if
 * neccessary. It also distinguishes the three arc types of #arc_type.
 */
void
PetriNet::dotOut ()
{
  trace (TRACE_DEBUG, "[PN]\tCreating DOT-output.\n");

  (*output) << "digraph N {" << endl;
  (*output) << " graph [fontname=\"Helvetica-Oblique\", label=\"";

  if (parameters[P_SIMPLIFY])
    (*output) << "structural simplified ";
  if (lowLevel)
    (*output) << "low-level ";

  (*output) << "Petri net generated from " << filename << "\"];" << endl;
  (*output) <<
    " node [fontname=\"Helvetica-Oblique\" fontsize=10 fixedsize];" << endl;
  (*output) << " edge [fontname=\"Helvetica-Oblique\" fontsize=10];" <<
    endl << endl;

  // list the places
  (*output) << endl << " node [shape=circle];" << endl;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    (*output) << (*p)->dotOut ();

  // list the transitions
  (*output) << endl << " node [shape=box regular=true];" << endl;
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
    (*output) << (*t)->dotOut ();

  // list the arcs
  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    (*output) << (*f)->dotOut ();

  (*output) << "}" << endl;
}





void
PetriNet::removeInterface ()
{
  trace (TRACE_DEBUG, "[PN]\tRemoving interface places.\n");

  list < Place * >killList;

  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    {
      if ((*p)->type == IN || (*p)->type == OUT)
	killList.push_back (*p);
    }

  for (list < Place * >::iterator it = killList.begin ();
       it != killList.end (); it++)
    removePlace (*it);

}





/*****************************************************************************
 * Output formats
 *****************************************************************************/

/*!
 * Outputs the net in PNML (Petri Net Markup Language).
 */
void
PetriNet::pnmlOut ()
{
  trace (TRACE_DEBUG, "[PN]\tCreating PNML-output.\n");

  if (!lowLevel)
    makeLowLevel ();

  removeInterface ();
  calculateInitialMarking();


  cout << "<!-- Petri net created by " << PACKAGE_STRING << " reading file " << filename << " -->" << endl << endl;
  
  cout << "<pnml>" << endl;
  cout << "  <net id=\"" << filename << "\" type=\"\">" << endl << endl;
  
  // places
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
  {
    cout << "    <place id=\"" << (*p)->nodeShortName() << "\">" << endl;
    cout << "      <name>" << endl;
    cout << "        <text>" << (*(*p)->history.begin()) << "</text>" << endl;
    cout << "      </name>" << endl;
    if ((*p)->initialMarking > 0)
    {
      cout << "      <initialMarking>" << endl;
      cout << "        <text>" << (*p)->initialMarking << "</text>" << endl;
      cout << "      </initialMarking>" << endl;
    }
    cout << "    </place>" << endl << endl;
  }

  // transitions
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
  {
    cout << "    <transition id=\"" << (*t)->nodeShortName() << "\">" << endl;
    cout << "      <name>" << endl;
    cout << "        <text>" << (*(*t)->history.begin()) << "</text>" << endl;
    cout << "      </name>" << endl;
    cout << "    </transition>" << endl << endl;
  }
  cout << endl;

  // arcs
  int arcNumber = 1;
  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++, arcNumber++)
  {
    cout << "    <arc id=\"a" << arcNumber << "\" ";
    cout << "source=\"" << (*f)->source->nodeShortName() << "\" ";
    cout << "target=\"" << (*f)->source->nodeShortName() << "\" />" << endl;
  }
  cout << endl;
  cout << "  </net>" << endl;
  cout << "</pnml>" << endl;
  cout << endl << "<!-- END OF FILE -->" << endl;
}





/*!
 * Outputs the net in low-level PEP notation.
 */
void
PetriNet::pepOut()
{
  trace (TRACE_DEBUG, "[PN]\tCreating PEP-output.\n");

  if (!lowLevel)
    makeLowLevel ();

  removeInterface ();
  calculateInitialMarking();
  
  // header
  cout << "PEP" << endl << "PTNet" << endl << "FORMAT_N" << endl;
  
  // places
  cout << "PL" << endl;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
  {
    cout << (*p)->id << "\"" << (*p)->nodeShortName() << "\"80@40";
    if ((*p)->initialMarking > 0)
      cout << "M" << (*p)->initialMarking;
    cout << "k1" << endl;
  }

  // transitions
  cout << "TR" << endl;
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
    cout << (*t)->id << "\"" << (*t)->nodeShortName() << "\"80@40" << endl;
  
  // arcs from transitions to places
  cout << "TP" << endl;
  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    if (((*f)->source->nodeType) == TRANSITION)
      cout << (*f)->source->id << "<" << (*f)->target->id << endl;

  // arcs from places to transitions
  cout << "PT" << endl;
  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    if (((*f)->source->nodeType) == PLACE)
      cout << (*f)->source->id << ">" << (*f)->target->id << endl;
}





/*!
 * Outputs the net in APNN (Abstract Petri Net Notation).
 */
void
PetriNet::appnOut ()
{
  trace (TRACE_DEBUG, "[PN]\tCreating APPN-output.\n");

  if (!lowLevel)
    makeLowLevel ();

  removeInterface ();
  calculateInitialMarking();
  
  (*output) << "\\beginnet{" << filename << "}" << endl << endl;

  // places
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
  {
    (*output) << "  \\place{" << (*p)->nodeShortName() << "}{";
    if ((*p)->initialMarking > 0)
      (*output) << "\\init{" << (*p)->initialMarking << "}";
    (*output) << "}" << endl;
  }
  (*output) << endl;

  // transitions
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
  {
    (*output) << "  \\transition{" << (*t)->nodeShortName() << "}{}" << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++, arcNumber++)
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
void
PetriNet::lolaOut ()
{
  trace (TRACE_DEBUG, "[PN]\tCreating LoLA-output.\n");

  if (!lowLevel)
    makeLowLevel ();

  removeInterface ();

  (*output) << "{ Petri net created by " << PACKAGE_STRING <<
    " reading " << filename << " }" << endl << endl;

  // places
  (*output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); count++, p++)
    {
      (*output) << "  " << (*p)->nodeShortName();

      if (count < P.size ())
	(*output) << ",";
      else
	(*output) << ";";
      (*output) << "\t\t { " << (*(*p)->history.begin()) << " }" << endl;
    }
  (*output) << endl << endl;


  // initial marking
  (*output) << "MARKING" << endl;
  (*output) << "  " << findPlace("1.internal.initial")->nodeShortName()
	  << ":\t1,\t { initial marking of the process }" << endl;

  for (list < string >::iterator variable = symMan.variables.begin ();
       variable != symMan.variables.end (); variable++)
    (*output) << "  " << findPlace("variable." + *variable)->nodeShortName() << ":\t1,\t { initial marking of variable" << *variable << " }" <<
      endl;

  (*output) << "  " << findPlace("1.internal.clock")->nodeShortName() << ":\t1\t { initial marking of the clock }" << endl;
  (*output) << ";" << endl << endl << endl;


  // transitions
  count = 1;
  for (set < Transition * >::iterator t = T.begin (); t != T.end ();
       count++, t++)
    {
      (*output) << "TRANSITION " << (*t)->nodeShortName() << "\t { " << (*(*t)->history.begin()) << " }" << endl;
      set < pair < Node *, arc_type > >consume = preset (*t);
      set < pair < Node *, arc_type > >produce = postset (*t);

      (*output) << "CONSUME" << endl;
      if (consume.empty ())
	(*output) << ";" << endl;

      unsigned int count2 = 1;
      for (set < pair < Node *, arc_type > >::iterator pre = consume.begin ();
	   pre != consume.end (); count2++, pre++)
	{
	  (*output) << "  " << (*pre).first->nodeShortName() << ":\t1";

	  if (count2 < consume.size ())
	    (*output) << "," << endl;
	  else
	    (*output) << ";" << endl;
	}

      (*output) << "PRODUCE" << endl;
      if (produce.empty ())
	(*output) << ";" << endl;

      count2 = 1;
      for (set < pair < Node *, arc_type > >::iterator post =
	   produce.begin (); post != produce.end (); count2++, post++)
	{
	  (*output) << "  " << (*post).first->nodeShortName() << ":\t1";

	  if (count2 < produce.size ())
	    (*output) << "," << endl;
	  else
	    (*output) << ";" << endl;
	}
      (*output) << endl;
    }
  (*output) << "{ END OF FILE }" << endl;
}





/*!
 * Outputs the net in oWFN-format.
 */
void
PetriNet::owfnOut ()
{
  trace (TRACE_DEBUG, "[PN]\tCreating oWFN-output.\n");

  if (!lowLevel)
    makeLowLevel ();

  (*output) << "{ oWFN created by " << PACKAGE_STRING <<
    " reading " << filename << " }" << endl << endl;

  // places
  (*output) << "PLACE" << endl;

  // internal places
  (*output) << "  INTERNAL" << endl;
  unsigned int count = 1;
  string comment;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    {
      if ((*p)->type == INTERNAL)
	{
	  if (count == 1)
	    (*output) << "    " << (*p)->nodeShortName();
	  else
	    (*output) << ", " << comment << "    " << (*p)->nodeShortName();

	  comment = "\t\t { " + (*(*p)->history.begin ()) + " }\n";
	  count++;
	}
    }
  (*output) << "; " << comment << endl;

  // input places
  (*output) << "  INPUT" << endl;
  count = 1;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    {
      if ((*p)->type == IN)
	{
	  if (count == 1)
	    (*output) << "    " << (*p)->nodeShortName();
	  else
	    (*output) << ", " << comment << "    " << (*p)->nodeShortName();

	  comment = "\t\t { " + (*(*p)->history.begin ()) + " }\n";
	  count++;
	}
    }
  (*output) << "; " << comment << endl;

  // output places
  (*output) << "  OUTPUT" << endl;
  count = 1;
  for (set < Place * >::iterator p = P.begin (); p != P.end (); p++)
    {
      if ((*p)->type == OUT)
	{
	  if (count == 1)
	    (*output) << "    " << (*p)->nodeShortName();
	  else
	    (*output) << ", " << comment << "    " << (*p)->nodeShortName();

	  comment = "\t\t { " + (*(*p)->history.begin ()) + " }\n";
	  count++;
	}
    }
  (*output) << "; " << comment << endl << endl;


  // initial marking
  (*output) << "INITIALMARKING" << endl;
  (*output) << "  " << findPlace("1.internal.initial")->nodeShortName() << ":\t1,\t { initial marking of the process }" << endl;
  
  for (list < string >::iterator variable = symMan.variables.begin ();
       variable != symMan.variables.end (); variable++)
    (*output) << "  " << findPlace("variable." + *variable)->nodeShortName() << ":\t1,\t { initial marking of variable" << *variable << " }" <<
      endl;

  (*output) << "  " << findPlace ("1.internal.clock")->nodeShortName() << ":\t1\t { initial marking of the clock }" << endl;
  (*output) << ";" << endl << endl;

  // final marking
  (*output) << "FINALMARKING" << endl;
  (*output) << "  " << findPlace("1.internal.final")->nodeShortName() << ":\t1\t { final marking of the process }" << endl;
  (*output) << ";" << endl << endl << endl;
  
  // transitions
  count = 1;
  for (set < Transition * >::iterator t = T.begin (); t != T.end ();
       count++, t++)
    {
      (*output) << "TRANSITION " << (*t)->nodeShortName() << "\t { " << (*(*t)->history.begin()) << " }" << endl;
      set < pair < Node *, arc_type > >consume = preset (*t);
      set < pair < Node *, arc_type > >produce = postset (*t);

      (*output) << "CONSUME" << endl;
      if (consume.empty ())
	(*output) << ";" << endl;

      unsigned int count2 = 1;
      for (set < pair < Node *, arc_type > >::iterator pre = consume.begin ();
	   pre != consume.end (); count2++, pre++)
	{
	  (*output) << "  " << (*pre).first->nodeShortName() << ":\t1";

	  if (count2 < consume.size ())
	    (*output) << "," << endl;
	  else
	    (*output) << ";" << endl;
	}

      (*output) << "PRODUCE" << endl;
      if (produce.empty ())
	(*output) << ";" << endl;

      count2 = 1;
      for (set < pair < Node *, arc_type > >::iterator post =
	   produce.begin (); post != produce.end (); count2++, post++)
	{
	  (*output) << "  " << (*post).first->nodeShortName() << ":\t1";

	  if (count2 < produce.size ())
	    (*output) << "," << endl;
	  else
	    (*output) << ";" << endl;
	}
      (*output) << endl;
    }
  (*output) << "{ END OF FILE }" << endl;
}





/*---------------------------------------------------------------------------*/


/*!
 * Merges two transitions. Given transitions t1 and t2:
 *
 * -# a new transition t12 with empty history is created
 * -# this transition gets the union of the histories of transition t1 and t2
 * -# the presets and postsets of t1 and t2 are calculated and united
 * -# t12 is connected with all the places in the preset and postset
 * -# the transitions t1 and t2 are removed
 * 
 * \param t1 first transition
 * \param t2 second transition
 *
 */
void
PetriNet::mergeTransitions (Transition * t1, Transition * t2)
{
  if (t1 == NULL || t2 == NULL)
    throw Exception (MERGING_ERROR, "One of the transitions is null!\n",
		     pos (__FILE__, __LINE__, __FUNCTION__));

  trace (TRACE_VERY_DEBUG,
	 "[PN]\tMerging transitions " + intToString (t1->id) + " and " +
	 intToString (t2->id) + "...\n");

  if (t1->guard != "" || t2->guard != "")
    throw Exception (MERGING_ERROR,
		     "Merging of guarded transition not yet supported!\n",
		     pos (__FILE__, __LINE__, __FUNCTION__));

  Node *t12 = newTransition ();

  for (vector < string >::iterator role = t1->history.begin ();
       role != t1->history.end (); role++)
    {
      roleMap[*role] = t12;
      t12->history.push_back (*role);
    }

  for (vector < string >::iterator role = t2->history.begin ();
       role != t2->history.end (); role++)
    {
      roleMap[*role] = t12;
      t12->history.push_back (*role);
    }

  set < pair < Node *, arc_type > >pre12 =
    setUnion (preset (t1), preset (t2));
  set < pair < Node *, arc_type > >post12 =
    setUnion (postset (t1), postset (t2));

  for (set < pair < Node *, arc_type > >::iterator n = pre12.begin ();
       n != pre12.end (); n++)
    newArc ((*n).first, t12, (*n).second);

  for (set < pair < Node *, arc_type > >::iterator n = post12.begin ();
       n != post12.end (); n++)
    newArc (t12, (*n).first, (*n).second);

  removeTransition (t1);
  removeTransition (t2);

  trace (TRACE_VERY_DEBUG, "[PN]\tMerging done.\n");
}





/*!
 * The actual function to merge two places. Given places p1 and p2:
 *
 * -# a new place p12 with empty history is created
 * -# this place gets the union of the histories of place p1 and p2
 * -# the presets and postsets of p1 and p2 are calculated and united
 * -# p12 is connected with all the transitions in the preset and postset
 * -# the places p1 and p2 are removed
 * 
 * \param p1 first place
 * \param p2 second place
 *
 * \todo
 *       - (nlohmann) Take care of arc inscriptions.
 */
void
PetriNet::mergePlaces (Place * p1, Place * p2)
{
  if (p1 == p2)
    return;

  if (p1 == NULL || p2 == NULL)
    return;

  if (p1->type != INTERNAL || p2->type != INTERNAL)
    throw Exception (MERGING_ERROR,
		     (string)
		     "Merging of interface places not yet supported!\n" +
		     "place " + p1->nodeShortName() + " (type " +
		     intToString (p2->type) + ") and " +
		     p2->nodeShortName() + " (type " +
		     intToString (p2->type) + ")", pos (__FILE__, __LINE__,
							__FUNCTION__));

  trace (TRACE_VERY_DEBUG, "[PN]\tMerging places " + intToString (p1->id) +
	 " and " + intToString (p2->id) + "...\n");

  Node *p12 = newPlace ();

  for (vector < string >::iterator role = p1->history.begin ();
       role != p1->history.end (); role++)
    {
      p12->history.push_back (*role);
      roleMap[*role] = p12;
    }

  for (vector < string >::iterator role = p2->history.begin ();
       role != p2->history.end (); role++)
    {
      p12->history.push_back (*role);
      roleMap[*role] = p12;
    }

  set < pair < Node *, arc_type > >pre12 =
    setUnion (preset (p1), preset (p2));
  set < pair < Node *, arc_type > >post12 =
    setUnion (postset (p1), postset (p2));

  for (set < pair < Node *, arc_type > >::iterator n = pre12.begin ();
       n != pre12.end (); n++)
    newArc ((*n).first, p12, (*n).second);

  for (set < pair < Node *, arc_type > >::iterator n = post12.begin ();
       n != post12.end (); n++)
    newArc (p12, (*n).first, (*n).second);

  removePlace (p1);
  removePlace (p2);
  trace (TRACE_VERY_DEBUG, "[PN]\tMerging done.\n");
}





/*!
 * \param role1 string describing the role of the first place
 * \param role2 string describing the role of the second place
 */
void
PetriNet::mergePlaces (string role1, string role2)
{
  mergePlaces (findPlace (role1), findPlace (role2));
}





/*!
 * Merges two places given two activity of the abstract syntax tree and the
 * roles of the places. The activities are used to complete the role-string
 * and pass the search request.
 * 
 * \param act1  activity of the AST represented by the first place
 * \param role1 string describing the role of the first place (beginning with a
 *              period: .empty
 * \param act2  activity of the AST represented by the second place (beginning
 *              with a period: .empty
 * \param role2 string describing the role of the second place
 */
void
PetriNet::mergePlaces (kc::impl_activity * act1, string role1,
		       kc::impl_activity * act2, string role2)
{
  mergePlaces (intToString (act1->id->value) + role1,
	       intToString (act2->id->value) + role2);
}





/*---------------------------------------------------------------------------*/


/*!
 * \param n a node of the Petri net
 * \result the preset of node n
 */
set < pair < Node *, arc_type > >PetriNet::preset (Node * n)
{
  set < pair < Node *, arc_type > >result;

  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    if ((*f)->target == n)
      {
	pair < Node *, arc_type > element =
	  pair < Node *, arc_type > ((*f)->source, (*f)->type);
	result.insert (element);
      }

  return result;
}





/*!
 * \param n a node of the Petri net
 * \result the postset of node n
 */
set < pair < Node *, arc_type > >PetriNet::postset (Node * n)
{
  set < pair < Node *, arc_type > >result;

  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    if ((*f)->source == n)
      {
	pair < Node *, arc_type > element =
	  pair < Node *, arc_type > ((*f)->target, (*f)->type);
	result.insert (element);
      }

  return result;
}





/*---------------------------------------------------------------------------*/


/*!
 * Finds a place of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *
PetriNet::findPlace (string role)
{
  Place *result = (Place *) roleMap[role];

  if (result == NULL)
    trace (TRACE_DEBUG,
	   "[PN]\tPlace with role \"" + role + "\" not found.\n");

  return result;
}





/*
 * Finds a place of the Petri net given an activity and a role.
 *
 * \param  activity an activity of which the id is taken
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *
PetriNet::findPlace (kc::impl_activity * activity, string role)
{
  return findPlace (intToString (activity->id->value) + role);
}





/*!
 * Finds a transition of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the transition or a NULL pointer if the place was not
 *         found.
 */
Transition *
PetriNet::findTransition (string role)
{
  Transition *result = (Transition *) roleMap[role];

  if (result == NULL)
    trace (TRACE_DEBUG,
	   "[PN]\tTransition with role \"" + role + "\" not found.\n");

  return result;
}





/*---------------------------------------------------------------------------*/


/*!
 * Implements some simple structural reduction rules for Petri nets:
 * 
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 * - If a transition has a singleton preset and postset, the transition can be
 *   removed (sequence) and the preset and postset can be merged.
 *
 * \todo
 *       - (nlohmann) improve performance
 *       - (nlohmann) implement more reduction rules
 *
 */
void
PetriNet::simplify ()
{
  trace (TRACE_DEBUG,
	 "[PN]\tPetri net size before simplification: " + information () +
	 "\n");
  trace (TRACE_INFORMATION, "Simplifying Petri net...\n");


  // a pair to store transitions to be merged
  vector < pair < string, string > >transitionPairs;

  trace (TRACE_VERY_DEBUG,
	 "[PN]\tSearching for transitions with same preset and postset...\n");
  // find transitions with same preset and postset  
  for (set < Transition * >::iterator t1 = T.begin (); t1 != T.end (); t1++)
    for (set < Transition * >::iterator t2 = t1; t2 != T.end (); t2++)
      if (*t1 != *t2)
	if ((preset (*t1) == preset (*t2))
	    && (postset (*t1) == postset (*t2)))
	  transitionPairs.push_back (pair < string,
				     string > (*((*t1)->history.begin ()),
					       *((*t2)->history.begin ())));

  trace (TRACE_VERY_DEBUG,
	 "[PN]\tFound " + intToString (transitionPairs.size ()) +
	 " transitions with same preset and postset...\n");

  // merge the found transitions
  for (unsigned int i = 0; i < transitionPairs.size (); i++)
    {
      Transition *t1 = findTransition (transitionPairs[i].first);
      Transition *t2 = findTransition (transitionPairs[i].second);

      if (t1 != NULL && t2 != NULL && t1 != t2)
	mergeTransitions (t1, t2);
    }




  trace (TRACE_VERY_DEBUG, "[PN]\tnew reduction rule\n");

  // a pair to store places to be merged
  vector < string > sequenceTransitions;
  vector < pair < string, string > >placeMerge;

  // find transitions with singelton preset and postset
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
    if (preset (*t).size () == 1 && postset (*t).size () == 1)
      {
	string id1 = *((*(preset (*t).begin ())).first->history.begin ());
	string id2 = *((*(postset (*t).begin ())).first->history.begin ());
	placeMerge.push_back (pair < string, string > (id1, id2));
	sequenceTransitions.push_back (*((*t)->history.begin ()));
      }

  // merge preset and postset
  for (unsigned int i = 0; i < placeMerge.size (); i++)
    mergePlaces (placeMerge[i].first, placeMerge[i].second);

  // remove "sequence"-transtions
  for (unsigned int i = 0; i < sequenceTransitions.size (); i++)
    {
      Transition *sequenceTransition =
	findTransition (sequenceTransitions[i]);
      if (sequenceTransition != NULL)
	removeTransition (sequenceTransition);
    }


  trace (TRACE_INFORMATION, "Simplifying complete.\n");
  trace (TRACE_DEBUG, "[PN]\tPetri net size after simplification: " +
	 information () + "\n");
}





/*!
 * Converts the created Petri net to low-level representation. Therefore the
 * following steps are taken:
 *
 * - Convert all places to low-level places.
 * - Remove all transition guards -- decisions are now taken
 *   non-deterministically.
 * - Remove arc inscriptions -- all places are low-level places anyway.
 * - Convert read arcs to loops.
 *
 * Only reset arcs remain as high-level constructs. They will be converted
 * (i.e. unfolded) in a later stage since this unfolding is target-format
 * specific.
 *
 */
void
PetriNet::makeLowLevel ()
{
  trace (TRACE_INFORMATION, "Converting Petri net to low-level...\n");

  // remove transition guards
  for (set < Transition * >::iterator t = T.begin (); t != T.end (); t++)
    (*t)->guard = "";


  // convert read arcs, remove inscriptions
  set < Arc * >readArcs;

  for (set < Arc * >::iterator f = F.begin (); f != F.end (); f++)
    {
      (*f)->inscription = "";
      if ((*f)->type == READ)
	{
	  (*f)->type = STANDARD;
	  readArcs.insert (*f);
	}
    }

  for (set < Arc * >::iterator f = readArcs.begin (); f != readArcs.end ();
       f++)
    newArc ((*f)->target, (*f)->source);

  lowLevel = true;
}





void
PetriNet::calculateInitialMarking ()
{
  findPlace("1.internal.initial")->initialMarking = 1;
  for (list < string >::iterator variable = symMan.variables.begin (); variable != symMan.variables.end (); variable++)
    findPlace("variable." + *variable)->initialMarking = 1;
}


/*---------------------------------------------------------------------------*/


/*!
 * \return id for new nodes
 */
unsigned int
PetriNet::getId ()
{
  return nextId++;
}






/*!
 * \return id for last added node
 */
unsigned int
PetriNet::id ()
{
  return nextId;
}

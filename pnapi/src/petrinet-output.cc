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

/*!
 * \file    petrinet-output.cc
 *
 * \brief   Petri Net API: file output
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   created: 2006-03-16
 *
 * \date    \$Date: 2008-06-19 11:42:06 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.11 $
 *
 * \ingroup petrinet
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <iomanip>		// (std::setw)

#include "util.h"		// helper functions (toString)
#include "petrinet.h"



using std::endl;
using std::cerr;
using std::setw;
using std::right;
using std::left;

using namespace pnapi;



/******************************************************************************
 * Functions to print information of the net and its nodes
 *****************************************************************************/


/*!
 * \brief   info file output
 *
 *          Prints information about the generated Petri net. In particular,
 *          for each place and transition all roles of the history are printed
 *          to understand the net and maybe LoLA's witness pathes later.
 *
 * \todo    Put this to the nodes.
 */
void PetriNet::output_info(ostream *output) const
{
  assert(output != NULL);

  // the places
  (*output) << "PLACES:\nID\tTYPE\t\tROLES\n";

  // the internal places
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    (*output) << (*p)->getName() << "\tinternal";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
	(*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the input places
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
  {
    (*output) << (*p)->getName() << "\tinput   ";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the output places
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
  {
    (*output) << (*p)->getName() << "\toutput  ";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the transitions
  (*output) << "\nTRANSITIONS:\n";
  (*output) << "ID\tROLES\n";

  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << (*t)->getName() + "\t";

    deque<string> names = (*t)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << (*t)->prefix + *role + "\n";
        (*output) << *role + "\n";
      else
        //(*output) << "\t" + (*t)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
  }
}





/******************************************************************************
 * DOT output of the net
 *****************************************************************************/


/*!
 * \brief   DOT-output of the arc (used by PetriNet::dotOut())
*/
string Arc::toString(bool draw_interface) const
{
  /* FIXME
  if (!draw_interface)
    if ((source->nodeType == PLACE && source->getType() != Node::INTERNAL) ||
        (target->nodeType == PLACE && target->getType() != Node::INTERNAL))
      return "";

  string result = " ";
  if (source->nodeType == PLACE)
    result += "p" + util::toString(source->id) + " -> t" + util::toString(target->id);
  else
    result += "t" + util::toString(source->id) + " -> p" + util::toString(target->id);

  result += "\t[";

  if (weight != 1)
    result += "label=\"" + util::toString(weight) + "\"";

  if ((source->nodeType == PLACE && source->getType() == Node::INTERNAL) ||
      (target->nodeType == PLACE && target->getType() == Node::INTERNAL))
    result += "weight=10000.0";

  result += "]\n";

  return result;
  */
  return "";
}





/*!
 * DOT-output of the transition. Transitions are colored
 * corresponding to their initial role.
 *
 * \note  This method might be replaced by operator<< in the future.
*/
string Transition::toString() const
{
  string result;

  //FIXME: result += " t" + util::toString(id) + "  \t";

#ifdef USING_BPEL2OWFN
  string label = getName();
#else
  string label = getName();
#endif

  switch(getType())
    {
    case(Node::INPUT):		result += "[fillcolor=orange "; break;
    case(Node::OUTPUT):		result += "[fillcolor=yellow "; break;
      /* FIXME
    case(INOUT):	result += "[fillcolor=gold ";
      result += "label=<";
      // the table size depends on the node size of .3 (inch);
      result += " <TABLE BORDER=\"1\"";
      result += " CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\"";
      result += " HEIGHT=\"21\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\"><TR>";
      result += "<TD HEIGHT=\"11\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\" BGCOLOR=\"ORANGE\">";
      result += "</TD></TR><TR>";
      result += "<TD HEIGHT=\"10\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\" BGCOLOR=\"YELLOW\">";
      result += "</TD></TR></TABLE> >"; break;
      */
    case(Node::INTERNAL):	result += "["; break;
  }


  // add labels for transitions with singleton history

  // internal activities
  if (getNameHistory().size() == 1 && getName().find("internal.empty") != string::npos)
    result += "label=\"empty\" fillcolor=gray";
  if (getNameHistory().size() == 1 && getName().find("internal.assign") != string::npos)
    result += "label=\"asgn\" fillcolor=gray";
  if (getNameHistory().size() == 1 && getName().find("internal.opaqueActivity") != string::npos)
    result += "label=\"opque\" fillcolor=gray";

  // communicating activities
  if (getNameHistory().size() == 1 && getName().find("internal.receive") != string::npos)
    result += "label=\"recv\"";
  if (getNameHistory().size() == 1 && getName().find("internal.reply") != string::npos)
    result += "label=\"reply\"";
  if (getNameHistory().size() == 1 && getName().find("internal.invoke") != string::npos)
    result += "label=\"invk\"";
  if (getNameHistory().size() == 1 && getName().find("internal.onMessage_") != string::npos)
    result += "label=\"on\\nmsg\"";
  if (getNameHistory().size() == 1 && getName().find(".onEvent.") != string::npos)
    result += "label=\"on\\nevent\"";

  // structured activities
  if (getNameHistory().size() == 1 && getName().find("internal.case") != string::npos)
    result += "label=\"case\" fillcolor=azure2";
  if (getNameHistory().size() == 1 && getName().find("internal.onAlarm") != string::npos)
    result += "label=\"on\\nalarm\" fillcolor=azure2";
  if (getNameHistory().size() == 1 && getName().find("internal.split") != string::npos)
    result += "label=\"flow\\nsplit\" fillcolor=azure2";
  if (getNameHistory().size() == 1 && getName().find("internal.join") != string::npos)
    result += "label=\"flow\\njoin\" fillcolor=azure2";
  if (getNameHistory().size() == 1 && getName().find("internal.leave") != string::npos)
    result += "label=\"leave\\nloop\" fillcolor=azure2";
  if (getNameHistory().size() == 1 && getName().find("internal.loop") != string::npos)
    result += "label=\"enter\\nloop\" fillcolor=azure2";

  // everything about links
  if (getNameHistory().size() == 1 && getName().find(".setLinks") != string::npos)
    result += "label=\"tc\" fillcolor=darkseagreen1";
  if (getNameHistory().size() == 1 && getName().find(".evaluate") != string::npos)
    result += "label=\"jc\\neval\" fillcolor=darkseagreen1";
  if (getNameHistory().size() == 1 && getName().find(".skip") != string::npos)
    result += "label=\"skip\" fillcolor=darkseagreen1";
  if (getNameHistory().size() == 1 && getName().find(".reset_false") != string::npos)
    result += "label=\"reset\\nlink\" fillcolor=darkseagreen1";
  if (getNameHistory().size() == 1 && getName().find(".reset_true") != string::npos)
    result += "label=\"reset\\nlink\" fillcolor=darkseagreen1";


  // stopping
  if (getNameHistory().size() == 1 && getName().find(".stopped.") != string::npos)
    result += "label=\"stop\" fillcolor=darksalmon";


  if (!labels_.empty())
  {
    result += " label=\"{";
    for (set<string>::const_iterator it = labels_.begin(); it != labels_.end(); it++)
    {
      if (it != labels_.begin())
        result += " ";
      result += (*it);
    }
    result += "}\"";
  }

  result += "]\n";

  // FIXME:result += " t" + util::toString(id) + "_l\t[style=invis];\n";
  // FIXME:result += " t" + util::toString(id) + "_l -> t" + util::toString(id) + " [headlabel=\"" + label + "\" ]\n";

  return result;
}





/*!
 * \brief   DOT-output of the place (used by PetriNet::dotOut())
 *
 *          DOT-output of the place. Places are colored corresponding to their
 *          initial role.
*/
string Place::toString() const
{
  string result;

  //FIXME: result += " p" + util::toString(id) + "  \t[";//label=\"\"";

#ifdef USING_BPEL2OWFN
    string label;
    if ( wasExternal != "")
      label = wasExternal;
  else
    label = getName();

  if (type == IN || type == OUT)
  {
    // strip "in." or "out."
    label = label.substr(label.find_first_of(".")+1, label.length());

    // if channel is instantiated, strip the ".instance_" part and replace
    if (label.find(".instance_") != string::npos)
    {
      string label_prefix = label.substr(0, label.find(".instance_"));
      string label_suffix = label.substr(label.find(".instance_")+10, label.length());
      label = label_prefix + " (" + label_suffix + ")";
    }
  }

#else
  string label;
  // FIXME
  //if ( wasInterface )
  //  label = wasExternal;
  //else
    label = getName();
#endif

  // truncate prefix (could be a problem with ports later on, but looks nice)
  //  if (type != Node::INTERNAL)
  // label = label.substr(label.find_last_of(".")+1, label.length());

  if (tokens_ == 1)
    result += "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
  else if (tokens_ > 1)
    result += "label=\"" + ::util::toString(tokens_) + "\" fontcolor=black fontname=\"Helvetica\" fontsize=10";

  switch (getType())
    {
    case (Node::INPUT):  result += "fillcolor=orange"; break;
    case (Node::OUTPUT): result += "fillcolor=yellow"; break;
    default:    break;
    }

  /* FIXME
  if (isFinal)
    result += "fillcolor=gray";
  else if (firstMemberIs("!link."))
    result += "fillcolor=red";
  else if (firstMemberIs("link."))
    result += "fillcolor=green";
  else if (firstMemberIs("variable."))
    result += "fillcolor=cyan";
  else if (historyContains("1.internal.clock"))
    result += "fillcolor=seagreen";
  else if (wasExternal != "")
    result += "fillcolor=lightgoldenrod1";
  */

  result += "]\n";

  // FIXME:result += " p" + util::toString(id) + "_l\t[style=invis];\n";

  if (getType() == Node::OUTPUT)
    ;// FIXME:result += " p" + util::toString(id) + " -> p" + util::toString(id) + "_l [taillabel=\"" + label + "\" ]\n";
  else
    ;// FIXME:result += " p" + util::toString(id) + "_l -> p" + util::toString(id) + " [headlabel=\"" + label + "\" ]\n";

  return result;
}





/*!
 * \brief   DOT (Graphviz) output
 *
 *          Creates a DOT output (see http://www.graphviz.org) of the Petri
 *          net. It uses the digraph-statement and adds labels to transitions,
 *          places and arcs if neccessary.
 *
 * \param   output the output stream
 * \param   draw_interface if set to true the interface will be drawn (standard)
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_dot(ostream *output, bool draw_interface) const
{
  assert(output != NULL);

  (*output) << "digraph N {" << endl;
  (*output) << " graph [fontname=\"Helvetica\" nodesep=0.25 ranksep=\"0.25\" fontsize=10 remincross=true label=\"";

  //if (globals::reduction_level == 5)
  //  (*output) << "structurally reduced ";

  //(*output) << "Petri net generated from " << globals::filename << "\"]" << endl;
  // REMEMBER The table size of the INOUT transitions depends on the size of a node!
  //          So a width of .3 (in) results in 21 pixel table width ( 0.3 in * 72 dpi ).
  (*output) << " node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white]" << endl;
  (*output) << " edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"]" << endl << endl;


  // list the places
  (*output) << "\n // places" << endl;
  (*output) << " node [shape=circle];" << endl;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    (*output) << (*p)->toString();

  if (draw_interface)
  {
    for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
      (*output) << (*p)->toString();
    for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
      (*output) << (*p)->toString();



    // list the transitions
    (*output) << "\n // transitions" << endl;
    (*output) << " node [shape=box]" << endl;
    for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
      (*output) << (*t)->toString();


    // the inner of the net
    (*output) << "\n // cluster the inner of the net" << endl;
    (*output) << " subgraph cluster1\n {\n ";
    for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
      //FIXME: (*output) << " t" << (*t)->id << " t" << (*t)->id << "_l";
    (*output) << "\n ";
    for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    {
      if ((*p)->getTokenCount() > 0)
        ;// FIXME: (*output) << " p" << (*p)->id;
      else
        ;// FIXME:(*output) << " p" << (*p)->id << " p" << (*p)->id << "_l";
    }

    if (draw_interface)
      (*output) << "\n  label=\"\" style=\"dashed\"" << endl;
    else
      (*output) << "\n  label=\"\" style=invis" << endl;

    (*output) << " }" << endl;
  }


  /* FIXME: use interfacePlacesByPort_
    // draw the ports
    for (map<string, set<Place *> >::const_iterator port = ports.begin();
         port != ports.end(); port++)
    {
        (*output) << " // cluster for port " << port->first << endl;
        (*output) << " subgraph cluster_" << port->first << "\n {\n";
        (*output) << "  label=\"" << port->first << "\";" << endl;
        (*output) << "  style=\"filled,rounded\"; bgcolor=grey95  labelloc=t;" << endl;
        //(*output) << "  rankdir=TB;" << endl;

        for (set<Place*>::const_iterator place = port->second.begin();
             place != port->second.end(); place++)
        {
            // FIXME:(*output) << "  p" + util::toString((*place)->id) << ";" << endl;
            // FIXME:(*output) << "  p" + util::toString((*place)->id) << "_l;" << endl;

            // make the port more compact
            for (set<Place*>::const_iterator place2 = port->second.begin();
                 place2 != port->second.end(); place2++)
            {
                if ( (*place) != (*place2) )
		  ;// FIXME:(*output) << "  p" + util::toString((*place)->id) + " -> p" + util::toString((*place2)->id) + " [style=invis];" << endl;
            }
        }

        (*output) << " }" << endl << endl;
    }
  */



  // list the arcs
  (*output) << "\n // arcs" << endl;
  (*output) << " edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black]" << endl;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
    (*output) << (*f)->toString(draw_interface);

  (*output) << endl << "}" << endl;
}





/******************************************************************************
 * Petri net file formats
 *****************************************************************************/

/*!
 * \brief   PNML (Petri Net Markup Language) output
 *
 *          Outputs the net in PNML (Petri Net Markup Language).
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \note    - See https://savannah.gnu.org/task/?6263
 *          - The complete syntax can be found at
 *            http://www.petriweb.org/specs/epnml-1.1/pnmldef.pdf
 */
void PetriNet::output_pnml(ostream *output) const
{
  assert(output != NULL);

  (*output) << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl << endl;
  (*output) << "<!--" << endl;
  //(*output) << "  Petri net created by " << getPackageString() << " reading file " << globals::filename << "." << endl;
  (*output) << "  See http://www.gnu.org/software/bpel2owfn for more details." << endl;
  (*output) << "-->" << endl << endl;

  (*output) << "<pnml>" << endl;
  (*output) << "  <net id=\"bpel-net\" type=\"owfn\">" << endl << endl;


  // places
  (*output) << "<!-- input places -->" << endl;
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>input</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }

  (*output) << "<!-- output places -->" << endl;
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>output</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }

  (*output) << "<!-- internal places -->" << endl;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    if ((*p)->getTokenCount() > 0)
    {
      (*output) << "      <initialMarking>" << endl;
      (*output) << "        <text>" << (*p)->getTokenCount() << "</text>" << endl;
      (*output) << "      </initialMarking>" << endl;
    }
    (*output) << "    </place>" << endl << endl;
  }


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*t)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "    </transition>" << endl << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++, arcNumber++)
  {
    (*output) << "    <arc id=\"a" << arcNumber << "\" ";
#ifdef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->getSourceNode().getName() << "\" ";
    (*output) << "target=\"" << (*f)->getTargetNode().getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->getSourceNode().getName() << "\" ";
    (*output) << "target=\"" << (*f)->getTargetNode().getName() << "\">" << endl;
#endif
    (*output) << "      <inscription>\n        <text>" << (*f)->getWeight() << "</text>\n      </inscription>" << endl;
    (*output) << "    </arc>" << endl;
  }
  (*output) << endl;

  (*output) << "    <toolspecific tool=\"owfn\" version=\"1.0\">" << endl;
  (*output) << "      <finalmarking xmlns=\"http://www.informatik.hu-berlin.de/top/tools4bpel\">" << endl;
  (*output) << "        <text>";

  /* FIXME
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    if ((*p)->isFinal)
      (*output) << (*p)->getName() << " ";
  */

  (*output) << "</text>" << endl;
  (*output) << "      </finalmarking>" << endl;
  (*output) << "    </toolspecific>" << endl;
  (*output) << "  </net>" << endl;
  (*output) << "</pnml>" << endl;
}





/*!
 * \brief   low-level PEP output
 *
 *          Outputs the net in low-level PEP notation.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo
 *          - Add syntax reference.
 *          - Does PEP also work with non-safe Petri nets?
 */
void PetriNet::output_pep(ostream *output) const
{
  assert(output != NULL);

  // header
  (*output) << "PEP" << endl << "PTNet" << endl << "FORMAT_N" << endl;

  // places (only internal)
  (*output) << "PL" << endl;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << (*p)->id << "\"" << (*p)->getName() << "\"80@40";
    if ((*p)->getTokenCount() > 0)
      (*output) << "M" << (*p)->getTokenCount();
    (*output) << "k1" << endl;
  }

  // transitions
  (*output) << "TR" << endl;
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
    ;// FIXME:(*output) << (*t)->id << "\"" << (*t)->getName() << "\"80@40" << endl;

  // arcs from transitions to places (no output places)
  /* FIXME
  (*output) << "TP" << endl;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if (((*f)->getSourceNode()->nodeType) == TRANSITION)
      if ( outputPlaces_.find(static_cast<Place*>((*f)->getTargetNode())) == outputPlaces_.end())
        (*output) << (*f)->getSourceNode()->id << "<" << (*f)->getTargetNode()->id << "w" << (*f)->getWeight() << endl;

  // arcs from places to transitions
  (*output) << "PT" << endl;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if (((*f)->getSourceNode()->nodeType) == PLACE)
      if ( inputPlaces_.find(static_cast<Place*>((*f)->getSourceNode())) == inputPlaces_.end())
        (*output) << (*f)->getSourceNode()->id << ">" << (*f)->getTargetNode()->id << "w" << (*f)->getWeight() << endl;
  */
}





/*!
 * \brief   INA output
 *
 *          Outputs the net in INA format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \node    The complete syntax can be found at
 *          http://www2.informatik.hu-berlin.de/lehrstuehle/automaten/ina/node14.html
 *
 * \todo    Check whether arc weights are in the correct order.
 */
void PetriNet::output_ina(ostream *output) const
{
  assert(output != NULL);

  // net header
  //(*output) << "P   M   PRE,POST  NETZ 1:" << globals::filename.substr(0, globals::filename.find_first_of(".")) << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << setw(3) << (*p)->id << " " << (*p)->getTokenCount() << "     ";

    for (set<Node*>::iterator t = (*p)->getPreset().begin(); t != (*p)->getPreset().end(); t++)
    {
      if (t != (*p)->getPreset().begin())
        (*output) << " ";

      // FIXME:(*output) << (*t)->id << ":" << arc_weight(*t, *p);
    }

    (*output) << ", ";

    for (set<Node*>::iterator t = (*p)->getPostset().begin(); t != (*p)->getPostset().end(); t++)
    {
      if (t != (*p)->getPostset().begin())
        (*output) << " ";

      // FIXME:(*output) << (*t)->id << ":" << arc_weight(*p, *t);
    }

    (*output) << endl;
  }

  (*output) << "@" << endl;


  // place names
  (*output) << "place nr.             name capacity time" << endl;

  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << right << setw(8) << (*p)->id << ": ";
    (*output) << left << setw(16) << (*p)->getName();
    (*output) << "       oo    0" << endl;
  }

  (*output) << "@" << endl;


  // transition names
  (*output) << "trans nr.             name priority time" << endl;

  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    // FIXME:(*output) << right << setw(8) << (*t)->id << ": ";
    (*output) << left << setw(16) << (*t)->getName();
    (*output) << "        0    0" << endl;
  }

  (*output) << "@" << endl;

  // info (ignored after last '@')
  //(*output) << "generated by: " << getPackageString() << endl;
  //(*output) << "input file:   `" << globals::filename << "'" << endl;
  //(*output) << "net size:     " << information() << endl;
}





/*!
 * \brief   SPIN output
 *
 *          Outputs the net in SPIN format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_spin(ostream *output) const
{
  assert(output != NULL);
  // net header
  (*output) << "/* NO 1-safe */" << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << "byte p" << (*p)->id << "=" << (*p)->getTokenCount() << ";" << endl;
  }

  (*output) << "int {" << endl;
  (*output) << "\tdo" << endl;


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    int follower=0;
    (*output) << "\t::atomic { (";

    if ((*t)->getPreset().empty())
    {
      (*output) << "0";
    }
    else
    {
      for (set<Node *>::iterator p = (*t)->getPreset().begin(); p != (*t)->getPreset().end(); p++)
      {
        if(follower)
        {
          (*output) << "&&";
        }
        else
        {
          follower=1;
        }
        // FIXME:(*output) << "(p" << (*p)->id << ">=" << arc_weight(*p,*t) << ")";
      }
    }

    (*output) << ") -> ";

    for (set<Node *>::iterator p = (*t)->getPreset().begin(); p != (*t)->getPreset().end(); p++)
    {
      // FIXME:(*output) << "p" << (*p)->id << "=p" << (*p)->id << "-" << arc_weight(*p,*t) << ";";
    }

    (*output) << endl << "\t\t\t";

    for (set<Node *>::iterator p = (*t)->getPostset().begin(); p != (*t)->getPostset().end(); p++)
    {
      // FIXME:(*output) << "p" << (*p)->id << "=p" << (*p)->id << "+" << arc_weight(*t,*p) << ";";
    }

    (*output) << " }" << endl;

  }

  (*output) << "\tod;" << endl;
  (*output) << "}" << endl;
}





/*!
 * \brief   APNN (Abstract Petri Net Notation) output
 *
 *          Outputs the net in APNN (Abstract Petri Net Notation).
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
*/
void PetriNet::output_apnn(ostream *output) const
{
  assert(output != NULL);

  //(*output) << "\\beginnet{" << globals::filename << "}" << endl << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    (*output) << "  \\place{" << (*p)->getName() << "}{";
    if ((*p)->getTokenCount() > 0)
      (*output) << "\\init{" << (*p)->getTokenCount() << "}";
    (*output) << "}" << endl;
  }
  (*output) << endl;

  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << "  \\transition{" << (*t)->getName() << "}{}" << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++, arcNumber++)
  {
    /* FIXME
    // ignore input places
    if ((*f)->getSourceNode()->nodeType == PLACE)
      if ( inputPlaces_.find(static_cast<Place*>((*f)->getSourceNode())) != inputPlaces_.end())
        continue;

    // ignore output places
    if ((*f)->getTargetNode()->nodeType == PLACE)
      if ( outputPlaces_.find(static_cast<Place*>((*f)->getTargetNode())) != outputPlaces_.end())
        continue;
    */

    (*output) << "  \\arc{a" << arcNumber << "}{ ";
    (*output) << "\\from{" << (*f)->getSourceNode().getName() << " } ";
    (*output) << "\\to{" << (*f)->getTargetNode().getName() << "} \\weight{" << (*f)->getWeight() << "} }" << endl;
  }
  (*output) << endl;

  (*output) << "\\endnet" << endl;
}





/*!
 * \brief   LoLA-output
 *
 *          Outputs the net in LoLA-format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_lola(ostream *output) const
{
  assert(output != NULL);

  //(*output) << "{ Petri net created by " << getPackageString() << " reading " << globals::filename << " }" << endl << endl;

  {
    /************************
     * STANDARD LOLA FORMAT *
     ************************/

    // places (only internal)
    (*output) << "PLACE" << endl;
    unsigned int count = 1;
    for (set<Place *>::iterator p = places_.begin(); p != places_.end(); count++, p++)
    {
      (*output) << "  " << (*p)->getName();

      if (count < places_.size())
        (*output) << "," << endl;
    }
    (*output) << endl << ";" << endl << endl << endl;
  }


  // from here on, both standard LoLA and I/O-annotated LoLA formats are equal


  // initial marking
  (*output) << "MARKING" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    if ((*p)->getTokenCount() > 0)
    {
      if (count++ != 1)
        (*output) << "," << endl;

      (*output) << "  " << (*p)->getName() << ":\t" << (*p)->getTokenCount();
    }
  }
  (*output) << endl << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->getName() << endl;

    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = (*t)->getPreset().begin(); pre != (*t)->getPreset().end(); count++, pre++)
    {
      // ignore input places
      // FIXME: if ( (*pre)->nodeType == PLACE )
        if ( inputPlaces_.find(static_cast<Place*>(*pre)) != inputPlaces_.end())
          continue;

	(*output) << "  " << (*pre)->getName() << ":\t" << findArc(**pre, **t)->getWeight();

      if (count < (*t)->getPreset().size())
        (*output) << "," << endl;
    }
    (*output) << ";" << endl;

    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = (*t)->getPostset().begin(); post != (*t)->getPostset().end(); count++, post++)
    {
      // ignore output places
      //FIXME: if ( (*post)->nodeType == PLACE )
        if ( outputPlaces_.find(static_cast<Place*>(*post)) != outputPlaces_.end())
          continue;

	(*output) << "  " << (*post)->getName() << ":\t" << findArc(**t, **post)->getWeight();

      if (count < (*t)->getPostset().size())
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
 * \brief   oWFN-output
 *
 *          Outputs the net in oWFN-format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 * \pre     PetriNet::calculate_max_occurrences() called
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_owfn(ostream *output) const
{
  assert(output != NULL);

  (*output) << "{" << endl;
  //(*output) << "  generated by: " << getPackageString() << endl;
#ifdef USING_BPEL2OWFN
  (*output) << "  input file:   `" << globals::filename << "' (process `" << globals::ASTEmap[1]->attributes["name"] << "')" << endl;
#endif
#ifndef USING_BPEL2OWFN
  //(*output) << "  input file:   `" << globals::filename << "'" << endl;
#endif
  //(*output) << "  invocation:   `" << getInvocation() << "'" << endl;
  //(*output) << "  net size:     " << information() << endl;
  (*output) << "}" << endl << endl;

  // places
  (*output) << "PLACE" << endl;

  // internal places
  (*output) << "  INTERNAL" << endl;
  (*output) << "    ";

  unsigned int count = 1;
  for (set<Place *>::iterator p = internalPlaces_.begin(); p != internalPlaces_.end(); count++, p++)
  {
    if( (*p)->getCapacity() > 0)
    {
      (*output) << "SAFE " << (*p)->getCapacity() << " : ";
    }
#ifdef USING_BPEL2OWFN
    (*output) << (*p)->getName();
#else
    (*output) << (*p)->getName();
#endif
    if (count < internalPlaces_.size())
      (*output) << "; ";
  }
  (*output) << ";" << endl << endl;


  // input places
  (*output) << "  INPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); count++, p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    " << (*p)->getName();
#else
    (*output) << "    " << (*p)->getName();
#endif

    /*
      if ((*p)->max_occurrences != UINT_MAX && (*p)->max_occurrences != 0)
        (*output) << " {$ MAX_OCCURRENCES = " << (*p)->max_occurrences << " $}";
      if ((*p)->max_occurrences == UINT_MAX)
        (*output) << " {$ MAX_OCCURRENCES = -1 $}";
    */

    if (count < inputPlaces_.size())
      (*output) << "," << endl;
  }
  (*output) << ";" << endl << endl;


  // output places
  (*output) << "  OUTPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); count++, p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    " << (*p)->getName();
#else
    (*output) << "    " << (*p)->getName();
#endif

    /*
      if ((*p)->max_occurrences != UINT_MAX && (*p)->max_occurrences != 0)
        (*output) << " {$ MAX_OCCURRENCES = " << (*p)->max_occurrences << " $}";
      if ((*p)->max_occurrences == UINT_MAX)
        (*output) << " {$ MAX_OCCURRENCES = -1 $}";
    */

    if (count < outputPlaces_.size())
      (*output) << "," << endl;
  }
  (*output) << ";" << endl << endl;


  // initial marking
  (*output) << endl << "INITIALMARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = internalPlaces_.begin(); p != internalPlaces_.end(); p++)
  {
    if ((*p)->getTokenCount() > 0)
    {
      if (count++ != 1)
        (*output) << "," << endl;

#ifdef USING_BPEL2OWFN
      (*output) << "  " << (*p)->getName() << ":\t" << (*p)->getTokenCount();
#else
      (*output) << "  " << (*p)->getName() << ":\t" << (*p)->getTokenCount();
#endif

      //FIXME: if ((*p)->historyContains("1.internal.initial"))
        (*output) << " {initial place}";
    }
  }
  (*output) << ";" << endl << endl;


  // final marking

  // if the set of final markings is not empty: iterate and add all final markings
  // else: there is only one final marking, have a look at all places and check isFinal.

  /*
  if (final_set_list.size() <= 1)
  {
    (*output) << "FINALMARKING" << endl << "  ";
    if (final_set_list.begin() != final_set_list.end())
    {
      set<pair<Place *, unsigned int> > finalMarking = *(final_set_list.begin());
      bool first_place = true;
      for( set<pair<Place *, unsigned int> >::const_iterator p = finalMarking.begin(); p != finalMarking.end(); p++)
      {
        if (!first_place)
        {
          (*output) << ", ";
        }

#ifdef USING_BPEL2OWFN
        (*output) << (p->first)->getName();
#else
        (*output) << (p->first)->getName();
#endif
        assert(p->second > 0);
        if (p->second > 1)
        {
          (*output) << " : " << p->second;
        }
        first_place = false;
      }
      // (*output) << ";" << endl;
    }
  }
  else
  {
    (*output) << "FINALCONDITION" << endl << "  (";

    // iterate the final set list and conjugate the disjunctive place sets in order to create the final condition
    bool first_set = true;
    for (list< set<pair<Place *,unsigned int> > >::const_iterator final_set = final_set_list.begin(); final_set != final_set_list.end(); final_set++)
    {
      if (!first_set)
      {
        (*output) << " OR ";
      }

      if ((*final_set).size() == 1)
      {
        pair<Place*, unsigned int> p = (*((*final_set).begin()));
#ifdef USING_BPEL2OWFN
        (*output) << "( (" << p.first->getName() << "=" << p.second << ") AND ALL_OTHER_PLACES_EMPTY )";
#else
        (*output) << "( (" << p.first->getName() << "=" << p.second << ") AND ALL_OTHER_PLACES_EMPTY )";
#endif
      }
      else
      {
        (*output) << "( ";
        bool first_place = true;
        for( set<pair<Place *,unsigned int> >::const_iterator p = (*final_set).begin(); p != (*final_set).end(); p++)
        {
          if (!first_place)
            (*output) << " AND ";

#ifdef USING_BPEL2OWFN
          (*output) << "(" << (p->first)->getName() << "=" p->second << ")";
#else
          (*output) << "(" << (p->first)->getName() << "=" << p->second << ")";
#endif
          first_place = false;
        }
        (*output) << " AND ALL_OTHER_PLACES_EMPTY )";
      }
      first_set = false;
    }
    (*output) << " )";

#ifdef USING_BPEL2OWFN
    (*output) << " AND ALL_OTHER_EXTERNAL_PLACES_EMPTY";
#endif

  }
  (*output) << ";" << endl << endl << endl;
  */


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "TRANSITION " << (*t)->getName();
#else
    (*output) << "TRANSITION " << (*t)->getName();
#endif
    switch( (*t)->getType() )
      {
      case (Node::INTERNAL):	(*output) << endl; break;
      case(Node::INPUT):		(*output) << " { input }" << endl; break;
      case(Node::OUTPUT):	(*output) << " { output }" << endl; break;
	// FIXME: case(INOUT):	(*output) << " { input/output }" << endl; break;
    }

    (*output) << "  CONSUME ";
    count = 1;
    for (set<Node *>::iterator pre = (*t)->getPreset().begin(); pre != (*t)->getPreset().end(); count++, pre++)
    {
#ifdef USING_BPEL2OWFN
      (*output) << (*pre)->getName();
#else
      (*output) << (*pre)->getName();
#endif

      unsigned int weight = findArc(**pre, **t)->getWeight();
      if (weight != 1)
        (*output) << ":" << weight;

      if (count < (*t)->getPreset().size())
        (*output) << ", ";
    }
    (*output) << ";" << endl;

    (*output) << "  PRODUCE ";

    count = 1;
    for (set<Node *>::iterator post = (*t)->getPostset().begin(); post != (*t)->getPostset().end(); count++, post++)
    {
#ifdef USING_BPEL2OWFN
      (*output) << (*post)->getName();
#else
      (*output) << (*post)->getName();
#endif

      unsigned int weight = findArc(**t, **post)->getWeight();
      if (weight != 1)
        (*output) << ":" << weight;

      if (count < (*t)->getPostset().size())
        (*output) << ", ";
    }

    (*output) << ";" << endl << endl;
  }
  //(*output) << endl << "{ END OF FILE `" << globals::output_filename << ".owfn' }" << endl << endl;
}




/*!
 * \brief   GaSTeX-Output
 *
 *          Outputs the Petri net in GasTeX format
 *
 * \pre     output != NULL
 *
 * \todo    Write the method!
 */
void PetriNet::output_gastex(ostream *output) const
{

}




/*!
 * \brief   outputs the Petri net
 *
 *          Stream the PetriNet object to a given output stream, using the
 *          file format set before uing PetriNet::set_format().
 *
 * \param   os  an output stream to which the net is streamed
 * \param   obj a PetriNet object
 */
ostream& pnapi::operator<< (ostream& os, const pnapi::PetriNet &obj)
{
  switch (obj.format_)
    {
    case(PetriNet::FORMAT_APNN):	obj.output_apnn(&os); break;
    case(PetriNet::FORMAT_DOT):	obj.output_dot(&os, true); break;
    case(PetriNet::FORMAT_INA):	obj.output_ina(&os); break;
    case(PetriNet::FORMAT_SPIN):	obj.output_spin(&os); break;
    case(PetriNet::FORMAT_INFO):	obj.output_info(&os); break;
    case(PetriNet::FORMAT_LOLA):	obj.output_lola(&os); break;
    case(PetriNet::FORMAT_OWFN):	obj.output_owfn(&os); break;
    case(PetriNet::FORMAT_PEP):	obj.output_pep(&os); break;
    case(PetriNet::FORMAT_PNML):	obj.output_pnml(&os); break;
    case(PetriNet::FORMAT_GASTEX): obj.output_gastex(&os); break;
    default:		break;
  }

  return os;
}

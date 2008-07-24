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
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   created: 2006-03-16
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.101 $
 *
 * \ingroup petrinet
 */
/******************************************************************************
 * Headers
 *****************************************************************************/
#include <cassert>		// for assert
#include <iomanip>		// (std::setw)
#include "helpers.h"		// helper functions (toString)
#include "options.h"		// (parameters)
#include "pnapi.h"
using std::endl;
using std::setw;
using std::right;
using std::left;
/******************************************************************************
 * Functions to print information of the net and its nodes
 *****************************************************************************/
/*!
 * \brief   the name of the type
 *
 *          dummy-implementation of virtual function
 */
string Node::nodeTypeName() const
{
  return "";
}
/*!
 * \brief   the name of the type
 *
 * \return  the name of the node type, i.e., "place"
 */
string Place::nodeTypeName() const
{
  return "place";
}
/*!
 * \brief   the name of the type
 *
 * \return  the name of the node type, i.e., "transition"
 */
string Transition::nodeTypeName() const
{
  return "transition";
}
/*!
 * \brief   the short name of the node
 *
 *          dummy-implementation of virtual function
 */
string Node::nodeShortName() const
{
  return "";
}
/*!
 * \brief   the name of the node
 *
 * \return  the (nice) name of the node for DOT output
 */
string Node::nodeName() const
{
  string result = *history.begin();
  if ( type == INTERNAL || nodeType == TRANSITION )
  {
    result = result.substr(result.find_last_of(".")+1,result.length());
    result = prefix + result;
  }
  return result;
}
/*!
 * \brief   the name of the node
 *
 * \return  the name of the node for DOT output
 */
string Node::nodeFullName() const
{
  string result = *history.begin();
  if ( type == INTERNAL || nodeType == TRANSITION )
    result = prefix + result;
  return result;
}
/*!
 * \brief   the short name of the place
 *
 * \return  the short name of the place, e.g. for LoLA output
 */
string Place::nodeShortName() const
{
  if (history.begin()->find("link") != string::npos || history.begin()->find("!link") != string::npos)
    return *history.begin();
  if (type == INTERNAL)
    return ("p" + toString(id));
  if (type == IN)
    return ("in" + toString(id));
  if (type == OUT)
    return ("out" + toString(id));
  
  return *history.begin();
}
/*!
 * \brief   the short name of the transition
 *
 * \return  the short name of the transition, e.g. for LoLA output
 */
string Transition::nodeShortName() const
{
  return ("t" + toString(id));
}
/*!
 * \brief   statistical output
 *
 * \return  string containing information about the net
 */
string PetriNet::information() const
{
  string result = "|P|=" + toString(P.size() + P_in.size() + P_out.size());
  result += ", |P_in|= " + toString(P_in.size());
  result += ", |P_out|= " + toString(P_out.size());
  result += ", |T|=" + toString(T.size());
  result += ", |F|=" + toString(F.size());
  return result;
}
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
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\tinternal";
    
    for (list<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
      if (role == (*p)->history.begin())
        (*output) << "\t" + (*p)->prefix + *role + "\n";
      else
        (*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
  }
  
  // the input places
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\tinput   ";
    
    for (list<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
      if (role == (*p)->history.begin())
        (*output) << "\t" + (*p)->prefix + *role + "\n";
      else
        (*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
  }
  
  // the output places
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
  {
    (*output) << (*p)->nodeShortName() << "\toutput  ";
    
    for (list<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
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
    
    for (list<string>::iterator role = (*t)->history.begin(); role != (*t)->history.end(); role++)
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
 * \brief   DOT-output of the arc (used by PetriNet::dotOut())
*/
string Arc::output_dot(bool draw_interface) const
{
  if (!draw_interface)
    if ((source->nodeType == PLACE && source->type != INTERNAL) ||
        (target->nodeType == PLACE && target->type != INTERNAL))
      return "";
  
  string result = " ";
  if (source->nodeType == PLACE)
    result += "p" + toString(source->id) + " -> t" + toString(target->id);
  else
    result += "t" + toString(source->id) + " -> p" + toString(target->id);
  
  result += "\t[";
  
  if (weight != 1)
    result += "label=\"" + toString(weight) + "\"";
  
  if ((source->nodeType == PLACE && source->type == INTERNAL) ||
      (target->nodeType == PLACE && target->type == INTERNAL))
    result += "weight=10000.0";
  
  result += "]\n";
  
  return result;
}
/*!
 * \brief   DOT-output of the transition (used by PetriNet::dotOut())
 *
 *          DOT-output of the transition. Transitions are colored
 *          corresponding to their initial role.
 *
 * \todo    Explain colors used.
*/
string Transition::output_dot() const
{
  string result;
  
  result += " t" + toString(id) + "  \t";
#ifdef USING_BPEL2OWFN
// <Dirk.F start> use full node names
//  string label = nodeShortName();
  string label = nodeFullName();
// <Dirk.F end>
#else
  string label = nodeName();
#endif
  
  
  switch(type)
  {
    case(IN):		result += "[fillcolor=orange "; break;
    case(OUT):		result += "[fillcolor=yellow "; break;
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
    case(INTERNAL):	result += "["; break;
  }
  
// <Dirk.F start> ignore BPEL qualifiers
/*
  // add labels for transitions with singleton history
  
  // internal activities
  if (history.size() == 1 && history.begin()->find("internal.empty") != string::npos)
    result += "label=\"empty\" fillcolor=gray";
  if (history.size() == 1 && history.begin()->find("internal.assign") != string::npos)
    result += "label=\"asgn\" fillcolor=gray";
  if (history.size() == 1 && history.begin()->find("internal.opaqueActivity") != string::npos)
    result += "label=\"opque\" fillcolor=gray";
  
  // communicating activities
  if (history.size() == 1 && history.begin()->find("internal.receive") != string::npos)
    result += "label=\"recv\"";
  if (history.size() == 1 && history.begin()->find("internal.reply") != string::npos)
    result += "label=\"reply\"";
  if (history.size() == 1 && history.begin()->find("internal.invoke") != string::npos)
    result += "label=\"invk\"";
  if (history.size() == 1 && history.begin()->find("internal.onMessage_") != string::npos)
    result += "label=\"on\\nmsg\"";
  if (history.size() == 1 && history.begin()->find(".onEvent.") != string::npos)
    result += "label=\"on\\nevent\"";
  
  // structured activities
  if (history.size() == 1 && history.begin()->find("internal.case") != string::npos)
    result += "label=\"case\" fillcolor=azure2";
  if (history.size() == 1 && history.begin()->find("internal.onAlarm") != string::npos)
    result += "label=\"on\\nalarm\" fillcolor=azure2";
  if (history.size() == 1 && history.begin()->find("internal.split") != string::npos)
    result += "label=\"flow\\nsplit\" fillcolor=azure2";
  if (history.size() == 1 && history.begin()->find("internal.join") != string::npos)
    result += "label=\"flow\\njoin\" fillcolor=azure2";
  if (history.size() == 1 && history.begin()->find("internal.leave") != string::npos)
    result += "label=\"leave\\nloop\" fillcolor=azure2";
  if (history.size() == 1 && history.begin()->find("internal.loop") != string::npos)
    result += "label=\"enter\\nloop\" fillcolor=azure2";
  
  // everything about links
  if (history.size() == 1 && history.begin()->find(".setLinks") != string::npos)
    result += "label=\"tc\" fillcolor=darkseagreen1";
  if (history.size() == 1 && history.begin()->find(".evaluate") != string::npos)
    result += "label=\"jc\\neval\" fillcolor=darkseagreen1";
  if (history.size() == 1 && history.begin()->find(".skip") != string::npos)
    result += "label=\"skip\" fillcolor=darkseagreen1";
  if (history.size() == 1 && history.begin()->find(".reset_false") != string::npos)
    result += "label=\"reset\\nlink\" fillcolor=darkseagreen1";
  if (history.size() == 1 && history.begin()->find(".reset_true") != string::npos)
    result += "label=\"reset\\nlink\" fillcolor=darkseagreen1";
  
  
  // stopping
  if (history.size() == 1 && history.begin()->find(".stopped.") != string::npos)
    result += "label=\"stop\" fillcolor=darksalmon";
*/
  // use coloring for places according to BOM
  // task transition
  if (history.size() == 1 	&& history.begin()->find(".fire") != string::npos
	  && (   history.begin()->find("task.") != string::npos 
		  || history.begin()->find("callToTask.") != string::npos
		  || history.begin()->find("callToProcess.") != string::npos
		  || history.begin()->find("callToTask.") != string::npos
		  || history.begin()->find("callToService.") != string::npos))
  {
	  if ( history.begin()->find("inputCriterion") != string::npos )
		  result += "fillcolor=khaki1";
	  else if  ( history.begin()->find("outputCriterion") != string::npos )
		  result += "fillcolor=khaki3";
	  else
		  result += "fillcolor=khaki2";
  }
  // split/merge/fork/join
  if (history.size() == 1
	  && (		history.begin()->find(".fire") != string::npos
			  ||history.begin()->find(".activate") != string::npos )
	  && (		history.begin()->find("fork.") != string::npos
			  ||history.begin()->find("decision.") != string::npos
			  ||history.begin()->find("merge.") != string::npos
			  ||history.begin()->find("join.") != string::npos))
  {
      result += "fillcolor=skyblue";
  }

  
  // format the full label
  string formatLabel = "";	// start with empty string
  string fullLabel = label;
  int counter = 0;
  for(string::iterator it = fullLabel.begin(); it != fullLabel.end(); it++ )
  {
	  formatLabel = formatLabel + (*it);		// extend label by next letter
	  if ( (counter > 15 && ((*it) == '#' || (*it) == '.'))
		   || (counter > 20))
	  {
		  counter = 0;
		  formatLabel = formatLabel + "\\n";	// insert a linebreak at appropriate points
	  }
	  counter++;
  }
  label = formatLabel;		// replace earlier label with formatted string
// <Dirk.F end>
  
  if (!labels.empty())
  {
    result += " label=\"{";
    for (set<string>::const_iterator it = labels.begin(); it != labels.end(); it++)
    {
      if (it != labels.begin())
        result += " ";
      result += (*it);
    }
    result += "}\"";
  }
  
  result += "]\n";
  
  result += " t" + toString(id) + "_l\t[shape=none];\n";
// <Dirk.F start> full string already written above  
/*
  string teststring = nodeFullName();
  if (history.size() == 1)
  {
    if (teststring.find(".connection.") != string::npos)
    {
      label = "connection";
    }
    if (teststring.find("task.") != string::npos)
    {
      label = teststring.substr(teststring.find("task.")+5, teststring.length());
    }
    if (teststring.find("service.") != string::npos)
    {
      label = teststring.substr(teststring.find("service.")+8, teststring.length());
    }
    
    //if (teststring.find("process.") != string::npos)
    //{
    //  std::cerr << "yay\n";
    //  label = teststring.substr(teststring.find("process.")+8, teststring.length());
    //}
    if (teststring.find("callToTask.") != string::npos)
    {
      label = teststring.substr(teststring.find("callToTask.")+11, teststring.length());
    }
    if (teststring.find("callToService.") != string::npos)
    {
      label = teststring.substr(teststring.find("callToService.")+14, teststring.length());
    }
    if (teststring.find("callToProcess.") != string::npos)
    {
      label = teststring.substr(teststring.find("callToProcess.")+14, teststring.length());
    }
    if (teststring.find("merge.") != string::npos)
    {
      label = teststring.substr(teststring.find("merge.")+6, teststring.length());
    }
    if (teststring.find("decision.") != string::npos)
    {
      label = teststring.substr(teststring.find("decision.")+9, teststring.length());
    }
    if (teststring.find(".initialize") != string::npos)
    {
      label = teststring;
    }
    if (teststring.find(".finalize") != string::npos)
    {
      label = teststring;
    }
  }
*/
// <Dirk.F end>
  
  result += " t" + toString(id) + "_l -> t" + toString(id) + " [headlabel=\"" + label + "\"]\n";
  
  return result;
}
/*!
 * \brief   DOT-output of the place (used by PetriNet::dotOut())
 *
 *          DOT-output of the place. Places are colored corresponding to their
 *          initial role.
 *
 * \todo    Explain colors used.
*/
string Place::output_dot() const
{
  string result;
  
  result += " p" + toString(id) + "  \t[";//label=\"\"";
    
#ifdef USING_BPEL2OWFN
    string label;
    if ( wasExternal != "")
      label = wasExternal;
  else
// <Dirk.F start> create proper output for place names
//  label = nodeShortName();
    {
  	  label = "";	// start with empty string
  	  string fullLabel = nodeFullName();
  	  int counter = 0;
  	  for(string::iterator it = fullLabel.begin(); it != fullLabel.end(); it++ )
  	  {
  		  label = label + (*it);		// extend label by next letter
  		  if ( (counter > 15 && ((*it) == '#' || (*it) == '.'))
  			   || (counter > 20))
  		  {
  			  counter = 0;
  			  label = label + "\\n";	// insert a linebreak at appropriate points
  		  }
  		  counter++;
  	  }
    }
// <Dirk.F end>
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
  string label = nodeName();
#endif
  
  // truncate prefix (could be a problem with ports later on, but looks nice)
  //  if (type != INTERNAL)
  // label = label.substr(label.find_last_of(".")+1, label.length());
  
  if (tokens == 1)
    result += "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
  else if (tokens > 1)
    result += "label=\"" + toString(tokens) + "\" fontcolor=black fontname=\"Helvetica\" fontsize=10";
  
  switch (type)
  {
    case (IN):  result += "fillcolor=orange"; break;
    case (OUT): result += "fillcolor=yellow"; break;
    default:    break;
  }
  
// <Dirk.F start> ignore BPEL specific colorings
/*
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
  
  // use coloring for places according to BOM
  if (isFinal)
      result += "fillcolor=gray";
  // process input places
  else if (history.size() == 1 	&& history.begin()->find("process.") != string::npos
		  				  		&& history.begin()->find("input.") != string::npos)
      result += "fillcolor=orange";
  // process output places
  else if (history.size() == 1 	&& history.begin()->find("process.") != string::npos
		  				  		&& history.begin()->find("output.") != string::npos)
      result += "fillcolor=yellow";
  // process start place
  else if (history.size() == 1 	&& history.begin()->find("process.") != string::npos
		  				  		&& history.begin()->find("startNode.") != string::npos)
      result += "fillcolor=forestgreen";
  // process end node
  else if (history.size() == 1 	&& history.begin()->find("process.") != string::npos
		  				  		&& history.begin()->find("endNode.") != string::npos)
      result += "fillcolor=grey80";
  // process stop node
  else if (history.size() == 1 	&& history.begin()->find("process.") != string::npos
		  				  		&& history.begin()->find("stopNode.") != string::npos)
      result += "fillcolor=grey20";
  // task activated/complete places
  else if (history.size() == 1
	  && (		history.begin()->find(".activated") != string::npos
			||	history.begin()->find(".completed") != string::npos )
	  && (   history.begin()->find("task.") != string::npos 
		  || history.begin()->find("callToTask.") != string::npos
		  || history.begin()->find("callToProcess.") != string::npos
		  || history.begin()->find("callToTask.") != string::npos
		  || history.begin()->find("callToService.") != string::npos))
  {
	  result += "fillcolor=khaki2";
  }
  // split/merge/fork/join central place
  else if (history.size() == 1
	  && (		history.begin()->find(".activated") != string::npos )
	  && (		history.begin()->find("fork.") != string::npos
			  ||history.begin()->find("decision.") != string::npos
			  ||history.begin()->find("merge.") != string::npos
			  ||history.begin()->find("join.") != string::npos))
  {
      result += "fillcolor=skyblue";
  }
// <Dirk.F end>
  
  result += "]\n";
  
  result += " p" + toString(id) + "_l\t[shape=none];\n";
  
  if (type == OUT)
    result += " p" + toString(id) + " -> p" + toString(id) + "_l [taillabel=\"" + label + "\"]\n";
  else
    result += " p" + toString(id) + "_l -> p" + toString(id) + " [headlabel=\"" + label + "\"]\n";
  
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
  (*output) << " graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10 label=\"";
  
  if (globals::reduction_level == 5)
    (*output) << "structurally reduced ";
  
  (*output) << "Petri net generated from " << globals::filename << "." << globals::currentProcessName << "\"]" << endl;
  // REMEMBER The table size of the INOUT transitions depends on the size of a node!
  //          So a width of .3 (in) results in 21 pixel table width ( 0.3 in * 72 dpi ).
  (*output) << " node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white]" << endl;
  (*output) << " edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"]" << endl << endl;
  
  
  // list the places
  (*output) << "\n // places" << endl;
  (*output) << " node [shape=circle];" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*output) << (*p)->output_dot();
  
  if (draw_interface)
  {
    for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
      (*output) << (*p)->output_dot();
    for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
      (*output) << (*p)->output_dot();
  }
  
  // list the transitions
  (*output) << "\n // transitions" << endl;
  (*output) << " node [shape=box]" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) << (*t)->output_dot();
  
  
  (*output) << "\n // cluster the inner of the net" << endl;
  (*output) << " subgraph cluster1\n {\n ";
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) << " t" << (*t)->id << " t" << (*t)->id << "_l";
  (*output) << "\n ";
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->tokens > 0)
      (*output) << " p" << (*p)->id;
    else
      (*output) << " p" << (*p)->id << " p" << (*p)->id << "_l";
  }
  
  if (draw_interface)
    (*output) << "\n  label=\"\" style=dashed" << endl;
  else
    (*output) << "\n  label=\"\" style=invis" << endl;
  
  (*output) << " }" << endl;
  
  
  // list the arcs
  (*output) << "\n // arcs" << endl;
  (*output) << " edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black]" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    (*output) << (*f)->output_dot(draw_interface);
  
  (*output) << "}" << endl;
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
  (*output) << "  Petri net created by " << PACKAGE_STRING << " reading file " << globals::filename<< "." << globals::currentProcessName << " ." << endl;
  (*output) << "  See http://www.gnu.org/software/bpel2owfn for more details." << endl;
  (*output) << "-->" << endl << endl;
  
  (*output) << "<pnml>" << endl;
  (*output) << "  <net id=\"bpel-net\" type=\"owfn\">" << endl << endl;
  
  
  // places
  (*output) << "<!-- input places -->" << endl;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeShortName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << *(*p)->history.begin() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>input</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }
  
  (*output) << "<!-- output places -->" << endl;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeShortName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << *(*p)->history.begin() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>output</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }
  
  (*output) << "<!-- internal places -->" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeShortName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->nodeName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << *(*p)->history.begin() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    if ((*p)->tokens > 0)
    {
      (*output) << "      <initialMarking>" << endl;
      (*output) << "        <text>" << (*p)->tokens << "</text>" << endl;
      (*output) << "      </initialMarking>" << endl;
    }
    (*output) << "    </place>" << endl << endl;
  }
  
  
  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->nodeShortName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->nodeName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << *(*t)->history.begin() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "    </transition>" << endl << endl;
  }
  (*output) << endl;
  
  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++, arcNumber++)
  {
    (*output) << "    <arc id=\"a" << arcNumber << "\" ";
#ifdef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->source->nodeShortName() << "\" ";
    (*output) << "target=\"" << (*f)->target->nodeShortName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->source->nodeName() << "\" ";
    (*output) << "target=\"" << (*f)->target->nodeName() << "\">" << endl;
#endif
    (*output) << "      <inscription>\n        <text>" << (*f)->weight << "</text>\n      </inscription>" << endl;
    (*output) << "    </arc>" << endl;
  }
  (*output) << endl;
  
  (*output) << "    <toolspecific tool=\"owfn\" version=\"1.0\">" << endl;
  (*output) << "      <finalmarking xmlns=\"http://www.informatik.hu-berlin.de/top/tools4bpel\">" << endl;
  (*output) << "        <text>";
  
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ((*p)->isFinal)
      (*output) << (*p)->nodeShortName() << " ";
  
  (*output) << "</text>" << endl;
  (*output) << "      </finalmarking>" << endl;
  (*output) << "    </toolspecific>" << endl;
  (*output) << "  </net>" << endl;
  (*output) << "</pnml>" << endl;
}



/*!
 * \brief   TPN-output
 *
 *          Outputs the net in TPN-format (Woflan). See
 *          http://is.tm.tue.nl/research/woflan/ for a reference.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Maybe I need to use the "toLoLAident" function.
 */
void PetriNet::output_tpn(ostream *output) const
{
    assert(output != NULL);
    
    // header line
    (*output) << "-- Petri net created by " << PACKAGE_STRING << " reading " << globals::filename << "." << globals::currentProcessName << endl << endl;
    
    // places (only internal)
    for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    {
        (*output) << "place \"" << (*p)->nodeFullName() << "\"";
        
        // initial marking
        if ((*p)->tokens > 0)
            (*output) << " init " << (*p)->tokens;
        
        (*output) << ";" << endl;
    }
    (*output) << endl;
    
    
    // transitions
    for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    {
        (*output) << "trans \"" << (*t)->nodeFullName() << "\"" << endl;
        
        (*output) << "    in";
        for (set<Node *>::iterator pre = (*t)->preset.begin(); pre != (*t)->preset.end(); pre++)
        {
            // ignore input places
            if ( (*pre)->nodeType == PLACE )
                if ( P_in.find(static_cast<Place*>(*pre)) != P_in.end())
                    continue;
            
            for (unsigned int i = 0; i < arc_weight(*pre, *t); i++)
                (*output) << " \"" << (*pre)->nodeFullName() << "\""; // << arc_weight(*pre, *t);
        }
        (*output) << endl;
        
        (*output) << "    out";
        for (set<Node *>::iterator post = (*t)->postset.begin(); post != (*t)->postset.end(); post++)
        {
            // ignore output places
            if ( (*post)->nodeType == PLACE )
                if ( P_out.find(static_cast<Place*>(*post)) != P_out.end())
                    continue;
            
            for (unsigned int i = 0; i < arc_weight(*t, *post); i++)
                (*output) << " \"" << (*post)->nodeFullName() << "\""; // << arc_weight(*t, *post);
            
        }
        (*output) << endl << ";" << endl << endl;
    }  
    (*output) << endl << ";" << endl;
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
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << (*p)->id << "\"" << (*p)->nodeShortName() << "\"80@40";
    if ((*p)->tokens > 0)
      (*output) << "M" << (*p)->tokens;
    (*output) << "k1" << endl;
  }
  
  // transitions
  (*output) << "TR" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) << (*t)->id << "\"" << (*t)->nodeShortName() << "\"80@40" << endl;
  
  // arcs from transitions to places (no output places)
  (*output) << "TP" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == TRANSITION)
      if ( P_out.find(static_cast<Place*>((*f)->target)) == P_out.end())
        (*output) << (*f)->source->id << "<" << (*f)->target->id << "w" << (*f)->weight << endl;
  
  // arcs from places to transitions
  (*output) << "PT" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == PLACE)
      if ( P_in.find(static_cast<Place*>((*f)->source)) == P_in.end())
        (*output) << (*f)->source->id << ">" << (*f)->target->id << "w" << (*f)->weight << endl;
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
  (*output) << "P   M   PRE,POST  NETZ 1:" << globals::filename.substr(0, globals::filename.find_first_of(".")) << endl;
  
  // places (only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << setw(3) << (*p)->id << " " << (*p)->tokens << "     ";
    
    for (set<Node*>::iterator t = (*p)->preset.begin(); t != (*p)->preset.end(); t++)
    {
      if (t != (*p)->preset.begin())
        (*output) << " ";
      
      (*output) << (*t)->id << ":" << arc_weight(*t, *p);
    }
    
    (*output) << ", ";
    
    for (set<Node*>::iterator t = (*p)->postset.begin(); t != (*p)->postset.end(); t++)
    {
      if (t != (*p)->postset.begin())
        (*output) << " ";
      
      (*output) << (*t)->id << ":" << arc_weight(*p, *t);
    }
    
    (*output) << endl;
  }
  
  (*output) << "@" << endl;
  
  
  // place names
  (*output) << "place nr.             name capacity time" << endl;
  
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << right << setw(8) << (*p)->id << ": ";
    (*output) << left << setw(16) << (*p)->nodeShortName();
    (*output) << "       oo    0" << endl;
  }
  
  (*output) << "@" << endl;
  
  
  // transition names
  (*output) << "trans nr.             name priority time" << endl;
  
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << right << setw(8) << (*t)->id << ": ";
    (*output) << left << setw(16) << (*t)->nodeShortName();
    (*output) << "        0    0" << endl;
  }
  
  (*output) << "@" << endl;
  
  // info (ignored after last '@')
  (*output) << "generated by: " << PACKAGE_STRING << endl;
  (*output) << "input file:   `" << globals::filename << "'" << endl;
  (*output) << "net size:     " << information() << endl;  
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
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "byte p" << (*p)->id << "=" << (*p)->tokens << ";" << endl;
  }
  
  (*output) << "int {" << endl;
  (*output) << "\tdo" << endl;
  
  
  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    int follower=0;
    (*output) << "\t::atomic { (";
    
    if ((*t)->preset.empty())
    {
      (*output) << "0";
    }
    else
    {
      for (set<Node *>::iterator p = (*t)->preset.begin(); p != (*t)->preset.end(); p++)
      {
        if(follower) 
        {
          (*output) << "&&";          
        } 
        else 
        {
          follower=1;
        }
        (*output) << "(p" << (*p)->id << ">=" << arc_weight(*p,*t) << ")";
      }
    }
    
    (*output) << ") -> ";
    
    for (set<Node *>::iterator p = (*t)->preset.begin(); p != (*t)->preset.end(); p++)
    {
      (*output) << "p" << (*p)->id << "=p" << (*p)->id << "-" << arc_weight(*p,*t) << ";";
    }
    
    (*output) << endl << "\t\t\t";
    
    for (set<Node *>::iterator p = (*t)->postset.begin(); p != (*t)->postset.end(); p++)
    {
      (*output) << "p" << (*p)->id << "=p" << (*p)->id << "+" << arc_weight(*t,*p) << ";";
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
  
  (*output) << "\\beginnet{" << globals::filename << "}" << endl << endl;
  
  // places (only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "  \\place{" << (*p)->nodeShortName() << "}{";
    if ((*p)->tokens > 0)
      (*output) << "\\init{" << (*p)->tokens << "}";
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
    // ignore input places
    if ((*f)->source->nodeType == PLACE)
      if ( P_in.find(static_cast<Place*>((*f)->source)) != P_in.end())
        continue;
    
    // ignore output places
    if ((*f)->target->nodeType == PLACE)
      if ( P_out.find(static_cast<Place*>((*f)->target)) != P_out.end())
        continue;
    
    (*output) << "  \\arc{a" << arcNumber << "}{ ";
    (*output) << "\\from{" << (*f)->source->nodeShortName() << " } ";
    (*output) << "\\to{" << (*f)->target->nodeShortName() << "} \\weight{" << (*f)->weight << "} }" << endl;
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
  
  
  (*output) << "{ Petri net created by " << PACKAGE_STRING << " reading " << globals::filename << "." << globals::currentProcessName << " }" << endl << endl;
  
  
  // places (only internal)
  (*output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
   
// <Dirk.F start> use full node name for output
//    (*output) << "  " << (*p)->nodeShortName();
  	(*output) << "  " << toLoLAident((*p)->nodeFullName());
// <Dirk.F end>
    
    if (count < P.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl << endl;
  
  
  // initial marking
  (*output) << "MARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->tokens > 0)
    {
      if (count++ != 1)
        (*output) << "," << endl;
      
// <Dirk.F start> use full node name for output
//      (*output) << "  " << (*p)->nodeShortName() << ":\t" << (*p)->tokens;
      (*output) << "  " << toLoLAident((*p)->nodeFullName()) << ":\t" << (*p)->tokens;
// <Dirk.F end>
    }
  }
  (*output) << endl << ";" << endl << endl << endl;
  
  
  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
// <Dirk.F start> use full node name for output
//    (*output) << "TRANSITION " << (*t)->nodeShortName() << endl;
	(*output) << "TRANSITION " << toLoLAident((*t)->nodeFullName()) << endl;
// <Dirk.F end>
    
    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = (*t)->preset.begin(); pre != (*t)->preset.end(); count++, pre++)
    {
      // ignore input places
      if ( (*pre)->nodeType == PLACE )
        if ( P_in.find(static_cast<Place*>(*pre)) != P_in.end())
          continue;
      
// <Dirk.F start> use full node name for output
//	    (*output) << "  " << (*pre)->nodeShortName() << ":\t" << arc_weight(*pre, *t);
      (*output) << "  " << toLoLAident((*pre)->nodeFullName()) << ":\t" << arc_weight(*pre, *t);
// <Dirk.F end>
      
      if (count < (*t)->preset.size())
        (*output) << "," << endl;
    }
    (*output) << ";" << endl;
    
    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = (*t)->postset.begin(); post != (*t)->postset.end(); count++, post++)
    {
      // ignore output places
      if ( (*post)->nodeType == PLACE )
        if ( P_out.find(static_cast<Place*>(*post)) != P_out.end())
          continue;
      
// <Dirk.F start> use full node name for output
//      (*output) << "  " << (*post)->nodeShortName() << ":\t" << arc_weight(*t, *post);
      (*output) << "  " << toLoLAident((*post)->nodeFullName()) << ":\t" << arc_weight(*t, *post);
// <Dirk.F end>
      
      if (count < (*t)->postset.size())
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
  (*output) << "  generated by: " << PACKAGE_STRING << endl;
#ifdef USING_BPEL2OWFN
  //(*output) << "  input file:   `" << globals::filename << "' (process `" << globals::ASTEmap[1]->attributes["name"] << "')" << endl;
#endif
#ifndef USING_BPEL2OWFN
  (*output) << "  input file:   `" << globals::filename << "." << globals::currentProcessName << "'" << endl;
#endif
  (*output) << "  net size:     " << information() << endl;
  (*output) << "}" << endl << endl;
  
  // places
  (*output) << "PLACE" << endl;
  
  // internal places
  (*output) << "  INTERNAL" << endl;
  (*output) << "    ";
  
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << (*p)->nodeShortName();
#else
    (*output) << (*p)->nodeName();
#endif
    if (count < P.size())
      (*output) << ", ";
  }
  (*output) << ";" << endl << endl;
  
  
  // input places
  (*output) << "  INPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); count++, p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    " << (*p)->nodeShortName();
#else
    (*output) << "    " << (*p)->nodeName();
#endif
    
    if ((*p)->max_occurrences != UINT_MAX && (*p)->max_occurrences != 0)
      (*output) << " {$ MAX_OCCURRENCES = " << (*p)->max_occurrences << " $}";
    if ((*p)->max_occurrences == UINT_MAX)
      (*output) << " {$ MAX_OCCURRENCES = -1 $}";
    
    if (count < P_in.size())
      (*output) << "," << endl;
  }
  (*output) << ";" << endl << endl;
  
  
  // output places
  (*output) << "  OUTPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); count++, p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    " << (*p)->nodeShortName();
#else
    (*output) << "    " << (*p)->nodeName();
#endif
    
    if ((*p)->max_occurrences != UINT_MAX && (*p)->max_occurrences != 0)
      (*output) << " {$ MAX_OCCURRENCES = " << (*p)->max_occurrences << " $}";
    if ((*p)->max_occurrences == UINT_MAX)
      (*output) << " {$ MAX_OCCURRENCES = -1 $}";
    
    if (count < P_out.size())
      (*output) << "," << endl;
  }
  (*output) << ";" << endl << endl << endl;
  
  
  // initial marking
  (*output) << "INITIALMARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->tokens > 0)
    {
      if (count++ != 1)
        (*output) << "," << endl;
      
#ifdef USING_BPEL2OWFN
      (*output) << "  " << (*p)->nodeShortName() << ":\t" << (*p)->tokens;
#else
      (*output) << "  " << (*p)->nodeName() << ":\t" << (*p)->tokens;
#endif
      
      if ((*p)->historyContains("1.internal.initial"))
        (*output) << " {initial place}";
    }
  }
  (*output) << ";" << endl << endl;  
  
  
  //if (globals::finalCondition == "")
  //{
      (*output) << "FINALCONDITION" << endl;
        globals::finalCondition = "(((";
        
        bool generatedFinalPlaces=false;
        
        bool firstrun = true;
        for ( set<Place*>::iterator place = P.begin(); place != P.end();place++)
        {
            if ((*place)->isFinal)
            {
                bool right = true;
                for (list<string>::const_iterator iter = (*place)->history.begin(); iter != (*place)->history.end(); iter++)
                {
                  if ( (*iter).find("endNode",0) != string::npos || (*iter).find("stopNode",0) != string::npos)
                  {
                    right = false;
                  }
                }
                
                if(!right)
                {
                    continue;
                }
                
                if (!firstrun)
                {
                    globals::finalCondition += " AND ";
                }
                generatedFinalPlaces = true;
                globals::finalCondition += (*place)->nodeShortName() + " = 1";
                firstrun = false;
            }
        }
               
        firstrun = true;
        for ( set<Place*>::iterator place = P.begin(); place != P.end();place++)
        {
            if ((*place)->isFinal)
            {
                bool right = true;
                for (list<string>::const_iterator iter = (*place)->history.begin(); iter != (*place)->history.end(); iter++)
                {
                  if ( (*iter).find("endNode",0) == string::npos && (*iter).find("stopNode",0) == string::npos)
                  {
                    right = false;
                  }
                }
                
                if(!right)
                {
                    continue;
                }
                
                if (!firstrun)
                {
                    globals::finalCondition += " OR ";
                }
                if (firstrun && generatedFinalPlaces)
                {
                    globals::finalCondition += ") OR (";
                }
                
                globals::finalCondition += (*place)->nodeShortName() + " = 1";
                firstrun = false;
            }
        }
        if (!generatedInitials.empty() && realStartNode != NULL)
        {
            globals::finalCondition += ")) AND (" + realStartNode->nodeShortName() + " = 1 OR (";
            firstrun = true;
            for ( set<Place*>::iterator place = generatedInitials.begin(); place != generatedInitials.end();place++)
            {
                if (!firstrun)
                {
                    globals::finalCondition += " AND ";
                }
            
                globals::finalCondition += (*place)->nodeShortName() + " = 1";
                firstrun = false;
            }
        }
       globals::finalCondition += "))) AND ALL_OTHER_PLACES_EMPTY";
      
      (*output) << globals::finalCondition << endl;
      (*output) << ";" << endl << endl;          
  /*}
  else
  {
      // final marking
      (*output) << "FINALMARKING" << endl;
      count = 1;
      for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
      {
        if ( (*p)->isFinal )
        {
          if (count != 1)
            (*output) << ",";
          
#ifdef USING_BPEL2OWFN
          (*output) << "  " << (*p)->nodeShortName();
#else
          (*output) << "  " << (*p)->nodeName();
#endif          
          (*output) << " {final place}";
          
          if (count++ != 1)
            (*output) << endl;      
        }
      }
      (*output) << ";" << endl << endl << endl;
  }
  
  */
  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "TRANSITION " << (*t)->nodeShortName();
#else
    (*output) << "TRANSITION " << (*t)->nodeName();
#endif
    switch( (*t)->type )
    {
      case (INTERNAL):	(*output) << endl; break;
      case(IN):		(*output) << " { input }" << endl; break;
      case(OUT):	(*output) << " { output }" << endl; break;
      case(INOUT):	(*output) << " { input/output }" << endl; break;
    }
    
    (*output) << "  CONSUME ";
    count = 1;
    for (set<Node *>::iterator pre = (*t)->preset.begin(); pre != (*t)->preset.end(); count++, pre++)
    {
#ifdef USING_BPEL2OWFN
      (*output) << (*pre)->nodeShortName();
#else
      (*output) << (*pre)->nodeName();
#endif
      
      if (arc_weight(*pre, *t) != 1)
        (*output) << ":" << arc_weight(*pre, *t);
      
      if (count < (*t)->preset.size())
        (*output) << ", ";
    }
    (*output) << ";" << endl;
    
    (*output) << "  PRODUCE ";
    
    count = 1;
    for (set<Node *>::iterator post = (*t)->postset.begin(); post != (*t)->postset.end(); count++, post++)
    {
#ifdef USING_BPEL2OWFN
      (*output) << (*post)->nodeShortName();
#else
      (*output) << (*post)->nodeName();
#endif
      if (arc_weight(*t, *post) != 1)
        (*output) << ":" << arc_weight(*t, *post);
      
      if (count < (*t)->postset.size())
        (*output) << ", ";
    }
    
    (*output) << ";" << endl << endl;
  }  
  (*output) << endl << "{ END OF FILE `" << globals::output_filename << ".owfn' }" << endl;
  
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
ostream& operator<< (ostream& os, const PetriNet &obj)
{
  switch (obj.format)
  {
  
    case(FORMAT_APNN):	obj.output_apnn(&os); break;
    case(FORMAT_DOT):	obj.output_dot(&os, obj.use_standard_style); break;
    case(FORMAT_INA):	obj.output_ina(&os); break;
    case(FORMAT_SPIN):	obj.output_spin(&os); break;
    case(FORMAT_INFO):	obj.output_info(&os); break;
    case(FORMAT_LOLA):	obj.output_lola(&os); break;
    case(FORMAT_OWFN):	obj.output_owfn(&os); break;
    case(FORMAT_PEP):	obj.output_pep(&os); break;
    case(FORMAT_PNML):	obj.output_pnml(&os); break;
    case(FORMAT_TPN):	obj.output_tpn(&os); break;
    
    default:		break;
    
  }
  
  return os;
}
/*!
 * \brief   set the output format
 *
 *          Set the output format to be used when the <<-operator is called
 *          the next time.
 *
 * \param   my_format  the output format from the enumeration output_format.
 * \param   standard   if set to false a different output format style is used
 */
void PetriNet::set_format(output_format my_format, bool standard)
{
  format = my_format;
  use_standard_style = standard;
}

/*!
 * \brief	Converts a given identifier into a LoLA/oWFN compatible identifier
 * 			warning: conversion may project two different strings onto the same
 * 			compatible string
 *
 * \param name  identifier to convert
 *
 * \pre name != NULL
 */
string toLoLAident(string name)
{
	bool nonUpperCaseFound = false;
 	string result = "";
	
	// Display the list
	string::iterator it;
	for( it = name.begin(); it != name.end(); it++ )
	{
		switch (*it) {
			case ',':
			case ';':
			case ':':
			case '(':
			case ')':
			case '{':
			case '}':
				result = result + "_"+toString(static_cast<int>(*it))+"_";	// write ASCII code
				nonUpperCaseFound = true;
				break;
			case ' ':
				result = result + "_";
				nonUpperCaseFound = true;
				break;
			default:
				if (!isupper(*it))
					nonUpperCaseFound = true;
				result = result + *it;
				break;
		}
   }
   if (!nonUpperCaseFound)
   {
   		int i;
		for( i=0,it = result.begin(); it != result.end(); it++,i++ )
			result.replace(i, 1,1, tolower(*it));
   }
   return result;
}

/*!
 * \file  io-format.cc
 */

#include "config.h"

#include <sstream>
#include <iostream>

#include "automaton.h"
#include "interface.h"
#include "petrinet.h"
#include "state.h"
#include "myio.h"
#include "util.h"

using std::cerr;
using std::endl;
using std::map;
using std::ostream;
using std::set;
using std::string;
using std::stringstream;

using pnapi::io::util::delim;
using pnapi::io::util::filterMarkedPlaces;
using pnapi::io::util::Mode;
using pnapi::io::util::ModeData;

using namespace pnapi::formula;

namespace pnapi
{

namespace io
{
using pnapi::io::util::operator<<;


/* FORMAT IMPLEMENTATION: add a corresponding section */

/*************************************************************************
 ***** STAT output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ostream & stat(std::ostream & ios)
{
  util::FormatData::data(ios) = util::STAT;
  return ios;
}


namespace __stat
{

/*!
 * \brief output petri net statistics
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  set<Label *> asynchronous = net.interface_.getAsynchronousLabels();
  
  unsigned int placeCount = net.places_.size() + asynchronous.size();
  unsigned int arcCount = net.arcs_.size();
  
  PNAPI_FOREACH(l, asynchronous)
  {
    arcCount += (*l)->getTransitions().size();
  }
  
  return (os << "|P|= " << placeCount << "  "
             << "|P_in|= " << net.interface_.getInputLabels().size() << "  "
             << "|P_out|= " << net.interface_.getOutputLabels().size() << "  "
             << "|T|= " << net.transitions_.size() << "  "
             << "|F|= " << arcCount);
}

/*!
 * \brief write service automaton statistics
 */
std::ostream & output(std::ostream & os, const Automaton & sa)
{
  return os << "|Q|= " << sa.states_.size() << "  "
            << "|E|= " << sa.edges_.size();
}

} /* namespace __stat */


/*************************************************************************
 ***** DOT output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ostream & dot(std::ostream & ios)
{
  util::FormatData::data(ios) = util::DOT;
  return ios;
}


namespace __dot
{

/*!
 * \brief output net 
 * 
 * Creates a DOT output (see http://www.graphviz.org) of the net.
 * It uses the digraph-statement and adds labels to transitions,
 * places and arcs if neccessary.
 * 
 * \todo print final condition below net
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  string filename = net.getMetaInformation(os, io::INPUTFILE);

  os  //< output everything to this stream

  << delim("\n")
  << util::mode(io::util::NORMAL)

  << "digraph N {\n"
  << " graph [fontname=\"Helvetica\" nodesep=0.25 ranksep=\"0.25\""
  << " remincross=true label=\""
  << "Petri net"
  << (filename.empty() ? "" : " generated from " + filename) << "\"]\n"
  
  << " node [fontname=\"Helvetica\" fixedsize width=\".3\""
  << " height=\".3\" label=\"\" style=filled]\n"
  << " edge [fontname=\"Helvetica\" color=white arrowhead=none"
  << " weight=\"20.0\"]\n\n"
  
  << net.interface_

  << "// places\n"
  << " node [shape=circle fillcolor=white]\n"
  << net.places_

  << "\n // transitions\n"
  << " node [shape=box]\n"
  << net.transitions_

  << util::mode(io::util::INNER)
  << delim(" ")
  << "\n\n // inner cluster\n"
  << " subgraph cluster1\n"
  << " {\n"
  << "  " << net.transitions_ << endl
  << "  " << net.places_ << endl
  << "  label=\"\" style=" << (net.interface_.isEmpty() ? "invis" : "\"dashed\"")
  << "\n }\n\n"

  << delim("\n")
  << util::mode(io::util::ARC)
  << " // arcs\n"
  << " edge [fontname=\"Helvetica\" arrowhead=normal color=black]\n"
  << net.arcs_ << endl
  << util::mode(io::util::PLACE)
  // actually not an appropriate mode but I did not want to introduce 
  // a further mode like e.g. "ARC2" for only this purpose.   
  << net.transitions_ // interface arcs

  << "\n}\n";
}

/*!
 * \brief place output
 */
std::ostream & output(std::ostream & os, const Place & p)
{
  // place attributes (used in NORMAL mode)
  stringstream attributes;
  if (p.getTokenCount() == 1)
  {
    attributes << "fillcolor=black peripheries=2 height=\".2\" "
               << "width=\".2\" ";
  }
  else
  {
    if (p.getTokenCount() > 1)
    {
      attributes << "label=\"" << p.getTokenCount() << "\" "
                 << "fontcolor=black fontname=\"Helvetica\" ";
    }
  }

  if (p.wasInterface())
  {
    attributes << "fillcolor=lightgoldenrod1";
  }

  // output the place as a node
  return output(os, p, attributes.str());
}

/*!
 * \brief transition output
 */
std::ostream & output(std::ostream & os, const Transition & t)
{
  if(ModeData::data(os) == util::PLACE) // mode when printing the arcs to synchronous labels; actually should be something like "ARC2"
  {
    PNAPI_FOREACH(l, t.getLabels())
    {
      string left;
      string right;
      stringstream attr;
      attr << "\t[weight=10000.0";
      
      if(l->second != 1)
      {
        attr << " label=\"" << (l->second) << "\"";
      }
      
      switch(l->first->getType())
      {
      case Label::SYNCHRONOUS:
        attr << " arrowhead=none";
      case Label::INPUT:
        left = getLabelName(*l->first);
        right = getNodeName(t);
        attr << "]\n";
        break;
      case Label::OUTPUT:
        left = getNodeName(t);
        right = getLabelName(*l->first);
        attr << "]\n";
        break;
      default: assert(false);
      }
      
      os << " " << left << " -> " << right << attr.str();
    }
    
    return os;
  }
  
  // transition attributes (used in NORMAL mode)
  stringstream attributes;
  if(ModeData::data(os) != util::ARC) // when printing arcs, no attributes are needed
  {
    switch(t.getType())
    {
    case(Transition::INPUT):  attributes << "fillcolor=orange"; break;
    case(Transition::OUTPUT): attributes << "fillcolor=yellow"; break;
    case(Transition::INOUT):
    {
      attributes
      << "fillcolor=gold label=< <TABLE BORDER=\"1\""
      << " CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\""
      << " HEIGHT=\"21\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\"><TR>"
      << "<TD HEIGHT=\"11\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\""
      << " BGCOLOR=\"ORANGE\">"
      << "</TD></TR><TR>"
      << "<TD HEIGHT=\"10\" WIDTH=\"21\" FIXEDSIZE=\"TRUE\""
      << " BGCOLOR=\"YELLOW\">"
      << "</TD></TR></TABLE> >";
      
      break;
    }
    default: /* do nothing */ ;
    }
  }

  // output the transition as a node
  output(os, t, attributes.str());

  return os;
}

/*!
 * \brief arc output
 */
std::ostream & output(std::ostream & os, const Arc & arc)
{
  os << " " << arc.getSourceNode() << " -> " << arc.getTargetNode() 
     << "\t[";
  
  if (arc.getWeight() != 1)
  {
    os << "label=\"" << arc.getWeight() << "\"";
  }
  
  return (os << "]");
}

/*!
 * \brief interface output
 */
std::ostream & output(std::ostream & os, const Interface & interface)
{
  os << "// interface\n";
  
  map<string, Port *> ports = interface.getPorts();
  
  if(ports.size() > 1)
  {
    PNAPI_FOREACH(port, ports)
    {
      if(port->first != "")
      {
        os << (*port->second) << endl;
      }
    }
  }
  else
  {
    os << "/// input\n node [shape=circle fillcolor=orange]\n"<< interface.getPort()->getInputLabels()
       << "\n/// output\n node [shape=circle fillcolor=yellow]\n" << interface.getPort()->getOutputLabels()
       << "\n/// synchronous\n node [shape=box fillcolor=black]\n" << interface.getPort()->getSynchronousLabels()
       << endl;
  }
  
  return (os << endl);
}

/*!
 * \brief port output
 */
std::ostream & output(std::ostream & os, const Port & port)
{
  static PetriNet * net = NULL;
  static int portNumber = 0;
  
  if(net != &port.getPetriNet())
  {
    net = &port.getPetriNet();
    portNumber = 0;
  }
  
  util::Mode oldMode = ModeData::data(os);
  
  os << mode(util::NORMAL) << "/// port '" << port.getName() 
     << "'\n//// input\n node [shape=circle fillcolor=orange]\n" << port.getInputLabels()
     << "\n//// output\n node [shape=circle fillcolor=yellow]\n" << port.getOutputLabels()
     << "\n//// synchronous\n node [shape=box fillcolor=black]\n" << port.getSynchronousLabels() 
     << "\n//// cluster\n subgraph cluster_port" << (++portNumber) << "\n {\n  "
     << mode(util::INNER) << delim(" ")
     << port.getInputLabels() << "\n  "
     << port.getOutputLabels() << "\n  "
     << port.getSynchronousLabels()
     << "\n  label=\"" << port.getName() << "\" style=\"filled,rounded\"; bgcolor=grey95  labelloc=t;\n }\n\n"
     << mode(oldMode) << delim("\n");
     
  return os;
}

/*!
 * \brief label output
 */
std::ostream & output(std::ostream & os, const Label & label)
{
  switch(ModeData::data(os))
  {
  case util::NORMAL:
  {
    string labelName =  getLabelName(label);
    string labelName_l = getLabelName(label, true);
    
    os << labelName
       << ((label.getType() == Label::SYNCHRONOUS) ? "\t[width=.1]\n  " : "\n  ")
       << labelName_l << "\t[style=invis]\n  "
       << labelName << " -> " << labelName_l
       << "\t[headlabel=\"" << label.getName() << "\"]";
    
    break;
  }
  case util::INNER:
  {
    os << getLabelName(label) << " " << getLabelName(label, true);
    break;
  }
    
  default: /* do nothing */ ;
  }
  return os;
}

/*!
 * \brief output node with attribute
 */
std::ostream & output(std::ostream & os, const Node & n, const std::string & attr)
{
  string dotName   = getNodeName(n);
  string dotName_l = getNodeName(n, true);

  os << dotName;

  switch(ModeData::data(os))
  {
  case util::NORMAL:
  {
    os << "\t[" << attr << "]" << endl
       << " " << dotName_l << "\t[style=invis]" << endl
       << " " << dotName_l << " -> " << dotName
       << " [headlabel=\"" << n.getName() << "\"]";
    
    break;
  }
  case util::INNER:
  {
    os << " " << dotName_l;
    break;
  }
  default: /* do nothing */ ;
  }

  return os;
}


/*!
 * \brief get a unique node name for dot output
 * 
 * \pre all nodes and labels have distinct names
 */
std::string getNodeName(const Node & n, bool withSuffix)
{
  static PetriNet * net = NULL;
  static map<string, string> names;

  if (net != &n.getPetriNet())
  {
    net = &n.getPetriNet();
    names.clear();

    int i = 0;
    set<Place *> places = net->getPlaces();
    PNAPI_FOREACH(it, places)
    {
      stringstream ss; ss << "p" << (++i);
      names[(*it)->getName()] = ss.str();
    }

    i = 0;
    set<Transition *> transitions = net->getTransitions();
    PNAPI_FOREACH(it, transitions)
    {
      stringstream ss; ss << "t" << ++i;
      names[(*it)->getName()] = ss.str();
    }
  }

  return names[n.getName()] + string(withSuffix ? "_l" : "");
}

/*!
 * \brief get a unique label name for dot output
 * 
 * \pre all nodes and labels have distinct names
 */
std::string getLabelName(const Label & l, bool withSuffix)
{
  static PetriNet * net = NULL;
  static map<string, string> names;

  if (net != &l.getPetriNet())
  {
    net = &l.getPetriNet();
    names.clear();

    int i = 0;
    set<Label *> input = net->getInterface().getInputLabels();
    PNAPI_FOREACH(it, input)
    {
      stringstream ss; ss << "i" << (++i);
      names[(*it)->getName()] = ss.str();
    }

    i = 0;
    set<Label *> output = net->getInterface().getOutputLabels();
    PNAPI_FOREACH(it, output)
    {
      stringstream ss; ss << "o" << (++i);
      names[(*it)->getName()] = ss.str();
    }
    
    i = 0;
    set<Label *> synchronous = net->getInterface().getSynchronousLabels();
    PNAPI_FOREACH(it, synchronous)
    {
      stringstream ss; ss << "s" << (++i);
      names[(*it)->getName()] = ss.str();
    }
  }

  return names[l.getName()] + string(withSuffix ? "_l" : "");
}

/*!
 * \brief service automaton output
 */
std::ostream & output(std::ostream & os, const Automaton & sa)
{
  os << "Digraph ServiceAutomaton {\n"
     << "{\n"
     << "q0 [style=invis];\n";
  
  for(int i = 0; i < (int) sa.states_.size(); ++i)
  {
    if (sa.states_[i]->isFinal())
      os << sa.states_[i]->getName() << " [];\n";
    else
      os << sa.states_[i]->getName() << ";\n";
  }
  
  os << "}\n"
     << "{\n"
     << "q0 -> " << (*sa.getInitialStates().begin())->getName() << ";\n";
  
  for(int i = 0; i < (int) sa.edges_.size(); ++i)
  {
    os << sa.edges_[i]->getSource().getName() << " -> "
       << sa.edges_[i]->getDestination().getName() << " [label=\""
       << sa.edges_[i]->getLabel() <<"\"];\n";
  }
  
  return (os << "}\n}\n");
}


} /* namespace __dot */



/*************************************************************************
 ***** LOLA output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ios_base & lola(std::ios_base & ios)
{
  util::FormatData::data(ios) = util::LOLA;
  return ios;
}

/*!
 * \brief makes the stream write also final condition formulae
 */
std::ostream & formula(std::ostream & os)
{
  util::FormulaData::data(os).formula = true;
  return os;
}


namespace __lola
{

/*!
 * \brief petri net output
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  string creator = net.getMetaInformation(os, CREATOR, PACKAGE_STRING);
  string inputfile = net.getMetaInformation(os, INPUTFILE);

  os //< output everything to this stream

  << "{ Petri net created by " << creator
  << (inputfile.empty() ? "" : " reading " + inputfile)
  << " }\n\n"

  << "PLACE\n  " << mode(io::util::PLACE) << delim(", ")
  << net.places_ << ";\n\n"

  << "MARKING\n  " << mode(io::util::PLACE_TOKEN)
  << filterMarkedPlaces(net.places_) << ";\n\n\n"

  // transitions
  << delim("\n") << net.transitions_ << endl
  << endl;

  if (util::FormulaData::data(os).formula)
  {
    os << "FORMULA\n  "
       << net.finalCondition_ << endl
       << endl;
  }

  return (os << "{ END OF FILE }\n");
}

/*!
 * \brief place output
 */
std::ostream & output(std::ostream & os, const Place & p)
{
  os << p.getName();
  
  if(ModeData::data(os) == io::util::PLACE_TOKEN)
  {
    os << ":" << p.getTokenCount();
  }
  
  return os;
}

/*!
 * \brief transition output
 */
std::ostream & output(std::ostream & os, const Transition & t)
{
  return (os << "TRANSITION " << t.getName()
            << delim(", ")
            << "\n  CONSUME "
            << t.getPresetArcs() << ";\n"
            << "  PRODUCE "
            << t.getPostsetArcs() << ";\n");
}

/*!
 * \brief arc output
 */
std::ostream & output(std::ostream & os, const Arc & arc)
{
  return (os << arc.getPlace().getName() << ":" << arc.getWeight());
}

/*!
 * \brief negation output
 */
std::ostream & output(std::ostream & os, const formula::Negation & f)
{
  if (f.getChildren().empty())
    assert(false); // FIXME: don't know what to do in this case
  else
    return (os << "NOT (" << **f.getChildren().begin() << ")");
}

/*
 * \brief conjunction output
 */
std::ostream & output(std::ostream & os, const formula::Conjunction & f)
{
  if(f.getChildren().empty())
  {
    //return os << formula::FormulaTrue();
    assert(false); // FIXME: don't know what to do in this case
  }
  else
    return (os << "(" << delim(" AND ") << f.getChildren() << ")");
}


/*!
 * \brief disjunction output
 */
std::ostream & output(std::ostream & os, const formula::Disjunction & f)
{
  if(f.getChildren().empty())
  {
    //return os << formula::FormulaFalse();
    assert(false); // FIXME: don't know what to do in this case
  }
  else
    return (os << "(" << delim(" OR ") << f.getChildren() << ")");
}

/*!
 * \brief output FormulaTrue
 */
std::ostream & output(std::ostream & os, const formula::FormulaTrue &)
{
  return (os << "TRUE");  // keyword not yet implemented in lola
  // TODO: remove comment when done
}

/*!
 * \brief output FormulaFalse
 */
std::ostream & output(std::ostream & os, const formula::FormulaFalse &)
{
  return (os << "FALSE"); // keyword not yet implemented in lola
  // TODO: remove comment when done
}

/*!
 * \brief output FormulaEqual
 */
std::ostream & output(std::ostream & os, const formula::FormulaEqual & f)
{
  return (os << f.getPlace().getName() << " = " << f.getTokens());
}

/*!
 * \brief output FormulaNotEqual
 */
std::ostream & output(std::ostream & os, const formula::FormulaNotEqual & f)
{
  return (os << f.getPlace().getName() << " # " << f.getTokens());
}

/*!
 * \brief output FormulaGreater
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreater & f)
{
  return (os << f.getPlace().getName() << " > " << f.getTokens());
}

/*!
 * \brief output FormulaGreaterEqual
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreaterEqual & f)
{
  return (os << f.getPlace().getName() << " >= " << f.getTokens());
}

/*!
 * \brief output FormulaLess
 */
std::ostream & output(std::ostream & os, const formula::FormulaLess & f)
{
  return (os << f.getPlace().getName() << " < " << f.getTokens());
}

/*!
 * \brief output FormulaLessEqual
 */
std::ostream & output(std::ostream & os, const formula::FormulaLessEqual & f)
{
  return (os << f.getPlace().getName() << " <= " << f.getTokens());
}

} /* namespace __lola */



/*************************************************************************
 ***** PNML output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ios_base & pnml(std::ios_base & ios)
{
  util::FormatData::data(ios) = util::PNML;
  return ios;
}


namespace __pnml
{

/*!
 * \brief petri net output
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  os //< output everything to this stream

  << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n"
  << "<!--\n"
  << "  generator:   " << net.getMetaInformation(os, CREATOR, PACKAGE_STRING) << endl
  << "  input file:  " << net.getMetaInformation(os, INPUTFILE) << endl
  << "  invocation:  " << net.getMetaInformation(os, INVOCATION) << endl
  << "  net size:    " << stat << net << pnml  
  << "\n-->\n\n"

  << "<pnml>\n"
  
  << "  <module>\n"

  << net.interface_

  << "\n    <net id=\"n1\" type=\"PTNet\">\n";

  if (!net.getMetaInformation(os, INPUTFILE).empty())
  {
    os << "    <name>\n"
       << "      <text>" << net.getMetaInformation(os, INPUTFILE) << "</text>\n"
       << "    </name>\n";
  }

  os << net.places_

  << net.transitions_

  << net.arcs_

  << "    </net>\n"

  << "    <finalmarkings>\n"
  << "      <marking>\n"
  << net.finalCondition_
  << "      </marking>\n"  
  << "    </finalmarkings>\n"

  << "  </module>\n"
  
  << "</pnml>\n";

  return os << endl;
}

/*!
 * \brief place output
 */
std::ostream & output(std::ostream & os, const Place & p)
{
  os << "      <place id=\"" << p.getName() << "\"";
  
  if (p.getTokenCount())
  {
    os << ">\n"
       << "        <initialMarking>\n"
       << "          <text>" << p.getTokenCount() << "</text>\n"
       << "        </initialMarking>\n"
       << "      </place>\n";
  } else {
    os << " />\n";
  }

  return os;
}

/*!
 * \brief transition output
 */
std::ostream & output(std::ostream & os, const Transition & t)
{
  os << "      <transition id=\"" << t.getName();
  
  if(t.getLabels().empty())
  {
    return (os << "\" />\n");
  }
  
  os << "\">\n";
  
  PNAPI_FOREACH(label, t.getLabels())
  {
    os << "        <";
    switch(label->first->getType())
    {
    case Label::INPUT: os << "receive"; break;
    case Label::OUTPUT: os << "send"; break;
    case Label::SYNCHRONOUS: os << "synchronize"; break;
    default: assert(false);
    }
    
    os << " idref=\"" + label->first->getName() + "\" />\n";
  }
  
  return (os << "      </transition>\n");
}

/*!
 * \brief arc output
 */
std::ostream & output(std::ostream & os, const Arc & arc)
{
  static PetriNet * net = NULL;
  static unsigned int arcId = 0;
  
  // check for different nets
  if(net != &arc.getPetriNet())
  {
    net = &arc.getPetriNet();
    arcId = 0;
  }
  
  os << "      <arc id=\"arcId" << (++arcId)
     << "\" source=\"" << arc.getSourceNode().getName()
     << "\" target=\"" << arc.getTargetNode().getName()
     << "\"";

  if (arc.getWeight() > 1)
  {
    os << ">\n"
       << "        <inscription>\n"
       << "          <text>" << arc.getWeight() << "</text>\n"
       << "        </inscription>\n"
       << "      </arc>\n";
  }
  else
  {
    os << " />\n";
  }

  return os;
}

/*!
 * \brief interface output
 */
std::ostream & output(std::ostream & os, const Interface & interface)
{
  os << "    <ports>\n";
  
  if(interface.getPorts().size() > 1)
  {
    PNAPI_FOREACH(port, interface.getPorts())
    {
      if(port->first != "")
      {
        os << "      <port id=\"" << port->first << "\">\n"
           << (*port->second)
           << "      </port>\n";
      }
    }
  }
  else
  {
    os << "      <port id=\"portId1\">\n"
       << (*interface.getPort())
       << "      </port>\n";
  }
    
  return (os << "    </ports>\n");
}

/*!
 * \brief port output
 */
std::ostream & output(std::ostream & os, const Port & port)
{
  return (os << delim("\n") << port.getAllLabels() << delim("") << "\n");
}

/*!
 * \brief label output
 */
std::ostream & output(std::ostream & os, const Label & l)
{
  os << "        <";
  
  switch(l.getType())
  {
  case Label::INPUT: os << "input"; break;
  case Label::OUTPUT: os << "output"; break;
  case Label::SYNCHRONOUS: os << "synchronous"; break;
  default: assert(false); 
  }
  
  return (os << " id=\"" << l.getName() << "\" />");
}

/*!
 * \brief negation output
 */
std::ostream & output(std::ostream & os, const formula::Negation & f)
{
  if(f.getChildren().empty())
    assert(false); // FIXME: don't know what to do in this case
  else
    return (os << "NOT (" << (**f.getChildren().begin()) << ")");
}

/*!
 * \brief Conjunction output
 */
std::ostream & output(std::ostream & os, const formula::Conjunction & f)
{
  if(f.getChildren().empty())
  {
    //return os << formula::FormulaTrue();
    assert(false); // FIXME: don't know what to do in this case
  }
  else
    return (os << f.getChildren());
}

/*!
 * \brief Disjunction output
 */
std::ostream & output(std::ostream & os, const formula::Disjunction & f)
{
  if(f.getChildren().empty())
  {
    //return os << formula::FormulaFalse();
    assert(false); // FIXME: don't know what to do in this case
  }
  else
    return (os << delim("      </marking>\n      <marking>\n") << f.getChildren());
}

/*!
 * \brief FormulaTrue output
 */
std::ostream & output(std::ostream & os, const formula::FormulaTrue &)
{
    return os;// << "TRUE";  // TODO: nothing to write?
}

/*!
 * \brief FormulaFalse output
 */
std::ostream & output(std::ostream & os, const formula::FormulaFalse &)
{
  return os;// << "FALSE"; // TODO: nothing to write?
}

/*!
 * \brief FormulaEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaEqual & f)
{
  return (os << "        <place idref=\"" << f.getPlace().getName()
             << "\">\n          <text>" << f.getTokens() 
             << "</text>\n       </place>\n");
}

/*!
 * \brief FormulaNotEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaNotEqual & f)
{
  return (os << f.getPlace().getName() << " # " << f.getTokens());
}

/*!
 * \brief FormulaGreater output
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreater & f)
{
  return (os << f.getPlace().getName() << " > " << f.getTokens());
}

/*!
 * \brief FormulaGreaterEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreaterEqual & f)
{
  return (os << f.getPlace().getName() << " >= " << f.getTokens());
}

/*!
 * \brief FormulaLess output
 */
std::ostream & output(std::ostream & os, const formula::FormulaLess & f)
{
  return (os << f.getPlace().getName() << " < " << f.getTokens());
}

/*!
 * \brief FormulaLessEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaLessEqual & f)
{
  return (os << f.getPlace().getName() << " <= " << f.getTokens());
}

} /* namespace __pnml */



/*************************************************************************
 ***** OWFN output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ios_base & owfn(std::ios_base & ios)
{
  util::FormatData::data(ios) = util::OWFN;
  return ios;
}

/*!
 * \brief suppress role output
 */
std::ostream & noRules(std::ostream & os)
{
  util::RoleData::data(os).role = true;
  return os;
}


namespace __owfn
{

/*!
 * \brief petri net output 
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  os  //< output everything to this stream

  << "{\n  generated by: "
  << net.getMetaInformation(os, io::CREATOR, PACKAGE_STRING)
  << "\n  input file:   "
  << net.getMetaInformation(os, io::INPUTFILE)
  << "\n  invocation:   "
  << net.getMetaInformation(os, io::INVOCATION)
  << "\n  net size:     "
  << stat << net << owfn
  << "\n}\n\n"

  << net.interface_
  
  << util::mode(io::util::PLACE) << delim(";\n  ")
  << "PLACE\n  "
  << io::util::groupPlacesByCapacity(net.places_)
  << ";\n\n" << delim(", ");
  
  if(!net.roles_.empty() && !(util::RoleData::data(os).role))
  {
    os << "ROLES\n  " << net.roles_ << ";\n\n";  
  }
  
  os
  << mode(io::util::PLACE_TOKEN)
  << "INITIALMARKING\n  "
  << io::util::filterMarkedPlaces(net.places_) << ";\n\n"

  << "FINALCONDITION\n  ";
  
  // if formel is total
  if(net.finalCondition_.concerningPlaces().size() == net.places_.size()) 
  {
    Condition tmpCond;
    tmpCond = net.finalCondition_.getFormula();
    set<const Place *> emptyPlaces = tmpCond.getFormula().getEmptyPlaces();
    
    if(emptyPlaces.size() == net.places_.size())
    {
      os << "ALL_PLACES_EMPTY;\n\n\n";
    }
    else
    {
      PNAPI_FOREACH(p, emptyPlaces)
      {
        tmpCond.removePlace(**p);
      }
      
      os << "(" << tmpCond << ") AND ALL_OTHER_PLACES_EMPTY;\n\n\n";
    }
  }
  else
  {
    os << net.finalCondition_ << ";\n\n\n";
  }

  return (os << delim("\n")
             << net.transitions_ << "\n\n{ END OF FILE '"
             << net.getMetaInformation(os, io::OUTPUTFILE) << "' }\n");
}


/*!
 * \brief place output
 */
std::ostream & output(std::ostream & os, const Place & p)
{
  os << p.getName();
  if((ModeData::data(os) == io::util::PLACE_TOKEN) && (p.getTokenCount() != 1))
  {
    os << ": " << p.getTokenCount();
  }
  
  return os;
}

/*!
 * \brief transition output
 */
std::ostream & output(std::ostream & os, const Transition & t)
{
  os << "TRANSITION " << t.getName() << endl << delim(", ");

  if(t.getCost() != 0)
    os << "  COST " << t.getCost() << ";\n";

  if(!t.getRoles().empty() && !(util::RoleData::data(os).role))
  {
    os << "  ROLES\n    " << t.getRoles() << ";\n";
  }

  bool wroteInterfaceLabel = false;
  
  os << "  CONSUME\n    ";
  
  PNAPI_FOREACH(label, t.getLabels())
  {
    if(label->first->getType() == Label::INPUT)
    {
      os << (wroteInterfaceLabel ? ", " : "")
         << label->first->getName();
      
      if(label->second > 1)
      {
        os << ":" << label->second;
      }
  
      wroteInterfaceLabel = true;
    }
  }
  
  os << ((wroteInterfaceLabel && !t.getPresetArcs().empty()) ? ", " : "")
     << t.getPresetArcs() << ";\n"
     << "  PRODUCE\n    ";
  
  wroteInterfaceLabel = false;
  
  PNAPI_FOREACH(label, t.getLabels())
  {
    if(label->first->getType() == Label::OUTPUT)
    {
      os << (wroteInterfaceLabel ? ", " : "")
         << label->first->getName();
      
      if(label->second > 1)
      {
        os << ":" << label->second;
      }
  
      wroteInterfaceLabel = true;
    }
  }
  
  os << ((wroteInterfaceLabel && !t.getPostsetArcs().empty()) ? ", " : "")
     << t.getPostsetArcs() << ";\n";

  if(t.isSynchronized())
  {
    os << "  SYNCHRONIZE\n    " << t.getSynchronousLabels() << ";\n";
  }
  
  return os;
}

/*!
 * \brief arc output
 */
std::ostream & output(std::ostream & os, const Arc & arc)
{
  os << arc.getPlace().getName();
  if (arc.getWeight() > 1)
    os << ":" << arc.getWeight();
  return os;
}

/*!
 * \brief interface output
 */
std::ostream & output(std::ostream & os, const Interface & interface)
{
  os << "INTERFACE\n" << delim(", ");
  
  if(interface.getPorts().size() > 1)
  {
    PNAPI_FOREACH(port, interface.getPorts())
    {
      if(port->first != "")
      {
        os << (*port->second);
      }
    }
  }
  else
  {
    if(!interface.getPort()->getInputLabels().empty())
    {
      os << "  INPUT\n    " << interface.getPort()->getInputLabels() << ";\n"; 
    }
    if(!interface.getPort()->getOutputLabels().empty())
    {
      os << "  OUTPUT\n    " << interface.getPort()->getOutputLabels() << ";\n"; 
    }
    if(!interface.getPort()->getSynchronousLabels().empty())
    {
      os << "  SYNCHRONOUS\n    " << interface.getPort()->getSynchronousLabels() << ";\n"; 
    }
  }
  
  return (os << endl);
}

/*!
 * \brief port output
 */
std::ostream & output(std::ostream & os, const Port & port)
{
  os << "  PORT " << port.getName() << endl;
  
  if(!port.getInputLabels().empty())
  {
    os << "    INPUT\n      " << delim(", ") << port.getInputLabels() << ";\n"; 
  }
  if(!port.getOutputLabels().empty())
  {
    os << "    OUTPUT\n      " << port.getOutputLabels() << ";\n"; 
  }
  if(!port.getSynchronousLabels().empty())
  {
    os << "    SYNCHRONOUS\n      " << port.getSynchronousLabels() << ";\n"; 
  }
  
  return os;
}

/*!
 * \brief label output
 */
std::ostream & output(std::ostream & os, const Label & l)
{
  return (os << l.getName());
}

std::ostream & output(std::ostream & os,
    const std::pair<std::string, std::set<Place *> > & p)
    {
  return os << p.first << ": " << delim(", ") << p.second;
    }

/*!
 * \brief writing internal places grouped by capacity
 */
std::ostream & output(std::ostream & os,
                      const std::pair<unsigned int, std::set<Place *> > & p)
{
  if (p.first > 0)
  {
    os << "SAFE " << p.first << ": ";
  }
  
  return (os << delim(", ") << p.second << delim(";\n  "));
}

/*!
 * \brief negation output
 */
std::ostream & output(std::ostream & os, const formula::Negation & f)
{
  return (os << "NOT (" << **f.getChildren().begin() << ")");
}

/*!
 * \brief Conjunction output
 */
std::ostream & output(std::ostream & os, const formula::Conjunction & f)
{
  if (f.getChildren().empty())
    return (os << formula::FormulaTrue());
  else
    return (os << "(" << delim(" AND ") << f.getChildren() << ")");
}

/*!
 * \brief Disjunction output
 */
std::ostream & output(std::ostream & os, const formula::Disjunction & f)
{
  if (f.getChildren().empty())
    return (os << formula::FormulaFalse());
  else
    return (os << "(" << delim(" OR ") << f.getChildren() << ")");
}

/*!
 * \brief FormulaTrue output
 */
std::ostream & output(std::ostream & os, const formula::FormulaTrue &)
{
  return (os << "TRUE");
}

/*!
 * \brief FormulaFalse output
 */
std::ostream & output(std::ostream & os, const formula::FormulaFalse &)
{
  return (os << "FALSE");
}

/*!
 * \brief FormulaEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaEqual & f)
{
  return (os << f.getPlace().getName() << " = " << f.getTokens());
}

/*!
 * \brief FormulaNotEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaNotEqual & f)
{
  return (os << f.getPlace().getName() << " # " << f.getTokens());
}

/*!
 * \brief FormulaGreater output
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreater & f)
{
  return (os << f.getPlace().getName() << " > " << f.getTokens());
}

/*!
 * \brief FormulaGreaterEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaGreaterEqual & f)
{
  return (os << f.getPlace().getName() << " >= " << f.getTokens());
}

/*!
 * \brief FormulaLess output
 */
std::ostream & output(std::ostream & os, const formula::FormulaLess & f)
{
  return (os << f.getPlace().getName() << " < " << f.getTokens());
}

/*!
 * \brief FormulaLessEqual output
 */
std::ostream & output(std::ostream & os, const formula::FormulaLessEqual & f)
{
  return (os << f.getPlace().getName() << " <= " << f.getTokens());
}

} /* namespace __owfn */


/*************************************************************************
 ***** SA output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ios_base & sa(std::ios_base &base)
{
  util::FormatData::data(base) = util::SA;
  return base;
}


namespace __sa
{

/*!
 * \brief service automaton output
 */
std::ostream & output(std::ostream & os, const Automaton & sa)
{
  os << "INTERFACE" << endl << delim(", ")
     << "  INPUT\n    "
     << sa.getInputLabels()
     << ";\n"
     << "  OUTPUT\n    "
     << sa.getOutputLabels()
     << ";\n"
     << "  SYNCHRONOUS\n    "
     << sa.getSynchronousLabels()
     << ";\n\n"
     << "NODES\n"
     << delim("\n")
     << sa.states_;

  return (os << endl);
}

/*!
 * \brief state output
 */
std::ostream & output(std::ostream & os, const State & s)
{
  os << "  " << s.getName();
  if (s.isInitial() || s.isFinal())
  {
    os << ": "
       << (s.isInitial()
           ? (s.isFinal() ? "INITIAL, FINAL" : "INITIAL")
           : "FINAL");
  }

  return (os << endl << s.getPostsetEdges());
}

/*!
 * \brief edge output
 */
std::ostream & output(std::ostream & os, const Edge & e)
{
  return (os << "    " << e.getLabel() << " -> " << e.getDestination().getName());
}

} /* namespace __sa */

/*************************************************************************
 ***** Woflan output
 *************************************************************************/

/*!
 * \brief writes output type to stream 
 */
std::ostream & woflan(std::ostream & os)
{
  util::FormatData::data(os) = util::WOFLAN;
  return os;
}

namespace __woflan
{

/*!
 * \brief petri net output
 */
std::ostream & output(std::ostream & os, const PetriNet & net)
{
  string creator = net.getMetaInformation(os, CREATOR, PACKAGE_STRING);
  string inputfile = net.getMetaInformation(os, INPUTFILE);

  os //< output everything to this stream

  << "-- Petri net created by " << creator
  << (inputfile.empty() ? "" : " reading " + inputfile) << endl
  << endl

  << mode(io::util::PLACE) << endl
  << "  " << delim(";\n") << net.places_ << ";\n\n"

  // transitions
  << net.transitions_ << ";\n\n";

  return (os << "-- END OF FILE\n");
}

/*!
 * \brief place output
 */
std::ostream & output(std::ostream & os, const Place & p)
{
  os << "place \"" << p.getName() << "\"";
  
  if (p.getTokenCount() > 0)
    os << "  init " << p.getTokenCount();
  
  return os;
}

/*!
 * \brief transition output
 */
std::ostream & output(std::ostream & os, const Transition & t)
{
  return (os << "trans \"" << t.getName() << "\"\n"
            << delim(" ")
            << "  in "
            << t.getPresetArcs()
            << "\n  out "
            << t.getPostsetArcs() << endl);
}

/*!
 * \brief arc output
 */
std::ostream & output(std::ostream & os, const Arc & arc)
{
  for(unsigned int i = 0; i < arc.getWeight(); ++i)
  {
    os << "\"" << arc.getPlace().getName() << "\" ";
  }
  
  return os;
}

} /* namespace __woflan */

} /* namespace io */

} /* namespace pnapi */

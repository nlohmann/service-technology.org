#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include <cassert>

#include "petrinet.h"
#include "parser.h"

#include "io.h"

using std::pair;
using std::string;
using std::ios_base;
using std::ostream;
using std::set;

namespace pnapi
{

  namespace io
  {

    std::ios_base & owfn(std::ios_base & ios)
    {
      util::FormatData::data(ios) = util::OWFN;
      return ios;
    }

    std::ostream & stat(std::ostream & ios)
    {
      util::FormatData::data(ios) = util::STAT;
      return ios;
    }

    std::ostream & dot(std::ostream & ios)
    {
      util::FormatData::data(ios) = util::DOT;
      return ios;
    }

    util::Manipulator<std::pair<MetaInformation, std::string> > 
    meta(MetaInformation i, const std::string & s)
    {
      return util::MetaManipulator(pair<MetaInformation, string>(i, s));
    }


    /*!
     */
    InputError::InputError(Type type, const string & filename, int line, 
			   const string & token, const string & msg) :
      type(type), message(msg), token(token), line(line), filename(filename)
    {
    }


    /*!
     */
    std::ostream & operator<<(std::ostream & os, const io::InputError & e)
    {
      os << e.filename << ":" << e.line << ": error"; 
      if (!e.token.empty())
	switch (e.type)
	  {
	  case io::InputError::SYNTAX_ERROR:   
	    os << " near ‘" << e.token << "’"; 
	    break;
	  case io::InputError::SEMANTIC_ERROR: 
	    os << ": ‘" << e.token << "’"; 
	    break;
	  }
      return os << ": " << e.message;
    }


    /*!
     * Stream the PetriNet object to a given output stream, using the
     * stream format (see pnapi::io).
     */
    std::ostream & operator<<(std::ostream & os, const PetriNet & net)
    {
      switch (util::FormatData::data(os))
	{
	case util::OWFN:   net.output_owfn  (os); break;
	case util::DOT:    net.output_dot   (os); break;
	case util::GASTEX: net.output_gastex(os); break;

	case util::STAT: 
	  os << "|P| = "     << net.internalPlaces_.size() << ", ";
	  os << "|P_in| = "  << net.inputPlaces_.size()    << ", ";
	  os << "|P_out| = " << net.outputPlaces_.size()   << ", ";
	  os << "|T| = "     << net.transitions_.size()    << ", ";
	  os << "|F| = "     << net.arcs_.size();
	  break;

	default: assert(false);
	}

      return os;
    }


    /*!
     * Reads a Petri net from a stream (in most cases backed by a file). The
     * format
     * of the stream data is not determined automatically. You have to set it
     * explicitly using a stream manipulator from pnapi::io.
     */
    std::istream & operator>>(std::istream & is, PetriNet & net) 
      throw (InputError)
    {
      switch (util::FormatData::data(is))
	{
	case util::OWFN:
	  {
	    parser::owfn::Parser parser;
	    parser::owfn::Visitor visitor;
	    parser.parse(is).visit(visitor);
	    net = visitor.getPetriNet();
	    net.meta_ = util::MetaData::data(is);
	    break;
	  }

	default:
	  assert(false);  // unsupported input format
	}

      return is;
    }


    std::istream & operator>>(std::istream & is, const util::Manipulator<
			      std::pair<MetaInformation, std::string> > & m)
    {
      util::MetaData::data(is)[m.data.first] = m.data.second;
      return is;
    }


    std::ostream & operator<<(std::ostream & os, const util::Manipulator<
			      std::pair<MetaInformation, std::string> > & m)
    {
      util::MetaData::data(os)[m.data.first] = m.data.second;
      return os;
    }



    /*************************************************************************
     ***** INTERNAL UTILITIES
     *************************************************************************/

    namespace util
    {

      Manipulator<Mode> mode(Mode m)
      {
	return Manipulator<Mode>(m);
      }


      set<Place *> filterMarkedPlaces(const set<Place *> & places)
      {
	set<Place *> filtered;
	for (set<Place *>::iterator it = places.begin(); it != places.end(); 
	     ++it)
	  if ((*it)->getTokenCount() > 0)
	    filtered.insert(*it);
	return filtered;
      }


      ostream & operator<<(ostream & os, const pnapi::Arc & arc)
      {
	bool interface = true;

	switch (FormatData::data(os))
	  {
	  case OWFN:    /* ARCS: OWFN    */
	    os << arc.getPlace().getName();
	    if (arc.getWeight() != 1)
	      os << ":" << arc.getWeight();
	    break;

	  case DOT:     /* ARCS: DOT     */
	    if (!interface && arc.getPlace().getType() != Place::INTERNAL)
	      break;
	    os << " " << arc.getSourceNode() << " -> " << arc.getTargetNode() 
	       << "\t["; 
	    if (arc.getWeight() != 1)
	      os << "label=\"" << arc.getWeight() << "\"";
	    if (arc.getPlace().getType() == Place::INTERNAL)
	      os << "weight=10000.0";
	    os << "]";
	    break;

	  default: assert(false);
	  }
	return os;
      }


      ostream & operator<<(ostream & os, const pnapi::Node & node)
      {
	const Place * p = dynamic_cast<const Place *>(&node);
	if (p != NULL)
	  return os << *p;
	else
	  return os << *dynamic_cast<const Transition *>(&node);
      }


      ostream & operator<<(ostream & os, const pnapi::Place & p)
      {
	switch (FormatData::data(os))
	  {
	  case OWFN:    /* PLACES: OWFN    */
	    if (ModeData::data(os) == PLACE_CAPACITY &&
		p.getCapacity() > 0)
	      os << "SAFE " << p.getCapacity() << " : ";
	    os << p.getName();
	    if (ModeData::data(os) == PLACE_TOKEN && 
		p.getTokenCount() != 1)
	      os << ": " << p.getTokenCount();
	    break;


	  case DOT:     /* PLACES: DOT     */
	    if (ModeData::data(os) == ARC)
	      {
		os << p.getName();
		break;
	      }
	    os << " " << p.getName() << "\t[";

	    if (p.getTokenCount() == 1)
	      os << "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
	    else if (p.getTokenCount() > 1)
	      os << "label=\"" << p.getTokenCount() << "\" fontcolor=black "
		 << "fontname=\"Helvetica\" fontsize=10";

	    switch (p.getType())
	      {
	      case (Node::INPUT):  os << "fillcolor=orange"; break;
	      case (Node::OUTPUT): os << "fillcolor=yellow"; break;
	      default:    break;
	      }

	    if (p.wasInterface())
	      os << "fillcolor=lightgoldenrod1";

	    os << "]" << endl

	       << " " << p.getName() << "_l\t[style=invis]" << endl
	       << " " << p.getName() << "_l -> " << p.getName() 
	       << " [headlabel=\"" << p.getName() << "\"]";

	    break;


	  default: assert(false);   /* PLACES: <UNKNOWN> */
	  }
	return os;
      }


      ostream & operator<<(ostream & os, const pnapi::Transition & t)
      {
	switch (FormatData::data(os))
	  {
	  case OWFN:    /* TRANSITIONS: OWFN    */
	    os << "TRANSITION " << t.getName();
	    switch (t.getType())
	      {
	      case Node::INTERNAL: os                        << endl; break;
	      case Node::INPUT:    os << " { input }"        << endl; break;
	      case Node::OUTPUT:   os << " { output }"       << endl; break;
	      case Node::INOUT:    os << " { input/output }" << endl; break;
	      }
	    os << "  CONSUME " << t.getPresetArcs()  << ";" << endl
	       << "  PRODUCE " << t.getPostsetArcs() << ";" << endl;
	    break;

	  case DOT:     /* TRANSITIONS: DOT     */
	    if (ModeData::data(os) == ARC)
	      {
		os << t.getName();
		break;
	      }
	    os << " " << t.getName() << "\t[";

	    switch(t.getType())
	      {
	      case(Node::INPUT):  os << "fillcolor=orange"; break;
	      case(Node::OUTPUT):	os << "fillcolor=yellow"; break;
	      case(Node::INOUT):  os 
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
	      default: break;
	      }

	    os << "]" << endl

	       << " " << t.getName() << "_l\t[style=invis]" << endl
	       << " " << t.getName() << "_l -> " << t.getName() 
	       << " [headlabel=\"" << t.getName() << "\"]";

	    break;


	  default: assert(false);   /* TRANSITIONS: <UNKNOWN> */
	  }
	return os;
      }


      ostream & operator<<(ostream & os, const pnapi::Condition & c)
      {
	switch (FormatData::data(os))
	  {
	  case OWFN:    /* CONDITION: OWFN    */
	    // TODO: implement
	    os << "{ NOT IMPLEMENTED YET }";
	    break;

	  default: assert(false);
	  }
	return os;
      }


      ostream & operator<<(ostream & os, 
			   const std::multimap<string, Place *> & places)
      {
	if (places.empty())
	  return os;

	std::multimap<string, Place *>::const_iterator it = places.begin(); 
	PetriNet & net = it->second->getPetriNet();
	while (it != places.end())
	  {
	    string port = it->first;
	    os << "  " << port << ": " << net.getInterfacePlaces(port) << ";" 
	       << endl;
	    it = places.upper_bound(port);
	  }
	return os;
      }

    }

  }

}

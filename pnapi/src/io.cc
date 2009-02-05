#include <cassert>
#include <iostream>

#include "parser.h"
#include "petrinet.h"
#include "io.h"

using std::endl;

namespace pnapi
{

  namespace io
  {

    /*!
     */
    FileIO::FileIO(const string & name) :
      filename(name)
    {
    }


    /*!
     */
    void FileIO::filenameCallback(ios_base::event ev, ios_base & ios, int index)
    {
      // FIXME: check index
      if (ev == ios_base::erase_event)
	{
	  string * & p = (string *&) filenamePointer(ios);
	  if (p != NULL) delete p;
	}
    }


    /*!
     */
    istream & operator>>(istream & is, const FileIO & io)
    {
      return FileIO::setFilename(is, io.filename);
    }


    /*!
     */
    PetriNetIO::PetriNetIO(Mode m) :
      mode_(m)
    {
    }


    /*!
     */
    set<Place *> PetriNetIO::filterMarkedPlaces(const set<Place *> & places)
    {
      set<Place *> filtered;
      for (set<Place *>::iterator it = places.begin(); it != places.end(); ++it)
	if ((*it)->getTokenCount() > 0)
	  filtered.insert(*it);
      return filtered;
    }


    /*!
     */
    ostream & operator<<(ostream & os, const PetriNetIO & io)
    {
      return PetriNetIO::setMode(os, io.getMode());
    }


    /*!
     * Stream the PetriNet object to a given output stream, using the
     * stream format (see pnapi::io).
     */
    ostream & operator<<(ostream & os, const pnapi::PetriNet & net)
    {
      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN:   net.output_owfn  (os); break;
	case PetriNetIO::DOT:    net.output_dot   (os); break;
	case PetriNetIO::GASTEX: net.output_gastex(os); break;

	case PetriNetIO::STAT: 
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
     */
    ostream & operator<<(ostream & os, const pnapi::Arc & arc)
    {
      bool interface = true;

      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN:    /* ARCS: OWFN    */
	  os << arc.getPlace().getName();
	  if (arc.getWeight() != 1)
	    os << ":" << arc.getWeight();
	  break;

	case PetriNetIO::DOT:     /* ARCS: DOT     */
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


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Node & node)
    {
      const Place * p = dynamic_cast<const Place *>(&node);
      if (p != NULL)
	return os << *p;
      else
	return os << *dynamic_cast<const Transition *>(&node);
    }


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Place & p)
    {
      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN:    /* PLACES: OWFN    */
	  if (PetriNetIO::getMode(os) == PetriNetIO::PLACE_CAPACITY &&
	      p.getCapacity() > 0)
	    os << "SAFE " << p.getCapacity() << " : ";
	  os << p.getName();
	  if (PetriNetIO::getMode(os) == PetriNetIO::PLACE_TOKEN && 
	      p.getTokenCount() != 1)
	    os << ": " << p.getTokenCount();
	  break;


	case PetriNetIO::DOT:     /* PLACES: DOT     */
	  if (PetriNetIO::getMode(os) == PetriNetIO::ARC)
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


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Transition & t)
    {
      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN:    /* TRANSITIONS: OWFN    */
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

	case PetriNetIO::DOT:     /* TRANSITIONS: DOT     */
	  if (PetriNetIO::getMode(os) == PetriNetIO::ARC)
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


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Condition & c)
    {
      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN:    /* CONDITION: OWFN    */
	  // TODO: implement
	  os << "{ NOT IMPLEMENTED YET }";
	  break;

	case PetriNetIO::DOT:     /* CONDITION: DOT     */
	  assert(false);
	  break;

	default: assert(false);
	}
      return os;
    }


    /*!
     */
    ostream & operator<<(ostream & os, const io::InputError & e)
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
     * Reads a Petri net from a stream (in most cases backed by a file). The
     * format
     * of the stream data is not determined automatically. You have to set it
     * explicitly using a stream manipulator from pnapi::io.
     */
    istream & operator>>(istream & is, pnapi::PetriNet & net)
    {
      switch (PetriNetIO::getFormat(is))
	{
	case PetriNetIO::OWFN:
	  {
	    parser::owfn::Parser parser;
	    parser::owfn::Visitor visitor;
	    parser.parse(is).visit(visitor);
	    net = visitor.getPetriNet();
	    break;
	  }

	default:
	  assert(false);  // unsupported input format
	}

      return is;
    }



    InputError::InputError(Type type, const string & filename, int line, 
			   const string & token, const string & msg) :
      type(type), message(msg), token(token), line(line), filename(filename)
    {
    }

  }

}

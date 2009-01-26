#include <cassert>

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
	case PetriNetIO::OWFN:   net.output_owfn(os);   break;

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
      os << arc.getPlace().getName();
      if (arc.getWeight() != 1)
	os << ":" << arc.getWeight();
      return os;
    }


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Place & p)
    {
      if (PetriNetIO::getMode(os) == PetriNetIO::PLACE_CAPACITY &&
	  p.getCapacity() > 0)
	os << "SAFE " << p.getCapacity() << " : ";

      os << p.getName();

      if (PetriNetIO::getMode(os) == PetriNetIO::PLACE_TOKEN && 
	  p.getTokenCount() != 1)
	os << ": " << p.getTokenCount();

      return os;
    }


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Transition & t)
    {
      os << "TRANSITION " << t.getName();
      switch (t.getType())
	{
	case Node::INTERNAL: os                        << endl; break;
	case Node::INPUT:    os << " { input }"        << endl; break;
	case Node::OUTPUT:   os << " { output }"       << endl; break;
	case Node::INOUT:    os << " { input/output }" << endl; break;
	}

      os << "  CONSUME " << t.getPresetArcs()  << ";" << endl
	 << "  PRODUCE " << t.getPostsetArcs() << ";" << endl
	 << endl;

      return os;
    }


    /*!
     */
    ostream & operator<<(ostream & os, const pnapi::Condition & c)
    {
      // TODO: implement
      os << "{ NOT IMPLEMENTED YET }";

      return os;
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

  }

}

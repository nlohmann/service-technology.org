#include <cassert>

#include "parser.h"
#include "automaton.h"
#include "io.h"

using std::map;
using std::string;
using std::pair;

namespace pnapi
{

  namespace io
  {

    /*************************************************************************
     ***** Basic I/O Implementation
     *************************************************************************/

    /* FORMAT IMPLEMENTATION: add case label below */


    /*!
     * Stream the PetriNet object to a given output stream, using the
     * stream format (see pnapi::io).
     */
    std::ostream & operator<<(std::ostream & os, const PetriNet & net)
    {
      switch (util::FormatData::data(os))
	{
	case util::DOT:  return __dot::output(os, net);
	case util::LOLA: return __lola::output(os, net);
	case util::OWFN: return __owfn::output(os, net);
	case util::STAT: return __stat::output(os, net);

	default: assert(false);
	}
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

	case util::ONWD:
	  {
	    map<string, PetriNet *> nets = util::PetriNetData::data(is);
	    parser::onwd::Parser parser;
	    parser::onwd::Visitor visitor(nets);
	    parser.parse(is).visit(visitor);
	    net.createFromWiring(visitor.instances(), visitor.wiring());
	    net.meta_ = util::MetaData::data(is);
	    break;
	  }

	default:
	  assert(false);  // unsupported input format
	}

      return is;
    }


    /*!
     * Stream the Automaton object to a given output stream using the stream
     * format (see pnapi::io).
     */
    std::ostream & operator<<(std::ostream &os, const Automaton &sa)
    {
      switch (util::FormatData::data(os))
      {
      case      util::SA:  sa.output_sa(os);   break;
      //case      util::STG: sa.output_stg(os);  break;
      default:  assert(false);
      }

      return os;
    }


    /*!
     * Second Automaton output has to be here because the ServiceAutomaton
     * class can convert Petri nets into a service automaton, and the Automaton
     * class can read automata from file (in .sa/.ig format).
     */
    std::ostream & operator<<(std::ostream &os, const ServiceAutomaton &sa)
    {
      switch (util::FormatData::data(os))
      {
      case      util::SA:   sa.output_sa(os);   break;
      //case      util::STG:  sa.output_stg(os);  break;
      default:  assert(false);
      }

      return os;
    }



    /*************************************************************************
     ***** Internal I/O Implementation
     *************************************************************************/

    /* FORMAT IMPLEMENTATION: add case labels for entities you use */

    namespace util
    {

      ostream & operator<<(ostream & os, const pnapi::Arc & arc)
      {
	switch (FormatData::data(os))
	  {
	  case DOT:  return __dot::output(os, arc);
	  case LOLA: return __lola::output(os, arc);
	  case OWFN: return __owfn::output(os, arc);

	  default: assert(false);
	  }
      }


      ostream & operator<<(ostream & os, const pnapi::Place & p)
      {
	switch (FormatData::data(os))
	  {
	  case DOT:  return __dot::output(os, p);
	  case LOLA: return __lola::output(os, p);
	  case OWFN: return __owfn::output(os, p);

	  default: assert(false);
	  }
      }


      ostream & operator<<(ostream & os, const pnapi::Transition & t)
      {
	switch (FormatData::data(os))
	  {
	  case DOT:  return __dot::output(os, t);
	  case LOLA: return __lola::output(os, t);
	  case OWFN: return __owfn::output(os, t);

	  default: assert(false);
	  }
	return os;
      }


      /* FORMAT IMPLEMENTATION: formula output is in io-format.cc (owfn) */
      /*        should be split up like above if used in a second format */


    } /* namespace util */




    // What follows is generic code, that hopefully rarely needs to be touched.

    
    /*************************************************************************
     ***** Generic I/O Implementation
     *************************************************************************/

    util::Manipulator<std::pair<MetaInformation, std::string> >
    meta(MetaInformation i, const std::string & s)
    {
      return util::MetaManipulator(pair<MetaInformation, string>(i, s));
    }


    InputError::InputError(Type type, const string & filename, int line,
			   const string & token, const string & msg) :
      type(type), message(msg), token(token), line(line), filename(filename)
    {
    }


    std::ostream & operator<<(std::ostream & os, const io::InputError & e)
    {
      os << e.filename << ":" << e.line << ": error";
      if (!e.token.empty())
	switch (e.type)
	  {
	  case io::InputError::SYNTAX_ERROR:
	    os << " near '" << e.token << "'";
	    break;
	  case io::InputError::SEMANTIC_ERROR:
	    os << ": '" << e.token << "'";
	    break;
	  }
      return os << ": " << e.message;
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
     ***** Internal Generic I/O Implementation
     *************************************************************************/

    namespace util
    {

      Manipulator<Mode> mode(Mode m)
      {
	return Manipulator<Mode>(m);
      }


      Manipulator<Delim> delim(const string & s)
      {
	Delim d; d.delim = s;
	return Manipulator<Delim>(d);
      }


      bool compareContainerElements(const Node * n1, const Node * n2)
      {
	return n1->getName() < n2->getName();
      }


      bool compareContainerElements(Place * p1, Place * p2)
      {
	return compareContainerElements((Node *) p1, (Node *) p2);
      }


      bool compareContainerElements(const Place * p1, const Place * p2)
      {
	return compareContainerElements((Node *) p1, (Node *) p2);
      }


      bool compareContainerElements(Transition * t1, Transition * t2)
      {
	return compareContainerElements((Node *) t1, (Node *) t2);
      }


      bool compareContainerElements(Arc * f1, Arc * f2)
      {
	return compareContainerElements(&f1->getPlace(), &f2->getPlace());
      }


      bool compareContainerElements(const formula::Formula *,
				    const formula::Formula *)
      {
	return false;
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


      set<Arc *> filterInternalArcs(const set<Arc *> & arcs)
      {
	set<Arc *> filtered;
	for (set<Arc *>::iterator it = arcs.begin(); it != arcs.end(); ++it)
	  if ((*it)->getPlace().getType() == Place::INTERNAL)
	    filtered.insert(*it);
	return filtered;
      }


      std::multimap<unsigned int, Place *>
      groupPlacesByCapacity(const set<Place *> & places)
      {
	std::multimap<unsigned int, Place *> grouped;
	for (set<Place *>::iterator it = places.begin(); it != places.end();
	     ++it)
	  grouped.insert(pair<unsigned int, Place *>((*it)->getCapacity(),*it));
	return grouped;
      }


      void outputGroupPrefix(std::ostream & os, const std::string & s)
      {
	os << s << ": ";
      }


      void outputGroupPrefix(std::ostream & os, unsigned int capacity)
      {
	if (capacity > 0)
	  os << "SAFE " << capacity << ": ";
      }


      ostream & operator<<(ostream & os, const pnapi::Node & node)
      {
	const Place * p = dynamic_cast<const Place *>(&node);
	if (p != NULL)
	  return os << *p;
	else
	  return os << *dynamic_cast<const Transition *>(&node);
      }


      ostream & operator<<(ostream & os, const pnapi::Condition & c)
      {
	return os << c.formula();
      }


      ostream & operator<<(ostream & os, const formula::Formula & f)
      {
	return f.output(os);
      }


    } /* namespace util */

  } /* namespace io */

} /* namespace pnapi */

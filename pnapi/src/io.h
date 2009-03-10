// -*- C++ -*-

/*!
 * \file    io.h
 *
 * \brief   Input/Output related Structures
 *
 * \author  Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2009/01/19
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#ifndef PNAPI_IO_H
#define PNAPI_IO_H

#include <set>
#include <map>
#include <list>
#include <vector>
#include <ostream>
#include <algorithm>

namespace pnapi
{

  // forward declaration
  class PetriNet;
  class Condition;
  class Automaton;
  class ServiceAutomaton;



  /*!
   * \brief   Input/Output related Structures
   *
   * Everything needed to read and write the structures of the Petri Net API
   * from and to streams.
   */
  namespace io
  {

    // forward declarations
    namespace util { template <typename T> class Manipulator; }


    /*!
     * \brief   Meta Information Types
     */
    enum MetaInformation { INPUTFILE, OUTPUTFILE, INVOCATION, CREATOR };


    /*!
     * \brief   Exception class thrown by operator>>()
     */
    class InputError
    {
    public:

      /// types of errors
      enum Type { SYNTAX_ERROR, SEMANTIC_ERROR };


      /// type of error
      const Type type;

      /// error message
      const std::string message;

      /// last read token
      const std::string token;

      /// line number
      const int line;

      /// filename
      const std::string filename;


      /// constructor
      InputError(Type, const std::string &, int, const std::string &,
		 const std::string &);

    };


    /*!
     * \name  Input/Output Operators
     *
     * reading from and writing to files (and other streams)
     */
    //@{

    /// %PetriNet input
    std::istream & operator>>(std::istream &, PetriNet &) throw (InputError);

    /// %MetaInformation manipulation
    std::istream & operator>>(std::istream &, const util::Manipulator<
			      std::pair<MetaInformation, std::string> > &);

    /// using Manipulators in input
    template <typename T> 
    std::istream & operator>>(std::istream &, const util::Manipulator<T>);

    /// %PetriNet output
    std::ostream & operator<<(std::ostream &, const PetriNet &);

    /// %InputError output
    std::ostream & operator<<(std::ostream &, const InputError &);

    /// %MetaInformation manipulation
    std::ostream & operator<<(std::ostream &, const util::Manipulator<
			      std::pair<MetaInformation, std::string> > &);

    /// %Automaton output
    std::ostream & operator<<(std::ostream &, const Automaton &);
    /// %ServiceAutomaton output
    std::ostream & operator<<(std::ostream &, const ServiceAutomaton &);

    //@}


    /*!
     * \name  Stream Manipulators
     *
     * changing format and meta information
     */
    //@{

    /// Open WorkFlow Net (OWFN) file format
    std::ios_base & owfn(std::ios_base &);

    /// Open Net Wiring Description (ONWD) file format
    std::istream & onwd(std::istream &);

    /// statistical output format
    std::ostream & stat(std::ostream &);

    /// GraphViz DOT output format
    std::ostream & dot(std::ostream &);

    /// Service Automaton (SA) file format
    std::ostream & sa(std::ostream &);

    /// meta information manipulator
    util::Manipulator<std::pair<MetaInformation, std::string> >
    meta(MetaInformation, const std::string &);

    /// manipulator for passing a Petri net collection to input
    util::Manipulator<std::map<std::string, PetriNet *> >
    nets(std::map<std::string, PetriNet *> &);

    //@}

  }



  /*************************************************************************
   ***** INTERNAL UTILITIES
   *************************************************************************/

  // forward declarations
  class Node;
  class Place;
  class Transition;
  class Arc;
  namespace formula
  {
    class Formula;
    class Negation;
    class Conjunction;
    class Disjunction;
    class FormulaEqual;
    class FormulaNotEqual;
    class FormulaGreater;
    class FormulaGreaterEqual;
    class FormulaLess;
    class FormulaLessEqual;
    class FormulaTrue;
    class FormulaFalse;
  }
  class State;
  template <class T> class Edge;

  namespace io
  {

    /*!
     * \brief   Utility Classes and Functions for pnapi::io
     */
    namespace util
    {

      /*** ENUM CONSTANTS ***/

      /// possible I/O formats
      enum Format { STAT, OWFN, DOT, GASTEX, ONWD, SA };

      /// I/O (sub-)mode
      enum Mode { NORMAL, PLACE, PLACE_TOKEN, ARC, INNER };

      /// delimiter type
      struct Delim { std::string delim; };


      /*** TEMPLATE CLASSES ***/

      template <typename T> class Manipulator
      {
      public:
	const T data;
	Manipulator(const T &);
      };


      template <typename T> class StreamMetaData
      {
      public:
	static T & data(std::ios_base &);

      private:
	static int index;
	static void ioscb(std::ios_base::event, std::ios_base &, int);
      };

      template <typename T> int StreamMetaData<T>::index;


      /*** TYPE NAME SHORTCUTS ***/

      typedef StreamMetaData<Format> FormatData;
      typedef StreamMetaData<Mode> ModeData;
      typedef StreamMetaData<Delim> DelimData;
      typedef StreamMetaData<std::map<MetaInformation, std::string> > MetaData;
      typedef Manipulator<std::pair<MetaInformation, std::string> >
              MetaManipulator;
      typedef StreamMetaData<std::map<std::string, PetriNet *> > PetriNetData;
      typedef Manipulator<std::map<std::string, PetriNet *> > 
              PetriNetManipulator;


      /*** NAMESPACE GLOBAL FUNCTIONS AND OPERATORS ***/

      bool compareContainerElements(const Node *, const Node *);
      bool compareContainerElements(Place *, Place *);
      bool compareContainerElements(const Place *, const Place *);
      bool compareContainerElements(Transition *, Transition *);
      bool compareContainerElements(Arc *, Arc *);
      bool compareContainerElements(const formula::Formula *,
				    const formula::Formula *);

      std::set<Place *> filterMarkedPlaces(const std::set<Place *> &);
      std::multimap<unsigned int, Place *>
      groupPlacesByCapacity(const std::set<Place *> &);

      void outputGroupPrefix(std::ostream &, const std::string &);
      void outputGroupPrefix(std::ostream &, unsigned int);

      Manipulator<Mode> mode(Mode);
      Manipulator<Delim> delim(const std::string &);

      std::ostream & operator<<(std::ostream &, const Arc &);
      std::ostream & operator<<(std::ostream &, const Node &);
      std::ostream & operator<<(std::ostream &, const Place &);
      std::ostream & operator<<(std::ostream &, const Transition &);
      std::ostream & operator<<(std::ostream &, const Condition &);
      std::ostream & operator<<(std::ostream &, const formula::Formula &);
      std::ostream & operator<<(std::ostream &, const formula::Negation &);
      std::ostream & operator<<(std::ostream &, const formula::Conjunction &);
      std::ostream & operator<<(std::ostream &, const formula::Disjunction &);
      std::ostream & operator<<(std::ostream &, const formula::FormulaTrue &);
      std::ostream & operator<<(std::ostream &, const formula::FormulaFalse &);
      std::ostream & operator<<(std::ostream &, const formula::FormulaEqual &);
      std::ostream & operator<<(std::ostream &,
				const formula::FormulaNotEqual &);
      std::ostream & operator<<(std::ostream &,
				const formula::FormulaGreater &);
      std::ostream & operator<<(std::ostream &,
				const formula::FormulaGreaterEqual &);
      std::ostream & operator<<(std::ostream &, const formula::FormulaLess &);
      std::ostream & operator<<(std::ostream &,
				const formula::FormulaLessEqual &);

      /*** INTERNALS OF AUTOMATA ***/

      std::ostream & operator<<(std::ostream &, const State &);

      template <class T>
      std::ostream & operator<<(std::ostream &os, const Edge<T> &e)
      {
        os << e.getSource() << " -> " << e.getDestination() << " : ";
        os << e.getLabel();

        return os;
      }


      /*** TEMPLATE IMPLEMENTATION ***/

      template <typename T>
      std::ostream & operator<<(std::ostream & os, const Manipulator<T> m)
      {
	StreamMetaData<T>::data(os) = m.data;
	return os;
      }


      template <typename T>
      std::ostream & outputContainerElement(std::ostream & os, const T * t)
      {
	return os << *t;
      }


      template <typename T>
      std::ostream & outputContainerElement(std::ostream & os,
				     const std::pair<T, std::set<Place *> > & p)
      {
	outputGroupPrefix(os, p.first);
	return os << delim(", ") << p.second;
      }


      template <typename T>
      std::ostream & operator<<(std::ostream & os, const std::vector<T> & v)
      {
	std::string delim = DelimData::data(os).delim;
	if (v.empty()) return os;
	for (typename std::vector<T>::const_iterator it = v.begin();
	     it != --v.end(); ++it)
	  outputContainerElement(os, *it) << delim;
	return outputContainerElement(os, *--v.end());
      }


      template <typename T>
      std::ostream & operator<<(std::ostream & os, const std::set<T> & s)
      {
	// sort the elements
	std::vector<T> v;
	for (typename std::set<T>::iterator it = s.begin(); it != s.end(); ++it)
	  v.push_back(*it);
	bool (*c)(T, T) = compareContainerElements;
	std::sort(v.begin(), v.end(), c);

	// output the sorted vector
	return os << v;
      }


      template <typename T> std::ostream &
      operator<<(std::ostream & os,
		 const std::multimap<T, Place *> & places)
      {
	std::vector<std::pair<T, std::set<Place *> > > metaVector;
	for (typename std::multimap<T, Place *>::const_iterator it =
	       places.begin();
	     it != places.end(); it = places.upper_bound(it->first))
	  {
	    std::set<Place *> subset;
	    for (typename std::multimap<T, Place *>::const_iterator subit = it;
		 subit != places.upper_bound(it->first); ++subit)
	      subset.insert(subit->second);
	    metaVector.push_back(std::pair<T, std::set<Place *> >(it->first, subset));
	  }
        return os << delim("; ") << metaVector;
      }


      template <typename T>
      Manipulator<T>::Manipulator(const T & data) :
	data(data)
      {
      }


      template <typename T>
      T & StreamMetaData<T>::data(std::ios_base & ios)
      {
	static int i = std::ios_base::xalloc();
	index = i;

	T * & p = (T * &) ios.pword(index);
	if (p == NULL)
	  {
	    ios.register_callback(ioscb, index);
	    p = new T;
	  }
	return *p;
      }

      template <typename T>
      void StreamMetaData<T>::ioscb(std::ios_base::event event,
				    std::ios_base & ios, int i)
      {
	T * & p = (T * &) ios.pword(index);
	if (i == index &&
	    event == std::ios_base::erase_event &&
	    p != NULL)
	  {
	    delete p;
	    p = NULL;
	  }
      }

    } /* namespace util */


    /*!
     * implementation of template function
     */
    template <typename T>
    std::istream & operator>>(std::istream & is, const util::Manipulator<T> m)
    {
      util::StreamMetaData<T>::data(is) = m.data;
      return is;
    }

  }

}

#endif

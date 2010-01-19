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


/*
 * FOR IMPLEMENTORS:
 *
 * Some tasks are marked by keywords in the code, so you can jump/grep the files
 * by looking for the keywords. They are:
 *
 * - FORMAT IMPLEMENTATION: where to add/change code when implementing a format
 *
 * Feel free to improve these helpers!
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

// forward declarations
class PetriNet;
class Condition;
class Node;
class Place;
class Transition;
class Arc;
class Automaton;
class State;
class Edge;
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
namespace io
{
namespace util
{
template <typename T> class StreamMetaData;
template <typename T> class Manipulator;
}
}



/***************************************************************************
 ***** PART I: Public I/O Interface
 ***************************************************************************/


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

/// %Automaton input
std::istream & operator>>(std::istream &, Automaton &);

//@}


/*!
 * \name  Stream Manipulators
 *
 * changing format and meta information
 */
//@{

/* FORMAT IMPLEMENTATION: add signature for format manipulator */

/// Open WorkFlow Net (OWFN) file format
std::ios_base & owfn(std::ios_base &);

/// Open Net Wiring Description (ONWD) file format
std::istream & onwd(std::istream &);

/// statistical output format
std::ostream & stat(std::ostream &);

/// GraphViz DOT output format
std::ostream & dot(std::ostream &);

/// Service Automaton (SA) file format
std::ios_base & sa(std::ios_base &);

/// Service Automaton (SA) to State-Machine
std::istream & sa2sm(std::istream &);

/// LOLA file format
std::ios_base & lola(std::ios_base &);

/// PNML file format
std::ios_base & pnml(std::ios_base &);

/// meta information manipulator
util::Manipulator<std::pair<MetaInformation, std::string> >
meta(MetaInformation, const std::string &);

/// manipulator for passing a Petri net collection to input
util::Manipulator<std::map<std::string, PetriNet *> >
nets(std::map<std::string, PetriNet *> &);

/// formula output manipulator
std::ostream & formula(std::ostream &);

//@}




/***************************************************************************
 ***** PART II: (Internal) Format Specific Implementation
 ***************************************************************************/

namespace util
{

/* FORMAT IMPLEMENTATION: add format constant */

/// possible I/O formats
enum Format { STAT, OWFN, DOT, GASTEX, ONWD, SA, SA2SM, LOLA, PNML };

/// I/O (sub-)mode
enum Mode { NORMAL, PLACE, PLACE_TOKEN, ARC, INNER };

} /* namespace util */


/* FORMAT IMPLEMENTATION: add namespace with format specific functions */


//*************************
//*** ONWD input format ***
//*************************

namespace util
{
typedef StreamMetaData<std::map<std::string, PetriNet *> > PetriNetData;
typedef Manipulator<std::map<std::string, PetriNet *> > PetriNetManipulator;
} /* namespace util */


//**************************
//*** STAT output format ***
//**************************
namespace __stat
{
std::ostream & output(std::ostream &, const PetriNet &);

std::ostream & output(std::ostream &, const Automaton &);
}


//**************************
//*** LOLA output format ***
//**************************

/*!
 * \brief   LOLA I/O implementation
 */
namespace __lola
{
std::ostream & output(std::ostream &, const PetriNet &);
std::ostream & output(std::ostream &, const Arc &);
std::ostream & output(std::ostream &, const Place &);
std::ostream & output(std::ostream &, const Transition &);
std::ostream & output(std::ostream &, const formula::Negation &);
std::ostream & output(std::ostream &, const formula::Conjunction &);
std::ostream & output(std::ostream &, const formula::Disjunction &);
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
std::ostream & output(std::ostream &, const formula::FormulaLess &);
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
std::ostream & output(std::ostream &, const formula::FormulaGreaterEqual &);
} /* namespace __lola */


//**************************
//*** PNML output format ***
//**************************

/*!
 * \brief   PNML I/O implementation
 */
namespace __pnml
{
std::ostream & outputInterface(std::ostream & os, const PetriNet & net);
std::ostream & output(std::ostream &, const PetriNet &);
std::ostream & output(std::ostream &, const Arc &);
std::ostream & output(std::ostream &, const Place &);
std::ostream & output(std::ostream &, const Transition &);
std::ostream & output(std::ostream &, const formula::Negation &);
std::ostream & output(std::ostream &, const formula::Conjunction &);
std::ostream & output(std::ostream &, const formula::Disjunction &);
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
std::ostream & output(std::ostream &, const formula::FormulaLess &);
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
std::ostream & output(std::ostream &, const formula::FormulaGreaterEqual &);
} /* namespace __pnml */


//*************************
//*** DOT output format ***
//*************************

namespace __dot
{
std::ostream & output(std::ostream &, const PetriNet &);
std::ostream & output(std::ostream &, const Arc &);
std::ostream & output(std::ostream &, const Place &);
std::ostream & output(std::ostream &, const Transition &);
std::ostream & output(std::ostream &,
    const std::pair<std::string, std::set<Place *> > &);
std::ostream & output(std::ostream &, const Node &, const std::string &);
std::string getNodeName(const Node &, bool withSuffix = false);

std::ostream & output(std::ostream &, const Automaton &);
}


//**************************
//*** OWFN output format ***
//**************************

namespace __owfn
{
std::ostream & output(std::ostream &, const PetriNet &);
std::ostream & output(std::ostream &, const Arc &);
std::ostream & output(std::ostream &, const Place &);
std::ostream & output(std::ostream &, const Transition &);
std::ostream & output(std::ostream &, const formula::Negation &);
std::ostream & output(std::ostream &, const formula::Conjunction &);
std::ostream & output(std::ostream &, const formula::Disjunction &);
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
std::ostream & output(std::ostream &, const formula::FormulaLess &);
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
std::ostream & output(std::ostream &,
    const formula::FormulaGreaterEqual &);
std::ostream & output(std::ostream &,
    const std::pair<std::string, std::set<Place *> > &);
std::ostream & output(std::ostream &,
    const std::pair<unsigned int, std::set<Place *> > &);
}


//**************************
//*** SA output format   ***
//**************************

namespace __sa
{
std::ostream & output(std::ostream &, const Automaton &);
std::ostream & output(std::ostream &, const State &);

std::ostream & output(std::ostream &, const std::vector<State *> &);
std::ostream & output(std::ostream &, const std::set<Edge *> &);
std::ostream & output(std::ostream &, const std::set<std::string> &);
}




/***************************************************************************
 ***** PART III: Internal Generic I/O Implementation
 ***************************************************************************/


/*!
 * \brief   Utility Classes and Functions for pnapi::io
 */
namespace util
{

/// delimiter type
struct Delim { std::string delim; };

/// formula type
struct Formula
{
  bool formula;
  Formula() : formula(false) {}
};


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

typedef StreamMetaData<io::util::Format> FormatData;
typedef StreamMetaData<io::util::Mode> ModeData;
typedef StreamMetaData<Delim> DelimData;
typedef StreamMetaData<Formula> FormulaData;
typedef StreamMetaData<std::map<pnapi::io::MetaInformation, std::string> > MetaData;
typedef Manipulator<std::pair<pnapi::io::MetaInformation, std::string> >
MetaManipulator;


/*** NAMESPACE GLOBAL FUNCTIONS AND OPERATORS ***/

bool compareContainerElements(std::string, std::string);
bool compareContainerElements(const Node *, const Node *);
bool compareContainerElements(Place *, Place *);
bool compareContainerElements(const Place *, const Place *);
bool compareContainerElements(Transition *, Transition *);
bool compareContainerElements(Arc *, Arc *);
bool compareContainerElements(const formula::Formula *,
    const formula::Formula *);

std::set<Place *> filterMarkedPlaces(const std::set<Place *> &);
std::set<Arc *> filterInternalArcs(const std::set<Arc *> &);
std::set<const formula::Formula *>
filterInterfacePropositions(const std::set<const formula::Formula *> &);
std::multimap<unsigned int, Place *>
groupPlacesByCapacity(const std::set<Place *> &);
std::set<std::string>
collectSynchronizeLabels(const std::set<Transition *> &);

Manipulator<io::util::Mode> mode(io::util::Mode);
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
std::ostream & operator<<(std::ostream &,
    const std::pair<std::string, std::set<Place *> > &);
std::ostream & operator<<(std::ostream &,
    const std::pair<unsigned int, std::set<Place *> > &);


/*** TEMPLATE IMPLEMENTATION ***/

template <typename T>
std::ostream & operator<<(std::ostream & os, const Manipulator<T> m)
{
  StreamMetaData<T>::data(os) = m.data;
  return os;
}


std::ostream & outputContainerElement(std::ostream &, const std::string &);


template <typename T>
std::ostream & outputContainerElement(std::ostream & os,
    const std::pair<T, std::set<Place *> > & p)
    {
  return os << p;
    }


template <typename T>
std::ostream & outputContainerElement(std::ostream & os, const T * t)
{
  return os << *t;
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
  return os << metaVector;
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


template <typename T>
std::istream & operator>>(std::istream & is, const util::Manipulator<T> m)
{
  util::StreamMetaData<T>::data(is) = m.data;
  return is;
}


} /* namespace io */

} /* namespace pnapi */

#endif

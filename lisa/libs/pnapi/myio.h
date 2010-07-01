// -*- C++ -*-

/*!
 * \file    myio.h
 *
 * \brief   Input/Output related Structures
 *
 * \author  Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author: georgstraube $
 *
 * \since   2009/01/19
 *
 * \date    $Date: 2010-03-22 20:02:11 +0100 (Mon, 22 Mar 2010) $
 *
 * \version $Revision: 5538 $
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


#ifndef PNAPI_MYIO_H
#define PNAPI_MYIO_H

#include "exception.h"
#include "util.h"

#include <vector>
#include <ostream>

namespace pnapi
{

// forward declarations
class PetriNet;
class Condition;
class Node;
class Place;
class Transition;
class Arc;
class Interface;
class Port;
class Label;
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

/*!
 * \brief   Meta Information Types
 */
enum MetaInformation
{
  INPUTFILE,
  OUTPUTFILE,
  INVOCATION,
  CREATOR
};


/*!
 * \name  Input/Output Operators
 *
 * reading from and writing to files (and other streams)
 */
//@{
/// %PetriNet input
std::istream & operator>>(std::istream &, PetriNet &) throw (exception::InputError);
/// %MetaInformation manipulation
std::istream & operator>>(std::istream &,
                          const util::Manipulator<std::pair<MetaInformation, std::string> > &);
/// using Manipulators in input
template <typename T>
std::istream & operator>>(std::istream &, const util::Manipulator<T>);
/// %PetriNet output
std::ostream & operator<<(std::ostream &, const PetriNet &);
/// general exception output
std::ostream & operator<<(std::ostream &, const exception::Error &);
/// %InputError output
std::ostream & operator<<(std::ostream &, const exception::InputError &);
/// assertion output
std::ostream & operator<<(std::ostream &, const exception::AssertionFailedError &);
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
/// statistical output format
std::ostream & stat(std::ostream &);
/// GraphViz DOT output format
std::ostream & dot(std::ostream &);
/// Service Automaton (SA) file format
std::ios_base & sa(std::ios_base &);
/// LOLA file format
std::ios_base & lola(std::ios_base &);
/// PNML file format
std::ios_base & pnml(std::ios_base &);
/// WOFLAN file format
std::ios_base & woflan(std::ios_base &);
/// formula output manipulator
std::ostream & formula(std::ostream &);
/// suppress role output
std::ostream & noRoles(std::ostream &);

/// meta information manipulator
util::Manipulator<std::pair<MetaInformation, std::string> >
meta(MetaInformation, const std::string &);
/// manipulator for passing a Petri net collection to input
util::Manipulator<std::map<std::string, PetriNet *> >
nets(std::map<std::string, PetriNet *> &);
//@}


/***************************************************************************
 ***** PART II: (Internal) Format Specific Implementation
 ***************************************************************************/

namespace util
{

/* FORMAT IMPLEMENTATION: add format constant */

/// possible I/O formats
enum Format {
  /// statistical output
  STAT,
  /// open net
  OWFN,
  /// dot output
  DOT,
  /// service automaton
  SA,
  /// LoLA format
  LOLA,
  /// petri net modeling language (?)
  PNML,
  /// WOFLAN format
  WOFLAN
};

/// I/O (sub-)mode
enum Mode
{
  NORMAL,
  PLACE,
  PLACE_TOKEN,
  ARC,
  INNER
};

} /* namespace util */


/* FORMAT IMPLEMENTATION: add namespace with format specific functions */


//**************************
//*** STAT output format ***
//**************************
/*!
 * \brief statistical output
 */
namespace __stat
{
/// petri net statistics
std::ostream & output(std::ostream &, const PetriNet &);
/// service autotmaton statistics
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
/// petri net output
std::ostream & output(std::ostream &, const PetriNet &);
/// arc output
std::ostream & output(std::ostream &, const Arc &);
/// place output
std::ostream & output(std::ostream &, const Place &);
/// transition output
std::ostream & output(std::ostream &, const Transition &);
/// negation output
std::ostream & output(std::ostream &, const formula::Negation &);
/// conjunction output
std::ostream & output(std::ostream &, const formula::Conjunction &);
/// disjunction output
std::ostream & output(std::ostream &, const formula::Disjunction &);
/// FormulaTrue output
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
/// FormulaFalse output
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
/// FormulaEqual output
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
/// FormulaNotEqual output
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
/// FormulaLess output
std::ostream & output(std::ostream &, const formula::FormulaLess &);
/// FormulaLessEqual output
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
/// FormulaGreater output
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
/// FormulaGreaterEqual output
std::ostream & output(std::ostream &, const formula::FormulaGreaterEqual &);
} /* namespace __lola */


//**************************
//*** PNML output format ***
//**************************

/*!
 * \brief   PNML I/O implementation
 * 
 * \todo review formula output
 */
namespace __pnml
{
/// petri net output
std::ostream & output(std::ostream &, const PetriNet &);
/// arc output
std::ostream & output(std::ostream &, const Arc &);
/// place output
std::ostream & output(std::ostream &, const Place &);
/// transition output
std::ostream & output(std::ostream &, const Transition &);
/// interface output
std::ostream & output(std::ostream &, const Interface &);
/// port output
std::ostream & output(std::ostream &, const Port &);
/// label output
std::ostream & output(std::ostream &, const Label &);
/// negation output
std::ostream & output(std::ostream &, const formula::Negation &);
/// conjunction output
std::ostream & output(std::ostream &, const formula::Conjunction &);
/// disjunction output
std::ostream & output(std::ostream &, const formula::Disjunction &);
/// FormulaTrue output
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
/// FormulaFalse output
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
/// FormulaEqual output
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
/// FormulaNotEqual output
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
/// FormulaLess output
std::ostream & output(std::ostream &, const formula::FormulaLess &);
/// FormulaLessEqual output
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
/// FormulaGreater output
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
/// FormulaGreaterEqual output
std::ostream & output(std::ostream &, const formula::FormulaGreaterEqual &);
} /* namespace __pnml */


//*************************
//*** DOT output format ***
//*************************

/*!
 * \brief dot output implementation
 */
namespace __dot
{
/// petri net output
std::ostream & output(std::ostream &, const PetriNet &);
/// arc output
std::ostream & output(std::ostream &, const Arc &);
/// place output
std::ostream & output(std::ostream &, const Place &);
/// transition output
std::ostream & output(std::ostream &, const Transition &);
/// interface output
std::ostream & output(std::ostream &, const Interface &);
/// port output
std::ostream & output(std::ostream &, const Port &);
/// label output
std::ostream & output(std::ostream &, const Label &);
/// node with attribut
std::ostream & output(std::ostream &, const Node &, const std::string &);
/// get a unique node name for dot output
std::string getNodeName(const Node &, bool = false);
/// get a unique label name for dot output
std::string getLabelName(const Label &, bool = false);
/// automaton output
std::ostream & output(std::ostream &, const Automaton &);
}


//**************************
//*** OWFN output format ***
//**************************

/*!
 *\brief owfn I/O Implementation
 */
namespace __owfn
{
/// petri net output
std::ostream & output(std::ostream &, const PetriNet &);
/// arc output
std::ostream & output(std::ostream &, const Arc &);
/// place output
std::ostream & output(std::ostream &, const Place &);
/// transition output
std::ostream & output(std::ostream &, const Transition &);
/// interface output
std::ostream & output(std::ostream &, const Interface &);
/// port output
std::ostream & output(std::ostream &, const Port &);
/// label output
std::ostream & output(std::ostream &, const Label &);
/// negation output
std::ostream & output(std::ostream &, const formula::Negation &);
/// conjunction output
std::ostream & output(std::ostream &, const formula::Conjunction &);
/// disjunction output
std::ostream & output(std::ostream &, const formula::Disjunction &);
/// FormulaTrue output
std::ostream & output(std::ostream &, const formula::FormulaTrue &);
/// FormulaFalse output
std::ostream & output(std::ostream &, const formula::FormulaFalse &);
/// FormulaEqual output
std::ostream & output(std::ostream &, const formula::FormulaEqual &);
/// FormulaNotEqual output
std::ostream & output(std::ostream &, const formula::FormulaNotEqual &);
/// FormulaLess output
std::ostream & output(std::ostream &, const formula::FormulaLess &);
/// FormulaLessEqual output
std::ostream & output(std::ostream &, const formula::FormulaLessEqual &);
/// FormulaGreater output
std::ostream & output(std::ostream &, const formula::FormulaGreater &);
/// FormulaGreaterEqual output
std::ostream & output(std::ostream &, const formula::FormulaGreaterEqual &);
/// port output
std::ostream & output(std::ostream &, const std::pair<std::string, std::set<Place *> > &);
/// safeness output (?)
std::ostream & output(std::ostream &, const std::pair<unsigned int, std::set<Place *> > &);
}


//**************************
//*** SA output format   ***
//**************************

/*!
 * \brief Service Automaton I/O Implementation
 */
namespace __sa
{
/// automaton output
std::ostream & output(std::ostream &, const Automaton &);
/// state output
std::ostream & output(std::ostream &, const State &);
/// edges output
std::ostream & output(std::ostream &, const Edge &);
}


//****************************
//*** Woflan output format ***
//****************************

/*!
* \brief Woflan I/O implementation
*/
namespace __woflan
{
/// petri net output
std::ostream & output(std::ostream &, const PetriNet &);
/// arc output
std::ostream & output(std::ostream &, const Arc &);
/// place output
std::ostream & output(std::ostream &, const Place &);
/// transition output
std::ostream & output(std::ostream &, const Transition &);
} /* namespace __woflan*/


/***************************************************************************
 ***** PART III: Internal Generic I/O Implementation
 ***************************************************************************/


/*!
 * \brief   Utility Classes and Functions for pnapi::io
 */
namespace util
{

/// delimiter type
struct Delim
{
  std::string delim;
};

/// formula type
struct Formula
{
  bool formula;
  Formula() : formula(false) {}
};

/// role type
struct Role
{
  bool role;
  Role() : role(false) {}
};

/*** TEMPLATE CLASSES ***/

/*!
 * \brief generic manipulator class
 */
template <typename T> class Manipulator
{
public:
  const T data;
  Manipulator(const T &);
};

/*!
 * \brief generic class to store meta data in a stream
 */
template <typename T> class StreamMetaData
{
public:
  /// get data from stream
  static T & data(std::ios_base &);

private:
  /// index of place in stream's void pointer array
  static int index;
  /// callback function for stream
  static void ioscb(std::ios_base::event, std::ios_base &, int);
};

/// black magic
template <typename T> int StreamMetaData<T>::index;


/*** TYPE NAME SHORTCUTS ***/

typedef StreamMetaData<io::util::Format> FormatData;
typedef StreamMetaData<io::util::Mode> ModeData;
typedef StreamMetaData<Delim> DelimData;
typedef StreamMetaData<Formula> FormulaData;
typedef StreamMetaData<Role> RoleData;
typedef StreamMetaData<std::map<pnapi::io::MetaInformation, std::string> > MetaData;
typedef Manipulator<std::pair<pnapi::io::MetaInformation, std::string> > MetaManipulator;


/*** NAMESPACE GLOBAL FUNCTIONS AND OPERATORS ***/

bool compareContainerElements(std::string, std::string);
bool compareContainerElements(const Node *, const Node *);
bool compareContainerElements(Place *, Place *);
bool compareContainerElements(const Place *, const Place *);
bool compareContainerElements(Transition *, Transition *);
bool compareContainerElements(Arc *, Arc *);
bool compareContainerElements(const formula::Formula *, const formula::Formula *);
bool compareContainerElements(Label *, Label *);
bool compareContainerElements(const Label *, const Label *);
bool compareContainerElements(Edge *, Edge *);

/// \todo check for obsolete methods

std::set<Place *> filterMarkedPlaces(const std::set<Place *> &);
std::multimap<unsigned int, Place *> groupPlacesByCapacity(const std::set<Place *> &);

Manipulator<io::util::Mode> mode(io::util::Mode);
Manipulator<Delim> delim(const std::string &);

std::ostream & operator<<(std::ostream &, const Arc &);
std::ostream & operator<<(std::ostream &, const Node &);
std::ostream & operator<<(std::ostream &, const Place &);
std::ostream & operator<<(std::ostream &, const Transition &);
std::ostream & operator<<(std::ostream &, const Interface &);
std::ostream & operator<<(std::ostream &, const Port &);
std::ostream & operator<<(std::ostream &, const Label &);
std::ostream & operator<<(std::ostream &, const Condition &);
std::ostream & operator<<(std::ostream &, const formula::Formula &);
std::ostream & operator<<(std::ostream &, const formula::Negation &);
std::ostream & operator<<(std::ostream &, const formula::Conjunction &);
std::ostream & operator<<(std::ostream &, const formula::Disjunction &);
std::ostream & operator<<(std::ostream &, const formula::FormulaTrue &);
std::ostream & operator<<(std::ostream &, const formula::FormulaFalse &);
std::ostream & operator<<(std::ostream &, const formula::FormulaEqual &);
std::ostream & operator<<(std::ostream &, const formula::FormulaNotEqual &);
std::ostream & operator<<(std::ostream &, const formula::FormulaGreater &);
std::ostream & operator<<(std::ostream &, const formula::FormulaGreaterEqual &);
std::ostream & operator<<(std::ostream &, const formula::FormulaLess &);
std::ostream & operator<<(std::ostream &, const formula::FormulaLessEqual &);
std::ostream & operator<<(std::ostream &, const std::pair<std::string, std::set<Place *> > &);
std::ostream & operator<<(std::ostream &, const std::pair<unsigned int, std::set<Place *> > &);
std::ostream & operator<<(std::ostream &, const State &);
std::ostream & operator<<(std::ostream &, const Edge &);


/*** TEMPLATE IMPLEMENTATION ***/

/*!
 * \brief write manipulator data to stream
 */
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

/*!
 * \brief given a const pointer to something, write the value, not the adress
 */
template <typename T>
std::ostream & outputContainerElement(std::ostream & os, const T * t)
{
  return os << *t;
}

/*!
 * \brief write vector elements, seperated by given delimeter, to stream
 */
template <typename T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & v)
{
  std::string delim = DelimData::data(os).delim;
  if (v.empty())
    return os;
  for(typename std::vector<T>::const_iterator it = v.begin();
      it != --v.end(); ++it)
  {
    outputContainerElement(os, *it) << delim;
  }
  return outputContainerElement(os, *--v.end());
}

/*!
 * \brief write set elements, seperated by given delimeter, to stream
 */
template <typename T>
std::ostream & operator<<(std::ostream & os, const std::set<T> & s)
{
  // sort the elements
  std::vector<T> v;
  PNAPI_FOREACH(it, s)
  {
    v.push_back(*it);
  }
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
  for(typename std::multimap<T, Place *>::const_iterator it = places.begin();
       it != places.end(); it = places.upper_bound(it->first))
  {
    std::set<Place *> subset;
    for(typename std::multimap<T, Place *>::const_iterator subit = it;
         subit != places.upper_bound(it->first); ++subit)
    {
      subset.insert(subit->second);
    }
    metaVector.push_back(std::pair<T, std::set<Place *> >(it->first, subset));
  }
  return os << metaVector;
}


template <typename T>
Manipulator<T>::Manipulator(const T & data) :
  data(data)
{
}


/*!
 * \brief access to stream meta data
 */
template <typename T>
T & StreamMetaData<T>::data(std::ios_base & ios)
{
  // get index in void pointer array
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

/*!
 * \brief callback function for streams
 */
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

#endif /* PNAPI_MYIO_H */

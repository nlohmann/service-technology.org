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
#include <ostream>

namespace pnapi
{

  // forward declaration
  class PetriNet;
  namespace formula
  {
    class Condition;
  }


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

    /// %PetriNet output
    std::ostream & operator<<(std::ostream &, const PetriNet &);

    /// %InputError output
    std::ostream & operator<<(std::ostream &, const InputError &);

    /// %MetaInformation manipulation
    std::ostream & operator<<(std::ostream &, const util::Manipulator<
			      std::pair<MetaInformation, std::string> > &);

    //@}


    /*!
     * \name  Stream Manipulators
     *
     * changing format and meta information
     */
    //@{

    /// Open WorkFlow Net (OWFN) file format
    std::ios_base & owfn(std::ios_base &);

    /// statistical output format
    std::ostream & stat(std::ostream &);

    /// GraphViz DOT output format
    std::ostream & dot(std::ostream &);

    /// meta information manipulator
    util::Manipulator<std::pair<MetaInformation, std::string> > 
    meta(MetaInformation, const std::string &);

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
  class Condition;

  namespace io
  {

    /*!
     * \brief   Utility Classes and Functions for pnapi::io
     */
    namespace util
    {
    
      /*** ENUM CONSTANTS ***/

      /// possible I/O formats
      enum Format { STAT, OWFN, DOT, GASTEX };
      
      /// I/O (sub-)mode
      enum Mode { PLACE, PLACE_CAPACITY, PLACE_TOKEN, ARC };

      
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
      typedef StreamMetaData<std::map<MetaInformation, std::string> > MetaData;
      typedef Manipulator<std::pair<MetaInformation, std::string> > 
              MetaManipulator;


      /*** NAMESPACE GLOBAL FUNCTIONS AND OPERATORS ***/

      /// filter places with token count > 0
      std::set<Place *> filterMarkedPlaces(const std::set<Place *> &);

      Manipulator<Mode> mode(Mode);

      std::ostream & operator<<(std::ostream &, const pnapi::Arc &);
      std::ostream & operator<<(std::ostream &, const pnapi::Node &);
      std::ostream & operator<<(std::ostream &, const pnapi::Place &);
      std::ostream & operator<<(std::ostream &, const pnapi::Transition &);
      std::ostream & operator<<(std::ostream &, 
				const pnapi::formula::Condition &);
      std::ostream & operator<<(std::ostream &, 
			    const std::multimap<std::string, pnapi::Place *> &);


      /*** TEMPLATE CLASS IMPLEMENTATION ***/

      template <typename T>
      std::ostream & operator<<(std::ostream & os, const Manipulator<T> m)
      {
	StreamMetaData<T>::data(os) = m.data;
	return os;
      }


      inline std::string getOWFNDelimiter(std::ostream & os, 
					  const std::set<Arc *> &) 
      { 
	switch (ModeData::data(os))
	  {
	  case PLACE_CAPACITY: return "; ";
	  default:             return ", "; 
	  }
      }
      inline std::string getOWFNDelimiter(std::ostream & os, 
					  const std::set<Place *> &)      
      { return "; "; }
      inline std::string getOWFNDelimiter(std::ostream & os, 
					  const std::set<Transition *> &) 
      { return "\n"  ; }


      template <typename T>
      std::ostream & operator<<(std::ostream & os, const std::set<T> & s)
      {
	std::string delim;
	switch (FormatData::data(os))
	  {
	  case OWFN: delim = getOWFNDelimiter(os, s); break;
	  case DOT:  delim = "\n";                break;
	  default:   delim = ", ";                break;
	  }
	if (s.empty()) return os;
	for (typename std::set<T>::iterator it = s.begin(); it != --s.end(); 
	     ++it)
	  os << **it << delim;
	return os << **--s.end();
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

    }

  }

}

#endif

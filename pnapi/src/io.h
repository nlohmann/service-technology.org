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

#include <istream>
#include <ostream>
#include <iterator>
#include <iostream>
#include <set>

using std::string;
using std::set;
using std::ios_base;
using std::istream;
using std::ostream;
using std::ostream_iterator;

namespace pnapi
{

  // forward declaration
  class PetriNet;
  class Node;
  class Place;
  class Transition;
  class Arc;
  class Condition;
  namespace parser { class InputError; }


  /*!
   * \brief   Input/Output related Structures
   *
   * Everything needed to read and write the structures of the Petri Net API
   * from and to streams.
   *
   * For the moment, only PetriNet output in the following formats is working:
   *
   * - statistical output (short summary)
   *   \code
   *   ostream << stat << petrinet;
   *   \endcode
   *   see also: stat(), operator<<(ostream &, const PetriNet &)
   *
   * - Open WorkFlow Net file format (OWFN)
   *   \code
   *   ostream << owfn << petrinet;
   *   \endcode
   *   see also: owfn(), operator<<(ostream &, const PetriNet &)
   *
   * - GraphViz file format (DOT)
   *   \code
   *   ostream << dot << petrinet;
   *   \endcode
   *   see also: dot(), operator<<(ostream &, const PetriNet &)
   */
  namespace io
  {

    /*!
     * \brief   Thrown by Parser::parse() and Node::visit()
     */
    class InputError
    {
    public:

      /// types of errors
      enum Type { SYNTAX_ERROR, SEMANTIC_ERROR };


      /// type of error
      const Type type;

      /// error message
      const string message;
      
      /// last read token
      const string token;

      /// line number
      const int line;

      /// filename
      const string filename;


      /// constructor
      InputError(Type, const string &, int, const string &, const string &);

    };


    enum MetaInformation { FILENAME };

    /*!
     */
    class FileIO
    {
    public:
      const string filename;

      FileIO(const string &);

      inline static string getFilename(ios_base &, 
				       const string & = "<unknown>");
      template <typename T> static T & setFilename(T &, const string &);

    private:
      inline static void * & filenamePointer(ios_base &);
      static void filenameCallback(ios_base::event, ios_base &, int);
    };


    /*!
     * \brief   Petri net I/O helpers
     */
    class PetriNetIO
    {
    public:

      /// possible I/O formats for Petri nets
      enum Format { STAT, OWFN, DOT, GASTEX };

      /// I/O (sub-)mode
      enum Mode { PLACE, PLACE_CAPACITY, PLACE_TOKEN, ARC };

      /// constructor
      PetriNetIO(Mode);

      /// filter places with token count > 0
      static set<Place *> filterMarkedPlaces(const set<Place *> &);

      inline static Format getFormat(ios_base &);
      template <typename T> static T & setFormat(T &, Format);

      inline Mode getMode() const;
      inline static Mode getMode(ios_base &);
      template <typename T> static T & setMode(T &, Mode);

    private:

      Mode mode_;

      inline static long & formatFlag(ios_base &);

      inline static long & modeFlag(ios_base &);

    };


    /// PetriNet input
    istream & operator>>(istream & , PetriNet &);

    /// filename setting
    istream & operator>>(istream &, const FileIO &);


    /// (sub-)mode setting
    ostream & operator<<(ostream &, const PetriNetIO &);

    /// pnapi::PetriNet output
    ostream & operator<<(ostream &, const pnapi::PetriNet &);

    /// pnapi::Node output
    ostream & operator<<(ostream &, const pnapi::Arc &);

    /// pnapi::Node output
    ostream & operator<<(ostream &, const pnapi::Node &);

    /// pnapi::Place output
    ostream & operator<<(ostream &, const pnapi::Place &);

    /// pnapi::Transition output
    ostream & operator<<(ostream &, const pnapi::Transition &);

    /// pnapi::Condition output
    ostream & operator<<(ostream &, const pnapi::Condition &);


    /// InputError output
    ostream & operator<<(ostream &, const pnapi::io::InputError &);


    /// mode manipulator
    inline PetriNetIO setMode(PetriNetIO::Mode m) {
      return PetriNetIO(m); };

    /// filename manipulator
    inline FileIO meta(MetaInformation key, const string & value) {
      return FileIO(value); };

    /// statistical output manipulator
    inline ostream & stat(ostream & os) {
      return PetriNetIO::setFormat(os, PetriNetIO::STAT); }

    /// OWFN manipulator
    inline ios_base & owfn(ios_base & os) {
      return PetriNetIO::setFormat(os, PetriNetIO::OWFN); }

    /// DOT manipulator
    inline ostream & dot(ostream & os) {
      return PetriNetIO::setFormat(os, PetriNetIO::DOT); }



    /*!
     */
    string FileIO::getFilename(ios_base & ios, const string & def)
    {
      string * name = static_cast<string *>(filenamePointer(ios));
      return name != NULL ? *name : def;
    }


    /*!
     */
    template <typename T>
    T & FileIO::setFilename(T & ios, const string & name)
    {
      // FIXME: register callback with DELETE
      filenamePointer(ios) = new string(name);
      return ios;
    }


    /*!
     */
    void * & FileIO::filenamePointer(ios_base & ios)
    {
      static int n = ios_base::xalloc();
      ios.register_callback(FileIO::filenameCallback, n);
      return ios.pword(n);
    }


    /*!
     */
    template <typename T>
    T & PetriNetIO::setFormat(T & ios, Format f)
    {
      formatFlag(ios) = f;
      return ios;
    }


    /*!
     */
    PetriNetIO::Format PetriNetIO::getFormat(ios_base & ios)
    {
      return static_cast<Format>(formatFlag(ios));
    }


    /*!
     */
    long & PetriNetIO::formatFlag(ios_base & ios)
    {
      static int n = ios_base::xalloc();
      return ios.iword(n);
    }


    /*!
     */
    template <typename T>
    T & PetriNetIO::setMode(T & ios, Mode m)
    {
      modeFlag(ios) = m;
      return ios;
    }


    /*!
     */
    PetriNetIO::Mode PetriNetIO::getMode() const
    {
      return mode_;
    }


    /*!
     */
    PetriNetIO::Mode PetriNetIO::getMode(ios_base & ios)
    {
      return static_cast<Mode>(modeFlag(ios));
    }


    /*!
     */
    long & PetriNetIO::modeFlag(ios_base & ios)
    {
      static int n = ios_base::xalloc();
      return ios.iword(n);
    }


    /*!
     * \brief   delimiters for set output
     */
    inline string getOWFNDelimiter(const set<Arc *> &)        { return ", "; }
    inline string getOWFNDelimiter(const set<Place *> &)      { return ", "; }
    inline string getOWFNDelimiter(const set<Transition *> &) { return "\n"  ; }

    /*!
     * \brief   ouputs a set of pointers to a stream
     */
    template <typename T>
    ostream & operator<<(ostream & os, const set<T> & s)
    {
      string delim;
      switch (PetriNetIO::getFormat(os))
	{
	case PetriNetIO::OWFN: delim = getOWFNDelimiter(s); break;
	case PetriNetIO::DOT:  delim = "\n"; break;
	default: delim = ", "; break;
	}
      if (s.empty()) return os;
      for (typename set<T>::iterator it = s.begin(); it != --s.end(); ++it)
	os << **it << delim;
      return os << **--s.end();
    }

  }

}

#endif

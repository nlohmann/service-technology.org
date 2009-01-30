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

using std::ios_base;
using std::istream;
using std::ostream;
using std::ostream_iterator;

namespace pnapi
{

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


    /// mode manipulator
    inline PetriNetIO setMode(PetriNetIO::Mode m) {
      return PetriNetIO(m); };

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

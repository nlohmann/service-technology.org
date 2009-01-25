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
      enum Format { STAT, OWFN };

      /// I/O (sub-)mode
      enum Mode { PLACE, PLACE_CAPACITY, PLACE_TOKEN };

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


    /// (sub-)mode setting
    ostream & operator<<(ostream &, const PetriNetIO &);

    /// pnapi::PetriNet output
    ostream & operator<<(ostream &, const pnapi::PetriNet &);

    /// pnapi::Node output
    ostream & operator<<(ostream &, const pnapi::Arc &);

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
    inline ostream & owfn(ostream & os) {
      return PetriNetIO::setFormat(os, PetriNetIO::OWFN); }



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
    inline string getDelimiter(const set<Arc *> &)        { return ", "; }
    inline string getDelimiter(const set<Place *> &)      { return ", "; }
    inline string getDelimiter(const set<Transition *> &) { return ""  ; }

    /*!
     * \brief   ouputs a set of pointers to a stream
     */
    template <typename T>
    ostream & operator<<(ostream & os, const set<T> & s)
    {
      const string delim = getDelimiter(s);
      if (s.empty()) return os;
      for (typename set<T>::iterator it = s.begin(); it != --s.end(); ++it)
	os << **it << delim;
      return os << **--s.end();
    }

  }

}

#endif

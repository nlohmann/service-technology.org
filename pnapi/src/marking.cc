#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include <set>

#include "petrinet.h"
#include "marking.h"

using std::set;
using std::map;

namespace pnapi
{


/****************************************************************************
 *** Class Marking Function Definitions
 ***************************************************************************/

/*!
 * \brief   Standard Constructor
 */
/*
  Marking::Marking() :
    net_(*new PetriNet())
  {
  }
 */


/*!
 * \brief   Constructor for marking class
 *
 * Reads marking from the Petri net.
 *
 * \param   n
 * \param   internalsOnly  specifies if interface places can
 *                         join the marking (if they can then false and if
 *                         not then false)
 * \param   empty          if true, initialize marking to empty marking
 *                         instead of reading marking from n
 */
Marking::Marking(PetriNet &n, bool empty) :
  net_(&n)
  {
  for (set<Place *>::const_iterator p = n.getPlaces().begin(); 
  p != n.getPlaces().end(); ++p)
    m_[*p] = empty ? 0 : (*p)->getTokenCount();
  }


/*!
 * \brief   Copy-constructor for marking class
 *
 * \param   Marking &mm
 */
Marking::Marking(const Marking &m) :
  m_(m.m_), net_(m.net_)
  {
  }


/*!
 * \brief   Another constructor.
 */
Marking::Marking(std::map<const Place *, unsigned int> m, PetriNet *net) :
  m_(m), net_(net)
  {
  }

/*!
 * \brief constructor for cross-net-copying
 */
Marking::Marking(const Marking & m, PetriNet* net, 
    std::map<const Place*, const Place*>& placeMap) :
      net_(net)
      {
      for(map<const Place*, unsigned int>::const_iterator it = m.m_.begin();
      it != m.m_.end(); ++it)
      {
        m_[placeMap[it->first]] = it->second;
      }
      }


    /*!
     * \brief   Returns the map..
     */
    const map<const Place *, unsigned int> & Marking::getMap() const
    {
      return m_;
    }


    /*!
     * \brief   Returns the Petri net laying under the marking
     */
    PetriNet & Marking::getPetriNet() const
    {
      return *net_;
    }

    map<const Place *, unsigned int>::const_iterator Marking::begin() const
    {
      return m_.begin();
    }

    map<const Place *, unsigned int>::const_iterator Marking::end() const
    {
      return m_.end();
    }

    /*!
     * \brief   Returns the size of the Marking
     */
    unsigned int Marking::size()
    {
      return m_.size();
    }


    /*!
     * \brief   Decides if this marking activates transition t
     *
     * \param   Transition &t
     *
     * \return  true iff this activates t
     */
    bool Marking::activates(const Transition &t)
    {
      for (set<Arc *>::const_iterator f = t.getPresetArcs().begin();
      f != t.getPresetArcs().end(); ++f)
      {
        if ((**f).getWeight() > m_[&(**f).getPlace()])
          return false;
      }


      return true;
    }


    /*!
     * \brief   Calculates the successor marking under transition t
     *
     * \param   Transition &t
     *
     * \return  Marking &m' which is the direct successor to this under t
     */
    Marking & Marking::successor(const Transition &t)
    {
      set<Node *> Ppre = t.getPreset();
      set<Node *> Ppost = t.getPostset();
      Marking &m = *new Marking(*this);

      for (set<Node *>::const_iterator p = Ppre.begin(); p != Ppre.end(); ++p)
        m[*static_cast<Place *>(*p)] -= net_->findArc(**p, t)->getWeight();

      for (set<Node *>::const_iterator p = Ppost.begin(); p != Ppost.end(); ++p)
        m[*static_cast<Place *>(*p)] += net_->findArc(t, **p)->getWeight();

      return m;
    }


    /*!
     * \brief   overloaded operator [] for Markings
     */
    unsigned int & Marking::operator [](const Place & offset)
    {
      return m_[&offset];
    }


    unsigned int Marking::operator[](const Place & p) const
    {
      if (m_.find(&p) == m_.end())
        return 0;
      //assert(m_.find(&p) != m_.end());

      return m_.find(&p)->second;
    }


    /*!
     * \brief   overloaded operator == for Markings
     */
    bool Marking::operator ==(const Marking &m) const
    {
      return m_ == m.getMap();
    }


    /*!
     * \brief   overloaded assignment operator
     */
    Marking & Marking::operator =(const Marking &m)
    {
      assert(this != &m);

      this->~Marking();
      return *new (this) Marking(m);
    }


    /*!
     * \brief   clears the marking
     */
    void Marking::clear()
    {
      m_.clear();
    }


}

#include <cassert>
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
   * \brief   Constructor for marking class
   *
   * Reads marking from the Petri net.
   *
   * \param   PetriNet &n
   * \param   bool internalsOnly specifies if interface places can
   *          join the marking (if they can then false and if not then false)
   */
  Marking::Marking(PetriNet &n, bool internalsOnly) :
    net_(n), internalsOnly_(internalsOnly)
  {
    for (set<Place *>::const_iterator p = n.getPlaces().begin(); p != n.getPlaces().end(); p++)
      if (!internalsOnly_ || (*p)->getType() == Place::INTERNAL)
        m_[*p] = (*p)->getTokenCount();
  }


  /*!
   * \brief   Copy-constructor for marking class
   *
   * \param   Marking &mm
   */
  Marking::Marking(const Marking &m) :
    m_(m.m_), net_(m.net_), internalsOnly_(m.internalsOnly_)
  {
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
    return net_;
  }


  /*!
   * \brief   Returns the value of the attribute internalsOnly_
   */
  bool Marking::internalsOnly() const
  {
    return internalsOnly_;
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
    for (set<Node *>::const_iterator p = t.getPreset().begin();
        p != t.getPreset().end(); p++)
      if (!internalsOnly_ || (*p)->getType() == Place::INTERNAL)
        if ((net_.findArc(**p, t)->getWeight()) >
            m_[static_cast<Place *> (*p)])
          return false;


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

    for (set<Node *>::const_iterator p = Ppre.begin(); p != Ppre.end(); p++)
      if (!internalsOnly_ || (*p)->getType() == Place::INTERNAL)
        m[*static_cast<Place *>(*p)] -= net_.findArc(**p, t)->getWeight();

    for (set<Node *>::const_iterator p = Ppost.begin(); p != Ppost.end(); p++)
      if (!internalsOnly_ || (*p)->getType() == Place::INTERNAL)
        m[*static_cast<Place *>(*p)] += net_.findArc(t, **p)->getWeight();

    return m;
  }


  /*!
   * \brief   overloaded operator [] for Markings
   */
  unsigned int & Marking::operator [](const Place & offset)
  {
    return m_.find(&offset)->second;
  }

  
  unsigned int Marking::operator[](const Place & p) const
  {
    assert(m_.find(&p) != m_.end());

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


}

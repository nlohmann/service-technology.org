/*!
 * \file  marking.cc
 */

#include "config.h"

#include "pnapi-assert.h"

#include "petrinet.h"
#include "marking.h"
#include "util.h"

#ifndef NDEBUG
#include <iostream>

using std::cout;
using std::endl;
#endif /* NDEBUG */

using std::set;
using std::map;


namespace pnapi
{

/*!
 * \brief the value "omega"
 */
const UnsignedOmegaInt UnsignedOmegaInt::OMEGA(true);

/*!
 * \brief standard constructor
 */
UnsignedOmegaInt::UnsignedOmegaInt() :
  isOmega_(false), value_(0)
{
}

/*!
 * \brief copy constructor
 */
UnsignedOmegaInt::UnsignedOmegaInt(const UnsignedOmegaInt & cc) :
  isOmega_(cc.isOmega_), value_(cc.value_)
{
}

/*!
 * \brief construction from unsigned int
 */
UnsignedOmegaInt::UnsignedOmegaInt(unsigned int i) :
  isOmega_(false), value_(i)
{
}

/*!
 * \brief omega contructor
 */
UnsignedOmegaInt::UnsignedOmegaInt(bool isOmega) :
  isOmega_(isOmega), value_(0)
{
}

/*!
 * \brief assignement operator
 */
UnsignedOmegaInt & UnsignedOmegaInt::operator=(const UnsignedOmegaInt & cc)
{
  isOmega_ = cc.isOmega_;
  value_ = cc.value_;
  return *this;
}

/*!
 * \brief assignement operator
 */
UnsignedOmegaInt & UnsignedOmegaInt::operator=(unsigned int i)
{
  isOmega_ = false;
  value_ = i;
  return *this;
}

/*!
 * \brief comparison operator
 */
bool UnsignedOmegaInt::operator==(const UnsignedOmegaInt & other) const
{
  return ((isOmega_ == other.isOmega_) && (value_ == other.value_));
}

/*!
 * \brief implicit cast to unsigned int
 * 
 * This cast allows application of all unsigned int arithmetics to this type.
 * 
 * \note If omega is to be casted, an exception will be thrown
 */
UnsignedOmegaInt::operator unsigned int() const
{
  PNAPI_ASSERT_USER((!isOmega_), "can not cast Omega to unsigned int", exception::UserCausedError::UE_OMEGA_CAST_ERROR);
  return value_;
}

/****************************************************************************
 *** Class Marking Function Definitions
 ***************************************************************************/

/// omega
const UnsignedOmegaInt & OmegaMarking::OMEGA = UnsignedOmegaInt::OMEGA;

/*!
 * \brief   Standard Constructor
 *
 * \note This constructor does not instantiate a Petri net object.
 */
Marking::Marking() :
  AbstractMarking<unsigned int>()
{
}


/*!
 * \brief   Constructor for marking class
 *
 * Reads marking from the Petri net.
 *
 * \param n underlying petri net      
 * \param empty  if true, initialize marking to empty marking
 *               instead of reading marking from n
 */
Marking::Marking(PetriNet & n, bool empty) :
  AbstractMarking<unsigned int>(n, empty)
{
}


/*!
 * \brief   Copy-constructor for marking class
 *
 * \param   m the other Marking
 */
Marking::Marking(const Marking & m) :
  AbstractMarking<unsigned int>(m)
{
}


/*!
 * \brief   Another constructor.
 */
Marking::Marking(const std::map<const Place *, unsigned int> & m, PetriNet * net) :
  AbstractMarking<unsigned int>(m, net)
{
}

/*!
 * \brief constructor for cross-net-copying
 */
Marking::Marking(const Marking & m, PetriNet * net, 
                 const std::map<const Place *, const Place *> & placeMap) :
  AbstractMarking<unsigned int>(m, net, placeMap)
{
}
  
/*!
 * \brief   overloaded assignment operator
 */
Marking & Marking::operator=(const Marking & m)
{
  *static_cast<AbstractMarking<unsigned int> *>(this) = m;
  return *this;
}


/*!
 * \brief   Decides if this marking activates transition t
 *
 * \param   t the Transition in question
 *
 * \return  true iff this activates t
 */
bool Marking::activates(const Transition & t) const
{
  PNAPI_FOREACH(f, t.getPresetArcs())
  {
    map<const Place *, unsigned int>::const_iterator it = m_.find(static_cast<Place *>(&(*f)->getSourceNode()));
    if((it == m_.end()) || ((**f).getWeight() > it->second))
    {
      return false;
    }
  }

  return true;
}


/*!
 * \brief   Calculates the successor marking under transition t
 *
 * \param   t Transition to be fired
 *
 * \return  direct successor of this marking under t
 * 
 * \note successor will be allocated on the heap;
 *       you have to delete it, when you don't need it anymore
 * 
 * \todo review this and Automaton::dfs() to avoid memory leaks
 */
Marking & Marking::getSuccessor(const Transition & t) const
{
  set<Node *> Ppre = t.getPreset();
  set<Node *> Ppost = t.getPostset();
  Marking & m = *new Marking(*this);

  PNAPI_FOREACH(p, Ppre)
  {
    m[*static_cast<Place *>(*p)] -= net_->findArc(**p, t)->getWeight();
  }

  PNAPI_FOREACH(p, Ppost)
  {
    m[*static_cast<Place *>(*p)] += net_->findArc(t, **p)->getWeight();
  }

  return m;
}


/*!
 * \brief   Standard Constructor
 *
 * \note This constructor does not instantiate a Petri net object.
 */
PlaceIndexVector::PlaceIndexVector() :
  AbstractMarking<int>()
{
}


/*!
 * \brief   Constructor for PlaceIndexVector class
 *
 * Reads marking from the Petri net.
 *
 * \param n underlying petri net      
 * \param empty  if true, initialize PlaceIndexVector to empty PlaceIndexVector
 *               instead of reading marking from n
 */
PlaceIndexVector::PlaceIndexVector(PetriNet & n, bool empty) :
  AbstractMarking<int>(n, empty)
{
}


/*!
 * \brief   Copy-constructor for PlaceIndexVector class
 *
 * \param   m the other PlaceIndexVector
 */
PlaceIndexVector::PlaceIndexVector(const PlaceIndexVector & m) :
  AbstractMarking<int>(m)
{
}


/*!
 * \brief   Another constructor.
 */
PlaceIndexVector::PlaceIndexVector(const std::map<const Place *, int> & m, PetriNet * net) :
  AbstractMarking<int>(m, net)
{
}

/*!
 * \brief constructor for cross-net-copying
 */
PlaceIndexVector::PlaceIndexVector(const PlaceIndexVector & m, PetriNet * net, 
                                   const std::map<const Place *, const Place *> & placeMap) :
  AbstractMarking<int>(m, net, placeMap)
{
}
  
/*!
 * \brief   overloaded assignment operator
 */
  PlaceIndexVector & PlaceIndexVector::operator=(const PlaceIndexVector & m)
{
  *static_cast<AbstractMarking<int> *>(this) = m;
  return *this;
}


/*!
 * \brief   Standard Constructor
 *
 * \note This constructor does not instantiate a Petri net object.
 */
OmegaMarking::OmegaMarking() :
  AbstractMarking<UnsignedOmegaInt>()
{
}


/*!
 * \brief   Constructor for Omega marking class
 *
 * Reads marking from the Petri net.
 *
 * \param n underlying petri net      
 * \param empty  if true, initialize Omega marking to empty marking
 *               instead of reading marking from n
 */
OmegaMarking::OmegaMarking(PetriNet & n, bool empty) :
  AbstractMarking<UnsignedOmegaInt>(n, empty)
{
}


/*!
 * \brief   Copy-constructor for Omega marking class
 *
 * \param   m the other OmegaMarking
 */
OmegaMarking::OmegaMarking(const OmegaMarking & m) :
  AbstractMarking<UnsignedOmegaInt>(m)
{
}


/*!
 * \brief   Another constructor.
 */
OmegaMarking::OmegaMarking(const std::map<const Place *, UnsignedOmegaInt> & m, PetriNet * net) :
  AbstractMarking<UnsignedOmegaInt>(m, net)
{
}

/*!
 * \brief constructor for cross-net-copying
 */
OmegaMarking::OmegaMarking(const OmegaMarking & m, PetriNet * net, 
                           const std::map<const Place *, const Place *> & placeMap) :
  AbstractMarking<UnsignedOmegaInt>(m, net, placeMap)
{
}
  
/*!
 * \brief   overloaded assignment operator
 */
OmegaMarking & OmegaMarking::operator=(const OmegaMarking & m)
{
  *static_cast<AbstractMarking<UnsignedOmegaInt> *>(this) = m;
  return *this;
}

} /* namespace pnapi */

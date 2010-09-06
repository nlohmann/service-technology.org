// -*- C++ -*-

/*!
 * \file  marking.h
 */

#ifndef PNAPI_MARKING_H
#define PNAPI_MARKING_H

#include "config.h"

#include "exception.h"
#include "petrinet.h"

#include <map>

namespace pnapi
{

/*!
 * \brief   Union of unsigned int an "Omega"
 */
class UnsignedOmegaInt
{
public: /* public static constants */
  /// the value "omega"
  static const UnsignedOmegaInt OMEGA;
  
private: /* private variables */
  /// whether this is omega
  bool isOmega_;
  /// the unsigned int value
  unsigned int value_;
  
public: /* public methods */
  /// standard constructor
  UnsignedOmegaInt();
  /// copy constructor
  UnsignedOmegaInt(const UnsignedOmegaInt &);
  /// construction from unsigned int
  UnsignedOmegaInt(unsigned int);
  /// assignement operator
  UnsignedOmegaInt & operator=(const UnsignedOmegaInt &);
  /// assignement operator
  UnsignedOmegaInt & operator=(unsigned int);
  /// comparison operator
  bool operator==(const UnsignedOmegaInt &) const;
  /// implicit cast to unsigned int
  operator unsigned int() const;
  
private: /* private methods */
  /// omega contructor
  UnsignedOmegaInt(bool);
};


template <typename T>
class AbstractMarking
{
private:
  /// wether to ignore negative subtraction results
  static bool ignoreNegatives_;
  
protected: /* protected variables */
  /// the actual marking
  std::map<const Place *, T> m_;
  /// the net this marking belongs to
  PetriNet * net_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  AbstractMarking();
  /// constructor
  AbstractMarking(PetriNet &, bool);
  /// copy constructor
  AbstractMarking(const AbstractMarking<T> &);
  /// constructor
  AbstractMarking(const std::map<const Place *, T> &, PetriNet * = NULL);
  /// copy constructor
  AbstractMarking(const AbstractMarking<T> &, PetriNet *, const std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~AbstractMarking() {};
  /// assignment operator
  AbstractMarking & operator=(const AbstractMarking<T> &);
  //@}
  
  /*!
   * \name comparison and arithmetic operators
   */
  //@{
  /// whether this marking is equal to a given other marking
  bool operator==(const AbstractMarking<T> &) const;
  /// whether this marking is equal to a given other marking
  bool operator!=(const AbstractMarking<T> &) const;
  /// whether this marking is smaller than the given other marking
  bool operator<(const AbstractMarking<T> &) const;
  /// whether this marking is larger than the given other marking
  bool operator>(const AbstractMarking<T> &) const;
  /// whether this marking is smaller than or equal to the given other marking
  bool operator<=(const AbstractMarking<T> &) const;
  /// whether this marking is larger than or equal to the given other marking
  bool operator>=(const AbstractMarking<T> &) const;
  
  /// adds the given marking and this one
  AbstractMarking<T> operator+(const AbstractMarking<T> &) const;
  /// subtracts this marking and the given one
  AbstractMarking<T> operator-(const AbstractMarking<T> &) const;
  /// adds the given marking to this one
  AbstractMarking<T> & operator+=(const AbstractMarking<T> &);
  /// subtracts the given marking from this one
  AbstractMarking<T> & operator-=(const AbstractMarking<T> &);
  //@}
  
  /*!
   * \name structural changes
   */
  //@{
  /// wether to ignore negative subtraction results
  static void ignoreNegativeSubtractionResults(bool);
  /// clear this marking
  void clear();
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// get mapping from places to their marking
  const std::map<const Place *, T> & getMap() const;
  /// get iterator to the first element in the marking map
  typename std::map<const Place *, T>::const_iterator begin() const;
  /// get end-iterator of the marking map
  typename std::map<const Place *, T>::const_iterator end() const;
  /// get size of the marking map
  unsigned int size() const;
  /// get unterlying petri net
  PetriNet & getPetriNet() const;
  /// get writing access to the marking of a given place
  T & operator[](const Place &);
  /// get the marking of a given place
  T operator[](const Place &) const;
  //@}
};


/*!
 * \brief   Marking of all places of a net
 *
 * \note    In the future this class might contain a highly efficient
 *          implementation if necessary. For now, we use a simple one to
 *          determine the needed functionality.
 */
class Marking : public AbstractMarking<unsigned int>
{
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Marking();
  /// constructor
  Marking(PetriNet &, bool = false);
  /// copy constructor
  Marking(const Marking &);
  /// constructor
  Marking(const std::map<const Place *, unsigned int> &, PetriNet * = NULL);
  /// constructor
  Marking(const Marking &, PetriNet *, const std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~Marking() {};
  /// assignment operator
  Marking & operator=(const Marking &);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// whether marking activates a given transition
  bool activates(const Transition &) const;
  /// get the successor by fireing a given transition
  Marking & getSuccessor(const Transition &) const;
  //@}
};

/*!
 * \brief place index vector
 */
class PlaceIndexVector : public AbstractMarking<int>
{
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  PlaceIndexVector();
  /// constructor
  PlaceIndexVector(PetriNet &, bool = false);
  /// copy constructor
  PlaceIndexVector(const PlaceIndexVector &);
  /// constructor
  PlaceIndexVector(const std::map<const Place *, int> &, PetriNet * = NULL);
  /// constructor
  PlaceIndexVector(const PlaceIndexVector &, PetriNet *, const std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~PlaceIndexVector() {};
  /// assignment operator
  PlaceIndexVector & operator=(const PlaceIndexVector &);
  //@}
};

/*!
 * \brief markings with omega
 */
class OmegaMarking : public AbstractMarking<UnsignedOmegaInt>
{
public: /* public static constants */
  /// omega
  static const UnsignedOmegaInt & OMEGA; 
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  OmegaMarking();
  /// constructor
  OmegaMarking(PetriNet &, bool = false);
  /// copy constructor
  OmegaMarking(const OmegaMarking &);
  /// constructor
  OmegaMarking(const std::map<const Place *, UnsignedOmegaInt> &, PetriNet * = NULL);
  /// constructor
  OmegaMarking(const OmegaMarking &, PetriNet *, const std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~OmegaMarking() {};
  /// assignment operator
  OmegaMarking & operator=(const OmegaMarking &);
  //@}
};



/****************************************************************************
 *** Template Implementation
 ***************************************************************************/

/// wether to ignore negative subtraction results
template <typename T> bool AbstractMarking<T>::ignoreNegatives_ = false;

/*!
 * \brief constructor
 * 
 * \note This constructor does not instantiate a Petri net object.
 */
template <typename T> AbstractMarking<T>::AbstractMarking() :
  net_(NULL)
{
}

/*!
 * \brief constructor
 */
template <typename T> AbstractMarking<T>::AbstractMarking(PetriNet & n, bool empty) :
  net_(&n)
{
  PNAPI_FOREACH(p, n.getPlaces())
  {
    m_[*p] = empty ? T((unsigned int) 0) : T((*p)->getTokenCount());
  }
}

/*!
 * \brief copy constructor
 */
template <typename T> AbstractMarking<T>::AbstractMarking(const AbstractMarking<T> & m) :
  m_(m.m_), net_(m.net_)
{
}

/*!
 * \brief constructor
 */
template <typename T> AbstractMarking<T>::AbstractMarking(const std::map<const Place *, T> & m, PetriNet * net) :
  m_(m), net_(net)
{
}

/*!
 * \brief copy constructor
 */
template <typename T> AbstractMarking<T>::AbstractMarking(const AbstractMarking & m, PetriNet * net, const std::map<const Place *, const Place *> & placeMap) :
  net_(net)
{
  PNAPI_FOREACH(it, m.m_)
  {
    typename std::map<const Place *, const Place *>::const_iterator p = placeMap.find(it->first);
    
    // must not use assertion macros in header files, hence this
    if(p == placeMap.end())
    {
      throw exception::UserCausedError(exception::UserCausedError::UE_MARKING_ERROR,
                                       (std::string("did not find place '") + it->first->getName() + "' in place mapping"));
    }
    
    m_[p->second] = it->second;
  }
}

/*!
 * \brief assignment operator
 */
template <typename T> AbstractMarking<T> & AbstractMarking<T>::operator=(const AbstractMarking<T> & m)
{
  net_ = m.net_;
  m_ = m.m_;
  return *this;
}

/*!
 * \brief wether to ignore negative subtraction results
 */
template <typename T> void AbstractMarking<T>::ignoreNegativeSubtractionResults(bool ignore)
{
  ignoreNegatives_ = ignore;
}

/*!
 * \brief clear this marking
 */
template <typename T> void AbstractMarking<T>::clear()
{
  m_.clear();
}

/*!
 * \brief get mapping from places to their marking
 */
template <typename T> const std::map<const Place *, T> & AbstractMarking<T>::getMap() const
{
  return m_;
}

/*!
 * \brief get iterator to the first element in the marking map
 */
template <typename T> typename std::map<const Place *, T>::const_iterator AbstractMarking<T>::begin() const
{
  return m_.begin();
}

/*!
 * \brief get end-iterator of the marking map
 */
template <typename T> typename std::map<const Place *, T>::const_iterator AbstractMarking<T>::end() const
{
  return m_.end();
}

/*!
 * \brief get size of the marking map
 */
template <typename T> unsigned int AbstractMarking<T>::size() const
{
  return m_.size();
}

/*!
 * \brief get unterlying petri net
 */
template <typename T> PetriNet & AbstractMarking<T>::getPetriNet() const
{
  // must not use assertion macros in header files, hence this
  if(net_ == NULL)
  {
    throw exception::AssertionFailedError( __FILE__ , __LINE__ , "net_ != NULL" );
  }
  
  return *net_;
}

/*!
 * \brief get writing access to the marking of a given place
 */
template <typename T> T & AbstractMarking<T>::operator[](const Place & p)
{
  return m_[&p];
}

/*!
 * \brief get the marking of a given place
 */
template <typename T> T AbstractMarking<T>::operator[](const Place & p) const
{
  typename std::map<const Place *, T>::const_iterator it = m_.find(&p); 

  return ((it == m_.end()) ? T(0) : it->second);
}



/*!
 * \brief whether this marking is equal to a given other marking
 */
template <typename T> bool AbstractMarking<T>::operator==(const AbstractMarking<T> & m) const
{
  return (m_ == m.m_);
}

/*!
 * \brief whether this marking is equal to a given other marking
 */
template <typename T> bool AbstractMarking<T>::operator!=(const AbstractMarking<T> & m) const
{
  return (m_ != m.m_);
}

/*!
 * \brief whether this marking is smaller than the given other marking
 */
template <typename T> bool AbstractMarking<T>::operator<(const AbstractMarking<T> & m) const
{
  return (m_ < m.m_);
}

/*!
 * \brief whether this marking is larger than the given other marking
 */
template <typename T> bool AbstractMarking<T>::operator>(const AbstractMarking<T> & m) const
{
  return (m_ > m.m_);
}

/*!
 * \brief whether this marking is smaller than or equal to the given other marking
 */
template <typename T> bool AbstractMarking<T>::operator<=(const AbstractMarking<T> & m) const
{
  return (m_ <= m.m_);
}

/*!
 * \brief whether this marking is larger than or equal to the given other marking
 */
template <typename T> bool AbstractMarking<T>::operator>=(const AbstractMarking<T> & m) const
{
  return (m_ >= m.m_);
}

/*!
 * \brief adds the given marking and this one
 */
template <typename T> AbstractMarking<T> AbstractMarking<T>::operator+(const AbstractMarking<T> & m) const
{
  AbstractMarking<T> result(*this);
  result += m;
  return result;
}

/*!
 * \brief subtracts this marking and the given one
 */
template <typename T> AbstractMarking<T> AbstractMarking<T>::operator-(const AbstractMarking<T> & m) const
{
  bool negativeFound = false;
  AbstractMarking<T> result(*this);
  
  PNAPI_FOREACH(p, m.m_)
  {
    negativeFound |= ((result.m_[p->first] -= p->second) < 0);
  }
  
  if((!ignoreNegatives_) && negativeFound)
  {
    throw exception::MarkingArithmeticError<AbstractMarking<T> >(result);
  }
  
  return result;
}

/*!
 * \brief adds the given marking to this one
 */
template <typename T> AbstractMarking<T> & AbstractMarking<T>::operator+=(const AbstractMarking<T> & m)
{
  PNAPI_FOREACH(p, m.m_)
  {
    m_[p->first] += p->second;
  }
  return *this;
}

/*!
 * \brief subtracts the given marking from this one
 */
template <typename T> AbstractMarking<T> & AbstractMarking<T>::operator-=(const AbstractMarking<T> & m)
{
  (*this) = (*this) - m;
  return *this;
}

} /* namespace pnapi */

#endif /* PNAPI_MARKING_H */

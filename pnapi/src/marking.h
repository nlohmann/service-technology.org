// -*- C++ -*-

/*!
 * \file  marking.h
 */

#ifndef PNAPI_MARKING_H
#define PNAPI_MARKING_H

#include <map>

namespace pnapi
{

// forward declarations
class PetriNet;
class Transition;
class Place;


/*!
 * \brief   Marking of all places of a net
 *
 * \note    In the future this class might contain a highly efficient
 *          implementation if necessary. For now, we use a simple one to
 *          determine the needed functionality.
 */
class Marking
{
private: /* private variables */
  std::map<const Place *, unsigned int> m_;
  PetriNet * net_;
  
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
  Marking(std::map<const Place *, unsigned int>, PetriNet * = NULL);
  /// constructor
  Marking(const Marking &, PetriNet *, std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~Marking() {};
  /// assignment operator
  Marking & operator=(const Marking &);
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// clear this marking
  void clear();
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// get mapping from places to their marking
  const std::map<const Place *, unsigned int> & getMap() const;
  /// get iterator to the first element in the marking map
  std::map<const Place *, unsigned int>::const_iterator begin() const;
  /// get end-iterator of the marking map
  std::map<const Place *, unsigned int>::const_iterator end() const;
  /// get size of the marking map
  unsigned int size() const;
  /// get unterlying petri net
  PetriNet & getPetriNet() const;
  /// whether this marking is equal to a given other marking
  bool operator==(const Marking &) const;
  /// whether marking activates a given transition
  bool activates(const Transition &) const;
  /// get the successor by fireing a given transition
  Marking & getSuccessor(const Transition &) const;
  /// get writing access to the marking of a given place
  unsigned int & operator[](const Place &);
  /// get the marking of a given place
  unsigned int operator[](const Place &) const;
  //@}
};

} /* namespace pnapi */

#endif /* PNAPI_MARKING_H */

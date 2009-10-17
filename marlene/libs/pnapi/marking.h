// -*- C++ -*-

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
public:
  //Marking();
  Marking(PetriNet &n, bool = false);
  Marking(const Marking &m);
  Marking(std::map<const Place *, unsigned int> m, PetriNet *net = NULL);
  Marking(const Marking &, PetriNet*, std::map<const Place*, const Place*>&);
  virtual ~Marking() {}

  const std::map<const Place *, unsigned int> & getMap() const;
  PetriNet & getPetriNet() const;

  std::map<const Place *, unsigned int>::const_iterator begin() const;
  std::map<const Place *, unsigned int>::const_iterator end() const;
  unsigned int size();

  bool activates(const Transition &t);
  Marking & successor(const Transition &t);

  unsigned int & operator[](const Place &);
  unsigned int operator[](const Place &) const;
  bool operator==(const Marking &m) const;
  Marking & operator=(const Marking &m);

  void clear();

private:
  std::map<const Place *, unsigned int> m_;
  PetriNet *net_;
};

}

#endif

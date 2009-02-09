// -*- C++ -*-

#ifndef PNAPI_AUTOMATON_H
#define PNAPI_AUTOMATON_H

#include <list>

#include "marking.h"
#include "state.h"

namespace pnapi
{

  // forward declarations
  class State;
  class Place;
  class PetriNet;
  class Transition;
  class Marking;

enum Format { SA, STG };

class Automaton
{
public:

  /// constructor with Petri net - initial marking taken from place marks
  Automaton(PetriNet &n);

  /// standard destructor
  virtual ~Automaton();

  /// initializes the service automaton
  void initialize();

  /// creates the service automaton
  void createAutomaton();

  /// friend operation <<
  friend std::ostream &operator <<(std::ostream &os, const Automaton &sa);

  void setFormat(Format f);

private:

  static const int HASHSIZE = 65535;

  std::vector<std::set<State *> > hashTable_;
  std::map<Place *, unsigned int> primes_;

  std::list<string> in_;
  std::list<string> out_;
  unsigned int first_;
  std::list<unsigned int> finals_;

  map<Transition *, string> reasons_;

  PetriNet &net_;
  Marking initialmarking_;

  /// temporary output flag
  Format f_;

  void initHashTable();

  void fillPrimes();
  bool isPrime(unsigned int &p) const;

  void dfs(State &i);

  /// temporary output methods
  void output_sa(std::ostream &os) const;
  void output_stg(std::ostream &os) const;
};

/*** overloaded operator << ***/

std::ostream &operator <<(std::ostream &os, const Automaton &sa);

}

#endif /* SERVICEAUTOMATON_H */

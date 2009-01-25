#ifndef SERVICEAUTOMATON_H
#define SERVICEAUTOMATON_H

#include <list>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include "state.h"
#include "marking.h"

using std::list;
using std::ostream;
using std::set;
using std::string;
using std::vector;
using std::map;

namespace pnapi
{

#ifndef HASHSIZE
#define HASHSIZE 65535
#endif

class PetriNet;
class Transition;

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
  friend ostream &operator <<(ostream &os, const Automaton &sa);

  void setFormat(Format f);

private:
  vector<set<State *> > hashTable_;
  map<Place *, unsigned int> primes_;

  list<string> in_;
  list<string> out_;
  unsigned int first_;
  list<unsigned int> finals_;

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
  void output_sa(ostream &os) const;
  void output_stg(ostream &os) const;
};

/*** overloaded operator << ***/

ostream &operator <<(ostream &os, const Automaton &sa);

}

#endif /* SERVICEAUTOMATON_H */

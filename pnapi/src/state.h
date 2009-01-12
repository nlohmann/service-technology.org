#ifndef STATE_H
#define STATE_H

#include <list>
#include <string>
#include "petrinet.h"

using std::list;
using std::string;

namespace pnapi
{

class State
{
public:
  //State();
  State(Marking &mm);
  virtual ~State();

  /// sets the marking represented by this state
  void setMarking(Marking &mm);

  /// returns the marking represented by this state
  Marking &getMarking() const;

  /// sets the dfs index by maxIndex
  void setIndex();

  /// returns the dfs index
  unsigned int getIndex() const;

  /// returns the marking's size
  unsigned int size() const;

  /// adds a state to the list of successors
  void addSuccessor(State &s);

  /// returns the list of successors
  list<State *> getSuccessors() const;

  /// adds a reason for getting to a successor
  void addReason(const string &r);

  /// returns the list of reasons
  list<string> getReason() const;

  void setHashValue(unsigned int hv);
  unsigned int getHashValue() const;
  bool isHashSet() const;

  bool operator ==(const State &m) const;

private:
  Marking &m;
  unsigned int index; // dfs index
  unsigned int *hashValue;
  list<State *> successors;
  list<string> reason;

  static unsigned int maxIndex;
};

}

#endif /* STATE_H */

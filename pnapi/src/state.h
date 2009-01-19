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
  /// Constructors & Destructor
  State(Marking &m);
  State(const State &s);
  virtual ~State();

  /// sets the marking represented by this state
  void setMarking(Marking &m);

  /// returns the marking represented by this state
  Marking & getMarking() const;

  /// sets the dfs index by maxIndex
  void setIndex();

  /// returns the dfs index
  unsigned int getIndex() const;

  /// returns the marking's size
  unsigned int size() const;

  /// adds a state to the list of successors
  void addSuccessor(State &s);

  /// returns the list of successors
  const list<State *> & getSuccessors() const;

  /// adds a reason for getting to a successor
  void addReason(const string &r);

  /// returns the list of reasons
  const list<string> & getReason() const;

  unsigned int getHashValue(map<Place *, unsigned int> &pt);

  bool operator ==(const State &m) const;

private:
  /// the representing marking
  Marking &m_;

  /// dfs index
  unsigned int index_;

  /// pointer to the hash value
  unsigned int *hashValue_;

  /// list of all successors to this state
  list<State *> successors_;

  /// list of reasons leading to the successors
  list<string> reasons_;

  /// current dfs index
  static unsigned int maxIndex_;
};

}

#endif /* STATE_H */

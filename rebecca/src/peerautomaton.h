#ifndef PEERAUTOMATON_H
#define PEERAUTOMATON_H

#include "peer.h"
#include "types.h"
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>


class PeerAutomaton
{
  friend std::ostream & operator <<(std::ostream &, const PeerAutomaton &);

private:
  // set of states
  std::set<int> * states_;
  std::set<std::set<int> > * stateSets_;
  // initial state
  int * initialState_;
  std::set<int> * initialStateSet_;
  // set of final states
  std::set<int> * finalStates_;
  std::set<std::set<int> > * finalStateSets_;
  // transition relation
  std::set<Edge *> * edges_;
  std::set<PEdge *> * pEdges_;
  // peers
  std::vector<const Peer *> collaboration_;
  // event messages
  std::vector<std::string> events_;

  std::vector<std::vector<int> > * equivalenceClasses_;

  PeerAutomatonType type_;

public:
  PeerAutomaton(PeerAutomatonType = CHOREOGRAPHY);
  ~PeerAutomaton();

  int createState();
  Edge * createTransition(int, std::string, int, EventType);
  PEdge * createTransition(std::set<int>, std::string, std::set<int>, EventType);

  void deleteState(int);
  void deleteTransition(Edge *);

  /// setter
  void pushState(int);
  void pushState(std::set<int>);
  void setInitialState(int);
  void setInitialState(std::set<int>);
  void pushFinalState(int);
  void pushFinalState(std::set<int>);
  void pushPeer(const Peer *);
  void pushEvent(const std::string &);

  /// getter
  std::set<int> & states() const;
  std::set<std::set<int> > & stateSets() const;
  const std::set<Edge *> edges() const;
  const std::set<Edge *> edgesFrom(int) const;
  const std::set<Edge *> edgesTo(int) const;
  const std::set<Edge *> edges(std::set<int>) const;
  const std::set<PEdge *> pEdges(std::set<int>) const;
  const std::vector<std::string> & events() const;
  const std::vector<const Peer *> & collaboration() const;
  int initialState() const;

  const std::set<int> findStateSet(int) const;
  const int findState(int, int) const;
  const int findState(int, const std::string &) const;
  const int findState(int, int, int) const;

  // checks sanity of collaboration
  bool isSane() const;

  bool isState(int) const;

  bool isFinal(int) const;
  bool isFinal(std::set<int>) const;

  bool ** distantMessages() const;

  std::set<int> & closure(std::set<int> &, int);

  bool enables(int, int, int) const;
  bool disables(int, int, int) const;
  bool equivalent(int, int) const;

  bool haveInput() const;
  bool haveOutput() const;
  bool haveSynchronous() const;

  void unite(int, int);

private:
  bool distant(std::string, std::string) const;

};

#endif /* PEERAUTOMATON_H */

#ifndef CHOREOGRAPHY_H
#define CHOREOGRAPHY_H

#include "peer.h"
#include "types.h"

#include <set>
#include <string>
#include <vector>


class Choreography
{
  friend std::ostream & operator <<(std::ostream &, const Choreography &);

private:
  std::set<int> states_;
  int initialState_;
  std::set<int> finalStates_;
  std::set<Edge *> edges_;

  std::vector<Peer *> collaboration_;
  std::vector<std::string> events_;

  std::vector<std::vector<int> > equivalenceClasses_;

public:
  /*
   * Constructors and Desctructor
   */
  Choreography();
  ~Choreography();

  /*
   * Setter and Adder (push) and Deleter
   */
  int createState();
  void pushState(int);
  void deleteState(int);

  void setInitialState(int);
  void pushFinalState(int);

  Edge * createEdge(int, const std::string &, int, EventType);
  void deleteEdge(Edge *);

  void pushPeer(Peer *);
  void pushEvent(const std::string &);

  /*
   * Getter
   */
  const std::set<int> & states() const;
  int initialState() const;

  int findState(int, int) const;
  int findState(int, const std::string &) const;
  int findState(int, int, int) const;

  const std::set<Edge *> & edges() const;
  const std::set<Edge *> edgesFrom(int) const;
  const std::set<Edge *> edgesFrom(std::set<int>) const;
  const std::set<Edge *> edgesTo(int) const;

  const std::vector<Peer *> & collaboration() const;
  const std::vector<std::string> & events() const;

  bool isFinal(int) const;
  bool isFinal(const std::set<int> &) const;

  /*
   * Choreography check
   */
  bool isChoreography() const;
  bool resolveDeadlocks();

  /*
   * Distance
   */
  bool ** distantEvents() const;
  bool distant(const std::string &, const std::string &) const;

  /*
   * Main Algorithm
   */
  bool enables(int, int, int) const;
  bool disables(int, int, int) const;
  bool equivalent(int, int) const;

  std::set<int> & closure(std::set<int> &, int) const;

  void unite(int, int);

private:

  bool bfs(int q, std::vector<int> &toBeDelted);

  bool defined(int q);

};

#endif /* CHOREOGRAPHY_H */

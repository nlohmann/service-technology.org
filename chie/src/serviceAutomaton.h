#ifndef SERVICEAUTOMATON_H_
#define SERVICEAUTOMATON_H_

#include <map>
#include <set>
#include <string>
#include <vector>

/*
 * \brief Service Automaton class
 */
class ServiceAutomaton
{
  friend class ProductAutomaton;
  friend class ProductAutomatonState;
  friend class Knowledge;

private: /* private static members */
  /// "mapping" from event id to its label
  static std::vector<std::string> labels;

  /// mapping from label to its ID
  static std::map<std::string, unsigned int> labelIDs;

  /// ID of first specification receive label
  static unsigned int firstReceiveID;
  /// ID of last specification receive label
  static unsigned int lastReceiveID;
  /// ID of first specification send label
  static unsigned int firstSendID;
  /// ID of last specification send label
  static unsigned int lastSendID;

private: /* private variables */
  // initial state
  unsigned int initialState;

  // set of final states
  std::set<unsigned int> finalStates;

  // set of states without successor
  std::set<unsigned int> noSuccessorStates;

  // mapping from states s to mapping from labels l to set of states reachable from s by transition l
  std::map<unsigned int, std::map<unsigned int, std::vector<unsigned int> > > stateSpace;

public: /* public methods */
  // constructor
  ServiceAutomaton(std::vector<std::string> &, std::vector<std::string> &, bool = false);

  // mark the initial state
  void setInitialState(unsigned int);

  // mark a final state
  void setFinalState(unsigned int);

  // mark a state without successor
  void setNoSuccessorState(unsigned int);

  // add a successor
  void addSuccessor(unsigned int, std::string &, unsigned int);
};


#endif /* SERVICEAUTOMATON_H_ */

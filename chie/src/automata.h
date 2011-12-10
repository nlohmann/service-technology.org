#ifndef AUTOMATA_H_
#define AUTOMATA_H_

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

/*
 * \brief Service Automaton struct
 */
struct ServiceAutomaton
{
  // mapping from communication events to event type (true: sending; false: receiving)
  std::map<std::string, bool> isSendingEvent;

  // set of final states
  std::set<unsigned int> finalStates;

  // set of states without successor
  std::set<unsigned int> noSuccessorStates;

  // mapping from states s to mapping from labels l to set of states reachable from s by transition l
  std::map<unsigned int, std::map<std::string, std::set<unsigned int> > > stateSpace;

  // initial state
  unsigned int initialState;
};

/*!
 * \brief type of a transition (for the product automaton)
 */
enum TransitionType
{
  // some service automaton made an internal step
  INTERNAL,
  // some service sent a message
  SENDING,
  // some service received a message
  RECEIVING
};


// state of communication channel (i.e. multiset of messages pending)
typedef std::multiset<unsigned int> ChannelState;

// internal state of product automaton (i.e. pair of states of service automata)
typedef std::pair<unsigned int, unsigned int> InternalState;

// transition of product automaton (pair of transition type and resulting state)
typedef std::pair<TransitionType, unsigned int> Transition;


/*!
 * \brief State struct for the product automaton
 */
struct ProductState
{
  // internal state of the automaton (i.e. pair of states of both service automata)
  InternalState internalState;

  // ID of the channel state
  unsigned int interfaceStateID;

  // DFS number
  unsigned int dfs;

  // lowlink of Tarjan's algorithm and ID of own SCC
  unsigned int lowlink;

  // constructor actual ment to be used
  ProductState(InternalState &, unsigned int);

  // these need to be implemented
  ProductState(); // standard constructor
  ProductState(const ProductState &); // copy constructor
};


/*!
 * \brief Product Automaton struct
 */
struct ProductAutomaton
{
  // list of communication events
  std::vector<std::string> events;

  // mapping from labels to their IDs
  std::map<std::string, unsigned int> events2IDs;

  // mapping to channel states from their IDs
  std::map<unsigned int, ChannelState> channelStates;

  // mapping from channel states to their IDs
  std::map<ChannelState, unsigned int> channelStateCache;

  // mapping from state IDs to explicit state (internal state and channel state ID)
  std::map<unsigned int, ProductState > states;

  // mapping from states to their IDs
  std::map<InternalState, std::map<unsigned int, unsigned int> > stateCache;

  // mapping from states to successors
  std::map<unsigned int, std::map<std::string, std::set<Transition> > > transitions;

  // set of states without successor
  std::set<unsigned int> noSuccessorStates;

  // set of states without successors for the service
  std::set<unsigned int> noSpecificationSuccessorStates;

  // set of states without successors for the test case
  std::set<unsigned int> noTestCaseSuccessorStates;

  // predecessor
  std::map<unsigned int, std::set<unsigned int> > predecessors;

  // strongly connected components
  std::map<unsigned int, std::set<unsigned int> > SCCs;

  // next channel state ID
  unsigned int nextChannelStateID; // = 1

  // next state ID
  unsigned int nextStateID; // = 2

  // node the problem occurs
  unsigned int badNode;
};


// create the product automaton of two service automata
bool createProductAutomaton(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);


#endif /* AUTOMATA_H_ */

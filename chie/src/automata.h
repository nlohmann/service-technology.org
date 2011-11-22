#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include <map>
#include <set>
#include <string>
#include <utility>

struct ServiceAutomaton
{
  std::map<std::string, bool> isSendingEvent;
  std::set<unsigned int> finalStates;
  std::set<unsigned int> noSuccessorStates;
  std::map<unsigned int, std::map<std::string, std::set<unsigned int> > > stateSpace;
  unsigned int initialState;
  std::set<std::string> getSendingEvents();
};

enum TransitionType
{
  INTERNAL,
  SENDING,
  RECEIVING
};

typedef std::multiset<std::string> ChannelState;
typedef std::pair<unsigned int, unsigned int> InternalState;
typedef std::pair<TransitionType, unsigned int> Transition;
//typedef std::pair<InternalState, unsigned int> ProductState;
struct ProductState
{
  InternalState internalState;
  unsigned int interfaceStateID;
  unsigned int dfs;
  unsigned int lowlink;
  ProductState(InternalState &, unsigned int); // constructor actual ment to be used

  // these need to be implemented
  ProductState(); // standard constructor
  ProductState(const ProductState &); // copy constructor
};


struct ProductAutomaton
{
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
  std::set<unsigned int> noServiceSuccessorStates;
  // set of states without successors for the test case
  std::set<unsigned int> noTestCaseSuccessorStates;
  // predecessor
  std::map<unsigned int, std::set<unsigned int> > predecessors;
  // strongly connected components
  std::map<unsigned int, std::set<unsigned int> > SCCs;
};



void createProductAutomaton(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);


#endif /* DATASTRUCTURES_H_ */

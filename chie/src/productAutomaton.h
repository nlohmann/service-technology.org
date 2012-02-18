#ifndef PRODUCTAUTOMATON_H_
#define PRODUCTAUTOMATON_H_

#include "serviceAutomaton.h"
#include "util.h"

/// channel state type
typedef std::vector<unsigned int> ChannelState;

/// hash function for channel states
unsigned int channelStateHash(ChannelState &);

// forward declaration
struct ProductAutomatonTransition;

/*!
 * \brief Product automaton state class
 */
class ProductAutomatonState
{
  friend class ProductAutomaton;
  friend class Knowledge;

private: /* private static members */
  /// hash map of all channel states
  static HashMap<ChannelState, channelStateHash> channelStates_;

  /// pointer to specification automaton
  static ServiceAutomaton * specificationAutomaton_;

  /// pointer to test case automaton
  static ServiceAutomaton * testCaseAutomaton_;

  /// pointer to initial state
  static ProductAutomatonState * initialState_;

public: /* public static methods */
  /// hash function
  static unsigned int hash(ProductAutomatonState &);

  /// compare pointers
  static bool isLessThan(ProductAutomatonState *, ProductAutomatonState *);

private: /* private variables */
  /// state of specification automaton
  unsigned int specificationState_;

  /// state of test case automaton
  unsigned int testCaseState_;

  /// pointer to channel state
  ChannelState * channelState_;

  /// successors by specification automaton
  std::map<unsigned int, std::vector<ProductAutomatonState *> > specificationSuccessors_;

  /// successors by test case automaton
  std::map<unsigned int, std::vector<ProductAutomatonState *> > testCaseSuccessors_;

public: /* public variables */
  /// DFS number
  unsigned int dfs;

  /// tarjan lowlink
  unsigned int lowlink;

  /// whether state is on tarjan stack
  bool onTarjanStack;

public: /* public methods */
  // initial state constructor
  ProductAutomatonState(ServiceAutomaton &, ServiceAutomaton &);

  // destructor
  ~ProductAutomatonState();

  // comparison operator
  bool operator==(ProductAutomatonState &);

  /// comparison operator
  bool operator<(const ProductAutomatonState &) const;

  // successor creation method
  std::vector<ProductAutomatonTransition *> generateSuccessors();

private: /* private methods */
  // successor generating constructor
  ProductAutomatonState(ProductAutomatonState &, unsigned int, bool, ChannelState *);
};


/*!
 * transition struct
 */
struct ProductAutomatonTransition
{
  // event occurring (i.e. TAU or message received/consumed)
  unsigned int event;

  // whether specification automaton did a step
  bool specificationStep;

  // next state
  ProductAutomatonState * resultingState;

  // contructor
  ProductAutomatonTransition(unsigned int, bool, ProductAutomatonState *);
};


/*!
 * \brief Product automaton class
 */
class ProductAutomaton
{
  friend class Knowledge;

private: /* private variables */
  /// reference to specification automaton
  ServiceAutomaton & specificationAutomaton_;

  /// reference to test case automaton
  ServiceAutomaton & testCaseAutomaton_;

  /// initial state
  ProductAutomatonState * initialState_;

  /// set of all states
  HashMap<ProductAutomatonState, ProductAutomatonState::hash> states_;

  /// SCCs
  std::map<unsigned int, std::vector<ProductAutomatonState *> > SCCs_;

  /// SCCs with states without specification successors
  std::vector<unsigned int> noSpecificationSuccessors_;

  /// bad composition state if test case is no conformance partner
  ProductAutomatonState * badState_;

public: /* public variables */
  /// error message when not a conformance partner
  std::string errorMessage;

public: /* public methods */
  /// constructor
  ProductAutomaton(ServiceAutomaton &, ServiceAutomaton &);

  /// destructor
  ~ProductAutomaton();

  /// whether messages from specification automaton to test case automaton are strong receivable
  bool checkStrongReceivability();

  /// write automaton do dot file with given filename
  void writeToDotFile(std::string &, bool = true);

private: /* private methods */
  /// generate shortest path to the "bad" state
  void findShortestPath(std::map<ProductAutomatonState *, ProductAutomatonState *> &);
};

#endif /* PRODUCTAUTOMATON_H_ */

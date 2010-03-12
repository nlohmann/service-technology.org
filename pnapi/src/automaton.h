/*!
 * \file  automaton.h
 */

#ifndef PNAPI_AUTOMATON_H
#define PNAPI_AUTOMATON_H

#include <set>
#include <string>
#include <vector>
#include "myio.h"
#include "Output.h"
#include "petrinet.h" 
#include "state.h"


namespace pnapi
{

/// forward declaration
class Marking;

/*!
 * \brief Service Automaton class
 */
class Automaton
{
  /// friend functions
  friend std::ostream & io::__sa::output(std::ostream &, const Automaton &);
  friend std::ostream & io::__stat::output(std::ostream &, const Automaton &);
  friend std::ostream & io::__dot::output(std::ostream &, const Automaton &);

public: /* public constants */
  /// hash table size
  static const unsigned int HASH_SIZE = 65535;
  
private: /* private variables */
  /// vector of states
  std::vector<State *> states_;
  /// vector of edges
  std::vector<Edge *> edges_;
  
  /// set of input labels
  std::set<std::string> input_;
  /// set of output labels
  std::set<std::string> output_;
  /// set of all synchronous labels
  std::set<std::string> synchronous_;

  /// mapping from transitions to strings (their label) [optional]
  std::map<Transition *, std::string> * edgeLabels_;
  /// mapping from transitions to their types [optional]
  std::map<Transition *, Edge::Type> * edgeTypes_;
  /// mapping from places to their weight [optional]
  std::map<const Place *, unsigned int> * weights_;

  /// underlying Petri net [optional]
  PetriNet * net_;
  /// the hash table [optional] needed by PetriNet => Automaton
  std::vector<std::set<State *> > * hashTable_;
  /// state counter
  unsigned int stateCounter_;

public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor
  Automaton();
  /// constructor generating automaton from Petri net
  Automaton(PetriNet &);
  /// standard copy constructor
  Automaton(const Automaton &);
  /// assignment operator
  Automaton & operator=(const Automaton &);
  /// standard destructor
  virtual ~Automaton();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// creating a state
  State & createState();
  /// creating a state with a given name
  State & createState(const unsigned int);
  /// creating a state from a given marking
  State & createState(Marking &);
  /// creating an edge from state 1 to state 2
  Edge & createEdge(State &, State &);
  /// creating an edge with label and type
  Edge & createEdge(State &, State &, const std::string &, Edge::Type = Edge::TAU);
  /// adds an input label
  void addInputLabel(const std::string &);
  /// adds an output label
  void addOutputLabel(const std::string &);
  /// adds a synchronous label
  void addSynchronousLabel(const std::string &);
  /// sets all synchronous labels
  void setSynchronousLabels(const std::set<std::string> &);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// finding a state by name
  State * findState(const unsigned int) const;
  /// returning a set of states with no preset
  std::set<State *> getInitialStates() const;
  /// returning a set of states with no postset
  std::set<State *> getFinalStates() const;
  /// returning a set of input labels (after PetriNet => Automaton)
  const std::set<std::string> & getInputLabels() const;
  /// returning a set of output labels (after PetriNet => Automaton)
  const std::set<std::string> & getOutputLabels() const;
  /// returns the set of all synchronous labels
  const std::set<std::string> & getSynchronousLabels() const;
  //@}
  
  /*!
   * \name transforming methods
   */
  //@{
  /// creates a state machine from automaton
  PetriNet toStateMachine() const;
  /// prints the automaton to a TG file (Automaton => Petri net)
  void printToTransitionGraph(std::vector<std::string> &, util::Output &, PetriNet::AutomatonConverter) const;
  //@}
  
private: /* private methods */
  /// deleting a state from the automaton
  void deleteState(State &);
  /// depth-first-search in the unknown automaton
  void dfs(State &);
};

} /* namespace pnapi */

#endif /* PNAPI_AUTOMATON_H */

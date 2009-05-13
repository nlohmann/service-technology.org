#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <set>
#include <string>
#include <vector>
#include "component.h"
#include "io.h"


namespace pnapi
{

  /// forward declaration
  class PetriNet;
  class Marking;


  class Automaton
  {
    friend std::ostream & io::__sa::output(std::ostream &, const Automaton &);

  public:
    /// standard constructor
    Automaton();
    /// constructor generating automaton from Petri net
    Automaton(PetriNet &);
    /// standard copy constructor
    Automaton(const Automaton &);
    /// standard destructor
    virtual ~Automaton();

    /// creating a state
    State & createState(const std::string = "");
    /// creating a state from given marking
    State & createState(Marking &, const std::string = "");
    /// finding a state by name
    State * findState(const std::string) const;

    /// creating an edge from state 1 to state 2
    Edge & createEdge(State &, State &);
    /// creating an edge with label
    Edge & createEdge(State &, State &, const std::string);
    /// creating an edge with label and transition's type
    Edge & createEdge(State &, State &, const std::string, Node::Type);

    /// creates a state machine from automaton
    PetriNet & stateMachine() const;

    /// returning a set of states with no preset
    const std::set<State *> initialStates() const;
    /// returning a set of states with no postset
    const std::set<State *> finalStates() const;

  private:
    /// vector of states
    std::vector<State *> states_;
    /// vector of edges
    std::vector<Edge *> edges_;

    /// mapping from transitions to strings (their label) [optional]
    std::map<Transition *, std::string> *edgeLabels_;
    /// mapping from transitions to their types (optional)
    std::map<Transition *, Node::Type> *edgeTypes_;

    /// underlying Petri net (optional)
    PetriNet *net_;
    /// the hash table (optional) needed by PetriNet => Automaton
    std::vector<std::set<State *> > *hashTable_;
    static const unsigned int HASH_SIZE = 65535;

    /// depth-first-search in the unknown automaton
    void dfs(State &);
    /// returning a set of input labels (after PetriNet => Automaton)
    const std::set<std::string> input() const;
    /// returning a set of output labels (after PetriNet => Automaton)
    const std::set<std::string> output() const;

    /// deleting a state from the automaton
    void deleteState(State *);

  };

} /* namespace pnapi */

#endif

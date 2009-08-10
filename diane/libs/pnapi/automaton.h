#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <set>
#include <string>
#include <vector>
#include "myio.h"


namespace pnapi
{

  /// forward declaration
  class PetriNet;
  class Marking;


  class Automaton
  {
    friend std::ostream & io::__sa::output(std::ostream &, const Automaton &);
    friend std::ostream & io::__stat::output(std::ostream &, const Automaton &);
    friend std::ostream & io::__dot::output(std::ostream &, const Automaton &);

  public:
    enum Type { INPUT, OUTPUT, TAU, SYNCHRONOUS };
    /// standard constructor
    Automaton();
    /// constructor generating automaton from Petri net
    Automaton(PetriNet &);
    /// standard copy constructor
    Automaton(const Automaton &);
    /// standard destructor
    virtual ~Automaton();

    /// creating a state
    State & createState();
    /// creating a state with a given name
    State & createState(const unsigned int);
    /// creating a state from given marking
    State & createState(Marking &);
    /// finding a state by name
    State * findState(const unsigned int) const;

    /// creating an edge from state 1 to state 2
    Edge & createEdge(State &, State &);
    /// creating an edge with label and type
    Edge & createEdge(State &, State &, const std::string, Type = TAU);

    /// creates a state machine from automaton
    PetriNet & stateMachine() const;

    /// returning a set of states with no preset
    const std::set<State *> initialStates() const;
    /// returning a set of states with no postset
    const std::set<State *> finalStates() const;

    /// prints the automaton to an STG file (Automaton => Petri net)
    std::string printToSTG(std::vector<std::string> &) const;
    /// returning a set of input labels (after PetriNet => Automaton)
    std::set<std::string> input() const;
    void addInput(std::string);
    /// returning a set of output labels (after PetriNet => Automaton)
    std::set<std::string> output() const;
    void addOutput(std::string);

    /// sets all synchronous labels
    void setSynchronousLabels(const std::set<std::string> &);
    /// returns the set of all synchronous labels
    std::set<std::string> getSynchronousLabels() const;

    static const unsigned int HASH_SIZE = 65535;

  private:
    /// vector of states
    std::vector<State *> states_;
    /// vector of edges
    std::vector<Edge *> edges_;
    /// set of input labels
    std::set<std::string> input_;
    /// set of output labels
    std::set<std::string> output_;
    /// set of all synchronous labels
    std::set<std::string> labels_;

    /// mapping from transitions to strings (their label) [optional]
    std::map<Transition *, std::string> *edgeLabels_;
    /// mapping from transitions to their types (optional)
    std::map<Transition *, Type> *edgeTypes_;
    /// mapping from places to their weight (optional)
    std::map<const Place *, unsigned int> *weights_;

    /// underlying Petri net (optional)
    PetriNet *net_;
    /// the hash table (optional) needed by PetriNet => Automaton
    std::vector<std::set<State *> > *hashTable_;
    /// state counter
    unsigned int counter_;

    /// depth-first-search in the unknown automaton
    void dfs(State &);

    /// deleting a state from the automaton
    void deleteState(State *);

    void printToSTGRecursively(State *, std::ostringstream &,
        std::map<State *, bool> &, std::vector<std::string> &) const;

  };

} /* namespace pnapi */

#endif

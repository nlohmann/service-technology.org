/*!
 * \file  state.h
 */

#ifndef STATE_H
#define STATE_H

#include <set>
#include <string>
#include "myio.h"


namespace pnapi
{

/// forward declarations
class Marking;
class Automaton;
namespace parser
{
namespace sa
{
int parse();
} /* namespace sa */
} /* namespace parser */


/*!
 * \class   State
 *
 * The class State describes a state of an automaton. It represents a
 * certain name, preset, and postset. If needed, there is the possibility
 * to defina a Marking according to a Petri net such that one can compute
 * a state's hash value.
 */
class State
{
  /// friend methods
  friend std::ostream & io::__sa::output(std::ostream &, const State &);
  friend int pnapi::parser::sa::parse();

  /// friend classes
  friend class Automaton;
  friend class Edge;

private: /* private variabled */
  /// the state's name
  unsigned int name_;
  /// the state's preset
  std::set<State *> preset_;
  /// the state's postset
  std::set<State *> postset_;
  /// the state's postset edges
  std::set<Edge *> postsetEdges_;
  /// whether the state is a final state
  bool isFinal_;
  /// whether the state is an initial state
  bool isInitial_;
  
  /*** optional properties ***/
  /// the marking which is represented by the state (needed by service a.)
  Marking * m_;
  /// the hash value of the state (computed through the markings)
  unsigned int hashValue_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor
  State(unsigned int *);
  /// constructor
  State(const unsigned int);
  /// standard constructor service automaton
  State(Marking &, std::map<const Place *, unsigned int> *, unsigned int &);
  /// copy constructor
  State(const State &);
  /// copy a state from one automaton to another one
  State(const State &, PetriNet *, 
        std::map<const Place *, const Place *> * = NULL);
  /// standard destructor
  ~State();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// make this state a final state
  void setFinal();
  /// make this state an initial state
  void setInitial();
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// method returning the state's name
  unsigned int getName() const;
  /// method returning the state's preset
  const std::set<State *> & getPreset() const;
  /// method returning the state's postset
  const std::set<State *> & getPostset() const;
  /// method returning the state's postset edges
  const std::set<Edge *> & getPostsetEdges() const;
  /// returns the state's marking
  Marking * getMarking() const;
  /// returns the state's hash value (only needed by service automaton)
  unsigned int getHashValue() const;
  /// returns the size of the represented marking
  unsigned int size() const;
  /// method returning whether the state is a final state
  bool isFinal() const;
  /// method returning whether the state is an initial state
  bool isInitial() const;
  /// checks if 2 states are equal (by name, preset, and postset, or marking)
  bool operator==(const State &) const;
  //@}

private: /* private functions */
  /// adding a state to the preset
  void addPreState(State &);
  /// adding a state to the postset
  void addPostState(State &);
  /// adding an edge to the postset edges
  void addPostEdge(Edge &);
  /// set the hash value
  void setHashValue(std::map<const Place *, unsigned int> *);
};


/*!
 * \class   Edge
 *
 * This class provides a representation of automata edges. One edge
 * consists of a source state and a destination state. Each state can have
 * a label and a firing type (according to the types given in the
 * transitions. This type is only needed by output methods which have
 * to differ between input and output signals.
 */
class Edge
{
public: /* public types */
  enum Type 
  { 
    INPUT, 
    OUTPUT, 
    TAU, 
    SYNCHRONOUS 
  };
  
private: /* private variables */
  /// edge's label
  std::string label_;
  /// edge's source state
  State & source_;
  /// edge's destination state
  State & destination_;
  /// edge's type
  Type type_;
    
public: /* public methods */
  /// constructor
  Edge(State &, State &, const std::string & = "", Type = TAU);
  /// standard destructor
  virtual ~Edge();

  /// method returning the edge's label
  const std::string & getLabel() const;
  /// method returning the egde's source state
  State & getSource() const;
  /// method returning the edge's destination state
  State & getDestination() const;
  /// method returning the type according to the edge (sa)
  Type getType() const;
};

} /* namespace pnapi */

#endif /* STATE_H */

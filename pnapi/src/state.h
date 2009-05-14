#ifndef STATE_H
#define STATE_H

#include <set>
#include <string>
#include "component.h"
#include "io.h"


namespace pnapi
{

  /// forward declarations
  class Marking;


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
    friend std::ostream & io::__sa::output(std::ostream &, const State &);

  public:
    /// standard constructor
    State(unsigned int * = NULL, bool isFinal = false);
    /// standard constructor service automaton
    State(Marking &m, std::map<const Place *, unsigned int> *,
        unsigned int * = NULL, bool isFinal = false);
    /// standard copy constructor
    State(const State &s);
    /// standard destructor
    virtual ~State() {}

    /// adding a state to the preset
    void addPre(State &);
    /// adding a state to the postset
    void addPost(State &);

    /// method which returns the state's name
    const std::string name() const;
    /// method which returns the state's preset
    const std::set<State *> preset() const;
    /// method which returns the state's postset
    const std::set<State *> postset() const;

    /// method which returns the state's final property
    bool isFinal() const;
    /// method which toggles the state's final property to true
    void final();
    /// method which returns the state's initial property
    bool isInitial() const;
    /// method which toggles the state's initial property to true
    void initial();

    /// checks if 2 states are equal (by name, preset, and postset, or marking)
    bool operator ==(const State &s2) const;

    /// returns the state's marking
    Marking * marking() const;
    /// returns the state's hash value (only needed by service automaton)
    const unsigned int hashValue();
    /// returns the size of the represented marking
    const unsigned int size() const;

  private:
    /// the state's name
    std::string name_;
    /// the state's preset
    std::set<State *> preset_;
    /// the state's postset
    std::set<State *> postset_;
    /// the state's final property
    bool isFinal_;
    /// the state's initial property
    bool isInitial_;

    /*** optional properties ***/
    /// the marking which is represented by the state (needed by service a.)
    Marking *m_;
    /// the hash value of the state (computed through the markings)
    unsigned int hashValue_;
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
    friend std::ostream & io::__sa::output(std::ostream &, const Edge &);

  public:
    /// standard constructor
    Edge(State &source, State &destination, const std::string label = "",
        const Node::Type type = Node::INTERNAL);
    /// standard destructor
    virtual ~Edge();

    /// method which returns the edge's label
    const std::string label() const;
    /// method which returns the egde's source state
    State &source() const;
    /// method which returns the edge's destination state
    State &destination() const;
    /// method which returns the type according to the edge (sa)
    Node::Type type() const;

  private:
    /// edge's label
    std::string label_;
    /// edge's source state
    State &source_;
    /// edge's destination state
    State &destination_;
    /// edge's type
    Node::Type type_;

  };

} /* END OF NAMESPACE pnapi */

#endif


/* -*- mode: c++ -*- */

/*!
 * \file    component.h
 *
 * \brief   Nodes and Arcs for Petri nets
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2005/10/18
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#ifndef PNAPI_COMPONENT_H
#define PNAPI_COMPONENT_H

#include <string>
#include <set>
#include <map>
#include <deque>

namespace pnapi
{

// forward declarations
class PetriNet;
class Arc;
class Marking;
class Label;
namespace util { class ComponentObserver; }


/*!
 * \brief   unspecified Petri net nodes
 *
 * Class to represent nodes (i.e. places and transitions) of Petri
 * nets. Each node has an id and a history (i.e. the list of roles the
 * node had during the processing).
 *
 * Important properties of a node:
 *  - belongs to exactly one PetriNet (getPetriNet(), setPetriNet())
 *  - has a communication type (getType(), setType())
 *  - has a name (and a history of previous names; getName(), setName())
 *  - updates its pre- and postset automatically
 */
class Node
{
  /// observer needs to update pre- and postsets
  friend class util::ComponentObserver;

protected: /* protected variables */
  /// the petri net this node belongs to
  PetriNet & net_;
  /// petri net's observer for this node
  util::ComponentObserver & observer_;

private: /* private variables */
  /// the set of roles (i.e. the history) of the node
  std::deque<std::string> history_;
  /// the preset of this node
  std::set<Node *> preset_;
  /// the arcs to the preset of this node
  std::set<Arc *> presetArcs_;
  /// the postset of this node
  std::set<Node *> postset_;
  /// the arcs to the postset of this node
  std::set<Arc *> postsetArcs_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Node(PetriNet &, util::ComponentObserver &, const std::string &);
  /// copy constructor
  Node(PetriNet &, util::ComponentObserver &, const Node &, const std::string &);
  /// destructor
  virtual ~Node();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// set new name of the node
  void setName(const std::string &);
  /// adds a prefix to all names
  void prefixNameHistory(const std::string &);
  /// merges the histories of two nodes
  void mergeNameHistory(const Node &);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// returns the petri net this node belongs to
  PetriNet & getPetriNet() const;
  /// checks if two nodes are parallel to each other
  bool isParallel(const Node &) const;
  /// returns the name of the node
  std::string getName() const;
  /// returns the name history
  std::deque<std::string> getNameHistory() const;
  /// returns the node's preset
  const std::set<Node *> & getPreset() const;
  /// returns the arcs to the node's preset
  const std::set<Arc *> & getPresetArcs() const;
  /// returns the node's postset
  const std::set<Node *> & getPostset() const;
  /// returns the arcs to the node's postset
  const std::set<Arc *> & getPostsetArcs() const;
  //@}

protected: /* protected methods */
  /// merges another node into this one
  void merge(Node &, bool);

private: /* private methods */
  /// no copying!
  Node(const Node &);
  /// merges the pre-/postsets of two nodes
  void mergeArcs(const Node &, bool, bool);
};


/*!
 * \brief   transitions of the Petri net
 *
 * Class to represent transitions of Petri nets. Each transition
 * inherits the functions and variables from class #Node.
 */
class Transition : public Node
{
  /// observer needs to update type
  friend class util::ComponentObserver;

public: /* public types */
  /*!
   * \brief transition communication types 
   */
  enum Type
  {
    /// internal transitions, only connected to places
    INTERNAL,
    /// input transitions, only connected to internal places or input labels
    INPUT,
    /// output transitions, only connected to internal places or output labels
    OUTPUT,
    /// transitions connected with both input and output places
    INOUT 
  };
  
private: /* private variables */
  /// the type of this transition
  Type type_;
  /// transition cost
  int cost_;
  /// roles
  std::set<std::string> roles_;
  /// interface labels
  std::map<Label *, unsigned int> labels_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Transition(PetriNet &, util::ComponentObserver &,
             const std::string &);
  /// copy constructor
  Transition(PetriNet &, util::ComponentObserver &, const Transition &, 
             const std::string &);
  /// destructor
  virtual ~Transition();
  //@}
  
  /*!
   * \name structural changes
   */
  //@{
  /// set transition cost
  void setCost(int);
  /// add role
  void addRole(const std::string &);
  /// add a set of roles
  void addRoles(const std::set<std::string> &);
  /// add an interface label
  void addLabel(Label & label, unsigned int = 1);
  /// remove an interface label
  void removeLabel(const Label & label);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// retrieves the communication type
  Type getType() const;
  /// compares the type with another transition's type
  bool isComplementType(Type) const;
  /// get transition cost
  int getCost() const;
  /// get roles
  const std::set<std::string> & getRoles() const;
  /// help method for normalize method
  bool isNormal() const;
  /// merges another transition into this one
  void merge(Transition &);
  /// returns true, if the transition is associated to at least one synchronous label
  bool isSynchronized() const;
  /// the set of input labels
  std::set<Label *> getInputLabels() const;
  /// the set of output labels
  std::set<Label *> getOutputLabels() const;
  /// the set of synchronous labels
  std::set<Label *> getSynchronousLabels() const;
  /// the names of synchronous labels
  std::set<std::string> getSynchronousLabelNames() const;
  /// compare interface labels with given transition
  bool equalLabels(const Transition &) const;
  /// get mapping of interface labels to their weight
  const std::map<Label *, unsigned int> & getLabels() const;
  //@}
  
private: /* private methods */
  /// no standard copying!
  Transition(const Transition &);
  /// updates the type
  void updateType();
  /// updates the type
  void updateType(const Label &);
  /// changes the type of this transition
  void setType(Type);
};


/*!
 * \brief   places of the Petri net
 *
 * Class to represent places of Petri nets. In addition to the
 * inherited functions and variables from class #Node, each place has
 * an initial marking.
 */
class Place : public Node
{
private: /* private variables */
  /// marking of the place
  unsigned int tokens_;
  /// capacity, where 0 means unlimited
  unsigned int capacity_;
  /// place was an interface label
  bool wasInterface_;
  /// maximum occurrence
  int maxOccurrence_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Place(PetriNet &, util::ComponentObserver &, const std::string & ,
        unsigned int, unsigned int);
  /// copy constructor
  Place(PetriNet &, util::ComponentObserver &, const Place &, const std::string &);
  //@}
  
  /*!
   * \name structural changes
   */
  //@{
  /// sets the number of tokens lying on this place
  void setTokenCount(unsigned int = 1);
  /// set the maximum occurrence
  void setMaxOccurrence(int);
  /// merges two places
  Place & merge(Place &, bool = true);
  /// set wasInterface flag
  void setWasInterface(bool = true);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// returns the number of tokens lying on this place
  unsigned int getTokenCount() const;
  /// returns the capacity
  unsigned int getCapacity() const;
  /// if the place was an interface label
  bool wasInterface() const;
  /// get the maximum occurrence
  int getMaxOccurrence();
  //@}

private: /* private methods */
  /// no standard copying!
  Place(const Place &);
  /// no standard copying!
  Place & operator=(const Place &);
};


/*!
 * \brief   arcs of the Petri net
 *
 * Class to represent arcs of Petri nets. An arc written as a tupel
 * (n1,n2) has n1 as source and n2 as target.
 */
class Arc
{
private:
  /// Petri net this arc belongs to
  PetriNet & net_;
  /// petri net's observer for this arc
  util::ComponentObserver & observer_;
  /// source node of the arc
  Node * source_;
  /// target node of the arc
  Node * target_;
  /// weight of the arc
  unsigned int weight_;
    
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Arc(PetriNet &, util::ComponentObserver &, Node &, Node &, unsigned int = 1);
  /// copy constructor
  Arc(PetriNet &, util::ComponentObserver &, const Arc &);
  /// copy constructor
  Arc(PetriNet &, util::ComponentObserver &, const Arc &, Node &, Node &);
  /// destructor
  virtual ~Arc();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// set the weight
  void setWeight(unsigned int);
  /// merges another arc into this one
  void merge(Arc &);
  /// swaps source and target node
  void mirror();
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// returns the Petri net this arc belongs to
  PetriNet & getPetriNet() const;
  /// source node
  Node & getSourceNode() const;
  /// target node
  Node & getTargetNode() const;
  /// transition
  Transition & getTransition() const;
  /// place
  Place & getPlace() const;
  /// weight
  unsigned int getWeight() const;
  //@}

private: /* private methods */
  /// no copying!
  Arc(const Arc &);
};

} /* namespace pnapi */

#endif /* PNAPI_COMPONENT_H */

/* -*- mode: c++ -*- */

/*!
 * \file    petrinode.h
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

#ifndef PNAPI_PETRINODE_H
#define PNAPI_PETRINODE_H

#include <string>
#include <deque>
#include <set>

using std::string;
using std::deque;
using std::set;


namespace pnapi
{
  
  // forward declarations
  class PetriNet;
  class ComponentObserver;
  class Arc;


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
    friend class ComponentObserver;

  public:

    /// node types (communication)
    enum Type { INTERNAL, INPUT, OUTPUT };

    /// constructor
    Node(PetriNet &, ComponentObserver &, const string &, Type);

    /// copy constructor
    Node(PetriNet &, ComponentObserver &, const Node &);

    /// destructor
    virtual ~Node();

    /// returns the petri net to which this node belongs
    PetriNet & getPetriNet() const;

    /// retrieves the communication type
    Type getType() const;

    /// compares the type with another node's type
    bool isComplementType(const Node &) const;

    /// returns the name of the node
    string getName() const;

    /// returns the name history
    deque<string> getNameHistory() const;

    /// adds a prefix to all names
    void prefixNameHistory(const string &);

    /// returns the node's preset
    const set<Node *> & getPreset() const;

    /// returns the node's postset
    const set<Node *> & getPostset() const;

    // DOT output of the node
    virtual string toString() const =0;


  protected:

    /// petri net's observer for this node
    ComponentObserver & observer_;

    /// changes the type of this node
    virtual void setType(Type);

    /// changes the history
    void addToHistory(const deque<string> &);


  private:

    /// the petri net this node belongs to
    PetriNet & net_;

    /// the type of this node
    Type type_;

    /// the set of roles (i.e. the history) of the node
    deque<string> history_;

    /// the preset of this node
    set<Node*> preset_;

    /// the postset of this node
    set<Node*> postset_;

    
    /// no copying!
    Node(const Node &);

  };


  /*!
   * \brief   transitions of the Petri net
   *
   * Class to represent transitions of Petri nets. Each transition
   * inherits the functions and variables from class #Node.
   */
  class Transition : public Node
  {
  public:
    /// constructor
    Transition(PetriNet &, ComponentObserver &, 
	       const string &, Type = INTERNAL);

    /// copy constructor
    Transition(PetriNet &, ComponentObserver &, const Transition &);

    /// help method for normalize method
    bool isNormal() const;

    /// output of the transition
    string toString() const;


  private:

    /// no standard copying!
    Transition(const Transition &);
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
  public:

    /// constructor
    Place(PetriNet &, ComponentObserver &, const string &, Type);

    /// copy constructor
    Place(PetriNet &, ComponentObserver &, const Place &);

    /// returns the number of tokens lying on this place
    unsigned int getTokenCount() const;

    /// returns the capacity
    unsigned int getCapacity() const;

    /// DOT-output of the place
    string toString() const;

    /// merges two places
    Place & merge(Place &);

    
  private:
    
    /// marking of the place
    unsigned int tokens_;

    /// capacity, where 0 means unlimited
    unsigned int capacity_;

    /// place was an interface place (now internal)
    bool wasInterface_;


    /// no standard copying!
    Place(const Place &);

    /// changes the communication type
    void setType(Type type);

    /// create arcs during merging
    void createArcs(const Place &, Place &, const set<Node *>, 
		    const set<Node *>, bool) const;

    /// internalizes an interface place
    void internalize();

  };


  /*!
   * \brief   arcs of the Petri net
   *
   * Class to represent arcs of Petri nets. An arc written as a tupel
   * (n1,n2) has n1 as #source and n2 as #target.
   */
  class Arc
  {
  public:

    /// constructor
    Arc(PetriNet &, ComponentObserver &, Node &, Node &, unsigned int = 1);

    /// copy constructor
    Arc(PetriNet &, ComponentObserver &, const Arc &);

    /// destructor
    virtual ~Arc();

    /// returns the Petri net this arc belongs to
    PetriNet & getPetriNet() const;

    /// source node
    Node & getSourceNode() const;

    /// target node
    Node & getTargetNode() const;

    /// weight
    unsigned int getWeight() const;

    /// DOT-output of the arc
    string toString(bool = true) const;


  private:

    /// Petri net this arc belongs to
    PetriNet & net_;

    /// petri net's observer for this arc
    ComponentObserver & observer_;

    /// source node of the arc
    Node & source_;

    /// target node of the arc
    Node & target_;

    /// weight of the arc
    unsigned int weight_;


    /// no copying!
    Arc(const Arc &);

    /// change the weight
    void setWeight(unsigned int);

    /// swaps source and target node of the arc
    void mirror();

  };

}

#endif

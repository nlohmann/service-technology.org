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
#include <set>
#include <deque>

namespace pnapi
{

  // forward declarations
  class PetriNet;
  class Arc;
  class Marking;
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

  public:

    /// node types (communication)
    enum Type { INTERNAL, INPUT, OUTPUT, INOUT };

    /// constructor
    Node(PetriNet &, util::ComponentObserver &, const std::string &, Type);

    /// copy constructor
    Node(PetriNet &, util::ComponentObserver &, const Node &, const std::string &);

    /// destructor
    virtual ~Node();

    /// returns the petri net to which this node belongs
    PetriNet & getPetriNet() const;

    /// retrieves the communication type
    Type getType() const;

    /// compares the type with another node's type
    bool isComplementType(Type) const;

    /// checks if two nodes are parallel to each other
    bool isParallel(const Node &) const;
    
    /// returns the name of the node
    std::string getName() const;

    /// returns the name history
    std::deque<std::string> getNameHistory() const;

    /// adds a prefix to all names
    void prefixNameHistory(const std::string &);

    /// merges the histories of two nodes
    void mergeNameHistory(Node &);

    /// returns the node's preset
    const std::set<Node *> & getPreset() const;

    /// returns the arcs to the node's preset
    const std::set<Arc *> & getPresetArcs() const;

    /// returns the node's postset
    const std::set<Node *> & getPostset() const;

    /// returns the arcs to the node's postset
    const std::set<Arc *> & getPostsetArcs() const;


  protected:

    /// the petri net this node belongs to
    PetriNet & net_;

    /// petri net's observer for this node
    util::ComponentObserver & observer_;

    /// changes the type of this node
    virtual void setType(Type);

    /// merges another node into this one
    void merge(Node &, bool);


  private:

    /// the type of this node
    Type type_;

    /// the set of roles (i.e. the history) of the node
    std::deque<std::string> history_;

    /// the preset of this node
    std::set<Node*> preset_;
    
    /// the arcs to the preset of this node
    std::set<Arc*> presetArcs_;
    
    /// the postset of this node
    std::set<Node*> postset_;
    
    /// the arcs to the postset of this node
    std::set<Arc*> postsetArcs_;


    /// no copying!
    Node(const Node &);

    /// merges the pre-/postsets of two nodes
    void mergeArcs(pnapi::Node&, pnapi::Node&, const std::set<Node*> &,
		   const std::set<Node*> &, bool, bool);

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

  public:
    /// constructor
    Transition(PetriNet &, util::ComponentObserver &,
	       const std::string &, 
	       const std::set<std::string> & = std::set<std::string>());

    /// copy constructor
    Transition(PetriNet &, util::ComponentObserver &, const Transition &, 
	       const std::string &);

    /// set transition cost
    void setCost(int);
    
    /// get transition cost
    int getCost() const;
    
    /// help method for normalize method
    bool isNormal() const;

    /// merges another transition into this one
    void merge(Transition &, bool = true);

    /// returns true, if the transition is associated to at least one label
    bool isSynchronized() const;

    /// the set of labels
    const std::set<std::string> & getSynchronizeLabels() const;
    
    /// set the set of labels
    void setSynchronizeLabels(const std::set<std::string> &);


  private:

    /// transition cost
    int cost_;
    
    /// synchronize labels
    std::set<std::string> labels_;


    /// no standard copying!
    Transition(const Transition &);

    /// updates the type
    void updateType();
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
    Place(PetriNet &, util::ComponentObserver &, const std::string &, Type, unsigned int, 
	  unsigned int, const std::string &);

    /// copy constructor
    Place(PetriNet &, util::ComponentObserver &, const Place &, const std::string &);

    /// returns the number of tokens lying on this place
    unsigned int getTokenCount() const;

    /// returns the capacity
    unsigned int getCapacity() const;

    /// if the place was an interface place before
    bool wasInterface() const;

    /// merges two places
    Place & merge(Place &, bool = true, bool = true);

    /// marks the place with token
    void mark(unsigned int = 1);

    /// returns the port this place belongs to
    std::string getPort() const;
    
    /// returns the port this place belongs to
    void setPort(std::string &);

    /// swaps interface type
    void mirror();


  private:

    /// marking of the place
    unsigned int tokens_;

    /// capacity, where 0 means unlimited
    unsigned int capacity_;

    /// place was an interface place (now internal)
    bool wasInterface_;

    /// port this place belongs to
    std::string port_;


    /// no standard copying!
    Place(const Place &);

    /// no standard copying!
    Place & operator=(const Place &);

    /// changes the communication type
    void setType(Type type);

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
    Arc(PetriNet &, util::ComponentObserver &, Node &, Node &, unsigned int = 1);

    /// copy constructor
    Arc(PetriNet &, util::ComponentObserver &, const Arc &);

    /// copy constructor
    Arc(PetriNet &, util::ComponentObserver &, const Arc &, Node &, Node &);

    /// destructor
    virtual ~Arc();

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

    /// merges another arc into this one
    void merge(Arc &);

    /// swaps source and target node
    void mirror();


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


    /// no copying!
    Arc(const Arc &);

  };

}

#endif

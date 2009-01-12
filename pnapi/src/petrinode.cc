#include <cassert>

#include "util.h"
#include "petrinet.h"
#include "petrinode.h"


namespace pnapi {


  /****************************************************************************
   *** Class Node Function Definitions
   ***************************************************************************/


  /*!
   */
  Node::Node(PetriNet & net, const string & name, Type type) :
    net_(net), type_(type)
  {
    history_.push_back(name);
  }

  
  /*!
   */
  Node::Node(PetriNet & net, const Node & node) :
    net_(net), type_(node.type_), history_(node.history_)
  {
  }


  /*!
   * Do not delete a node directly but ask the net instead: 
   * PetriNet::deleteNode()
   */
  Node::~Node()
  {
    assert(!net_.containsNode(*this));
  }


  /*!
   */
  Node::Type Node::getType() const
  {
    return type_;
  }


  /*!
   */
  void Node::setType(Type type)
  {
    assert(type != type_);

    type_ = type;
  }


  /*!
   */
  bool Node::isComplementType(const Node & node) const
  {
    return (type_ == INPUT && node.type_ == OUTPUT) ||
           (type_ == OUTPUT && node.type_ == INPUT);
  }


  /*!
   */
  string Node::getName() const
  {
    return *history_.begin();
  }


  /*!
   */
  void Node::prefixNameHistory(const string & prefix)
  {
    deque<string> oldHistory = history_;
    for (deque<string>::iterator it = history_.begin(); it != history_.end();
	 ++it)
      *it = prefix + *it;
    //FIXME: net_.notifyNameHistoryChanged(oldHistory);
  }

  
  /*!
   */
  deque<string> Node::getNameHistory() const
  {
    return history_;
  }


  /*!
   */
  void Node::addToHistory(const deque<string> & history)
  {
    history_.insert(history_.end(), history.begin(), history.end());
    //FIXME: net_.updateNodeNameHistory(*this);
  }


  /*!
   */
  PetriNet & Node::getPetriNet() const
  {
    return net_;
  }


  /*!
   */
  const set<Node *> & Node::getPreset() const
  {
    return preset_;
  }


  /*!
   */
  const set<Node *> & Node::getPostset() const
  {
    return postset_;
  }


  /*!
   */
  void Node::updateNeighbourSets(Arc & arc) 
  {
    Node * source = &arc.getSourceNode();
    Node * target = &arc.getTargetNode();

    assert(source == this || target == this);

    if (source == this)
      postset_.insert(target);
    else
      preset_.insert(source);
  }



  /****************************************************************************
   *** Class Place Function Definitions
   ***************************************************************************/


  /*!
   */
  Place::Place(PetriNet & net, const string & name, Node::Type type) :
    Node(net, name, type), capacity_(0), 
    wasInterface_(type == Node::INTERNAL ? false : true)
  {
    //FIXME: notifyPetriNetPlaces();
  }


  /*!
   */
  Place::Place(PetriNet & net, const Place & place) :
    Node(net, place), capacity_(place.capacity_), 
    wasInterface_(place.wasInterface_)
  {
    //FIXME: notifyPetriNetPlaces();
  }

  
  /*!
   */
  void Place::setType(Type type)
  {
    //FIXME: Type oldType = getType();
    Node::setType(type);
    //FIXME: getPetriNet().updatePlaceType(*this, oldType);
  }


  /*!
   */
  unsigned int Place::getTokenCount() const
  {
    return tokens_;
  }


  /*!
   */
  unsigned int Place::getCapacity() const
  {
    return capacity_;
  }

  
  /*!
   * \pre   The place must be an interface place.
   * \post  The place is internal and WasInterface is true.
   */
  void Place::internalize()
  {
    assert(getType() != Node::INTERNAL);

    setType(Node::INTERNAL);

    assert(getType() == Node::INTERNAL);
    assert(wasInterface_);
  }


  /*!
   * Merges the properties of the given node into this one. If the place 
   * resides in the same PetriNet it is deleted while merging.
   *
   * The following properties are merged:
   * - NameHistory (concatenation)
   * - WasInterface (disjunction)
   * - Capacity (maximum)
   * - TokenCount (maximum)
   *
   * The following properties are ignored:
   * - PetriNet
   * - Type
   * - Pre-/Postset
   *
   * Use PetriNet::mergePlaces() if you want to merge pre- and postset as well.
   */
  Place & Place::merge(Place & place)
  {
    // collect data
    deque<string> newHistory = place.getNameHistory();
    bool newWasInterface = wasInterface_ || place.wasInterface_;
    unsigned int newCapacity = util::max(capacity_, place.capacity_);
    unsigned int newTokens = util::max(tokens_, place.tokens_);

    // possibly delete place to avoid collisions
    PetriNet & net = getPetriNet();
    if (&net == &place.getPetriNet())
      ;//FIXME: net.deletePlace(place);

    // actually merge
    addToHistory(newHistory);
    wasInterface_ = newWasInterface;
    capacity_ = newCapacity;
    tokens_ = newTokens;

    return *this;
  }

  /* FIXME
  void Place::notifyPetriNetPlaces()
  {
    getPetriNet().updatePlaces(*this);
  }
  */



  /****************************************************************************
   *** Class Transition Function Definitions
   ***************************************************************************/


  /*!
   */
  Transition::Transition(PetriNet & net, const string & name, Type type) :
    Node(net, name, type)
  {
    //FIXME: net.updateTransitions(*this);
  }


  /*!
   */
  Transition::Transition(PetriNet & net, const Transition & trans) :
    Node(net, trans)
  {
    //FIXME: net.updateTransitions(*this);
  }


  /*!
   */
  void Transition::addLabel(const string & label)
  {
    labels_.insert(label);
  }


  /*!
   */
  set<string> Transition::getLabels() const
  {
    return labels_;
  }



  /****************************************************************************
   *** Class Arc Function Definitions
   ***************************************************************************/

  
  /*!
   */
  Arc::Arc(PetriNet & net, Node & source, Node & target, unsigned int weight) :
    net_(net), source_(source), target_(target), weight_(weight)
  {
    source_.updateNeighbourSets(*this);
    target_.updateNeighbourSets(*this);

    //FIXME: net_.updateArcs(*this);
  }


  /*!
   */
  Arc::Arc(PetriNet & net, const Arc & arc) :
    net_(net), source_(*net.findNode(arc.source_.getName())), 
    target_(*net.findNode(arc.target_.getName())), weight_(arc.weight_)
  {
    assert(net.findNode(arc.source_.getName()) != NULL);
    assert(net.findNode(arc.target_.getName()) != NULL);

    source_.updateNeighbourSets(*this);
    target_.updateNeighbourSets(*this);

    //FIXME: net_.updateArcs(*this);
  }


  /*!
   */
  PetriNet & Arc::getPetriNet() const
  {
    return net_;
  }


  /*!
   */
  Node & Arc::getSourceNode() const
  {
    return source_;
  }


  /*!
   */
  Node & Arc::getTargetNode() const
  {
    return target_;
  }


  /*!
   */
  unsigned int Arc::getWeight() const
  {
    return weight_;
  }


  /*!
   */
  void Arc::setWeight(unsigned int weight)
  {
    weight_ = weight;
  }

}

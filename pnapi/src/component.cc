#include <cassert>

#include "util.h"
#include "petrinet.h"
#include "component.h"


namespace pnapi {


  /****************************************************************************
   *** Class Node Function Definitions
   ***************************************************************************/


  /*!
   */
  Node::Node(PetriNet & net, ComponentObserver & observer, const string & name, 
	     Type type) :
    observer_(observer), net_(net), type_(type)
  {
    assert(&observer.getPetriNet() == &net);

    history_.push_back(name);
  }

  
  /*!
   */
  Node::Node(PetriNet & net, ComponentObserver & observer, const Node & node) :
    observer_(observer), net_(net), type_(node.type_), history_(node.history_)
  {
    assert(&observer.getPetriNet() == &net);
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
    observer_.updateNodeNameHistory(*this, oldHistory);
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
    deque<string> oldHistory = history_;
    history_.insert(history_.end(), history.begin(), history.end());
    observer_.updateNodeNameHistory(*this, oldHistory);
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



  /****************************************************************************
   *** Class Place Function Definitions
   ***************************************************************************/


  /*!
   */
  Place::Place(PetriNet & net, ComponentObserver & observer, 
	       const string & name, Node::Type type) :
    Node(net, observer, name, type), capacity_(0), 
    wasInterface_(type == Node::INTERNAL ? false : true)
  {
    observer_.updatePlaces(*this);
  }


  /*!
   */
  Place::Place(PetriNet & net, ComponentObserver & observer, 
	       const Place & place) :
    Node(net, observer, place), capacity_(place.capacity_), 
    wasInterface_(place.wasInterface_)
  {
    observer_.updatePlaces(*this);
  }

  
  /*!
   */
  void Place::setType(Type type)
  {
    Type oldType = getType();
    Node::setType(type);
    observer_.updatePlaceType(*this, oldType);
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



  /****************************************************************************
   *** Class Transition Function Definitions
   ***************************************************************************/


  /*!
   */
  Transition::Transition(PetriNet & net, ComponentObserver & observer, 
			 const string & name, Type type) :
    Node(net, observer, name, type)
  {
    observer_.updateTransitions(*this);
  }


  /*!
   */
  Transition::Transition(PetriNet & net, ComponentObserver & observer, 
			 const Transition & trans) :
    Node(net, observer, trans)
  {
    observer_.updateTransitions(*this);
  }



  /****************************************************************************
   *** Class Arc Function Definitions
   ***************************************************************************/

  
  /*!
   */
  Arc::Arc(PetriNet & net, ComponentObserver & observer, Node & source, 
	   Node & target, unsigned int weight) :
    net_(net), observer_(observer), source_(source), target_(target), 
    weight_(weight)
  {
    assert(&observer.getPetriNet() == &net);

    observer_.updateArcCreated(*this);
  }


  /*!
   */
  Arc::Arc(PetriNet & net, ComponentObserver & observer, const Arc & arc) :
    net_(net), observer_(observer), 
    source_(*net.findNode(arc.source_.getName())), 
    target_(*net.findNode(arc.target_.getName())), weight_(arc.weight_)
  {
    assert(&observer.getPetriNet() == &net);
    assert(net.findNode(arc.source_.getName()) != NULL);
    assert(net.findNode(arc.target_.getName()) != NULL);

    observer_.updateArcCreated(*this);
  }


  /*!
   * You must not destroy an Arc directly.
   */
  Arc::~Arc()
  {
    assert(net_.findArc(source_, target_) == NULL);

    observer_.updateArcRemoved(*this);
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

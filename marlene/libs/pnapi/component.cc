#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#include "io.h"
using std::cout;
using std::endl;
#endif

#include <set>

#include "util.h"
#include "petrinet.h"
#include "component.h"

using std::set;

namespace pnapi {

  using util::ComponentObserver;


  /****************************************************************************
   *** Class Node Function Definitions
   ***************************************************************************/


  /*!
   */
  Node::Node(PetriNet & net, ComponentObserver & observer, const string & name,
	     Type type) :
    net_(net), observer_(observer), type_(type)
  {
    assert(&observer.getPetriNet() == &net);

    history_.push_back(name);
  }


  /*!
   */
  Node::Node(PetriNet & net, ComponentObserver & observer, const Node & node,
	     const string & prefix) :
    net_(net), observer_(observer), type_(node.type_), history_(node.history_)
  {
    assert(&observer.getPetriNet() == &net);
    if (!prefix.empty())
      for (std::deque<string>::iterator it = history_.begin();
	   it != history_.end(); ++it)
	*it = prefix + *it;
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
    //assert(type != type_);

    type_ = type;
  }


  /*!
   */
  bool Node::isComplementType(Type type) const
  {
    return (type_ == INPUT && type == OUTPUT) ||
           (type_ == OUTPUT && type == INPUT);
  }


  /*!
   * \brief Checks if two Nodes are parallel to each other.
   *
   * According to [STA90] (see petrinet-reduction.cc):
   * If there exist two distinct nodes n1 (this) and n2 (precondition 1)
   * with identical preset (precondition 2)
   * and identical postset (precondition 3)
   * and the arc weight for each node of the pre- and postset to/from p1
   * equals the arc weiht to/from n2 appropriate (precondition 4)
   * then these nodes are parallel to each other.
   *
   * This function is needed by PetriNet::reduce_rule_3p()
   * and PetriNet::reduce_rule_3t().
   *
   * \param n2 is the other node to which parallelism will be checked.
   */
  bool Node::isParallel(const Node & n2) const
  {
    if (
        (this != &n2) &&   // precondition 1
        (getPreset() == n2.getPreset()) && // precondition 2
        (getPostset() == n2.getPostset()) // precondition 3
       )
    {
      // precondition 4 - preset
        for(set<Node*>::iterator prenode_ = preset_.begin(); prenode_!=preset_.end(); ++prenode_)
        {
          if(net_.findArc((*(*prenode_)),(*this))->getWeight() !=
             net_.findArc((*(*prenode_)),n2)->getWeight() )
            return false;
        }

        // precondition 4 - postset
        for(set<Node*>::iterator postnode_ = postset_.begin(); postnode_!=postset_.end(); ++postnode_)
        {
          if(net_.findArc((*this),(*(*postnode_)))->getWeight() !=
             net_.findArc(n2,(*(*postnode_)))->getWeight() )
            return false;
        }

        // all preconditions fullfilled
        return true;
    }
    return false;
  }

  /*!
   */
  string Node::getName() const
  {
    assert(!history_.empty());
    return *history_.begin();
  }


  /*!
   */
  void Node::prefixNameHistory(const string & prefix)
  {
    assert(!prefix.empty());
    std::deque<string> oldHistory = history_;
    for (std::deque<string>::iterator it = history_.begin();
	 it != history_.end(); ++it)
      *it = prefix + *it;
    observer_.updateNodeNameHistory(*this, oldHistory);
  }


  /*!
   */
  std::deque<string> Node::getNameHistory() const
  {
    return history_;
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
  const set<Arc *> & Node::getPresetArcs() const
  {
    return presetArcs_;
  }


  /*!
   */
  const set<Arc *> & Node::getPostsetArcs() const
  {
    return postsetArcs_;
  }


  void Node::merge(Node & node, bool addArcWeights)
  {
    assert(&net_ == &node.net_);

    mergeNameHistory(node);

    mergeArcs(*this, node, preset_, node.preset_, addArcWeights, false);
    mergeArcs(*this, node, postset_, node.postset_, addArcWeights, true);

    observer_.updateNodesMerged(*this, node);
  }


  void Node::mergeNameHistory(Node & node)
  {
    std::deque<string> nodeHistory = node.history_;

    /*
    // remove history of node
    node.history_.clear();
    observer_.updateNodeNameHistory(node, nodeHistory);
    */

    // add history of node to this
    std::deque<string> oldHistory = history_;
    history_.insert(history_.end(), nodeHistory.begin(), nodeHistory.end());
    observer_.updateNodeNameHistory(*this, oldHistory);
  }


  void Node::mergeArcs(Node & node1, Node & node2, const set<Node *> & set1,
		       const set<Node *> & set2, bool addWeights,
		       bool isPostset)
  {
    for (set<Node *>::iterator it = set2.begin(); it != set2.end(); ++it)
      {
	Node & node1Source = isPostset ? node1 : **it;
	Node & node2Source = isPostset ? node2 : **it;
	Node & node1Target = isPostset ? **it  : node1;
	Node & node2Target = isPostset ? **it  : node2;

	Arc * arc1 = net_.findArc(node1Source, node1Target);
	Arc * arc2 = net_.findArc(node2Source, node2Target);

	assert(arc2 != NULL);
	if (arc1 == NULL)
	  net_.createArc(node1Source, node1Target, arc2->getWeight());
	else if (addWeights)
	  arc1->merge(*arc2);
	else
	  assert(arc1->getWeight() == arc2->getWeight());
      }
  }



  /****************************************************************************
   *** Class Place Function Definitions
   ***************************************************************************/


  /*!
   */
  Place::Place(PetriNet & net, ComponentObserver & observer,
	       const string & name, Type type, unsigned int tokens,
	       unsigned int capacity, const string & port) :
    Node(net, observer, name, type), tokens_(tokens), capacity_(capacity),
    wasInterface_(type == INTERNAL ? false : true), port_(port)
  {
    assert(type != INTERNAL || port.empty());

    observer_.updatePlaces(*this);
    setType(type);
    maxOccurrence_ = -1;
  }


  /*!
   */
  Place::Place(PetriNet & net, ComponentObserver & observer,
	       const Place & place, const string & prefix) :
    Node(net, observer, place, prefix), tokens_(place.tokens_),
    capacity_(place.capacity_), wasInterface_(place.wasInterface_),
    port_(place.port_)
  {
    observer_.updatePlaces(*this);
    setType(getType());
  }


  /*!
   */
  void Place::setType(Type type)
  {
    assert(type != INOUT);  // INOUT may only be used for transitions

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
   */
  string Place::getPort() const
  {
    return port_;
  }

  
  /*!
   * \todo  maybe private
   */
  void Place::setPort(string & port)
  {
    port_ = port;
  }
  

  /*!
   */
  bool Place::wasInterface() const
  {
    return getType() == INTERNAL && wasInterface_;
  }


  /*!
   * Merges the properties of the given node into this one.
   *
   * The following properties are merged:
   * - NameHistory (concatenation)
   * - WasInterface (disjunction)
   * - Capacity (maximum)
   * - TokenCount (maximum)
   * - Pre-/Postset (union)
   *
   * \pre   the places must reside in the same PetriNet instance
   * \post  the place is internal after merging
   */
  Place & Place::merge(Place & place, bool addArcWeights, bool internalize)
  {
    // be sure to internalize this place
    if (getType() != INTERNAL && internalize)
      setType(INTERNAL);

    // merge place properties
    wasInterface_ = wasInterface_ || place.wasInterface_;
    capacity_ = capacity_ > place.capacity_ ? capacity_ : place.capacity_;
    tokens_ = tokens_ > place.tokens_ ? tokens_ : place.tokens_;

    // Node::merge does all the rest
    Node::merge(place, addArcWeights);

    return *this;
  }


  /*!
   */
  void Place::mark(unsigned int t)
  {
    tokens_ = t;
  }


  /*!
   */
  void Place::mirror()
  {
    typedef set<Arc *> Arcs;

    assert(getType() != INTERNAL);

    if (getType() == INPUT)
      {
	setType(OUTPUT);
	assert(getPresetArcs().empty());
	Arcs postset = getPostsetArcs();
	for (Arcs::iterator it = postset.begin(); it != postset.end(); ++it)
	  (*it)->mirror();
      }
    else
      {
	setType(INPUT);
	assert(getPostsetArcs().empty());
	Arcs preset = getPresetArcs();
	for (Arcs::iterator it = preset.begin(); it != preset.end(); ++it)
	  (*it)->mirror();
      }
  }

  /*!
   * \brief set the maximum occurrence
   */
  void Place::setMaxOccurrence(int maxOccurrence)
  {
    maxOccurrence_ = maxOccurrence;
  }
  
  /*!
   * \brief get the maximum occurrence
   */
  int Place::getMaxOccurrence()
  {
    return maxOccurrence_;
  }
  


  /****************************************************************************
   *** Class Transition Function Definitions
   ***************************************************************************/


  /*!
   * \brief constructor
   */
  Transition::Transition(PetriNet & net, ComponentObserver & observer,
			 const string & name, const set<string> & labels) :
    Node(net, observer, name, INTERNAL), labels_(labels)
  {
    cost_ = 0;
		observer_.updateTransitions(*this);
  }


  /*!
   * \brief copy constructor
   */
  Transition::Transition(PetriNet & net, ComponentObserver & observer,
			 const Transition & trans, const string & prefix) :
    Node(net, observer, trans, prefix), labels_(trans.labels_)
  {
    cost_ = trans.cost_;
    observer_.updateTransitions(*this);
  }

  /*!
   * \brief set transition cost
   */
  void Transition::setCost(int cost)
  {
    cost_ = cost;
  }
  
  /*!
   * \brief get transition cost
   */
  int Transition::getCost() const
  {
    return cost_;
  }
  
  /*!
   * \brief   checks if the transition is normal
   *
   * \note    This is a help method for normalize method
   *          in class PetriNet.
   */
  bool Transition::isNormal() const
  {
    if (getSynchronizeLabels().size() > 1)
      return false;

    int counter = 0;

    for (set<Node *>::const_iterator p = getPreset().begin(); p != getPreset().end(); p++)
      if ((*p)->getType() != Node::INTERNAL)
        counter++;
    for (set<Node *>::const_iterator p = getPostset().begin(); p != getPostset().end(); p++)
      if ((*p)->getType() != Node::INTERNAL)
        counter++;

    return counter <= 1;
  }


  /*!
   * Merges the properties of the given node into this one.
   *
   * The following properties are merged:
   * - NameHistory (concatenation)
   * - Pre-/Postset (union)
   *
   * \pre   the transitions must reside in the same PetriNet instance
   */
  void Transition::merge(Transition & trans, bool addArcWeights)
  {
    // Node::merge does all the work
    Node::merge(trans, addArcWeights);
  }


  /*!
   */
  void Transition::updateType()
  {
    Type newType = INTERNAL;
    set<Node *> neighbors = util::setUnion(getPreset(), getPostset());

    for (set<Node *>::iterator it = neighbors.begin(); it != neighbors.end();
	 ++it)
      {
	Type itType = (*it)->getType();
	if (itType == INPUT || itType == OUTPUT)
	  {
	    if ((*it)->isComplementType(newType))
	      {
		newType = INOUT;
		break;
	      }
	    else
	      newType = itType;
	  }
      }

    setType(newType);
  }


  /*!
   */
  bool Transition::isSynchronized() const
  {
    return !labels_.empty();
  }


  /*!
   */
  const set<string> & Transition::getSynchronizeLabels() const
  {
    return labels_;
  }
  
  /*!
   */
  void Transition::setSynchronizeLabels(const std::set<std::string> & labels)
  {
    labels_ = labels;
    observer_.updateTransitionLabels(*this);
  }



  /****************************************************************************
   *** Class Arc Function Definitions
   ***************************************************************************/


  /*!
   */
  Arc::Arc(PetriNet & net, ComponentObserver & observer, Node & source,
	   Node & target, unsigned int weight) :
    net_(net), observer_(observer), source_(&source), target_(&target),
    weight_(weight)
  {
    assert(&observer.getPetriNet() == &net);

    observer_.updateArcCreated(*this);
  }


  /*!
   */
  Arc::Arc(PetriNet & net, ComponentObserver & observer, const Arc & arc) :
    net_(net), observer_(observer),
    source_(net.findNode(arc.source_->getName())),
    target_(net.findNode(arc.target_->getName())), weight_(arc.weight_)
  {
    assert(&observer.getPetriNet() == &net);
    assert(net.findNode(arc.source_->getName()) != NULL);
    assert(net.findNode(arc.target_->getName()) != NULL);

    observer_.updateArcCreated(*this);
  }


  /*!
   */
  Arc::Arc(PetriNet & net, ComponentObserver & observer, const Arc & arc,
	   Node & source, Node & target) :
    net_(net), observer_(observer),
    source_(&source),
    target_(&target), weight_(arc.weight_)
  {
    assert(&observer.getPetriNet() == &net);
    assert(net.containsNode(source));
    assert(net.containsNode(target));

    observer_.updateArcCreated(*this);
  }


  /*!
   * You must not destroy an Arc directly.
   */
  Arc::~Arc()
  {
    assert(net_.findArc(*source_, *target_) == NULL);
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
    return *source_;
  }


  /*!
   */
  Node & Arc::getTargetNode() const
  {
    return *target_;
  }


  /*!
   */
  Transition & Arc::getTransition() const
  {
    Transition * t = dynamic_cast<Transition *>(source_);
    if (t != NULL)
      return *t;

    t = dynamic_cast<Transition *>(target_);
    assert(t != NULL);
    return *t;
  }


  /*!
   */
  Place & Arc::getPlace() const
  {
    Place * t = dynamic_cast<Place *>(source_);
    if (t != NULL)
      return *t;

    t = dynamic_cast<Place *>(target_);
    assert(t != NULL);
    return *t;
  }


  /*!
   */
  unsigned int Arc::getWeight() const
  {
    return weight_;
  }


  /*!
   */
  void Arc::merge(Arc & arc)
  {
    weight_ += arc.weight_;
  }


  /*!
   */
  void Arc::mirror()
  {
    observer_.updateArcMirror(*this);
    observer_.updateArcRemoved(*this);
    Node * oldSource = source_;
    source_ = target_;
    target_ = oldSource;
    observer_.updateArcCreated(*this);
  }

}

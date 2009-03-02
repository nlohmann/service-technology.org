#include <cassert>

#include "component.h"
#include "petrinet.h"
#include "link.h"

using std::string;
using std::set;
using std::vector;

namespace pnapi
{

  /*!
   * \brief   constructs a new node
   *
   *          A new LinkNode for the interface place #place with 
   *          distribution mode #mode is created.
   */
  LinkNode::LinkNode(Place & place, Mode mode) : 
    place_(&place),
    type_(place.getType() == Place::OUTPUT ? SOURCE : TARGET), 
    mode_(mode)
  {
    assert(place.getType() == Place::OUTPUT || place.getType() == Place::INPUT);
  }

  
  /*!
   * \brief   destructor
   */
  LinkNode::~LinkNode()
  {
  }

  
  /*!
   * \brief   returns the partner in case of one-to-one links
   */
  LinkNode & LinkNode::getPartner()
  {
    assert(links_.size() == 1);

    return **links_.begin();
  }


  /*!
   * \brief   joins the places of two partner links
   */
  void LinkNode::joinPlaces()
  {
    LinkNode & partner = getPartner();
    assert(&partner.getPartner() == this);
    PetriNet & net = place_->getPetriNet();
    assert(&net == &partner.place_->getPetriNet());

    // FIXME: TODO
    const_cast<Place &>(*partner.place_).merge(const_cast<Place &>(*place_));
  }


  /*!
   * \brief   replaces links with a set of new nodes
   *
   *          Expands the interface of the underlying petri net to represent
   *          the links of this node (by adding a new interface place for each
   *          link). The new interface places are encapsulated in LinkNodes and
   *          returned as a set. After calling #expand() for a node the node
   *          should no longer be used.
   */
  vector<LinkNode *> & LinkNode::expand()
  {
    assert(!links_.empty());

    // construct a new pattern net with a "root" place (to be connected)
    PetriNet pattern;
    /*
    Place & place = pattern.createPlace(place_->getName(), 
				type_ == SOURCE ? Place::INPUT : Place::OUTPUT);

    // broadcast (ALL) patterns have only one transition
    Transition * trans;
    if (mode_ == ALL)
      trans = &connectTransition(place);

    // create numbered place (and transition in ANY mode) for each link
    int nLinks = links_.size();
    for (int i = 0; i < nLinks; i++)
    {
      if (mode_ == ANY)
	trans = &connectTransition(place);
      connectPlace(*trans);
    }

    // compose the pattern net into the node net (joins on root place by name)
    place_->getPetriNet() += pattern;
    */

    // create nodes for new interface and reconnect old links
    return createNodes(pattern);
  }


  /*!
   * \brief   adds a new link between this node and the one specified
   */
  void LinkNode::addLink(LinkNode & node)
  {
    assert((place_->getType() == Place::INPUT && 
	    node.place_->getType() == Place::OUTPUT) || 
	   (place_->getType() == Place::OUTPUT && 
	    node.place_->getType() == Place::INPUT));
    //assert(&place_->getPetriNet() == &node.place_->getPetriNet());

    links_.insert(&node);
    node.links_.insert(this);
  }


  /*!
   */
  LinkNode & LinkNode::replacePlace(const Place & p)
  {
    place_ = &p;
    return *this;
  }


  /*!
   * \brief   replaces one specific link with a link to another node
   */
  void LinkNode::replaceLink(LinkNode & orig, LinkNode & repl)
  {
    int n = links_.erase(&orig);
    assert(n == 1);

    n = orig.links_.erase(this);
    assert(n == 1);

    addLink(repl);
  }


  Transition & LinkNode::connectTransition(Place & place)
  {
    PetriNet & net = place.getPetriNet();
    Transition & trans = 
      net.createTransition(getUniqueNodeName(net, place_->getPetriNet(),
					     place_->getName()));

    if (type_ == SOURCE)
      net.createArc(place, trans);
    else
      net.createArc(trans, place);

    return trans;
  }


  Place & LinkNode::connectPlace(Transition & trans)
  {
    Place * place;
    PetriNet & net = trans.getPetriNet();
    string name = getUniqueNodeName(net, place_->getPetriNet(), 
				    place_->getName());

    if (type_ == SOURCE)
    {
      place = &net.createPlace(name, Place::OUTPUT);
      net.createArc(trans, *place);
    }
    else
    {
      place = &net.createPlace(name, Place::INPUT);
      net.createArc(*place, trans);
    }

    return *place;
  }


  string LinkNode::getUniqueNodeName(const PetriNet & net1, 
				     const PetriNet & net2,
				     const string & base)
  {
    //FIXME: string name = net1.getUniqueNodeName(base);
    //FIXME: return net2.getUniqueNodeName(name);
    return "";
  }


  vector<LinkNode *> & LinkNode::createNodes(const PetriNet & pattern)
  {
    Place * place;
    LinkNode * node;
    vector<LinkNode *> * nodes = new vector<LinkNode *>();
    nodes->reserve(links_.size());
    
    // prepare access to old pattern interface (just for naming information)
    set<Place *> patternInterface =
      type_ == SOURCE ? pattern.getOutputPlaces() : pattern.getInputPlaces();
    set<Place *>::iterator patternIter = patternInterface.begin();

    assert(patternInterface.size() == links_.size());

    // for each link of this node create a new one and reconnect
    while (!links_.empty())
    {
      place = place_->getPetriNet().findPlace((*patternIter)->getName());
      node = new LinkNode(*place);
      (*links_.begin())->replaceLink(*this, *node);
      nodes->push_back(node);
      ++patternIter;
    }

    return *nodes;
  }

  
  LinkNode::LinkNode(const LinkNode & n) :
    place_(n.place_),
    type_(n.type_),
    mode_(n.mode_)
  {
    assert(false);
  }

  LinkNode & LinkNode::operator=(const LinkNode & n)
  {
    assert(false);
  }

}

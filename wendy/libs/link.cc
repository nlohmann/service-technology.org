#include "config.h"
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
  LinkNode::LinkNode(Place & place, Mode mode, bool internalizePlace) : 
    place_(&place),
    type_(place.getType() == Place::OUTPUT ? SOURCE : TARGET), 
    mode_(mode),
    internalizePlace_(internalizePlace)
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
    PetriNet & net = place_->getPetriNet();
    LinkNode & partner = getPartner();
    assert(&partner.getPartner() == this);
    assert(&net == &partner.place_->getPetriNet());

    partner.place_->merge(*place_, true, partner.internalizePlace_);
    net.finalCondition().addProposition(*partner.place_ == 0);
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
  vector<LinkNode *> LinkNode::expand()
  {
    assert(!links_.empty());

    // construct a new pattern net with a "root" place (to be connected)
    PetriNet & net = place_->getPetriNet();
    Place & place = place_->merge(net.createPlace(), true, internalizePlace_);
    

    // broadcast (ALL) patterns have only one transition
    Transition * trans = NULL;
    if (mode_ == ALL)
      trans = &connectTransition(place);

    // create numbered place (and transition in ANY mode) for each link
    vector<Place *> places;
    int nLinks = links_.size();
    for (int i = 0; i < nLinks; i++)
    {
      if (mode_ == ANY)
	trans = &connectTransition(place);
      places.push_back(&connectPlace(*trans));
    }

    // create nodes for new interface and reconnect old links
    return createNodes(places);
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
  LinkNode & LinkNode::replacePlace(Place & p)
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
    Transition & trans = net.createTransition();

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

    if (type_ == SOURCE)
    {
      place = &net.createPlace("", Place::OUTPUT);
      net.createArc(trans, *place);
    }
    else
    {
      place = &net.createPlace("", Place::INPUT);
      net.createArc(*place, trans);
    }

    return *place;
  }


  vector<LinkNode *> LinkNode::createNodes(vector<Place *> & places)
  {
    assert(places.size() == links_.size());

    Place * place;
    LinkNode * node;
    vector<LinkNode *> nodes;
    nodes.reserve(links_.size());
    vector<Place *>::iterator placeIter = places.begin();
    
    // for each link of this node create a new one and reconnect
    while (!links_.empty())
    {
      place = *placeIter;
      node = new LinkNode(*place);
      (*links_.begin())->replaceLink(*this, *node);
      nodes.push_back(node);
      ++placeIter;
    }

    return nodes;
  }

}

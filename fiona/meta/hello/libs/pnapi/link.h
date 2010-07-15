// -*- C++ -*-

#ifndef PNAPI_LINK_H
#define PNAPI_LINK_H

#include <string>
#include <set>
#include <vector>

namespace pnapi
{

  // forward declarations
  class Place;
  class Transition;
  class PetriNet;


  class LinkNode
  {
  public:

    /*** nested datatype definitions ***/

    /// node types
    enum Type { SOURCE, TARGET };

    /// node modes (how to distribute messages)
    enum Mode { ANY, ALL, ONE };


    /*** constructors & destructor ***/

    /// single constructor
    LinkNode(Place &, Mode = ONE, bool = true);

    /// destructor
    ~LinkNode();

    
    /*** query (const) member functions ***/

    /// get partner in case of one-to-one links
    LinkNode & getPartner();


    /*** non-query member functions ***/

    /// add a link to another node
    void addLink(LinkNode &);

    /// replace the internal place reference
    LinkNode & replacePlace(Place &);

    /// replace links by a set of new nodes
    std::vector<LinkNode *> expand();

    /// joins places in case of one-to-one links
    void joinPlaces();


  private:
    
    /*** private attributes ***/

    /// associated place
    Place * place_;

    /// type of this node
    const Type type_;

    /// mode of this node
    const Mode mode_;

    /// if the place may be internalized
    const bool internalizePlace_;

    /// links to other nodes
    std::set<LinkNode *> links_;

    
    /*** private constructors and operators ***/

    // no copying
    LinkNode(const LinkNode &);

    // no copying
    LinkNode & operator=(const LinkNode &);


    /*** private member functions ***/

    /// redirect link from one node to another
    void replaceLink(LinkNode &, LinkNode &);

    /// connects a transition to place (during expansion)
    Transition & connectTransition(Place &);

    /// connects a place to transition (during expansion)
    Place & connectPlace(Transition &);

    /// creates the vector of new nodes (during expansion)
    std::vector<LinkNode *> createNodes(std::vector<Place *> &);
  };

}

#endif

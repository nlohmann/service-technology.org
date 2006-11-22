/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or(at your     *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\*****************************************************************************/

/*!
 * \file    petrinet.h
 *
 * \brief   functions for Petri nets
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2006/11/22 15:29:51 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.105 $
 *
 * \ingroup petrinet
 */





#ifndef PETRINET_H
#define PETRINET_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief place and transition types
 *
 * Enumeration of the possible types of a place or transition, i.e. whether
 * a place or transition is internal, receives a message, sends a message or
 * both.
 *
 * \ingroup petrinet
 */
typedef enum
{
  INTERNAL,			///< non-communicating (standard)
  IN,				///< input of an open workflow net (oWFN)
  OUT,				///< output of an open workflow net (oWFN)
  INOUT				///< input and output of an open workflow net (oWFN)
} communication_type;





/*!
 * \brief arc types
 *
 * Enumeration of the possible types of an arc, i.e. whether it is a standard
 * arc or a read arc. The latter will be "unfolded" to a loop.
 *
 * \ingroup petrinet
 */
typedef enum
{
  STANDARD,			///< standard arc
  READ				///< read arc (will be unfolded to a loop)
} arc_type;





/*!
 * \brief node types
 *
 * Enumeration of the possible types of a node, i.e. whether it is a place or
 * a transition.
 *
 * \ingroup petrinet
 */
typedef enum
{
  PLACE,			///< a place
  TRANSITION			///< a transition
} node_type;





/******************************************************************************
 * Classes
 *****************************************************************************/

/*!
 * \class Node
 *
 * \brief unspecified Petri net nodes
 *
 * Class to represent nodes (i.e. places and transitions) of Petri nets. Each
 * node has an id and a history (i.e. the list of roles the node had during
 * the processing of a BPEL-file).
 *
 * \ingroup petrinet
*/

class Node
{
  public:
    /// the id of the node
    unsigned int id;

    /// the name of the type
    virtual string nodeTypeName();

    /// the short name of the node
    virtual string nodeShortName();

    /// the type of the node
    node_type nodeType;

    /// the set of roles (i.e. the history) of the node
    vector<string> history;

    /// true if first role contains role
    bool firstMemberAs(string role);

    /// true if first role begins with role
    bool firstMemberIs(string role);
    
    /// true if history contains role
    bool historyContains(string role);

    /// type of node as defined in #communication_type
    communication_type type;

    /// the name of the node
    string nodeName();

    /// an additional prefix for the name in order to distinguish nodes of different nets
    string prefix;

    /// destructor
    virtual ~Node();
};





/*****************************************************************************/


/*!
 * \class Transition
 *
 * \brief transitions of the Petri net
 *
 * Class to represent transitions of Petri nets. Each transition inherits the
 * functions and variables from class #Node.
 *
 * \ingroup petrinet
*/

class Transition: public Node
{
  public:
    /// constructor which creates a transition and adds a first role to the history
    Transition(unsigned int id, string role);

    /// DOT-output of the transition (used by PetriNet::dotOut())
    string dotOut();

    /// the name of the type
    string nodeTypeName();
    
    /// the short name of the transition
    string nodeShortName();
};





/*****************************************************************************/


/*!
 * \class  Place
 *
 * \brief Places of the Petri net
 *
 * Class to represent places of Petri nets. In addition to the inherited
 * functions and variables from class #Node, each place has a type defined in
 * the enumeration #communication_type and an initial marking. As the
 * generated net and its inital marking is 1-safe, it is sufficent to
 * represent the initial marking as a Boolean value.
 *
 * \ingroup petrinet
*/

class Place: public Node
{
  public:
    /// constructor which creates a place and adds a first role to the history
    Place(unsigned int id, string role, communication_type type);

    /// DOT-output of the place (used by PetriNet::dotOut())
    string dotOut();

    /// the name of the type
    string nodeTypeName();

    /// the short name of the place
    string nodeShortName();

    /// mark the place
    void mark(unsigned int tokens = 1);

    /// initial marking of the place
    unsigned int tokens;
};





/*****************************************************************************/


/*!
 * \class Arc
 *
 * \brief arcs of the Petri net
 *
 * Class to represent arcs of Petri nets. An arc written as a tupel (n1,n2)
 * has n1 as #source and n2 as #target.
 *
 * \ingroup petrinet
*/

class Arc
{
  public:
    /// source node of the arc
    Node *source;
  
    /// target node of the arc
    Node *target;

    /// constructor to create an arc
    Arc(Node *source, Node *target, unsigned int weight = 1);
  
    /// DOT-output of the arc (used by PetriNet::dotOut())
    string dotOut();

    /// weight of the arc (experimental)
    unsigned int weight;
};





/*****************************************************************************/


/*!
 * \class PetriNet
 *
 * \brief A Petri net
 *
 * Class to represent Petri nets. The net is consists of places of class
 * #Place, transitions of class #Transition and arcs of class #Arc. The sets
 * are saved in three lists #P, #T and #F.
 *
 * \ingroup petrinet
 */


class PetriNet
{
  public:
    /// adds a place with a given role and type
    Place* newPlace(string role, communication_type type = INTERNAL);
  
    /// adds a transition with a given role
    Transition*newTransition(string role);
  
    /// adds an arc given source and target node, and arc type
    Arc *newArc(Node *source, Node *target, arc_type type = STANDARD, unsigned int weight = 1);

    /// information about the net including histories of all nodes
    void printInformation();


    /// DOT (Graphviz) output
    void dotOut();

    /// PNML (Petri Net Markup Language) output
    void pnmlOut();

    /// low-level PEP output
    void pepOut();

    /// APNN (Abstract Petri Net Notation) output
    void apnnOut();

    /// LoLA-output
    void lolaOut();

    /// oWFN-output
    void owfnOut();


    /// merges places given two places
    void mergePlaces(Place *p1, Place *p2);

    /// merges places given two roles
    void mergePlaces(string role1, string role2);

    /// merges places given two identifiers and roles
    void mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2);
    
    /// merges transitions given two transitions
    void mergeTransitions(Transition *t1, Transition *t2);


    /// finds place given a role
    Place* findPlace(string role);

    /// finds place given an id with a role
    Place* findPlace(unsigned int id, string role);

    /// finds a place given the ids of two transitions (one in the pre and one in the postset)
    Place *findPlace(unsigned int id1, unsigned int id2);

    /// rename a node (i.e., rename one role in its history)
    void renamePlace(string old_name, string new_name);


    /// finds transition given a role
    Transition* findTransition(string role);

    /// calculates the preset of a node
    set<Node*> preset(Node *n);

    /// calculates the postset of a node
    set<Node*> postset(Node *n);

    /// simplifies the Petri net
    void simplify();

    /// removes all variable places
    void removeVariables();

    /// adds a prefix to the name of all nodes of the net
    void addPrefix(string prefix);

    /// connects a second oWFN
    void connectNet(PetriNet *net);

    /// moves channel places to the list of internal places
    void makeChannelsInternal();

    /// re-enumerates the nodes
    void reenumerate();

    /// statistical output
    string information();
    
    /// constructor
    PetriNet();


  private:
    /// adds a place without an initial role
    Place* newPlace();

    /// removes a place from the net
    void removePlace(Place *p);

    /// adds a transition without an initial role
    Transition* newTransition();

    /// returns the arc weight between two nodes
    unsigned int arc_weight(Node *source_nodes, Node *target_node);

    /// removes a transition from the net
    void removeTransition(Transition *t);

    /// removes an arc from the net
    void removeArc(Arc *f);

    /// removes all ingoing and outgoing arcs of a node
    void detachNode(Node *n);


    /// remove unused status places
    void removeUnusedStatusPlaces();

    /// remove transitions with empty pre or postset
    void removeSuspiciousTransitions();

    /// remove dead nodes of the Petri net
    void removeDeadNodes();

    /// elimination of identical places (RB1)
    void elminiationOfIdenticalPlaces();
    
    /// elimination of identical transitions (RB2)
    void elminiationOfIdenticalTransitions();

    /// fusion of series places (RA1)
    void fusionOfSeriesPlaces();

    /// fusion of series transitions (RA2)
    void fusionOfSeriesTransitions();

    /// remove transitive nodes and arcs
    void transitiveReduction();

    
    /// removes interface places (for non-oWFN formats)
    void removeInterface();
  
    /// returns an id for new nodes
    unsigned int getId();

    /// returns current id
    unsigned int id();


    /// set of places of the Petri net
    set<Place *> P;

    /// set of input places of the oWFN
    set<Place *> P_in;

    /// set of output places of the oWFN
    set<Place *> P_out;

    /// set of transitions of the Petri net
    set<Transition *> T;

    /// set of arcs of the Petri net
    set<Arc *> F;


    /// id that will be assigned to the next node
    unsigned int nextId;

    /// true if function #PetriNet::removeInterface() was called
    bool hasNoInterface;

    /// mapping of roles to nodes of the Petri net
    map<string, Node *> roleMap;
};





#endif



/**
 * \defgroup petrinet Petri Net Base Module
 *
 * \author   responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *
 * All functions needed to organize a Petri net representation that can be
 * written to several output file formats and that supports structural
 * reduction rules.
 */

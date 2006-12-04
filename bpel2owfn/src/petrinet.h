/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    petrinet.h
 *
 * \brief   Petri Net API
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2006/12/04 14:32:17 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.114 $
 *
 * \ingroup petrinet
 */





#ifndef PETRINET_H
#define PETRINET_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <map>





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
  INTERNAL,		///< non-communicating (standard)
  IN,			///< input of an open workflow net (oWFN)
  OUT,			///< output of an open workflow net (oWFN)
  INOUT			///< input and output of an open workflow net (oWFN)
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
  STANDARD,		///< standard arc
  READ			///< read arc (will be unfolded to a loop)
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
  PLACE,		///< a place
  TRANSITION		///< a transition
} node_type;





/*!
 * \brief file formats
 *
 * Enumeration of the possible output file formats.
 *
 * \ingroup petrinet
 */
typedef enum
{
  FORMAT_APNN,		///< Abstract Petri Net Notation (APNN)
  FORMAT_DOT,		///< Graphviz dot
  FORMAT_INFO,		///< Info File
  FORMAT_LOLA,		///< LoLA
  FORMAT_OWFN,		///< Fiona open workflow net (oWFN)
  FORMAT_PEP,		///< Low-Level PEP Notation
  FORMAT_PNML		///< Petri Net Markup Language (PNML)
} output_format;





/******************************************************************************
 * Classes
 *****************************************************************************/

/*!
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
  /// class PetriNet is allowed to access the privates of class Node
  friend class PetriNet;

  /// class Place is allowed to access the privates of class Node
  friend class Place;

  /// class Transition is allowed to access the privates of class Node
  friend class Transition;

  /// class Arc is allowed to access the privates of class Node
  friend class Arc;

  public:
  private:
    /// type of node as defined in #communication_type
    communication_type type;

    /// the id of the node
    unsigned int id;

    /// the type of the node
    node_type nodeType;

    /// an additional prefix for the name in order to distinguish nodes of different nets
    std::string prefix;

    /// true if first role contains role
    bool firstMemberAs(std::string role) const;

    /// true if first role begins with role
    bool firstMemberIs(std::string role) const;

    /// the set of roles (i.e. the history) of the node
    std::vector<std::string> history;

    /// true if history contains role
    bool historyContains(std::string role) const;

    /// the name of the node
    std::string nodeName() const;

    /// the short name of the node
    virtual std::string nodeShortName() const;

    /// the name of the type
    virtual std::string nodeTypeName() const;
};





/*****************************************************************************/


/*!
 * \brief transitions of the Petri net
 *
 * Class to represent transitions of Petri nets. Each transition inherits the
 * functions and variables from class #Node.
 *
 * \ingroup petrinet
*/

class Transition: public Node
{
  /// class PetriNet is allowed to access the privates of class Transition
  friend class PetriNet;

  private:
    /// DOT-output of the transition (used by PetriNet::dotOut())
    std::string output_dot() const;

    /// the short name of the transition
    std::string nodeShortName() const;

    /// the name of the type
    std::string nodeTypeName() const;
    
    /// constructor which creates a transition and adds a first role to the history
    Transition(unsigned int id, std::string role);
};





/*****************************************************************************/


/*!
 * \brief places of the Petri net
 *
 * Class to represent places of Petri nets. In addition to the inherited
 * functions and variables from class #Node, each place has a type defined in
 * the enumeration #communication_type and an initial marking.
 *
 * \todo make nodeShortName() private
 *
 * \ingroup petrinet
*/

class Place: public Node
{
  /// class PetriNet is allowed to access the privates of class Place
  friend class PetriNet;
 
  private:
    /// initial marking of the place
    unsigned int tokens;

    /// DOT-output of the place (used by PetriNet::dotOut())
    std::string output_dot() const;

    /// the name of the type
    std::string nodeTypeName() const;

    /// constructor which creates a place and adds a first role to the history
    Place(unsigned int id, std::string role, communication_type type);

  public:
    /// mark the place
    void mark(unsigned int tokens = 1);    

    /// the short name of the place (public to bpel2owfn.cc)
    std::string nodeShortName() const;
};





/*****************************************************************************/


/*!
 * \brief arcs of the Petri net
 *
 * Class to represent arcs of Petri nets. An arc written as a tupel (n1,n2)
 * has n1 as #source and n2 as #target.
 *
 * \ingroup petrinet
*/

class Arc
{
  /// class PetriNet is allowed to access the privates of class Arc
  friend class PetriNet;

  private:
    /// source node of the arc
    Node *source;
  
    /// target node of the arc
    Node *target;

    /// weight of the arc (experimental)
    unsigned int weight;
 
    /// DOT-output of the arc (used by PetriNet::dotOut())
    std::string output_dot() const;

    /// constructor to create an arc
    Arc(Node *source, Node *target, unsigned int weight = 1);
};





/*****************************************************************************/


/*!
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
    Place* newPlace(std::string my_role, communication_type my_type = INTERNAL);
  
    /// adds a transition with a given role
    Transition *newTransition(std::string my_role);
  
    /// adds an arc given source and target node, and arc type
    Arc *newArc(Node *my_source, Node *my_target, arc_type my_type = STANDARD, unsigned int my_weight = 1);


    /// outputs the Petri net
    friend std::ostream& operator<< (std::ostream& os, const PetriNet &obj);
    

    /// merges places given two places
    void mergePlaces(Place *p1, Place *p2);

    /// merges places given two roles
    void mergePlaces(std::string role1, std::string role2);

    /// merges places given two identifiers and roles
    void mergePlaces(unsigned int id1, std::string role1, unsigned int id2, std::string role2);
    
    /// merges transitions given two transitions
    void mergeTransitions(Transition *t1, Transition *t2);

    /// finds place given a role
    Place* findPlace(std::string role);

    /// finds place given an id with a role
    Place* findPlace(unsigned int id, std::string role);

    /// finds transition given a role
    Transition* findTransition(std::string role);


    /// rename a node (i.e., rename one role in its history)
    void renamePlace(std::string old_name, std::string new_name);


    /// simplifies the Petri net
    void simplify();

    /// adds a prefix to the name of all nodes of the net
    void addPrefix(std::string prefix);

    /// connects a second oWFN
    void connectNet(PetriNet *net);

    /// moves channel places to the list of internal places
    void makeChannelsInternal();

    /// re-enumerates the nodes
    void reenumerate();

    /// statistical output
    std::string information() const;

    /// set the output format
    void set_format(output_format my_format);
    
    /// constructor
    PetriNet();

    /// destructor
    ~PetriNet();


  private:
    /// removes a place from the net
    void removePlace(Place *p);

    /// removes a transition from the net
    void removeTransition(Transition *t);

    /// removes an arc from the net
    void removeArc(Arc *f);

    /// removes all ingoing and outgoing arcs of a node
    void detachNode(Node *n);

    /// finds a place given the ids of two transitions (one in the pre and one in the postset)
    Place *findPlace(unsigned int id1, unsigned int id2);  

    /// returns the arc weight between two nodes
    unsigned int arc_weight(Node *my_source, Node *my_target) const;


    /// APNN (Abstract Petri Net Notation) output
    void output_apnn(std::ostream *output) const;

    /// DOT (Graphviz) output
    void output_dot(std::ostream *output) const;

    /// info file output
    void output_info(std::ostream *output) const;

    /// LoLA-output
    void output_lola(std::ostream *output) const;

    /// oWFN-output
    void output_owfn(std::ostream *output) const;

    /// low-level PEP output
    void output_pep(std::ostream *output) const;
    
    /// PNML (Petri Net Markup Language) output
    void output_pnml(std::ostream *output) const;


    /// calculates the prestd::set of a node
    std::set<Node*> preset(Node *n) const;

    /// calculates the poststd::set of a node
    std::set<Node*> postset(Node *n) const;
    

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


    /// returns an id for new nodes
    unsigned int getId();


    /// set of internal places of the Petri net
    std::set<Place *> P;

    /// set of input places of the Petri net
    std::set<Place *> P_in;

    /// set of output places of the Petri net
    std::set<Place *> P_out;

    /// set of transitions of the Petri net
    std::set<Transition *> T;

    /// set of arcs of the Petri net
    std::set<Arc *> F;


    /// id that will be assigned to the next node
    unsigned int nextId;

    /// output file format
    output_format format;

    /// mapping of roles to nodes of the Petri net
    std::map<std::string, Node *> roleMap;
};





#endif



/*!
 * \defgroup petrinet Petri Net API
 *
 * \author Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *
 * All functions needed to organize a Petri net representation that can be
 * written to several output file formats and that supports structural
 * reduction rules.
 */

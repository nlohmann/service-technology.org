/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    petrinet.h
 *
 * \brief   Petri Net API
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/07/27 14:24:20 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.156 $
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
#include <list>
#include <deque>

using std::string;
using std::vector;
using std::deque;
using std::set;
using std::map;
using std::list;
using std::pair;
using std::ostream;





namespace PNapi
{

    
/******************************************************************************
 * Data structure
 *****************************************************************************/
    
/*!
 * \brief   place and transition types
 *
 *          Enumeration of the possible types of a place or transition, i.e.
 *          whether a place or transition is internal, receives a message,
 *          sends a message or both.
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
 * \brief   arc types
 *
 *          Enumeration of the possible types of an arc, i.e. whether it is a
 *          standard arc or a read arc. The latter will be "unfolded" to a
 *          loop.
 *
 * \ingroup petrinet
 */
typedef enum
{
  STANDARD,		///< standard arc
  READ			///< read arc (will be unfolded to a loop)
} arc_type;





/*!
* \brief   node types
 *
 *          Enumeration of the possible types of a node, i.e. whether it is a
 *          place or a transition.
 *
 * \ingroup petrinet
 */
typedef enum
{
  PLACE,		///< a place
  TRANSITION		///< a transition
} node_type;





/*!
* \brief   file formats
 *
 *          Enumeration of the possible output file formats.
 *
 * \ingroup petrinet
 */
typedef enum
{
  FORMAT_APNN,		///< Abstract Petri Net Notation (APNN)
  FORMAT_DOT,		///< Graphviz dot
  FORMAT_INA,		///< INA
  FORMAT_SPIN,		///< INA
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
 * \brief   unspecified Petri net nodes
 *
 *          Class to represent nodes (i.e. places and transitions) of Petri
 *          nets. Each node has an id and a history (i.e. the list of roles the
 *          node had during the processing of a BPEL-file).
 *
 * \ingroup petrinet
*/

class Node
{
  /// class PetriNet is allowed to access the privates of class Node
  friend class PetriNet;

  /// class Arc is allowed to access the privates of class Node
  friend class Arc;

  public:
    /// the name of the node
    string nodeFullName() const;

    /// type of node as defined in #communication_type
    communication_type type;
    
    /// destructor
    virtual ~Node();

  protected:
    /// the set of roles (i.e. the history) of the node
    list<string> history;

    /// the id of the node
    unsigned int id;

    /// the type of the node
    node_type nodeType;

    /// an additional prefix for the name in order to distinguish nodes of different nets
    string prefix;

    /// the preset of this node
    set<Node*> preset;

    /// the postset of this node
    set<Node*> postset;

    /// the maximal occurrences of this (communication) place or transition
    unsigned int max_occurrences;


    /// true if first role contains role
    bool firstMemberAs(string role) const;

    /// true if first role begins with role
    bool firstMemberIs(string role) const;

    /// true if history contains role
    bool historyContains(string role) const;

    /// the name of the node
    string nodeName() const;

    /// the short name of the node
    virtual string nodeShortName() const;

    /// the name of the type
    virtual string nodeTypeName() const;
};





/*****************************************************************************/


/*!
 * \brief   transitions of the Petri net
 *
 *          Class to represent transitions of Petri nets. Each transition
 *          inherits the functions and variables from class #Node.
 *
 * \ingroup petrinet
*/

class Transition: public Node
{
  /// class PetriNet is allowed to access the privates of class Transition
  friend class PetriNet;

  private:
    /// DOT-output of the transition (used by PetriNet::dotOut())
    string output_dot() const;

    /// the short name of the transition
    string nodeShortName() const;

    /// the name of the type
    string nodeTypeName() const;

    /// label (used for constraint oWFN)
    set<string> labels;

    /// create a transition and add a first role to the history
    Transition(unsigned int id, string role);

  public:
    /// destructor
    virtual ~Transition();

    /// add a label to the transition (used for constraint oWFN)
    void add_label(string new_label);
};





/*****************************************************************************/


/*!
 * \brief   places of the Petri net
 *
 *          Class to represent places of Petri nets. In addition to the
 *          inherited functions and variables from class #Node, each place has
 *          a type defined in the enumeration #communication_type and an
 *          initial marking.
 *
 * \ingroup petrinet
 *
 * \todo
 *       - Make nodeShortName() private.
 *       - #isFinal should allow more than one token.
*/

class Place: public Node
{
  /// class PetriNet is allowed to access the privates of class Place
  friend class PetriNet;

  private:
    /// initial marking of the place
    unsigned int tokens;

    /// not empty if place was once an communication place and is now internal because of choreographie
    string wasExternal;

    /// DOT-output of the place (used by PetriNet::dotOut())
    string output_dot() const;

    /// the name of the type
    string nodeTypeName() const;

    /// create a place and add a first role to the history
    Place(unsigned int id, string role, communication_type type);

  public:
    /// destructor
    virtual ~Place();

    /// mark the place
    void mark(unsigned int tokens = 1);

    /// the short name of the place
    string nodeShortName() const;

    /// true if place is marked in the final marking
    bool isFinal;
};





/*****************************************************************************/


/*!
 * \brief   arcs of the Petri net
 *
 *          Class to represent arcs of Petri nets. An arc written as a tupel
 *          (n1,n2) has n1 as #source and n2 as #target.
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

    /// swaps source and target node of the arc
    void mirror();

    /// weight of the arc (experimental)
    unsigned int weight;

    /// DOT-output of the arc (used by PetriNet::dotOut())
    string output_dot(bool draw_interface = true) const;

    /// create an arc with a given weight
    Arc(Node *source, Node *target, unsigned int weight = 1);

  public:
    /// destructor
    virtual ~Arc();
};





/*****************************************************************************/


/*!
 * \brief   A Petri net
 *
 *          Class to represent Petri nets. The net is consists of places of
 *          class #Place, transitions of class #Transition and arcs of class
 *          #Arc. The sets are saved in three lists #P, #T and #F.
 *
 * \ingroup petrinet
 */


class PetriNet
{
  public:
    /// adds a place with a given role and type
    Place* newPlace(string my_role, communication_type my_type = INTERNAL, string my_port = "");

    /// adds a transition with a given role
    Transition *newTransition(string my_role);

    /// adds an arc given source and target node, and arc type
    Arc *newArc(Node *my_source, Node *my_target, arc_type my_type = STANDARD, unsigned int my_weight = 1);


    /// merges two places
    void mergePlaces(Place * & p1, Place * & p2);

    /// merges two places
    void mergePlaces(string role1, Place * & p2);

    /// merges two places
    void mergePlaces(Place * & p1, string role2);

    /// merges two places given two roles
    void mergePlaces(string role1, string role2);

    /// merges two places given two identifiers and roles
    void mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2);

    /// merges two transitions
    void mergeTransitions(Transition *t1, Transition *t2);

    /// merges two parallel transitions
    void mergeParallelTransitions(Transition *t1, Transition *t2);

    /// finds place given a role
    Place* findPlace(string role) const;

    /// finds place given an id with a role
    Place* findPlace(unsigned int id, string role) const;

    /// finds transition given a role
    Transition* findTransition(string role) const;

    /// returns a set of all final places
    set< Place * > getFinalPlaces() const;

    /// rename a node (i.e., rename one role in its history)
    void renamePlace(string old_name, string new_name);

    /// removes a transition from the net
    void removeTransition(Transition *t);    

    /// applies structral reduction rules
    unsigned int reduce(unsigned int reduction_level = 5);

    /// swaps input and output places
    void mirror();

    /// adds a prefix to the name of all nodes of the net
    void addPrefix(string prefix);

    /// adds a suffix to the name of all interface places of the net
    void add_interface_suffix(string suffix);

    /// composes a second Petri net
    void compose(const PetriNet &net);

    /// produces a second constraint oWFN
    void produce(const PetriNet &net);

    /// moves channel places to the list of internal places
    void makeChannelsInternal();

    /// re-enumerates the nodes
    void reenumerate();

    /// add a loop to the final states to check deadlock freedom with LoLA
    void loop_final_state();
    
    /// statistical output
    string information() const;

    /// set the output format
    void set_format(output_format my_format, bool standard = true);

    /// calculate the maximal occurrences of communication
    void calculate_max_occurrences();


    /// add a suffix for a forEach activity
    unsigned int push_forEach_suffix(string suffix);

    /// remove the last added suffix
    unsigned int pop_forEach_suffix();


    /// calculates the preset of a node
    set<Node*> preset(Node *n) const;

    /// calculates the postset of a node
    set<Node*> postset(Node *n) const;


    /// outputs the Petri net
    friend ostream& operator<< (ostream& os, const PetriNet &obj);

    /// constructor
    PetriNet();

    /// copy constructor
    PetriNet(const PetriNet &);

    /// assignment operator
    PetriNet & operator=(const PetriNet &);

    /// destructor
    ~PetriNet();


  private:
    /// removes a place from the net
    void removePlace(Place *p);

    /// removes an arc from the net
    void removeArc(Arc *f);

    /// removes all ingoing and outgoing arcs of a node
    void detachNode(Node *n);

    /// returns the arc weight between two nodes
    unsigned int arc_weight(Node *my_source, Node *my_target) const;

    /// returns true if all arcs connecting to n have a weight of 1
    bool sameweights(Node *n) const;


    /// APNN (Abstract Petri Net Notation) output
    void output_apnn(ostream *output) const;

    /// DOT (Graphviz) output
    void output_dot(ostream *output, bool draw_interface = true) const;

    /// INA output
    void output_ina(ostream *output) const;

    /// SPIN output
    void output_spin(ostream *output) const;

    /// info file output
    void output_info(ostream *output) const;

    /// LoLA-output
    void output_lola(ostream *output) const;

    /// oWFN-output
    void output_owfn(ostream *output) const;

    /// low-level PEP output
    void output_pep(ostream *output) const;

    /// PNML (Petri Net Markup Language) output
    void output_pnml(ostream *output) const;


    /// remove unused status places
    unsigned int reduce_unused_status_places();

    /// remove transitions with empty pre or postset
    unsigned int reduce_suspicious_transitions();

    /// remove dead nodes of the Petri net
    void reduce_dead_nodes();

    /// elimination of identical places
    void reduce_identical_places();

    /// elimination of identical transitions
    void reduce_identical_transitions();

    /// fusion of series places
    void reduce_series_places();

    /// fusion of series transitions
    void reduce_series_transitions();

    /// elimination of self-loop places
    unsigned int reduce_self_loop_places();

    /// elimination of self-loop transitions
    unsigned int reduce_self_loop_transitions();

    /// elimination of identical places
    void reduce_equal_places();

    /// remove unneeded initially marked places in choreographies
    void reduce_remove_initially_marked_places_in_choreographies();


    /// returns an id for new nodes
    unsigned int getId();


    /// a role suffix for the forEach activity
    deque<string> forEach_suffix;


    /// set of internal places of the Petri net
    set<Place *> P;

    /// set of input places of the Petri net
    set<Place *> P_in;

    /// set of output places of the Petri net
    set<Place *> P_out;

    /// set of transitions of the Petri net
    set<Transition *> T;

    /// set of arcs of the Petri net
    set<Arc *> F;

    
    /// the ports of the oWFN as mapping from a name to the interface places
    map<string, set<Place *> > ports;
    

    /// id that will be assigned to the next node
    unsigned int nextId;

    /// output file format
    output_format format;

    /// a switch to change the style of the output format
    bool use_standard_style;

    /// mapping of roles to nodes of the Petri net
    map<string, Node *> roleMap;

    /// mapping of arcs to their appropriate weight
    map< pair< Node*, Node* >, int > weight;
};



}


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

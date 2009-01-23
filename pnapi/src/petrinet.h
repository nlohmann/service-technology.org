// -*- C++ -*-

/*!
 * \file    petrinet.h
 *
 * \brief   Class PetriNet
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

#ifndef PNAPI_PETRINET_H
#define PNAPI_PETRINET_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <istream>
#include <ostream>

#include "component.h"
#include "condition.h"
#include "formula.h"
#include "io.h"

using std::string;
using std::vector;
using std::set;
using std::map;
using std::multimap;
using std::stack;
using std::istream;
using std::ostream;

namespace pnapi
{


  /*!
   * \brief   observes PetriNet components (Place, Transition, Arc)
   *
   * Each component of a PetriNet (Place, Transition, Arc) notifies its observer
   * of changes that might influence the PetriNet. There is only one
   * ComponentObserver instance per PetriNet instance.
   */
  class ComponentObserver
  {
    /// PetriNet may even do internal updates (on itself)
    friend class PetriNet;

  public:

    /// constructor
    ComponentObserver(PetriNet &);

    /// the PetriNet this observer belongs to
    PetriNet & getPetriNet() const;

    void updateArcCreated(Arc &);
    void updateArcRemoved(Arc &);
    void updateNodesMerged(Node &, Node &);
    void updateNodeNameHistory(Node &, const deque<string> &);
    void updatePlaces(Place &);
    void updatePlaceType(Place &, Node::Type);
    void updateTransitions(Transition &);


  private:

    PetriNet & net_;

    void updateNodes(Node &);
    void initializeNodeNameHistory(Node &);
    void finalizeNodeNameHistory(Node &, const deque<string> &);
    void initializePlaceType(Place &);
    void finalizePlaceType(Place &, Node::Type);

  };


  /*!
   * \brief   A Petri net
   *
   * Class to represent Petri nets. The net consists of places of
   * class #Place, transitions of class #Transition and arcs of class
   * #Arc.
   */
  class PetriNet
  {

    /// needs to update internal structures
    friend class ComponentObserver;

    /// Petri net output, see pnapi::io
    friend ostream & io::operator<<(ostream &, const PetriNet &);


  public:

    /// standard constructor
    PetriNet();

    /// destructor
    virtual ~PetriNet();

    /// copy constructor
    PetriNet(const PetriNet &);

    /// copy assignment operator
    PetriNet & operator=(const PetriNet &);



    /*!
     * \name   Querying Structural Properties
     *
     * Functions returning node sets or single nodes etc.
     */
    //@{

    bool containsNode(Node &) const;

    bool containsNode(const string &) const;

    Node * findNode(const string &) const;

    Place * findPlace(const string &) const;

    Transition * findTransition(const string &) const;

    Arc * findArc(const Node &, const Node &) const;

    const set<Node *> & getNodes() const;

    const set<Place *> & getPlaces() const;

    const set<Place *> & getInputPlaces() const;

    const set<Place *> & getOutputPlaces() const;

    const set<Place *> & getInterfacePlaces() const;

    const set<Transition *> & getTransitions() const;

    //@}


    /*!
     * \name   Basic Structural Changes
     *
     * Functions to add nodes (Node, Place, Transition) and arcs (Arc).
     */
    //@{

    /// creates an Arc
    Arc & createArc(Node &, Node &, int = 1);

    /// creates a Place
    Place & createPlace(const string & = "", Node::Type = Node::INTERNAL, 
			unsigned int = 0, unsigned int = 0);

    /// creates a Transition
    Transition & createTransition(const string & = "");

    /// deletes all interface places
    //void deleteInterfacePlaces();

    //@}


    /*!
     * \name   Advanced Petri Net Criteria and Operations
     *
     * Composition, Reduction, Normalization, ...
     */
    //@{

    /// checks the Petri net for workflow criteria
    bool isWorkflow() const;

    /// checks the Petri net for free choice criterion
    bool isFreeChoice() const;

    /// compose two nets by adding the given one and merging interfaces
    void compose(const PetriNet &, const string & = "net1",
		 const string & = "net2");

    bool isNormal() const;

    /// normalizes the Petri net
    void normalize();

    /// applies structral reduction rules
    unsigned int reduce(unsigned int = 5, bool = false);

    /// produces a second constraint oWFN
    void produce(const PetriNet &);

    //@}


    /*!
     * \name   Experimental
     *
     * Operations under development
     */
    //@{

    /// TODO: decide how to work with final conditions
    void setFinalCondition(Condition &fc);

    /// TODO: move to Condition/Formula classes
    /// checks the finalcondition for Marking m
    bool checkFinalCondition(Marking &m);

    /// TODO: can this be templated and moved to pnapi::util (util.{h,cc})?
    /// DFS with Tarjan's algorithm
    unsigned int dfsTarjan(Node *n, stack<Node *> &S, set<Node *> &stacked, unsigned int &i, map<Node *, int> &index, map<Node *, unsigned int> &lowlink) const;

    /// TODO: move to class Marking
    /// looks for a living transition under m
    Transition *findLivingTransition(Marking &m) const;

    //@}



  private:

    /* general properties */

    /// observer for nodes and arcs
    ComponentObserver observer_;

    /// final condition
    Condition condition_;


    /* (overlapping) sets for net structure */

    /// set of all nodes
    set<Node *> nodes_;

    /// all nodes indexed by name
    map<string, Node *> nodesByName_;

    /// all transitions
    set<Transition *> transitions_;

    /// all places
    set<Place *> places_;

    /// all internal places
    set<Place *> internalPlaces_;

    /// all input places
    set<Place *> inputPlaces_;

    /// all output places
    set<Place *> outputPlaces_;

    /// all interface places
    set<Place *> interfacePlaces_;

    /// ports (grouping of interface places)
    multimap<string, Place *> interfacePlacesByPort_;

    /// all arcs
    set<Arc *> arcs_;


    /* structural changes */

    /// deletes a place (used by e.g. merging and reduction rules)
    void deletePlace(Place &);

    /// deletes a transition (used by e.g. merging and reduction rules)
    void deleteTransition(Transition &);

    /// deletes a node
    void deleteNode(Node &);

    /// deletes an arc
    void deleteArc(Arc &);


    /* miscellaneous */

    /// adds the structure of a second net
    PetriNet & operator+=(const PetriNet &);

    /// returns a name for a node to be added
    string getUniqueNodeName(const string &) const;

    /// adds a given prefix to all nodes
    PetriNet & prefixNodeNames(const string &);

    /// returns true if all arcs connecting to n have a weight of 1
    bool sameweights(Node *n) const;


    /* petrify */

    /// crates a petri net from an STG file
    void createFromSTG(vector<string> & edgeLabels, const string & fileName, set<string>& inputPlacenames, set<string>& outputPlacenames);

    /// helper function for STG2oWFN
    string remap(string edge, vector<string> & edgeLabels);


    /* output */

    /// APNN (Abstract Petri Net Notation) output
    void output_apnn(ostream &) const;

    /// DOT (Graphviz) output
    void output_dot(ostream &) const;

    /// INA output
    void output_ina(ostream &) const;

    /// SPIN output
    void output_spin(ostream &) const;

    /// info file output
    void output_info(ostream &) const;

    /// LoLA-output
    void output_lola(ostream &) const;

    /// oWFN-output
    void output_owfn(ostream &) const;

    /// low-level PEP output
    void output_pep(ostream &) const;

    /// PNML (Petri Net Markup Language) output
    void output_pnml(ostream &) const;

    /// GasTeX output
    void output_gastex(ostream &) const;


    /* reduction */

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
    void reduce_series_transitions(bool keepNormal);

    /// elimination of self-loop places
    unsigned int reduce_self_loop_places();

    /// elimination of self-loop transitions
    unsigned int reduce_self_loop_transitions();

    /// elimination of identical places
    void reduce_equal_places();

    /// remove unneeded initially marked places in choreographies
    void reduce_remove_initially_marked_places_in_choreographies();

  };

}

#endif

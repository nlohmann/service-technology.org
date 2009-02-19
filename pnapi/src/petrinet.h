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

#include <vector>

#include "io.h"
#include "condition.h"
#include "component.h"

namespace pnapi
{
  
  // forward declarations
  class PetriNet;
  class Condition;
  namespace io 
  { 
    class InputError;
    std::ostream & operator<<(std::ostream &, const PetriNet &); 
    std::istream & operator>>(std::istream &, PetriNet &) throw (InputError); 
  }


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
    void updateNodeNameHistory(Node &, const std::deque<std::string> &);
    void updatePlaces(Place &);
    void updatePlaceType(Place &, Node::Type);
    void updateTransitions(Transition &);


  private:

    PetriNet & net_;

    void updateNodes(Node &);
    void initializeNodeNameHistory(Node &);
    void finalizeNodeNameHistory(Node &, const std::deque<std::string> &);
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
    friend std::ostream & io::operator<<(std::ostream &, const PetriNet &);

    /// Petri net input, see pnapi::io
    friend std::istream & io::operator>>(std::istream &, PetriNet &) throw (io::InputError);


  public:

    /// standard constructor
    PetriNet();

    /// destructor
    virtual ~PetriNet();

    /// copy constructor
    PetriNet(const PetriNet &);

    /// copy assignment operator
    PetriNet & operator=(const PetriNet &);

    /// final condition
    Condition & finalCondition();
    const Condition & finalCondition() const;


    /*!
     * \name   Querying Structural Properties
     *
     * Functions returning node sets or single nodes etc.
     */
    //@{

    bool containsNode(Node &) const;

    bool containsNode(const std::string &) const;

    Node * findNode(const std::string &) const;

    Place * findPlace(const std::string &) const;

    Transition * findTransition(const std::string &) const;

    Arc * findArc(const Node &, const Node &) const;

    const std::set<Node *> & getNodes() const;

    const std::set<Place *> & getPlaces() const;
    
    const std::set<Place *> & getInternalPlaces() const;

    const std::set<Place *> & getInputPlaces() const;

    const std::set<Place *> & getOutputPlaces() const;

    const std::set<Place *> & getInterfacePlaces() const;

    std::set<Place *> getInterfacePlaces(const std::string &) const;

    const std::set<Transition *> & getTransitions() const;

    //@}


    /*!
     * \name   Basic Structural Changes
     *
     * Functions to add nodes (Node, Place, Transition) and arcs (Arc).
     */
    //@{

    /// creates an Arc
    Arc & createArc(Node &, Node &, unsigned int = 1);

    /// creates a Place
    Place & createPlace(const std::string & = "", Node::Type = Node::INTERNAL,
			unsigned int = 0, unsigned int = 0, 
			const std::string & = "");

    /// creates a Transition
    Transition & createTransition(const std::string & = "");

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

    /// checks the Petri net for being normalized
    bool isNormal() const;

    /// compose two nets by adding the given one and merging interfaces
    void compose(const PetriNet &, const std::string & = "net1",
		 const std::string & = "net2");

    /// normalizes the Petri net
    void normalize();

    /// applies structral reduction rules
    unsigned int reduce(unsigned int reduction_level = 5, unsigned int reduction_mode = 0);

    /// produces a second constraint oWFN
    void produce(const PetriNet &);

    //@}


  private:

    /* (overlapping) sets for net structure */

    /// set of all nodes
    std::set<Node *> nodes_;

    /// all nodes indexed by name
    std::map<std::string, Node *> nodesByName_;

    /// all transitions
    std::set<Transition *> transitions_;

    /// all places
    std::set<Place *> places_;

    /// all internal places
    std::set<Place *> internalPlaces_;

    /// all input places
    std::set<Place *> inputPlaces_;

    /// all output places
    std::set<Place *> outputPlaces_;

    /// all interface places
    std::set<Place *> interfacePlaces_;

    /// ports (grouping of interface places)
    std::multimap<std::string, Place *> interfacePlacesByPort_;

    /// all arcs
    std::set<Arc *> arcs_;


    /* general properties */

    /// observer for nodes and arcs
    ComponentObserver observer_;

    /// final condition
    Condition condition_;

    /// meta information
    std::map<io::MetaInformation, std::string> meta_;


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
    std::map<const Place *, const Place *> copyStructure(const PetriNet &);

    /// returns a name for a node to be added
    std::string getUniqueNodeName(const std::string &) const;

    /// adds a given prefix to all nodes
    PetriNet & prefixNodeNames(const std::string &);

    /// returns true if all arcs connecting to n have a weight of 1
    bool sameweights(Node *n) const;

    /// returns the meta information if available
    std::string getMetaInformation(std::ios_base &, io::MetaInformation, 
			      const std::string & = "") const;


    /* petrify */

    /// crates a petri net from an STG file
    void createFromSTG(std::vector<std::string> &, const std::string &, 
		       std::set<std::string> &, std::set<std::string> &);

    /// helper function for STG2oWFN
    std::string remap(std::string edge, std::vector<std::string> & edgeLabels);


    /* output */

    /// APNN (Abstract Petri Net Notation) output
    void output_apnn(std::ostream &) const;

    /// DOT (Graphviz) output
    void output_dot(std::ostream &) const;

    /// INA output
    void output_ina(std::ostream &) const;

    /// SPIN output
    void output_spin(std::ostream &) const;

    /// info file output
    void output_info(std::ostream &) const;

    /// LoLA-output
    void output_lola(std::ostream &) const;

    /// oWFN-output
    void output_owfn(std::ostream &) const;

    /// low-level PEP output
    void output_pep(std::ostream &) const;

    /// PNML (Petri Net Markup Language) output
    void output_pnml(std::ostream &) const;

    /// GasTeX output
    void output_gastex(std::ostream &) const;


    /* reduction */

    /// remove unused status places
    unsigned int reduce_unused_status_places();

    /// remove transitions with empty pre or postset
    unsigned int reduce_suspicious_transitions();

    /// remove dead nodes of the Petri net
    unsigned int reduce_dead_nodes();
    
    /// remove unneeded initially marked places in choreographies
    unsigned int reduce_remove_initially_marked_places_in_choreographies();
    
    /// elimination of parallel places
    unsigned int reduce_rule_3p();
    
    /// elimination of parallel transitions
    unsigned int reduce_rule_3t();
    
    /// equality check for rule 4 
    bool reduce_isEqual(Transition* t1, Transition* t2, Place* p1, Place* p2);
    
    /// elimination of equal places
    unsigned int reduce_rule_4();
    
    /// check if the postset of a set is empty
    bool reduce_emptyPostset(const std::set<Node*> & nodes);
    
    /// check if the preset of a set stores only one item
    bool reduce_singletonPreset(const std::set<Node*> & nodes);
    
    /// fusion of pre- with posttransition (m to n)
    unsigned int reduce_rule_5(bool keepNormal);
    
    /// fusion of pre- with posttransition (1 to n)
    unsigned int reduce_rule_6(bool keepNormal);
    
    /// elimination of self-loop places
    unsigned int reduce_rule_7();
    
    /// elimination of self-loop transitions
    unsigned int reduce_rule_8();
    
    /// fusion of pre- with postplaces
    unsigned int reduce_rule_9(bool keepNormal);

    /// elimination of identical places
    unsigned int reduce_identical_places();

    /// elimination of identical transitions
    unsigned int reduce_identical_transitions();

    /// fusion of series places
    unsigned int reduce_series_places();

    /// fusion of series transitions
    unsigned int reduce_series_transitions(bool keepNormal);

    /// elimination of self-loop places
    unsigned int reduce_self_loop_places();

    /// elimination of self-loop transitions
    unsigned int reduce_self_loop_transitions();

    /// elimination of identical places
    unsigned int reduce_equal_places();

  };

}

#endif

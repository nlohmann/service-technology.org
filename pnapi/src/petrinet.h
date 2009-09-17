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

#include "myio.h"
#include "condition.h"
#include "component.h"

namespace pnapi
{

  // forward declarations
  class PetriNet;
  class Condition;
  class LinkNode;
  namespace io
  {
    class InputError;
    std::ostream & operator<<(std::ostream &, const PetriNet &);
    std::istream & operator>>(std::istream &, PetriNet &) throw (InputError);
  }


  namespace util
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
    friend class pnapi::PetriNet;

  public:

    /// constructor
    ComponentObserver(PetriNet &);

    /// the PetriNet this observer belongs to
    PetriNet & getPetriNet() const;

    void updateArcCreated(Arc &);
    void updateArcRemoved(Arc &);
    void updateArcMirror(Arc &);
    void updateNodesMerged(Node &, Node &);
    void updateNodeNameHistory(Node &, const std::deque<std::string> &);
    void updatePlaces(Place &);
    void updatePlaceType(Place &, Node::Type);
    void updateTransitions(Transition &);
    void updateTransitionLabels(Transition &);


  private:

    PetriNet & net_;

    void updateNodes(Node &);
    void initializeNodeNameHistory(Node &);
    void finalizeNodeNameHistory(Node &, const std::deque<std::string> &);
    void initializePlaceType(Place &);
    void finalizePlaceType(Place &, Node::Type);

  };

  } /* namespace util */

  /*!
   * \brief
   */
  namespace exceptions
  {
    /*!
     * \brief general exception class
     */
    class GeneralException
    {
    public:
      GeneralException(std::string);
      const std::string msg_;
    private:
    };
    
    /*!
     * \brief exception class thrown by PetriNet::compose()
     */
    class ComposeError : public GeneralException
    {
    public:
      ComposeError(std::string);
    };
    
    /*!
     * \brief exception class thrown by PetriNet::getSynchronizedTransitions(std::string)
     */
    class UnknownTransitionError : public GeneralException
    {
    public:
      UnknownTransitionError();
    };
    
  } /* namespace exceptions */

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
    friend class util::ComponentObserver;

    /// Petri net input, see pnapi::io
    friend std::istream & io::operator>>(std::istream &, PetriNet &)
      throw (io::InputError);

    /// Petri net output, see pnapi::io
    friend std::ostream & io::__dot::output(std::ostream &, const PetriNet &);

    /// Petri net output, see pnapi::io
    friend std::ostream & io::__owfn::output(std::ostream &, const PetriNet &);

    /// Petri net output, see pnapi::io
    friend std::ostream & io::__stat::output(std::ostream &, const PetriNet &);

    /// Petri net output, see pnapi::io
    friend std::ostream & io::__lola::output(std::ostream &, const PetriNet &);


  public:

    /// determines applied reduction rules
    enum ReductionLevel {
      NONE = 0,
      UNUSED_STATUS_PLACES = 1 << 0,
      SUSPICIOUS_TRANSITIONS = 1 << 1,
      DEAD_NODES = 1 << 2,
      INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES = 1 << 3,
      STARKE_RULE_3_PLACES = 1 << 4,
      STARKE_RULE_3_TRANSITIONS = 1 << 5,
      STARKE_RULE_4 = 1 << 6,
      STARKE_RULE_5 = 1 << 7,
      STARKE_RULE_6 = 1 << 8,
      STARKE_RULE_7 = 1 << 9,
      STARKE_RULE_8 = 1 << 10,
      STARKE_RULE_9 = 1 << 11,
      IDENTICAL_PLACES = 1 << 12,
      IDENTICAL_TRANSITIONS = 1 << 13,
      SERIES_PLACES = 1 << 14,
      SERIES_TRANSITIONS = 1 << 15,
      SELF_LOOP_PLACES = 1 << 16,
      SELF_LOOP_TRANSITIONS = 1 << 17,
      EQUAL_PLACES = 1 << 18,
      KEEP_NORMAL = 1 << 19,
      ONCE = 1 << 20,
      LEVEL_1 = DEAD_NODES,
      LEVEL_2 = (LEVEL_1 | UNUSED_STATUS_PLACES | SUSPICIOUS_TRANSITIONS),
      LEVEL_3 = (LEVEL_2 | IDENTICAL_PLACES | IDENTICAL_TRANSITIONS),
      LEVEL_4 = (LEVEL_3 | SERIES_PLACES | SERIES_TRANSITIONS),
      LEVEL_5 = (LEVEL_4 | SELF_LOOP_PLACES | SELF_LOOP_TRANSITIONS |
                  INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES),
      LEVEL_6 = (LEVEL_5 | EQUAL_PLACES),
      SET_UNNECCESSARY = (UNUSED_STATUS_PLACES | SUSPICIOUS_TRANSITIONS |
                          DEAD_NODES | INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES),
      SET_PILLAT = (IDENTICAL_PLACES | IDENTICAL_TRANSITIONS | SERIES_PLACES |
                    SERIES_TRANSITIONS | SELF_LOOP_PLACES | SELF_LOOP_TRANSITIONS |
                    EQUAL_PLACES),
      SET_STARKE = (STARKE_RULE_3_PLACES | STARKE_RULE_3_TRANSITIONS |
                    STARKE_RULE_4 | STARKE_RULE_5 | STARKE_RULE_6 |
                    STARKE_RULE_7 | STARKE_RULE_8 | STARKE_RULE_9),
      K_BOUNDEDNESS = SET_PILLAT,
      BOUNDEDNESS = (SET_PILLAT | SET_STARKE),
      LIVENESS = (SET_PILLAT | SET_STARKE)
    };


    /// standard constructor
    PetriNet();

    /// constructor Automaton => Petri net
    PetriNet(const Automaton &);

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

    const std::set<Arc *> & getArcs() const;

    const std::set<Transition *> & getSynchronizedTransitions() const;
    
    const std::set<Transition *> & getSynchronizedTransitions(const std::string & label) const;

    std::set<std::string> getSynchronousLabels() const;

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
    Transition & createTransition(const std::string & = "",
		       const std::set<std::string> & = std::set<std::string>());

    //@}


    /*!
     * \name   Advanced Petri Net Criteria and Operations
     *
     * Composition, Reduction, Normalization, ...
     */
    //@{

    /// checks the Petri net for workflow criteria
    bool isWorkflow();

    /// checks the Petri net for free choice criterion
    bool isFreeChoice() const;

    /// checks the Petri net for being normalized
    bool isNormal() const;

    /// compose two nets by adding the given one and merging interfaces
    void compose(const PetriNet &, const std::string & = "net1",
		 const std::string & = "net2");

    /// compose the given nets into a new one
    static PetriNet composeByWiring(const std::map<std::string, PetriNet *> &);

    /// normalizes the Petri net
    const std::map<Transition *, std::string> normalize();

    /// makes the inner structure of the Petri net (OWFN without interface)
    void makeInnerStructure();

    /// applies structral reduction rules
    unsigned int reduce(unsigned int = LEVEL_5);

    /// product with Constraint oWFN
    void produce(const PetriNet &, const std::string & = "net",
		 const std::string & = "constraint") throw (io::InputError);
    
    /// adds a given prefix to all nodes
    PetriNet & prefixNodeNames(const std::string &, bool = false);
    
    /// swaps input and output places
    void mirror();
    
    /// sets synchronous labels
    void setSynchronousLabels(const std::set<std::string> &);
    
    /// sets labels (and translates references)
    void setConstraintLabels(const std::map<Transition *, std::set<std::string> > &);

    //@}


  private:

    /* (overlapping) sets for net structure */

    /// set of all nodes
    std::set<Node *> nodes_;

    /// all nodes indexed by name
    std::map<std::string, Node *> nodesByName_;

    /// all transitions
    std::set<Transition *> transitions_;

    /// all synchronized transitions
    std::set<Transition *> synchronizedTransitions_;
    
    /// synchronized transitions by label
    std::map<std::string, std::set<Transition *> > transitionsByLabel_; 

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

    /// all synchronous labels
    std::set<std::string> labels_;


    /* general properties */

    /// observer for nodes and arcs
    util::ComponentObserver observer_;

    /// final condition
    Condition condition_;

    /// meta information
    std::map<io::MetaInformation, std::string> meta_;

    /// labels for constraint oWFNs
    std::map<Transition *, std::set<std::string> > constraints_;


    /* structural changes */

  public:

    /// deletes a place (used by e.g. merging and reduction rules)
    void deletePlace(Place &);

    /// deletes a transition (used by e.g. merging and reduction rules)
    void deleteTransition(Transition &);

  private:

    /// deletes a node
    void deleteNode(Node &);

    /// deletes an arc
    void deleteArc(Arc &);


    /* miscellaneous */

    /// creates a transition as a copy of another one
    Transition & createTransition(const Transition &, const std::string &);

    /// creates arcs for a transition based on the arcs of another one
    void createArcs(Transition &, Transition &,
		    const std::map<const Place *, const Place *> * = NULL);

    /// cleans up the net
    void clear();

    /// adds the structure of a second net
    std::map<const Place *, const Place *>
    copyStructure(const PetriNet &, const std::string & = "");

    /// adds the places of a second net
    std::map<const Place *, const Place *>
    copyPlaces(const PetriNet &, const std::string &);

    /// returns a name for a node to be added
    std::string getUniqueNodeName(const std::string &) const;

    /// returns the meta information if available
    std::string getMetaInformation(std::ios_base &, io::MetaInformation,
				   const std::string & = "") const;

    /// translates constraint labels to transitions
    std::map<Transition *, std::set<Transition *> >
    translateConstraintLabels(const PetriNet &) throw (io::InputError);

    /* petrify */

    /// crates a petri net from an STG file
    void createFromSTG(std::vector<std::string> &, const std::string &,
		       std::set<std::string> &, std::set<std::string> &, std::set<std::string> &);

    /// helper function for STG2oWFN
    std::string remap(std::string edge, std::vector<std::string> & edgeLabels);


    /* wiring */

    /// creates the net by assembling the given nets
    PetriNet & createFromWiring(std::map<std::string, std::vector<PetriNet> > &,
				const std::map<Place *, LinkNode *> &);

    /// wires two nets implicitly
    static void wire(const PetriNet &, const PetriNet &,
		     std::map<Place *, LinkNode *> &);


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

    //*** normalization helper methods ***//

    /// classical normalization through expanding the interface
    void normalize_classical();

    /// normalization after [Aalst07]
    void normalize_rules();

  };

}

#endif

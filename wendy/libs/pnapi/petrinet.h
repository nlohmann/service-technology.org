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
 *          last changes of: $Author: cas $
 *
 * \since   2005/10/18
 *
 * \date    $Date: 2010-03-14 13:44:15 +0100 (Sun, 14 Mar 2010) $
 *
 * \version $Revision: 5513 $
 */

#ifndef PNAPI_PETRINET_H
#define PNAPI_PETRINET_H

#include "config.h"

#include "component.h"
#include "condition.h"
#include "exception.h"
#include "interface.h"

#include <inttypes.h>

#ifndef CONFIG_PETRIFY
#define CONFIG_PETRIFY "not found"
#endif

#ifndef CONFIG_GENET
#define CONFIG_GENET "not found"
#endif

#ifdef HAVE_STDINT_H
// #include <stdint.h>
#endif

namespace pnapi
{

// forward declarations
class PetriNet;
namespace io
{
std::ostream & operator<<(std::ostream &, const PetriNet &);
std::istream & operator>>(std::istream &, PetriNet &) throw (exception::InputError);
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

private: /* private variables */
  /// the net this observer belongs to
  PetriNet & net_;
  
public: /* public methods */
  /// constructor
  ComponentObserver(PetriNet &);

  /// the PetriNet this observer belongs to
  PetriNet & getPetriNet() const;

  /// inform the net about a new arc
  void updateArcCreated(Arc &);
  /// inform nodes abput removed arc between them
  void updateArcRemoved(Arc &);
  /// inform the net about a mirrored arc
  void updateArcMirror(Arc &);
  /// inform the net about a new history of a node
  void updateNodeNameHistory(Node &, const std::deque<std::string> &);
  /// inform the net about new place
  void updatePlaces(Place &);
  /// inform the net about new transition
  void updateTransitions(Transition &);
  /// inform the net about changes in a transitions label set
  void updateTransitionLabels(Transition &);

private: /* private methods */
  /// inform net about new node
  void updateNodes(Node &);
  /// make a node accessable in a net by its name
  void initializeNodeNameHistory(Node &);
};

} /* namespace util */


/*!
 * \brief   A Petri net
 *
 * Class to represent Petri nets. The net consists of places of
 * class Place, transitions of class Transition and arcs of class
 * Arc.
 */
class PetriNet
{
  /* PetriNet's happy little friends */
  
  /// needs to update internal structures
  friend class util::ComponentObserver;
  /// Petri net input, see pnapi::io
  friend std::istream & io::operator>>(std::istream &, PetriNet &) throw (exception::InputError);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__dot::output(std::ostream &, const PetriNet &);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__owfn::output(std::ostream &, const PetriNet &);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__stat::output(std::ostream &, const PetriNet &);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__lola::output(std::ostream &, const PetriNet &);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__pnml::output(std::ostream &, const PetriNet &);
  /// Petri net output, see pnapi::io
  friend std::ostream & io::__woflan::output(std::ostream &, const PetriNet &);

public: /* public types */
  /*!
   * \brief determining applied reduction rules
   */
  enum ReductionLevel
  {
    NONE = 0,
    UNUSED_STATUS_PLACES = (1 << 0),
    SUSPICIOUS_TRANSITIONS = (1 << 1),
    DEAD_NODES = (1 << 2),
    INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES = (1 << 3),
    STARKE_RULE_3_PLACES = (1 << 4),
    STARKE_RULE_3_TRANSITIONS = (1 << 5),
    STARKE_RULE_4 = (1 << 6),
    STARKE_RULE_5 = (1 << 7),
    STARKE_RULE_6 = (1 << 8),
    STARKE_RULE_7 = (1 << 9),
    STARKE_RULE_8 = (1 << 10),
    STARKE_RULE_9 = (1 << 11),
    IDENTICAL_PLACES = (1 << 12),
    IDENTICAL_TRANSITIONS = (1 << 13),
    SERIES_PLACES = (1 << 14),
    SERIES_TRANSITIONS = (1 << 15),
    SELF_LOOP_PLACES = (1 << 16),
    SELF_LOOP_TRANSITIONS = (1 << 17),
    EQUAL_PLACES = (1 << 18),
    KEEP_NORMAL = (1 << 19),
    ONCE = (1 << 20),
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
                  SERIES_TRANSITIONS | SELF_LOOP_PLACES |
                  SELF_LOOP_TRANSITIONS | EQUAL_PLACES),
    SET_STARKE = (STARKE_RULE_3_PLACES | STARKE_RULE_3_TRANSITIONS |
                  STARKE_RULE_4 | STARKE_RULE_5 | STARKE_RULE_6 |
                  STARKE_RULE_7 | STARKE_RULE_8 | STARKE_RULE_9),
    K_BOUNDEDNESS = SET_PILLAT,
    BOUNDEDNESS = (SET_PILLAT | SET_STARKE),
    LIVENESS = (SET_PILLAT | SET_STARKE)
  };

  /*!
   * \brief determining the Service Automaton => PetriNet Conerter
   */
  enum AutomatonConverter
  {
    /// petrify
    PETRIFY,
    /// genet
    GENET,
    /// transform to a state machine (using own methods)
    STATEMACHINE
  };
  
  /*!
   * \brief Warning Flags
   * 
   * \todo actually _use_ this
   */
  enum Warning
  {
    W_NONE = 0,
    W_INTERFACE_PLACE_IN_FINAL_CONDITION = 1
  };
  
private: /* private static variables */
  /// path to petrify
  static std::string pathToPetrify_;
  /// path to genet
  static std::string pathToGenet_;
  /// capacity for genet
  static uint8_t genetCapacity_;
  /// converter Automaton => PetriNet
  static AutomatonConverter automatonConverter_;
  
private: /* private variables */
  /*!
   * \name vital first components
   * 
   * Order of members in header decides about initialize order in
   * constructor. These members must be initialized
   * FIRST and IN THIS ORDER.
   */
  //@{
  /// observer for nodes and arcs
  util::ComponentObserver observer_;
  //@}
  
  /*!
   * \name (overlapping) sets for net structure
   * \todo Vielleicht auch sendTransitions_ ?
   */
  //@{
  /// set of all nodes
  std::set<Node *> nodes_;
  /// all nodes indexed by name
  std::map<std::string, Node *> nodesByName_;
  /// all places
  std::set<Place *> places_;
  /// all transitions
  std::set<Transition *> transitions_;
  /// all synchronized transitions
  std::set<Transition *> synchronizedTransitions_;
  /// all arcs
  std::set<Arc *> arcs_;
  /// roles
  std::set<std::string> roles_;
  //@}
  
  /*! 
   * \name general properties
     \todo Vielleicht Rollen symmetrisch zu Kosten organisieren.
   */
  //@{
  /// meta information
  std::map<io::MetaInformation, std::string> meta_;
  /// labels for constraint oWFNs
  std::map<Transition *, std::set<std::string> > constraints_;
  /// warning flags
  unsigned int warnings_;
  /// cache for reduction
  std::set<const Place *> * reducablePlaces_;
  //@}
  
  /*!
   * \name vital last components
   * 
   * Order of members in header decides about initialize order in
   * constructor. These members must be initialized
   * LAST and IN THIS ORDER.
   */
  //@{
  /// interface
  Interface interface_;
  /// final condition
  Condition finalCondition_;
  //@}
  
public: /* public static methods */
  /// setting path to Petrify
  static void setPetrify(const std::string & = CONFIG_PETRIFY);
  /// setting path to Genet
  static void setGenet(const std::string & = CONFIG_GENET, uint8_t = 2);
  /// setting automaton converter
  static void setAutomatonConverter(AutomatonConverter = PETRIFY);

public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor
  PetriNet();
  /// constructor Automaton => Petri net
  PetriNet(const Automaton &);
  /// copy constructor
  PetriNet(const PetriNet &);
  /// destructor
  virtual ~PetriNet();
  /// copy assignment operator
  PetriNet & operator=(const PetriNet &);
  //@}

  
  /*!
   * \name   Querying Structural Properties
   *
   * Functions returning node sets or single nodes etc.
   */
  //@{
  /// whether net contains given node
  bool containsNode(const Node &) const;
  /// whether net contains given node
  bool containsNode(const std::string &) const;
  /// find a node by name
  Node * findNode(const std::string &) const;
  /// find a place by name
  Place * findPlace(const std::string &) const;
  /// find a transition by name
  Transition * findTransition(const std::string &) const;
  /// find an arc by its connected nodes
  Arc * findArc(const Node &, const Node &) const;
  /// get the interface
  Interface & getInterface();
  /// get all nodes
  const std::set<Node *> & getNodes() const;
  /// get places
  const std::set<Place *> & getPlaces() const;
  /// get transitions
  const std::set<Transition *> & getTransitions() const;
  /// get synchronized transitions
  const std::set<Transition *> & getSynchronizedTransitions() const;
  /// get arcs
  const std::set<Arc *> & getArcs() const;
  /// get roles
  const std::set<std::string> & getRoles() const;
  /// get the final condition
  Condition & getFinalCondition();
  /// get the final condition
  const Condition & getFinalCondition() const;
  //@}
  
  
  /*!
   * \name   Basic Structural Changes
   *
   * Functions to add or delete nodes (Node, Place, Transition), arcs (Arc)
   * and interface, i.e. ports (Port) and labels (Label).
   */
  //@{
  /// creates a Place
  Place & createPlace(const std::string & = "", unsigned int = 0, unsigned int = 0);
  /// creates a Transition
  Transition & createTransition(const std::string & = "");
  /// creates an Arc
  Arc & createArc(Node &, Node &, unsigned int = 1);
  /// creates a Port
  Port & createPort(const std::string &);
  /// creates an input Label
  Label & createInputLabel(const std::string &, const std::string & = "");
  /// creates an input Label
  Label & createInputLabel(const std::string &, Port &);
  /// creates an output Label
  Label & createOutputLabel(const std::string &, const std::string & = "");
  /// creates an output Label
  Label & createOutputLabel(const std::string &, Port &);
  /// creates a synchronous Label
  Label & createSynchronizeLabel(const std::string &, const std::string & = "");
  /// creates a synchronous Label
  Label & createSynchronizeLabel(const std::string &, Port &);
  /// adds a role
  void addRole(const std::string &);
  /// add a set of roles
  void addRoles(const std::set<std::string> &);
  
  /// deletes a place
  void deletePlace(Place &);
  /// deletes a transition
  void deleteTransition(Transition &);
  /// deletes an arc
  void deleteArc(Arc &);
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
  /// checks whether a transition role name is specified
  bool isRoleSpecified(const std::string &) const;

  /// compose two nets by adding the given one and merging interfaces
  void compose(const PetriNet &, const std::string & = "net1",
               const std::string & = "net2");
  /// normalizes the Petri net
  std::map<Transition *, std::string> normalize();
  /// applies structral reduction rules
  unsigned int reduce(unsigned int = LEVEL_5);
  /// product with Constraint oWFN
  void produce(const PetriNet &, const std::string & = "net",
               const std::string & = "constraint") throw (exception::InputError);
  /// adds a given prefix to all nodes
  PetriNet & prefixNodeNames(const std::string &);
  /// swaps input and output labels
  void mirror();
  /// sets labels (and translates references)
  void setConstraintLabels(const std::map<Transition *, std::set<std::string> > &);
  /// get warnings
  unsigned int getWarnings() const;
  /// set warnings
  void setWarnings(unsigned int = W_NONE);
  //@}

private: /* private methods */

  /*!
   * \name structural changes
   */
  //@{
  /// creates a transition as a copy of another one
  Transition & createTransition(const Transition &, const std::string &);
  /// creates arcs for a transition based on the arcs of another one
  void createArcs(Transition &, Transition &,
                  const std::map<const Place *, const Place *> * = NULL);
  
  /// adds the structure of a second net
  std::map<const Place *, const Place *>
  copyStructure(const PetriNet &, const std::string & = "");
  /// adds the places of a second net
  std::map<const Place *, const Place *>
  copyPlaces(const PetriNet &, const std::string &);

  /// cleans up the net
  void clear();
  /// deletes a node
  void deleteNode(Node &);
  //@}

  
  /*!
   * \name miscellaneous
   */
  //@{
  /// returns a name for a node to be added
  std::string getUniqueNodeName(const std::string &) const;
  /// returns the meta information if available
  std::string getMetaInformation(std::ios_base &, io::MetaInformation,
                                 const std::string & = "") const;
  /// translates constraint labels to transitions
  std::map<Transition *, std::set<Transition *> >
  translateConstraintLabels(const PetriNet &) throw (exception::InputError);
  /// compose constructor
  PetriNet(const Interface &, const Interface &, std::map<Label *, Label *> &,
           std::map<Label *, Place *> &, std::set<Label *> &);
  //@}

  /*!
   * \name Service Automaton to PetriNet conversation
   */
  //@{
  /// creates a petri net from an STG file
  void createFromSTG(std::vector<std::string> &, const std::string &,
                     std::set<std::string> &, std::set<std::string> &, std::set<std::string> &);
  /// helper function for STG2oWFN
  std::string remap(const std::string & edge, std::vector<std::string> & edgeLabels);
  //@}

  
  /*!
   * \name reduction
   */
  //@{
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
  bool reduce_isEqual(Transition *, Transition *, Place *, Place *);
  /// elimination of equal places
  unsigned int reduce_rule_4();
  /// check if the postset of a set is empty
  bool reduce_emptyPostset(const std::set<Node *> &);
  /// check if the preset of a set stores only one item
  bool reduce_singletonPreset(const std::set<Node *> &);
  /// fusion of pre- with posttransition (m to n)
  unsigned int reduce_rule_5(bool);
  /// fusion of pre- with posttransition (1 to n)
  unsigned int reduce_rule_6(bool);
  /// elimination of self-loop places
  unsigned int reduce_rule_7();
  /// elimination of self-loop transitions
  unsigned int reduce_rule_8();
  /// fusion of pre- with postplaces
  unsigned int reduce_rule_9(bool);
  /// elimination of identical places
  unsigned int reduce_identical_places();
  /// elimination of identical transitions
  unsigned int reduce_identical_transitions();
  /// fusion of series places
  unsigned int reduce_series_places();
  /// fusion of series transitions
  unsigned int reduce_series_transitions(bool);
  /// elimination of self-loop places
  unsigned int reduce_self_loop_places();
  /// elimination of self-loop transitions
  unsigned int reduce_self_loop_transitions();
  /// elimination of identical places
  unsigned int reduce_equal_places();
  //@}

  /*!
   * \name normalization helper methods
   */
  //@{
  /// classical normalization through expanding the interface
  void normalize_classical();
  /// normalization defined in [Aalst07]
  void normalize_rules();
  //@}
};

} /* namespace pnapi */

#endif /* PNAPI_PETRINET_H */

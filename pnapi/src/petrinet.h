/* -*- mode: c++ -*- */

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
#include <ostream>
#include <istream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <stack>

#include "petrinode.h"

using std::string;
using std::vector;
using std::deque;
using std::set;
using std::map;
using std::multimap;
using std::list;
using std::pair;
using std::stack;
using std::ostream;
using std::istream;


namespace pnapi
{

  /*!
   * \brief   Marking of all places of a net
   *
   * \note    In the future this class might contain a highly efficient
   *          implementation if necessary. For now, we use a simple one to
   *          determine the needed functionality.
   */
  class Marking
  {
  public:
    Marking();
    Marking(PetriNet &n);
    virtual ~Marking();

    map<Place *, unsigned int> getMap() const;

    unsigned int size();

    unsigned int & operator[](Place *offset);
    bool operator==(const Marking &mm) const;

  private:
    map<Place *, unsigned int> m;
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
  public:

    /// standard constructor
    PetriNet();

    /// destructor
    ~PetriNet();

    /// copy constructor
    PetriNet(const PetriNet &);

    /// copy assignment operator
    PetriNet & operator=(const PetriNet &);

    /// adds the structure of a second net
    PetriNet & operator+=(const PetriNet &);


    /*!
     * \name   Input/Output Operations
     *
     * Reading and writing Petri nets from and to files.
     */
    //@{

    /// possible file formats for I/O operations
    enum IOFormat
      {
	FORMAT_OWFN,      ///< Open Workflow Net, format used e.g. by Fiona
	FORMAT_ONWD,      ///< Open Net Wiring Description
	FORMAT_APNN,      ///< Abstract Petri Net Notation (APNN)
	FORMAT_DOT,       ///< Graphviz dot
	FORMAT_INA,       ///< INA
	FORMAT_SPIN,      ///< INA
	FORMAT_INFO,      ///< Info File
	FORMAT_LOLA,      ///< LoLA
	FORMAT_PEP,       ///< Low-Level PEP Notation
	FORMAT_PNML,      ///< Petri Net Markup Language (PNML)
	FORMAT_GASTEX     ///< GasTeX format
      };

    /// sets the format for I/O operations
    void setIOFormat(IOFormat);

    /// outputs a Petri net
    friend ostream & operator<<(ostream &, const PetriNet &);

    /// reads a Petri net
    friend istream & operator>>(istream &, PetriNet &);

    //@}


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
     * \name   Creating a Petri net
     *
     * Functions to add nodes (Node, Place, Transition) and arcs (Arc).
     */
    //@{

    /// creates an Arc
    Arc & createArc(Node &, Node &, int = 1);

    /// creates a Place
    Place & createPlace(const string & = "", Node::Type = Node::INTERNAL);

    /// creates a Transition
    Transition & createTransition(const string & = "");

    //@}


    /*!
     * \name   Advanced Petri Net Operations
     *
     * Composition, Reduction, Normalization, ...
     */
    //@{

    /// compose two nets by adding the given one and merging interfaces
    void compose(const PetriNet &, const string & = "net1",
		 const string & = "net2");

    /// deletes all interface places
    void makeInnerStructure();

    /// normalizes the Petri net
    void normalize();

    /// checks the finalcondition for Marking m
    bool checkFinalCondition(Marking &m) const;

    /// calculates the successor m' from m using transition t
    Marking & successorMarking(Marking &m, Transition *t) const;

    bool activates(Marking &m, Transition &t) const;

    // TODO: add reduce()

    //@}



  private:

    /* general properties */

    /// format for input/output operations
    IOFormat format_;


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


    /* update functions */

    // friend declarations to allow access to update*
    friend void Arc::notifyCreated();
    friend void Node::notifyNameHistoryChanged();
    friend void Place::notifyCreated();
    friend void Place::notifyTypeChanged();
    friend void Transition::notifyCreated();

    // update receivers
    void updateArcs(Arc &);
    void updateNodeNameHistory(Node &, const deque<string> &);
    void updatePlaces(Place &);
    void updatePlaceType(Place &, Node::Type);
    void updateTransitions(Transition &);

    // helpers
    void updateNodes(Node &);
    void initializeNodeNameHistory(Node &);
    void finalizeNodeNameHistory(Node &, const deque<string> &);
    void initializePlaceType(Place &);
    void finalizePlaceType(Place &);
    void finalizePlaceType(Place &, Node::Type);


    /* structural changes */

    /// deletes a place (used by e.g. merging and reduction rules)
    void deletePlace(Place &);

    /// deletes a transition (used by e.g. merging and reduction rules)
    void deleteTransition(Transition &);

    /// deletes a node
    void deleteNode(Node &);

    /// merge arcs (used by mergePlaces())
    void mergeArcs(Place &, Place &, const set<Node *> &, const set<Node *> &,
		   bool);

    /// merges two (internal) places
    void mergePlaces(Place &, Place &);


    /* miscellaneous */

    /// returns a name for a node to be added
    string getUniqueNodeName(const string &) const;

    /// adds a given prefix to all nodes
    PetriNet & prefixNodeNames(const string &);



    /*** NOT YET REFACTORED ***/

    /// swaps input and output places
    void mirror();

    /// produces a second constraint oWFN
    void produce(const PetriNet &net);

    /// moves channel places to the list of internal places
    void makeChannelsInternal();

    /// add a loop to the final states to check deadlock freedom with LoLA
    void loop_final_state();

    /// calculate the maximal occurrences of communication
    void calculate_max_occurrences();

    /// reevaluates the type of a transition
    void reevaluateType(Transition *t);

    /// returns true if all arcs connecting to n have a weight of 1
    bool sameweights(Node *n) const;


    /* Petri net criteria */

    /// checks the Petri net for workflow criteria
    bool isWorkflowNet();

    /// DFS with Tarjan's algorithm
    unsigned int dfsTarjan(Node *n, stack<Node *> &S, set<Node *> &stacked, unsigned int &i, map<Node *, int> &index, map<Node *, unsigned int> &lowlink) const;

    /// checks the Petri net for free choice criterion
    bool isFreeChoice() const;


    /* markings and final condition */

    /// calculates the current marking m
    Marking calcCurrentMarking() const;

    /// reforms the marking m to the places' token
    void marking2Places(Marking &m);

    /// looks for a living transition under m
    Transition *findLivingTransition(Marking &m) const;


    /* petrify */

    /// crates a petri net from an STG file
    void createFromSTG(vector<string> & edgeLabels, const string & fileName, set<string>& inputPlacenames, set<string>& outputPlacenames);

    /// helper function for STG2oWFN
    string remap(string edge, vector<string> & edgeLabels);


    /* output */

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

    /// GasTeX output
    void output_gastex(ostream *output) const;


    /* reduction */

    /// applies structral reduction rules
    unsigned int reduce(unsigned int reduction_level = 5, bool = false);

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

    /// merges two transitions
    void mergeTransitions(Transition &, Transition &);

    /// merges two parallel transitions
    void mergeParallelTransitions(Transition *t1, Transition *t2);

    /// merges two parallel places
    void mergeParallelPlaces(Place *p1, Place *p2);

  };


  // repeated declaration to avoid compilation errors using gcc 4.3
  // see <https://gna.org/bugs/?12113> for more information
  ostream & operator<<(ostream &, const PetriNet &);
  istream & operator>>(istream &, PetriNet &);

}

#endif

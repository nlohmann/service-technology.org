// -*- C++ -*-

/*!
 * \file    imatrix.h
 *
 * \brief   Class IMatrix
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/14/07
 *
 * \date    $Date: 2013-05-17 12:00:00 +0200 (Fr, 17. May 2013) $
 *
 * \version $Revision: 1.00 $
 */

#ifndef IMATRIX_H
#define IMATRIX_H

#include <set>
#include <map>
#include <string>
#include <pnapi/pnapi.h>
#include "JSON.h"

using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;
using pnapi::Node;
using pnapi::Label;
using pnapi::formula::Formula;
using std::set;
using std::map;
using std::vector;
using std::string;

// nodes will be put into equivalence classes according to their pre/postset sizes
// sizes are differentiated only below the following number
#define NODE_SET_LIMIT 3
// ID representing "no node found"
#define NO_NODE 0xFFFFFFFF

// nodes
typedef unsigned int Vertex;
// maps for representing pre/postsets, but also some other constructions
typedef map<Vertex,unsigned int> Map;
// reduction rule modes
typedef uint32_t Mode;

class Rules;
class Facts;

/*! \brief For dynamically computing and storing the incidence matrix of a Petri net
	and handling the firing of transitions
	
	This class also handles locks to different forms of node data for
	multithreaded runs.
*/
class IMatrix {

public:
	/// Constructor. Builds an empty incidence matrix.
	IMatrix(PetriNet& pn, JSON& log);

	/// Destructor
	~IMatrix();

	/// Check if a node is actually a place
	bool isPlace(Vertex id);

	/// Check if a node is an input or output label
	bool isIO(Vertex id);

	/// Types of IO labels
	enum IOLabelTypes {
		INPUT = 0,
		OUTPUT = 1
	};

	/// Check if a node belongs to the original net
	bool isOriginal(Vertex id);

	/// Check if a node exists at this time
	bool exists(Vertex id);

	/// Get the pre- or postset of a node
	Map& getPrePost(Vertex id, bool prepost);

	/// Get the preset of a node
	Map& getPre(Vertex id);

	/// Get the postset of a node
	Map& getPost(Vertex id);

	/// Get the initial number of tokens on a place
	unsigned int& getTokens(Vertex id);

	/// Add some tokens to a place
	void addTokens(Vertex id, unsigned int tokens);

	/// Get the time at which the node has been changed last
	unsigned int getTimeStamp(unsigned int id);

	/// Get missing (=0) time stamps for a set of nodes
	void completeTimeStamps(Map& stamps);

	/// Increment the time stamp of a node
	void stamp(Vertex id);

	/// Get a read lock for a node's edges
	unsigned int rdlock(Vertex id);

	/// Get read locks for two nodes
	unsigned int rdlock(Vertex id1, Vertex id2);

	/// Get a write lock for a node
	void wrlock(Vertex id);

	/// Get write locks for a set of nodes
	bool wrlock(Map& stamps);

	/// Return lock for a node, set the reduction done, and possibly delete the node
	void unlock(Vertex id, Mode mode = 0, bool remove = false);

	/// Unlock a set of nodes, delete those with a value of zero
	void unlock(Map& stamps);

	/// Set the checked reduction rule for this node
	void setMode(Vertex id, Mode flag, bool lock = true);

	/// Get all reductions checked for this node
	Mode getModes(Vertex id);

	/// Enable re-checking of all reduction rules for this node
	void clearModes(Vertex id, bool lock = false);

	/// If a reduction rule for a node has been checked
	bool isDone(Vertex id, Mode flag);

	/// get the number of a reduction rule that needs to be done
	unsigned int findMode(Rules& rules, unsigned int start);

	// Places and transitions
	enum NodeTypes {
		PL = 0,
		TR = 1
	};

	/// Find a node that needs a reduction check (with NodeType type, reduction rule flag, and given pre-/postset sizes)
	bool getFirstNode(Vertex& id, unsigned int type, Mode flag, unsigned int presize, unsigned int postsize);

	/// Find a node that needs a reduction check (as above but with a range of pre-/postset sizes)
	bool getFirstNode(Vertex& id, unsigned int type, Mode flag, unsigned int presizemin, unsigned int presizemax, unsigned int postsizemin, unsigned int postsizemax);

	/// Get all isolated nodes (of NodeType type) if at least one of them needs to be check with the given reduction rule
	Map getIsolated(unsigned int type, Mode flag);

	/// Put a node into the list for the correct pre-/postsize sets (or delete it)
	void adaptList(Vertex id, bool remove);

	/// Create a PetriNet object from the internal data structures
	PetriNet* exportNet(Facts& facts);

	/// Get the name of a node
	string getName(Vertex id);

	/// Set the name of a node
	void setName(Vertex id, string name);

	/// Make a node unremovable (by name)
	void makePersistent(string node);

	/// Declare a node visible in formulas (by name)
	void makeVisible(string node);

	/// Make a transition or place visible
	void makeVisible(Vertex id);

	/// Make all transitions (or all places) visible
	void makeVisible(bool transitions);

	/// Get the visibility status of a node
	unsigned int visible(Vertex id);

	/// Check if a node is visible (but removable)
	bool isVisible(Vertex id);

	/// Check if a node is invisible and removable
	bool isInvisible(Vertex id);

	/// Check if a node is irremovable
	bool isPersistent(Vertex id);

	/// Visibility states
	enum Visibility {
		INVISIBLE = 0,
		VISIBLE = 1,
		PERSISTENT = 2
	};

	/// Get the label of a transition, 0=tau/lambda
	unsigned int getLabel(Vertex id);

	/// Get the label of a transition if the latter is visible, 0 otherwise
	unsigned int getLabelVis(Vertex id);

	/// Project the label ID to the label itself
	string& getLabeltext(unsigned int action);

	/// Set a transition's label
	void setLabel(Vertex id, unsigned int action);

	/// Get the number of nodes with a given label
	unsigned int getLabelCount(unsigned int action);

	/// Remove the label from a node
	void clearLabel(Vertex id);

	/// For Debugging: Print equivalence classes for pre-/postset sizes
	void printLists();

	/// Fire a transitions, correctly changing the initial marking
	bool fire(Vertex t);

	/// Add a multiple of the preset of a node to the preset of another node
	void addPre(Vertex id, Vertex id2, unsigned int times);

	/// Add a multiple of the postset of a node to the postset of another node
	void addPost(Vertex id, Vertex id2, unsigned int times);

	/// Add a multiset of places (multiplied by times) to the postset of a node
	void addPost(const Map& pmap, Vertex id2, unsigned int times);

	/// Remove all edges incident to a node
	void removeArcs(Vertex id);

	/// Remove all edges in the preset of a node
	void removePre(Vertex id);

	/// Remove all edges in the postset of a node
	void removePost(Vertex id);

	/// Return num new transitions and lock them (with time stamps) if available
	set<Vertex> reserveTransitions(unsigned int num, Map& stamp);

	/// Return the node belonging to an ID
	Node* getNode(Vertex id);

	/// Return the node belonging to an ID
	Vertex getID(const Node&);


private:
	/// The Petri net for this incidence matrix
	const PetriNet& petrinet;

public:
	/// Number of places in the net
	unsigned int numplaces;

	/// Number of transitions in the net
	unsigned int numtransitions;

	/// Number of asynchronous labels in the net
	unsigned int numiolabels;

	/// Number of nodes in the original net
	unsigned int orignodes;

	/// Presets of nodes
	Map* pre;

	/// Postsets of nodes
	Map* post;

	/// Initial marking
	unsigned int* marking;

	/// Type of an IO label
	unsigned int* iotype;

	/// Connector from ID to IO label
	Label** iovec;

	/// Connector from ID to Node
	Node** idvec;

	/// Locks for Nodes
	pthread_rwlock_t* rwlocks;

	/// Flags showing write locks
	bool* writing;

	/// In which list of the node partition the node resides (-1 = node does not exist)
	int* list;

	/// action labels (for transitions, 0=tau)
	unsigned int* label;

	/// names
	string* name;

	/// time stamp
	unsigned int* timestamp;

	/// Locks for Modes
	pthread_rwlock_t* modelocks;

	/// Modes
	Mode* modes;


	/// Locks for NodeLists
	pthread_rwlock_t*** listlocks;

	/// NodeLists
	set<unsigned int> nodelists[2][NODE_SET_LIMIT+1][NODE_SET_LIMIT+1];


	/// node to id
	map<const Node*, unsigned int> nodeToID;

	/// io label to id
	map<Label*, unsigned int> ioToID;

	/// if nodes are visible (appear in formulas)
	unsigned int* visibility;

	/// visibility of action labels
	map<unsigned int,bool> labelvis;

	/// action label strings
	map<unsigned int,string> labeltext;

	/// mapping from strings to action label numbers
	map<string,unsigned int> labelnum;

	/// counting action labels
	map<unsigned int, unsigned int> labelcount;

	/// Lock for action label counter
	pthread_rwlock_t labellock;

	/// set of unused transitions
	set<Vertex> unused;

	/// lock for unused transitions set
	pthread_rwlock_t unusedlock;
};

#endif


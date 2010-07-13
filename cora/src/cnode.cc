// -*- C++ -*-

/*!
 * \file    cnode.cc
 *
 * \brief   Class CNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/14
 *
 * \date    $Date: 2010-05-14 14:47:01 +0200 (Fr, 16. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#include <map>
#include <iostream>

#include "pnapi/pnapi.h"
#include "cnode.h"
#include "extmarking.h"
#include "imatrix.h"
#include "verbose.h"

using pnapi::PetriNet;
using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::map;
using std::cout;
using std::endl;
using std::ostream;
using std::vector;

extern vector<Place*> po;

	/********************************
	* Implementation of class CNode *
	********************************/

/** Standard constructor. */ 
CNode::CNode() : root(NULL),em(),pos(1) {
	father.push_back(-1); // the index of the father subnode (none=-1)
	lson.push_back(-1); // the index of the left son subnode (none=-1)
	rson.push_back(-1); // the index of the right son subnode (none=-1)
	pcond.push_back(NULL); // if a subnode is a decision point, the place for which the decision is made
	tcond.push_back(0); // if a subnode is a decision point, the token number for the place
	leaf.push_back(this); // if a subnode is a leaf, a pointer to the node
}

/** Constructor with marking.
	@param extm An extended marking as label for this node.
*/
CNode::CNode(ExtMarking extm) : root(NULL),em(extm),pos(1) {
	father.push_back(-1);
	lson.push_back(-1);
	rson.push_back(-1);
	pcond.push_back(NULL);
	tcond.push_back(0);
	leaf.push_back(this);
}

/** Constructor for subnodes.
	@param extm An extended marking as label for this node.
	@param r This node's root node.
	@param father This node's father's index number.
	@param higher Is this is a lower or a higher son.
*/
CNode::CNode(ExtMarking& extm, CNode& r, int father, bool higher) : root(&r),em(extm),pos(r.pos) {
	if (father<0 || father>=r.pos) abort(11,"internal error, non-existent subtree node");
	r.father.push_back(father); // root entry: this node's father
	r.lson.push_back(-1); // root entry: this node has no left son
	r.rson.push_back(-1); // root entry: this node has no right son
	r.pcond.push_back(NULL); // root entry: this node is a leaf, so no (inner) condition
	r.tcond.push_back(0); // see above
	r.leaf.push_back(this); // root entry: this node's leaf data structure
	if (higher) r.rson[father]=r.pos; // if the new node should be left/lower or right/higher son
	else r.lson[father]=r.pos;
	++(r.pos); // the next free position in the root entry list
}
 
/** Destructor. */
CNode::~CNode() {} // freeing will be done by the covergraph where necessary

/** Declare if this node is a global node (root) or a subnode. A root is a node in a standard
	coverability graph while a subnode represents a subset of markings of a root node.
	@param cnode This node's root node, making this node a subnode. If NULL, this node
		becomes a root node itself and its extended marking and edges may become invalid.
*/
void CNode::setRoot(CNode* cnode) { root = cnode; }

/** Get the root node of this node.
	@return This node's root or the node itself (if it is a root node).
*/
CNode* CNode::getRoot() { return (root?root:this); }

/** Check if this node is a root node.
	@return If it is a root node.
*/
bool CNode::isRoot() { return (!root); }

/** Set the label of this node to the extended marking extm.
	@param extm The extended marking.
	@return If the extended marking has been set. If the extended marking is not regular
		(contains other intervals besides omega)
		and the node is a root node, this method will fail.
*/
bool CNode::setMarking(ExtMarking& extm) { 
	if (root) { em = extm; return true; }
	else if (extm.isRegular()) { em = extm; return true; }
	return false;
}

/** Get the label of this node in form of an extended marking.
	@return The extended marking.
*/
ExtMarking& CNode::getMarking() { return em; } 

/** Obtain the set of all firable transitions for this node. Before this method is
	called, the successor list for this node should be cleared. (Not necessary if
	the node has just been created and its extended marking has been set.)
	@param pn The Petri net.
	@param im The incidence matrix of the Petri net.
*/
void CNode::computeFirables(PetriNet& pn, IMatrix& im) {
	todo = em.firableTransitions(pn,im); // do not return anything, just set the todo list
}

/** Obtain the set of all firable transitions from this subnode's root node. Before 
	this method is called, the successor list for this node should be cleared. 
	(Not necessary if the node has just been created and its extended marking has 
	been set.) Note: The transitions obtained through this method need not be firable
	under the subnode's marking!
	@return Reports false if this node has no root node (nothing is done).
*/
bool CNode::computeFirables() {
	if (!root) return false; // don't do anything if this is a root node
	todo = root->todo; // obtain the root's todos
	map<Transition*,CNode*>::iterator cit;
	for(cit=root->succ.begin(); cit!=root->succ.end(); ++cit)
		todo.insert(cit->first); // and add all transitions that already have been done for the root
	return true;
}

/** Add the given set of transitions to the todo list for later edge creation.
	@param tvec A list of transitions for which outgoing edges should be created.
*/
void CNode::addToFirables(vector<Transition*> tvec) {
	for(unsigned int i=0; i<tvec.size(); ++i)
		todo.insert(tvec[i]);
}

/** Set an edge from this node to the node cnode labelled with the transition t.
	Edges of a subnode precede edges of its father node except if a
	subnode does not declare a t-edge, then the t-edge of its father node is inherited.
	Disabling a transition t in a subnode is done by giving it a NULL pointer as cnode.
	@param t The transition.
	@param cnode The successor node.
*/
void CNode::setSuccessor(Transition& t, CNode* cnode) {
	if (succ.find(&t)!=succ.end() && succ[&t]!=NULL) 
	{ // possibly remove this node from the former successor's predecessor list
		int cnt=0;
		map<Transition*,CNode*>::iterator cit;
		for(cit=succ.begin(); cit!=succ.end(); ++cit)
			if (cit->second==succ[&t]) ++cnt; // count how often we point to the successor
		if (cnt<2) succ[&t]->pred.erase(this->getRoot()); // if just once, remove it
	}
	succ[&t] = cnode; // set the new successor (possibly none=NULL)
	if (cnode) cnode->pred.insert(this->getRoot()); // and add us to our successor's predecessor list
}

/** Before a new root node is created, this method checks for nodes covered by an extended
	marking on a path from the initial marking in the coverability graph to this node. 
	Omegas are introduced accordingly into the extended marking (which is changed).
	@param extm The extended marking of the new node, being changed by the method.
	@return If this node is a root node and omegas have been introduced.
*/
bool CNode::addOmega(ExtMarking& extm) {
	if (root) return false; // for root nodes only
	bool added(false); // if we have added an omega
	set<CNode*> done,todo; // which nodes we have already checked and which ones are still to do
	todo.insert(this); // we begin with this node
	while (!todo.empty()) // if we still have something to do ...
	{
		CNode* active(*(todo.begin())); // get the first node on our todo list
		if (extm.isCovering(active->em)) // if we cover that node's marking
		{ 
			set<Place*> ps(extm.setOmega(active->em)); // set the covered places (as in "larger") to omega
			if (!ps.empty()) added=true; // and remember if we did something
		}
		todo.erase(todo.begin()); // we are done with this node
		done.insert(active); // remember that so we don't come back here later
		set<CNode*>::iterator pit; // now go through all of this node's predecessors
		for(pit=active->pred.begin(); pit!=active->pred.end(); ++pit)
			if (done.find(*pit)==done.end()) todo.insert(*pit); // and add them to todo unless we have already done them
	}
	if (added) addOmega(extm); // check (again) if further omegas can be introduced (smaller graph, not necessary)
	return added;
}

/** Check if work has to be done for transition t or if all edges with label t emerging from
	this node have already been created.
	@param t The transition.
	@return If work still has to be done.
*/
bool CNode::needsWork(Transition& t) {
	return (todo.find(&t)!=todo.end());
}

/** Get a transition t for which work still needs to be done, i.e. it is possible that not
	all edges with label t emerging at this node have been created.
	@return A transition that needs work if one exists, NULL otherwise.
*/
Transition* CNode::getWork() {
	if (todo.empty()) return NULL;
	return *(todo.begin());
}

/** Remove a transition from the todo list.
	@param t The transition for which work has been completed.
*/
void CNode::workDone(Transition& t) {
	todo.erase(&t);
}

/** Check if this node has the successor cnode via an edge labelled with transition t.
	@param t The transition.
	@param cnode The successor node.
	@return If the t-edge to cnode exists.
*/
bool CNode::hasSuccessor(Transition& t, CNode& cnode) {
	if (succ.find(&t)!=succ.end())
		if (succ[&t] == &cnode) return true;
	return false;
}

/** Get the set of successors from this subnode (non-inherited edges) with an edge labelled by a transition t.
	There may be more than one edge as the successor node may have been split while this subnode has not.
	@param t The transition.
	@return The set of successors if the subnode explicitly defines the edge.
*/
set<CNode*> CNode::getSuccessors(Transition& t) {
	set<CNode*> result; // the set of successors, empty at first
	if (succ.find(&t)==succ.end()) return result; // if the subnode does not define a t-edge (the root may still do)
	if (succ[&t]==NULL) return result; // if the subnode explicitly forbids a t-edge
	set<int> todo; // the subnodes of the global successor we have to check
	if (subsucc.find(&t)==subsucc.end()) todo.insert(0); // if t points to a global node, start at subnode zero
	else todo.insert(subsucc[&t]); // otherwise start where t points to
	while (!todo.empty()) { // now work on the list
		int n(*(todo.begin())); // get the first subnode index to check
		todo.erase(todo.begin()); // and remove it from the list
		result.insert(succ[&t]->leaf.at(n)); // insert the leaf pointer (possible NULL) into the result list
		if (succ[&t]->lson[n]>=0) todo.insert(succ[&t]->lson[n]); // add the left son to the todo list
		if (succ[&t]->rson[n]>=0) todo.insert(succ[&t]->rson[n]); // add the right son to the todo list
	}
	result.erase(NULL); // forget about those NULL pointers
	return result; // we have colleted all those leaf pointers
}

/** Get the successor map of this node.
	@return The map of successors.
*/
map<Transition*,CNode*> CNode::getSuccessors() { return succ; }

/** Get the subnode successor map of this node.
	@return The map of subnode indices.
*/
map<Transition*,int> CNode::getSubSuccessors() { return subsucc; }

/** Get the root successor of a (root or sub) node by an edge labelled with transition t.
	@param t The transition.
	@return The successor root node or NULL if non-existent.
*/
CNode* CNode::getSuccessor(Transition& t) {
	if (succ.find(&t)!=succ.end()) return succ[&t]; // if this node has a t-successor stored (even NULL)
	if (!root) return NULL; // if this is a root node, we are done, no edge
	if (root->succ.find(&t)!=root->succ.end()) return root->succ[&t]; // now check the root
	return NULL; // also no success, no edge
}

/** Get the subnode index this node points to via the edge labelled by transition t.
	@param t The transition with which the edge should be labelled.
	@return The index of the subnode to which the t-edge points, -1 in case there is no edge.
*/
int CNode::getSubSuccessor(Transition& t) {
	if (subsucc.find(&t)!=subsucc.end()) return subsucc[&t]; // if this node directly points to some index
	if (!root) return -1; // if we are a root, we don't point to a subnode
	if (root->subsucc.find(&t)!=root->subsucc.end()) return root->subsucc[&t]; // otherwise check our root
	return -1; // we either don't have a t-edge or don't point to a subnode but a root node
}

/** Get the full subnode list, it may contain any number of NULL pointers.
	@return The subnode list.
*/
vector<CNode*> CNode::getSubNodes() { return leaf; }

/** Get all subnodes in the subtree starting at an index number.
	@param index The index number of the subtree root, i.e. some subnode.
	@return All subnode leaves of the subtree.
*/
set<CNode*> CNode::getSubNodes(int index) {
	set<CNode*> result; // the resulting set
	if (index<0 || index>=pos) return result; // index out of bounds
	set<int> todo; // what subnodes we need to check
	todo.insert(index); // we begin with the given index
	while (!todo.empty()) { // as long as there is work
		int n(*(todo.begin())); // get the next index
		todo.erase(todo.begin()); // remove it from the list
		result.insert(leaf.at(n)); // add the leaf pointer (possibly NULL) belonging to the index to our result
		if (lson[n]>=0) todo.insert(lson[n]); // put the left son into the todo list if there is one
		if (rson[n]>=0) todo.insert(rson[n]); // put the right son into the todo list if there is one
	}
	result.erase(NULL); // remove that NULL pointer, it's nonsense
	return result;
}

/** Get the labels of all edges starting at this (root or sub-) node.
	@return The set of edge labels.
*/
set<Transition*> CNode::getEdgeLabels() {
	set<Transition*> tmp,result; // the result, initially empty
	map<Transition*,CNode*>::iterator cit;
	if (root) { // for subnodes, check the root's edges to successors and collect the labels
		for(cit=root->succ.begin(); cit!=root->succ.end(); ++cit)
			if (cit->second!=NULL) tmp.insert(cit->first); 
	}
	for(cit=succ.begin(); cit!=succ.end(); ++cit) // go through all edges
		if (cit->second==NULL) tmp.erase(cit->first); // remove a transition if the edge is forbidden
		else tmp.insert(cit->first); // but add it to the result if there is a successor node
	if (stubbornset.empty()) return tmp; // stubborn set is not used or empty, so return every edge
	for(unsigned int i=0; i<stubbornset.size(); ++i)
		if (tmp.find(stubbornset[i])!=tmp.end()) result.insert(stubbornset[i]); // only return edges that are in the stubborn set
	return result;
}

/** Split a node into two parts. Markings where a place p has less than token tokens go to one part
	(the left son), all other markings to the other part (right son). The nodes for the sons are
	created and this node is reverted to an inner (decision) node. The split fails (nothing is done)
	if all markings either have less than token tokens on p or if they all have at least token 
	tokens on p.
	@param p The place on which the split is based.
	@param token The number of tokens for the decision based on p.
	@param im The incidence matrix of the Petri net involved.
	@param sb The stubborn set structure for partial order reductions. NULL if no partial order
		reduction should occur.
	@return If the split was done.
*/
bool CNode::simpleSplit(Place& p, int token, IMatrix& im, StubbornSet* sb) {
	ExtMarking m1(em); // a copy of this node's extmarking
	if (!m1.splitExists(p,token)) return false; // if all markings would belong to the same son
	getRoot()->pcond[getPos()] = &p; // set this node up as a decision point for place p
	getRoot()->tcond[getPos()] = token; // with token as the threshold
	ExtMarking m2(m1.split(p,token)); // split the extmarking into two parts
	CNode* c1(new CNode(m1,*getRoot(),getPos(),true)); // and create a new node around each of them,
	CNode* c2(new CNode(m2,*getRoot(),getPos(),false)); // the new nodes becoming sons of the old one
	c1->succ = succ; // copy the successors
	c1->subsucc = subsucc; // and indices to the subnodes
	if (sb) c1->stubbornset = stubbornset; // and copy the stubborn set if used
	c2->succ = succ; // the same for the second node
	c2->subsucc = subsucc;
	if (sb) // for the lower son a new stubborn set has to be computed
	{ 
		vector<Transition*> tvec(c2->getStubbornSet(*sb,true)); // compute the stubborn set
		for(unsigned int i=0; i<tvec.size(); ++i) // check if the arcs for these transitions already exist
			if (getRoot()->succ.find(tvec[i])==getRoot()->succ.end())
				getRoot()->todo.insert(tvec[i]); // if not, put the transition into the todo list
	}
	map<Transition*,CNode*>::iterator cit; // forbid those edges in the lower/left son
	for(cit=c2->getRoot()->succ.begin(); cit!=c2->getRoot()->succ.end(); ++cit) 
		if (m2.isDisabled(*(cit->first),im)) // where the marking becomes insufficient
			c2->setSuccessor(*(cit->first),NULL); // so set the successor to NULL (forbidden)
	getRoot()->leaf[getPos()]=NULL; // this node is no longer a leaf, but a decision node
	return true;
}

/** If this node's t-successor has just been split and the node lies on the path we
	are interested in, we must split this node. 
	@param t The transition on the path between the two nodes.
	@param cn This node's successor that has just been split. Can be the same node.
	@param im The incidence matrix of the Petri net involved.
	@param sb The stubborn set structure if we use stubborn sets, NULL otherwise.
	@return If the split was successful. Note that only root nodes can be split.
*/
bool CNode::preSplit(Transition& t, CNode& cn, IMatrix& im, StubbornSet* sb) {
	if (root) return false; // did you forget this->getRoot()? This isn't allowed
	bool changed(false); // if we have done something to the node
	bool loop(this==cn.getRoot()); // if the t-edge is a loop
	set<CNode*> todo(getSubNodes(0)); // the whole partition of subnodes with marking subsets
	set<int> done; // to remember if a subnode is new or has recently been changed, in case of a loop only
	while (!todo.empty()) // do them all
	{
		CNode* act(*(todo.begin())); // get the first one from the list
		todo.erase(todo.begin()); // and remove it
		CNode* aimroot(getSuccessor(t)); // now get its root successor
		if (aimroot!=cn.getRoot()) continue; // are we the correct predecessor to split? If not, do nothing
		int subaim(0); // if so, compute the subnode index we aim at (default 0=root)
		if (act->succ.find(&t)!=act->succ.end()) // if we have a successor via t
		{
			if (act->succ[&t]==NULL) continue; // that is not forbidden
			if (act->subsucc.find(&t)!=act->subsucc.end()) // and if we aim at a subnode index
				subaim = act->subsucc[&t]; // get that index
		}
		if (aimroot->leaf[subaim]) continue; // if we point at a leaf: no need for splitting ourselves
		Place* p(aimroot->pcond[subaim]); // now we point at a decision, so get the place
		int num(aimroot->tcond[subaim]-im.getEntry(t,*p)); // and the token number, but minus what t does to the place (we are a t-predeccessor after all!)
		if (num<0) num=0; // less than zero tokens are impossible
		int actpos(act->root?act->pos:0); // get our own index before the split
		if (act->em.splitExists(*p,num)) // check if our subset of markings is splittable (into one set with p<num and one with p>=num)
		{	// if yes ...
			if (loop) if (done.find(subaim)!=done.end()) continue; // if the subnode we aim at is new, stop! no splitting!
			if (loop) done.insert(actpos); // this subnode becomes new now
			if (!act->simpleSplit(*p,num,im,sb)) continue; // do the split
			CNode* n1(leaf[lson[actpos]]); // get the new left son
			if (n1->em.isDisabled(t,im)) // if the transition t cannot fire
			{
				n1->setSuccessor(t,NULL); // forbid the t-edge
			} else {
				n1->setSuccessor(t,aimroot); // otherwise point it to our successor's subnode's
				n1->subsucc[&t] = aimroot->lson[subaim]; // left son!
				todo.insert(n1); // and mark it as "to do"
			}
			if (loop) done.insert(lson[actpos]); // the new left son is now also new :-)
			CNode* n2(leaf[rson[actpos]]); // now get the new right son
			n2->setSuccessor(t,aimroot); // which must enable t, so point it to our successor's
			n2->subsucc[&t] = aimroot->rson[subaim]; // subnode's right son
			if (loop) done.insert(rson[actpos]); // same for the other son
			todo.insert(n2); // and mark it as "to do"
			if (act->root) delete act; // if we are not root, delete our pointer as we are now just some inner node
		} else { // if we are unsplittable ...
			act->setSuccessor(t,aimroot); // we still aim at our successor, but the subnode aim is shifted:
			if (act->em.splitLowerExists(*p,num)) // if all our markings lead (via t) to the aim's left son 
				act->subsucc[&t] = aimroot->lson[subaim]; // we shift the edge to that left son
			else act->subsucc[&t] = aimroot->rson[subaim]; // otherwise, of course, to the right son
			if (!loop) todo.insert(act); // and we have to work on this node again (as the son may again be a decision point)
			else if (done.find(subaim)==done.end()) todo.insert(act); // but don't reenter this node if we have a loop and now point to a newly created aim
			if (loop) done.insert(actpos); // mark ourselves as newly created
		}
		changed = true; // if we came here we have at least shifted a pointer or may even have created new nodes
	}
	return changed;
}

/** Print all subnodes of this node's root.
	@param o The stream to print on.
	@param pord The ordering by which the places in a marking are sorted.
*/ 
void CNode::show(ostream& o, vector<Place*>& pord) {
	CNode* src(getRoot()); // get the root of this node
	for(unsigned int i=0; i<(unsigned int)(src->pos); ++i) // go through all subnodes
	{
		cout << "(" << i << "): "; // index of the subnode
		if (src->lson[i]>=0 || src->rson[i]>=0) // if this is a decision node, print the if-then-else
			cout << "if (" << src->pcond[i]->getName() << "<" << src->tcond[i] << ") ";
		if (src->lson[i]>=0)
			cout << "then (" << src->lson[i] << ") "; 
		if (src->rson[i]>=0)
			cout << "else (" << src->rson[i] << ") ";
		if (src->leaf[i]) src->leaf[i]->getMarking().show(o,pord); // otherwise print the extmarking
		cout << endl; 
	}
}

/** Get one leaf node that can be reached from this node via a t-edge.
	@param t The transition labeling the edge.
	@return A pointer to a leaf.
*/
CNode* CNode::getDefaultSubSuccessor(Transition& t) {
	CNode* src(this); // the node from which we start
	if (succ.find(&t)==succ.end()) // if we have no t-successor
	{
		if (!root) return NULL; // and are root, there is no result
		if (root->succ.find(&t)==root->succ.end()) return NULL; // same if our root has no t-edge
		src = root; // otherwise we start at our root
	}
	CNode* aim = src->succ[&t]; // where does our t-edge point to?
	if (src->subsucc.find(&t)==src->subsucc.end()) return aim; // if it's the root node, return it
	int subnode = src->subsucc[&t]; // otherwise get the subnode index we're aiming at
	while (!aim->leaf[subnode]) // as long as this is not a leaf
	{
		if (aim->rson[subnode]>=0) subnode=aim->rson[subnode]; // go to a son
		else subnode=aim->lson[subnode]; // it doesn't really matter which
	}
	return (aim->leaf[subnode]); // now we are at a leaf and return it
}

/** Get the index of this node inside its root node.
	@return The index.
*/
int CNode::getPos() { return (root?pos:0); }

/** If at an inner node in a subtree, decide which son contains the given marking m.
	@param subnode The index of an inner (decision) node in the subtree. If the marking
		m is not contained in this inner node, the results become unpredictable.
	@param m The marking to check for. This must be one single marking, no intervals
		or omegas, otherwise the results are unpredictable.
	@return The index of the son of the subnode that contains the marking or -1 if this
		node is not an inner node.
*/
int CNode::goToSon(int subnode, ExtMarking& m) {
	Place* p(pcond[subnode]); // get the place which is set only for decision nodes
	if (!p) return -1; // if it is not set, the marking belongs here (there is no elsewhere)
	return (m.getLowerBound(*p)<tcond[subnode] ? lson[subnode] : rson[subnode]);
	// check which son the marking belongs to and return its index
}

/** Check if the subnode's index in this root node points to a leaf in the root node's subtree.
	@param subnode The index of the subnode we are interested in.
	@return If the subnode is a leaf, thus having its own CNode structure.
*/
bool CNode::isLeaf(int subnode) {
	if (root) return false; // we must be a root node for this to work 
	if (subnode<0 || subnode>pos) return false; // if the index is out of bounds
	if (lson[subnode]>=0 || rson[subnode]>=0) return false; // are we an inner node?
	return true; // if not, we are a leaf
}

/** Get a subnode of this root node by its index in the subtree.
	@param index The subnode's index.
	@return The subnode-leaf or NULL if the subnode is an inner node or does not exist.
*/
CNode* CNode::getSubNode(int index) { return ((root||index<0||index>pos) ? NULL : leaf[index]); }

/** Set the stubborn set to a given vector of transitions (ordered by priority).
	@param stubset The vector of stubborn set transitions.
*/
void CNode::setStubbornSet(vector<Transition*> stubset) { stubbornset=stubset; 
//cout << "SB: ";
//getMarking().show(cout,po);
//cout << ": ";
//for(unsigned int i=0; i<stubbornset.size(); ++i)
//	cout << stubbornset[i]->getName() << ",";
//cout << endl;
}

/** Get/Compute the stubborn set for this node.
	@param stubset The Stubborn Set structure needed for computing stubborn sets.
	@param compute If the stubborn set should be computed. It will be computed anyway
		if there is no previously computed set.
	@return A vector of stubborn set transitions ordered by priorities (distance to
		the goal marking).
*/
vector<Transition*> CNode::getStubbornSet(StubbornSet& stubset, bool compute) {
	if (!compute && !stubbornset.empty()) return stubbornset;
	stubbornset = stubset.compute(em);
//cout << "SB: ";
//getMarking().show(cout,po);
//cout << ": ";
//for(unsigned int i=0; i<stubbornset.size(); ++i)
//	cout << stubbornset[i]->getName() << ",";
//cout << endl;
	return stubbornset;
}


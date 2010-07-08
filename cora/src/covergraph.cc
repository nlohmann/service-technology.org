// -*- C++ -*-

/*!
 * \file    covergraph.cc
 *
 * \brief   Class CoverGraph
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/14
 *
 * \date    $Date: 2010-05-14 14:47:01 +0200 (Fr, 16. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#include <deque>
#include "pnapi/pnapi.h"
#include "covergraph.h"
#include "rnode.h"
#include "cnode.h"
#include "extmarking.h"
#include "imatrix.h"

using pnapi::Marking;
using pnapi::PetriNet;
using std::deque;
using std::vector;
using std::cout;
using std::endl;

	/*************************************
	* Implementation of class CoverGraph *
	*************************************/

/** Standard constructor. 
	@param pn The Petri net for which the coverability graph is to be built.
	@param im The incidence matrix of the Petri net.
	@param m The marking of the initial node. If this node's marking should contain
		omegas, the node's ExtMarking can be changed after the constructor returns.
		This ExtMarking must remain regular, i.e. places may only be attributed with
		one single number of tokens or with omega (no other intervals).
*/ 
CoverGraph::CoverGraph(PetriNet& pn, IMatrix& im, Marking& m, StubbornSet* stubset) : net(pn),imat(im),sb(stubset) {
	init = new CNode(ExtMarking(m)); // construct the initial node
	if (sb) // if stubborn sets are used ...
	{
		vector<Transition*> stset(sb->compute(init->getMarking())); // compute one for the initial marking
		init->setStubbornSet(stset); // store the stubborn set
		init->addToFirables(stset); // make a todo list with the edges to successors
	}
	else init->computeFirables(pn,im); // otherwise make a todo list of all potential successors for the initial node
	nodes.insert(RNode(*init)); // add the node to a global set to prohibit duplicates
	pushToDo(init); // globally tag the initial node as "has not been done yet"
}

/** Destructor. */
CoverGraph::~CoverGraph() { // just free all the memory, including nodes and subnodes
	set<RNode>::iterator rit;
	for(rit=nodes.begin(); rit!=nodes.end(); ++rit) // go through the global node list
	{
		vector<CNode*> subnodes((*rit).cnode->getSubNodes()); // get the leaf subnodes, which still have their own memory
		if (subnodes.size()>1) // if there are leafs beside the root node
			for(unsigned int i=1; i<subnodes.size(); ++i) // go through them
				delete subnodes[i]; // and free them
		delete (*rit).cnode; // finally free the global (=root) node
	}
}

/** Check whether an edge labelled by transition t starting at the root node cn already
	exists or should be created. In the latter case, create such an edge and possibly
	the root node it should point to.
	@param cn The node from which the edge should emerge.
	@param t The transition that should fire and thus create the edge.
	@return If the edge was created.
*/
bool CoverGraph::createSuccessor(CNode& cn, Transition& t) {
	if (!cn.needsWork(t)) return false; // the t-successor has already been done
	cn.workDone(t); // tag the t-successor as done, though the work is really done after this point
	ExtMarking em(cn.getMarking()); // extract the active extmarking
	if (em.isDisabled(t,imat)) return false; // check if t can fire at all, if not, no successor
	em.successor(t,true,imat); // obtain the t-successor's extmarking
	bool newomega(cn.addOmega(em)); // and check if we need to set some numbers to omega (and do so)
	CNode* c(new CNode(em)); // create a new node from the extmarking
	RNode r(*c); // and also contain it in a root list entry to check if we already created it
	if (nodes.find(r)==nodes.end()) // if the extmarking does not exists in the coverability graph ...
	{
		if (sb) // check if we use stubborn sets, if so ...
		{
			vector<Transition*> stset(sb->compute(c->getMarking())); // compute one,
			c->setStubbornSet(stset); // store the stubborn set
			c->addToFirables(stset); // make a todo list containing the stubborn set (future edges)
		}
		else c->computeFirables(net,imat); // otherwise compute all its potential successors (for later work)
		nodes.insert(r); // add the root list node to the global list (for duplicate checks)
		pushToDo(c); // put the node into the global todo list
		cn.setSuccessor(t,c); // and finally add the edge to this new node
		return true;
	}
	CNode* c2((*(nodes.find(r))).cnode); // the node already exists, so get the original
	delete c; // and forget this copy
	if (cn.hasSuccessor(t,*c2)) return false; // if the edge already exists, do nothing
	cn.setSuccessor(t,c2); // otherwise add the edge
	return true;
}

/** Get the initial (root) node of the coverability graph.
	@return The node.
*/
CNode* CoverGraph::getInitial() { return init; }

/** Given a root node, create all necessary direct successors and the edges to them.
	@return If anything was done.
*/
bool CoverGraph::createSuccessors(CNode& cn) {
	Transition* t(cn.getWork()); // the next edge to work on
	if (t==NULL) return false; // everything already has been done
	while (t!=NULL) {
		createSuccessor(cn,*t); // possibly create the successor, create the edge, remove it from the todo list
		t = cn.getWork(); // check if another edge should be worked upon
	}
	return true;
}

/** Print the coverability graph.
	@param porder Ordering of the places for printing markings.
	@param rootonly Only print the original graph without node splitting.
*/
void CoverGraph::printGraph(vector<Place*>& porder, bool rootonly) {
	set<RNode>::iterator rit;
	for(rit=nodes.begin(); rit!=nodes.end(); ++rit) 
	{ // go through all the global nodes (with regular ExtMarkings)
		(*rit).cnode->getMarking().show(cout,porder); // print the marking
		set<CNode*> subnodes((*rit).cnode->getSubNodes(0)); // get the partition this ExtMarking was split into
		if (!rootonly && subnodes.size()!=1) // if this node was split and we should show subnodes
			cout << " has been split into:" << endl;
		else cout << endl;
		map<Transition*,CNode*> smap((*rit).cnode->getSuccessors()); // edges to the global successors
		map<Transition*,CNode*>::iterator sit;
		for(sit=smap.begin(); sit!=smap.end(); ++sit)
		{ // go through all the edges
			if (rootonly) { // if only the standard coverability graph is to be printed ...
				cout << " --" << sit->first->getName() << "-> "; // print the edge
				sit->second->getMarking().show(cout,porder); // and the node reached
				cout << endl;
				continue; // ... we are done with showing just THE successor
			}
			set<CNode*>::iterator cit;
			for(cit=subnodes.begin(); cit!=subnodes.end(); ++cit)
			{ // otherwise we have to go through the whole partition of subnodes
				map<Transition*,CNode*> submap((*cit)->getSuccessors()); // get their global successors
				map<Transition*,int> subsmap((*cit)->getSubSuccessors()); // and to which of the successor's subnodes the edge points to
				if (submap.find(sit->first)!=submap.end())
					if (submap[sit->first]==NULL) continue; // if the subnode forbids the edge that is globally possible, do nothing
				cout << "  ";
				if ((*cit)->getRoot()!=(*cit)) cout << "(" << (*cit)->getPos() << "): ";
				(*cit)->getMarking().show(cout,porder); // otherwise print the subnode's index number and the submarking
				cout << " --" << sit->first->getName() << "-> "; // print the edge label
				if (subsmap.find(sit->first)==subsmap.end())
				{
					sit->second->getMarking().show(cout,porder);
					cout << endl; // if the edge leads to a global node, print that one
					continue;
				}
					cout << "["; // otherwise print the global node and its subnode index 
					sit->second->getMarking().show(cout,porder); // where the edge points to
					cout << "](" << subsmap[sit->first] << "): ";
				int n=subsmap[sit->first];
				set<CNode*> gnodes(sit->second->getSubNodes(n)); // the subnode itself may also have been split (being an inner node)
				if (gnodes.size()!=1) cout << endl;
				set<CNode*>::iterator git;
				for(git=gnodes.begin(); git!=gnodes.end(); ++git)
				{	// so print all the possible submarkings of the subnode, the edge leads to all of them
					if (gnodes.size()!=1) cout << "     -> ";
					(*git)->getMarking().show(cout,porder);
					cout << endl;
				}
			}
		}
	}
}

/** Create the full coverability graph.
*/
void CoverGraph::completeGraph() {
	while (!todo.empty()) // as long as not all nodes have been worked upon
	{
		CNode* c(firstToDo()); // take the first node in the list
		popToDo(); // remove it from the list
		createSuccessors(*c); // and possibly create its successors
	}
}

/** Add the successors of one node to the coverability graph.
	@return If there was some node to work upon.
*/
bool CoverGraph::completeOneNode() {
	if (todo.empty()) return false; // nothing to be done, there are no new nodes
	CNode* c(firstToDo()); // take the first new node
	popToDo(); // remove it from the todo list
	createSuccessors(*c); // and possibly create its successors
	return true;
}

/** Create all nodes necessary for a given sequence of transitions.
	@param tvec The sequence of edge labels that should form a path in the graph.
	@return If the path could be constructed.
*/
bool CoverGraph::createPath(vector<Transition*> tvec) {
	CNode* act(init); // begin at the initial node
	for(unsigned int i=0; i<tvec.size(); ++i)
	{
		createSuccessors(*act); // create all its successors
		popToDo(act); // remove the node from the todo list
		act = act->getSuccessor(*(tvec[i])); // get the next node along the given path
		if (!act) return false;	// if the path does not continue, give up
	}
	return true; // the path was created
}

/** Check if a given path (global nodes only) exists in the coverability graph.
	@param tvec The path to check for.
	@return If the path exists.
*/
bool CoverGraph::checkPath(vector<Transition*> tvec) {
	CNode* act(init); // get the initial node
	for(unsigned int i=0; i<tvec.size(); ++i)
	{
		act = act->getSuccessor(*(tvec[i])); // get the successor along the given path
		if (!act) return false; // if the path does not continue, give up
	}
	return true; // the path exists
}

/** Find a path to the subnode cn in the coverability graph.
	@param cn The subnode to which the path should lead.
	@return A path in form of a sequence of transitions if one was found.
		Otherwise, a deque(vector) containing a single NULL pointer.
*/ 
deque<Transition*> CoverGraph::findPath(CNode& cn) {
	if (init==&cn) { deque<Transition*> result; return result; } // in case we look for the initial node
	map<CNode*,Transition*> pret; // for each node: an label of an edge leading into the node
	map<CNode*,CNode*> pren; // for each node: a predecessor of the node using the above edge
	set<CNode*> go; // a set of nodes to work on
	go.insert(init); // beginning with just the initial node
	bool found(false); // if we have found the path we are looking for
	while (!go.empty() && !found) // as long as there is work to do, go on ...
	{
		set<CNode*> next; // the next set of nodes to work upon, initially empty
		while (!go.empty() && !found) // the same condition as above
		{
			CNode* act(*(go.begin())); // take the first node from our list
			go.erase(go.begin()); // and remove it
			set<Transition*> edges(act->getEdgeLabels()); // get all labels of edges starting at this node
			set<Transition*>::iterator eit;
			for(eit=edges.begin(); eit!=edges.end(); ++eit) // and inspect them
			{
				CNode* nx(act->getSuccessor(**eit)); // get the global successor using an edge
				int subnx(act->getSubSuccessor(**eit)); // and the subnode index the edge points to
				if (subnx<0) subnx=0; // patch if the subnode index points to the global node itself
				set<CNode*> snnx(nx->getSubNodes(subnx)); // get the subnode set the index points to
				set<CNode*>::iterator cit;
				for(cit=snnx.begin(); cit!=snnx.end(); ++cit) // and walk through this set
				{
					if (pret.find(*cit)==pret.end()) // if the global successor has not been visited yet
					{
						pret[*cit] = *eit; // point it to its predecessor via the edge
						pren[*cit] = act; // forming a path to the initial node
						next.insert(*cit); // and tag this node as todo, but only in the next run of the loop
					}
					if (*cit==&cn) { found=true; break; } // if we reach the node we were looking for, stop
					else if (cn.getRoot()==&cn && (*cit)->getRoot()==&cn) { found==true; break; }
					// also stop if we were looking for a global node of which this node is a subnode
				}
				if (found) break; // leave the loops if we have found a path
			}
		}
		go.swap(next); // we are through with our set of nodes, but there may be new work
	}
	deque<Transition*> result; // now construct the path
	if (!found) { result.push_back(NULL); return result; } // unless we haven't found one
	CNode* follow(&cn); // by following the pret/pren maps backwards through the graph
	while (follow!=init) { // until we reach the initial node
		result.push_front(pret[follow]); // put the node at the beginning of our path (backwards construction)
		follow = pren[follow]; // and get the predecessor node
	}
	return result;
}

/** Find a path to an extended marking in the coverability graph.
	@param em An extended marking (set of markings) of which one marking should be reached.
	@return A path in form of a sequence of transitions if one was found.
		Otherwise, a deque(vector) containing a single NULL pointer.
*/ 
deque<Transition*> CoverGraph::findPath(ExtMarking& em) {
	map<CNode*,Transition*> pret; // for each node: an edge label leading into the node
	map<CNode*,CNode*> pren; // for each node: a predecessor of the node using the above edge
	set<CNode*> go; // a set of nodes to work on
	go.insert(init); // beginning with just the initial node
	bool found(false); // if we have found the path we are looking for
	CNode* follow; // then we need to know which node we reached
	// first check if the initial node is already our aim ...
	if (init->getMarking().hasIntersectionWith(em)) { found=true; follow=init; }
	while (!go.empty() && !found) // as long as there is work to do, go on ...
	{
		set<CNode*> next; // the next set of nodes to work upon, initially empty
		while (!go.empty() && !found) // the same condition as above
		{
			CNode* act(*(go.begin())); // take the first node from our list
			go.erase(go.begin()); // and remove it
			set<Transition*> edges(act->getEdgeLabels()); // get all labels of edges starting at this node
			set<Transition*>::iterator eit;
			for(eit=edges.begin(); eit!=edges.end(); ++eit) // and inspect them
			{
				CNode* nx(act->getSuccessor(**eit)); // get the global successor using an edge
				int subnx(act->getSubSuccessor(**eit)); // and the subnode index the edge points to
				if (subnx<0) subnx=0; // patch if the subnode index points to the global node itself
				set<CNode*> snnx(nx->getSubNodes(subnx)); // get the subnode set the index points to
				set<CNode*>::iterator cit;
				for(cit=snnx.begin(); cit!=snnx.end(); ++cit) // and walk through this set
				{
					if (pret.find(*cit)==pret.end()) // if the global successor has not been visited yet
					{
						pret[*cit] = *eit; // point it to its predecessor via the edge
						pren[*cit] = act; // forming a path to the initial node
						next.insert(*cit); // and tag this node as todo, but only in the next run of the loop
					}
					if ((*cit)->getMarking().hasIntersectionWith(em)) { found=true; follow=*cit; break; } // if we reach the node we were looking for, stop
				}
				if (found) break; // leave the loops if we have found a path
			}
		}
		go.swap(next); // we are through with our set of nodes, but there may be new work
	}
	deque<Transition*> result; // construct the path
	if (!found) { result.push_back(NULL); return result; } // unless we haven't found one
	// follow the pret/pren maps backwards through the graph
	while (follow!=init) { // until we reach the initial node
		result.push_front(pret[follow]); // put the node at the beginning of our path (backwards construction)
		follow = pren[follow]; // and get the predecessor node
	}
	return result; // the path is complete
}

/** Check if a given transition sequence in the coverability graph is firable and
	split the nodes up to the first non-firable transition if there is one. If the
	path is fully firable, check if we reach the goal marking. If not, split the
	whole path to distinguish reached and goal marking.
	@param path The sequence of transitions to fire.
	@return Whether the path is firable and leads to the goal marking. If not, 
		the path either does not exist (nothing is done) or it is just not firable 
		(some nodes are split).
*/ 
bool CoverGraph::splitPath(deque<Transition*>& path) {
	CNode* act(init); // start at the initial node
	vector<CNode*> rpath; // store the root nodes of the path
	vector<int> spath; // and the subnode index numbers defining the path
	ExtMarking m(act->getMarking()); // to check enabledness we need the marking
	unsigned int i;
	int snx(0); // a subnode index
	for(i=0; i<path.size(); ++i) // now step through the path
		if (m.isDisabled(*(path[i]),imat)) break; // if the transition on the path cannot fire, stop
		else {
			m.successor(*(path[i]),true,imat); // otherwise obtain the next "real" marking
			CNode* nx(act->getSuccessor(*(path[i]))); // and find the successor node
			if (nx==NULL) return false; // fail if the path does not exist
			snx = act->getSubSuccessor(*(path[i])); // find the subnode index the edge points to
			if (snx<0) snx=0; // patch it in case of a global node
			while (!nx->getRoot()->isLeaf(snx)) { // find the subnode to which the "real" marking belongs
				snx = nx->getRoot()->goToSon(snx,m); // (the "real" marking therefore must not contain omegas, otherwise both sons could be right!)
			}
			rpath.push_back(act->getRoot()); // add its root node to the root path
			spath.push_back((act->isRoot()?0:act->getPos())); // and its subnode index to the subnode index path
			act = nx->getSubNode(snx); // and make it the new node to work on
		}
	Place* p; // a place at which to potentially cut the reached node
	int token; // the lowest token number for the place p such that a marking goes to the upper portion of the cut
	if (i==path.size()) // next we need to check whether a final marking was reached
	{ // all transitions on the path were firable, so we reached the goal node, but did we also reach the goal marking?
		if (!sb) return true; // if we have no final marking (just a node), we are done
		p = m.distinguish(sb->getGoal()); // check if reached and goal marking are identical
		if (!p) return true; // if the path was fully firable and the goal is reached, no split is necessary
		token = sb->getGoal()[*p]; // we cut the node below the goal token number ...
		if (!m.lessThanOn(sb->getGoal(),*p)) ++token; // unless the reached marking is higher, then cut above it
	} else { // some transition on the path was not firable
		p = act->getMarking().findSplitPlace(*(path[i]),imat); // find a place where the non-firable transition needed more tokens
		token = imat.getPreset(*(path[i]))[p]; // and get the number of tokens it actually needed (for the split)
	}
	CNode* actroot(act->getRoot()); // the root node of the node to split
	if (act->simpleSplit(*p,token,imat,sb)) // split the node where the transition could not fire / reached and goal marking differed
		if (!act->isRoot()) delete act; // delete the node structure on successful split of a subnode
	createSuccessors(*actroot); // check if there should be new arcs, as a split can lead to new transitions in stubborn sets
	unsigned int j(i-1);
	if (i>0) do { // then go backwards along our path and split the predecessor nodes
		if (!rpath[j]->isLeaf(spath[j])) break; // in case we have already split the previous node and deleted it, stop
		rpath[j]->preSplit(*(path[j]),*actroot,imat,sb); // split this node according to the newly split successors
		actroot = rpath[j]; // and go to the next predecessor on the path
		createSuccessors(*actroot); // check if the possibly changed stubborn sets (from the split) induce new arcs
	} while (j--!=0); // go on until the initial node has (possibly) been split
	return false;
}

/** Get the set of all root nodes.
	@return The set of root nodes (encapsulated).
*/
set<RNode>& CoverGraph::getAllNodes() { return nodes; }

/** Get the first node in the todo list, i.e. for which successor need to be checked/created.
	@return A node that needs to be worked upon. NULL if there is no such node.
*/
CNode* CoverGraph::firstToDo() { 
	if (todo.empty()) return NULL;
	return *(todo.begin()->second.begin()); 
}

/** Remove the first node from the todo list.
	@return If a node was removed.
*/
bool CoverGraph::popToDo() {
	if (todo.empty()) return false;
	todo.begin()->second.erase(todo.begin()->second.begin());
	if (todo.begin()->second.empty()) todo.erase(todo.begin());
	return true;
}

/** Remove a given node from the todo list.
	@param cn The node which should be removed.
	@return If the node actually has been removed from the todo list.
*/
bool CoverGraph::popToDo(CNode* cn) {
	if (todo.empty()) return false;
	map<int,set<CNode*> >::iterator mit;
	for(mit=todo.begin(); mit!=todo.end(); ++mit)
		if (mit->second.find(cn)!=mit->second.end())
		{
			mit->second.erase(cn);
			break;
		}
	if (mit==todo.end()) return false;
	if (mit->second.empty()) todo.erase(mit->first);
	return true;
}

/** Push a node into the todo list. If stubborn sets are used, the list will be sorted
	according to the distance of the node's marking to the goal marking, otherwise the
	node will just be appended to the list.
	@param cn The node to add to the todo list.
*/
void CoverGraph::pushToDo(CNode* cn) {
	if (sb) todo[cn->getMarking().distanceTo(sb->getGoal())].insert(cn);
 	else if (todo.empty()) todo[0].insert(cn);
	else todo[(todo.rend()->first)+1].insert(cn);
}

/** Enable or disable (set to NULL) the stubborn set method.
	@param A pointer to a StubbornSet object implementing the stubborn set methods.
		NULL to disable the stubborn set methods.
*/
void CoverGraph::useStubbornSetMethod(StubbornSet* stubset) { sb=stubset; }


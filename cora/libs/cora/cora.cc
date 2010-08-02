// -*- C++ -*-

/*!
 * \file    cora.cc
 *
 * \brief   Class Cora
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/07/30
 *
 * \date    $Date: 2010-07-30 12:00:00 +0200 (Fr, 30. Jul 2010) $
 *
 * \version $Revision: 0.3 $
 */

#include "cora.h"
#include "rnode.h"

	/********************************
	* Implementation of class RNode *
	********************************/

/** Standard constructor. */ 
Cora::Cora(PetriNet& net, Marking& m0, const vector<Place*>& porder, const vector<Transition*>& torder) : pn(&net), init(&m0), pord(porder), tord(torder), im(net) {
	debug = false;
	final2 = NULL;
	stubset = NULL;
	cg = NULL;
	loops = 0;
	aim = NULL;
	final = NULL;
}

/** Destructor. */
Cora::~Cora() {
	delete final2;
	delete stubset;
	delete cg;
}

/** Setter method for debug mode.
	@param d If the debug output should be printed.
*/
void Cora::setDebug(bool d) { debug=d; }

/** Main functionality for testing if a marking can be reached and finding 
	a firing sequence to that marking.
	@param goal The marking to reach.
	@return A firing sequence to the marking. If no firing sequence exists, the deque
		contains exactly one NULL pointer (since an empty deque represents a solution - 
		the empty firing sequence).
*/
deque<Transition*> Cora::findReach(Marking& goal) {
	final = &goal;
	delete final2;
	final2 = new ExtMarking(*final);
	if (!stubset) stubset = new StubbornSet(tord,*final,im,false); // obtain the stubborn set methods for reachability
	else { stubset->setGoalMarking(*final); stubset->setCover(false); } // or just set some parameters
	if (!cg) cg = new CoverGraph(*pn,im,*init,stubset); // create a new coverability graph if it does not exist
	else cg->useStubbornSetMethod(stubset);
	return solve(REACH);
}

/** Main functionality for testing if a marking can be covered and finding 
	a firing sequence to a covering marking.
	@param goal The marking to cover.
	@return A firing sequence to a covering marking. If no firing sequence exists (i.e. the
		marking cannot be covered), the deque contains exactly one NULL pointer (since an 
		empty deque represents a solution - the empty firing sequence).
*/
deque<Transition*> Cora::findCover(Marking& goal) {
	final = &goal;
	delete final2;
	final2 = new ExtMarking(*final);
	final2->setUnlimited(pn->getPlaces()); // declare all token numbers as lower bound
	if (!stubset) stubset = new StubbornSet(tord,*final,im,true); // obtain the stubborn set methods for coverability
	else { stubset->setGoalMarking(*final); stubset->setCover(true); } // or just set some parameters
	if (!cg) cg = new CoverGraph(*pn,im,*init,stubset); // create a new coverability graph if it does not exist
	else cg->useStubbornSetMethod(stubset);
	return solve(COVER);
}

/** Main functionality for testing if a firable path to some node in the coverability graph
	can be found. The node is given by a path leading to it.
	@param path A (typically nonfirable) path that may lead to some node in the coverability graph.
	@return A firing sequence leading to the same node as the path. If the given path does not
		exist in the coverability graph, the deque contains exactly two NULL pointers.
*/
deque<Transition*> Cora::findPath(vector<Transition*>& path) {
	if (!cg) cg = new CoverGraph(*pn,im,*init,NULL); // create the coverability graph with one (initial) node
	else cg->useStubbornSetMethod(NULL);
	if (!cg->createPath(path)) { // create the path formed by the given sequence in the coverability graph
		deque<Transition*> solution;
		solution.push_back(NULL);
		solution.push_back(NULL); // two null pointers meaning: no solution, path does not exist
		return solution;
	}
	// Since the path exists, we can now obtain the last node on this path, which is our aim
	aim = cg->getInitial();
	for(unsigned int i=0; i<path.size(); ++i)
		aim = aim->getSuccessor(*(path[i]));
	return solve(PATH);
}

/** Common functionality for all problems to be solved.
	@param mode The kind of problem: PATH, REACH, or COVER.
	@return A firing sequence solving the problem (or one or two NULL pointers).
*/
deque<Transition*> Cora::solve(int mode) {
	deque<Transition*> path; // for storing a path in the coverability graph
	bool done(false); // if we are done, i.e. the path is firable
	loops = 0; // just for counting
	while (!done) {
		if (mode!=PATH) path = cg->findPath(*final2); // find a shortest path to our final marking
		else path = cg->findPath(*aim); // or find a shortest path to our goal node
		if (debug) { // print the graph as it looks now
			cout << "cora: Graph Structure ";
			if (loops==0) cout << "BEFORE "; else cout << "AFTER ";
			cout << "split:" << endl;
			cg->printGraph(pord,false);
			cout << endl;
			cout << "Path to Aim: "; // print the path chosen
			for(unsigned int i=0; i<path.size(); ++i)
				if (path[i]) cout << path[i]->getName() << " "; else cout << "NULL ";
			cout << endl;
		}
		if (path.size()==0) done = cg->splitPath(path); // just check for the final marking if it is within the initial node
		else if (path.at(0)==NULL) // no path was found, we need to add new nodes/edges
		{
			if (debug) cout << "cora: Adding new successors" << endl;
			if (!cg->completeOneNode()) break; // stop if there is no new node, we have failed to find a solution
		}
		else if (mode==COVER) done=true; // if we just want to cover, we already found our basic path, so we quit here
		else done = cg->splitPath(path); // there is a path, so split the nodes along the chosen path
		// either the path becomes/is firable or it is destructed and cannot be found again
		++loops;
	}
	if (done && mode==COVER) path = cg->pumpPath(*init,path,*final); // pump up the path so it becomes firable
	return path;
}

/** Prints the built portion of the coverability graph.
	@param root If only the root nodes should be printed (false = also subnodes are printed).
*/
void Cora::printGraph(bool root) {
	cg->printGraph(pord,root);
	cout << "Initial Node: ";
	cg->getInitial()->getMarking().show(cout,pord);
	cout << endl;
}

/** Completes any partially built coverability graph to a full one. */
void Cora::buildFullGraph() {
	if (!cg) cg = new CoverGraph(*pn,im,*init,NULL); // create the coverability graph with one (initial) node
	else cg->useStubbornSetMethod(NULL); // we need all successors everywhere
	set<RNode>& allnodes(cg->getAllNodes());
	set<RNode>::iterator rit;
	for(rit=allnodes.begin(); rit!=allnodes.end(); ++rit)
	{
		cg->pushToDo((*rit).cnode); // put out an order to revisit all nodes
		(*rit).cnode->computeFirables(*pn,im); // and all edges from there
	}
	cg->completeGraph(); // now complete the graph
}

/** Prints info on how many paths in the coverability graph were tested until a solution
	was found.
	@param mode The kind of problem that was solved: PATH, REACH, or COVER.
*/
void Cora::printStatus(int mode) {
	cout << loops << " paths to ";
	if (mode!=PATH) 
	{ 
		cout << (mode==COVER?"a ":"the ") << "marking " << (mode==COVER?"in ":""); 
		final2->show(cout,pord); 
	}
	else { cout << "the node "; aim->getMarking().show(cout,pord); }
	cout << " were investigated." << endl;
}


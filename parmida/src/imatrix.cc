// -*- C++ -*-

/*!
 * \file    imatrix.cc
 *
 * \brief   Class IMatrix
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/14/07
 *
 * \date    $Date: 2013-06-26 12:00:00 +0200 (Mi, 26. Jun 2013) $
 *
 * \version $Revision: 1.00 $
 */

#include <set>
#include <map>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "cmdline.h"
#include "imatrix.h"
#include "rules.h"
#include <pthread.h>
#include "Runtime.h"

using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;
using pnapi::Arc;
using pnapi::Node;
using pnapi::Port;
using pnapi::Label;
using pnapi::formula::Negation;
using pnapi::formula::Conjunction;
using pnapi::formula::Disjunction;
using pnapi::formula::FormulaTrue;
using pnapi::formula::FormulaFalse;
using pnapi::formula::FormulaEqual;
using pnapi::formula::FormulaNotEqual;
using pnapi::formula::FormulaGreaterEqual;
using pnapi::formula::FormulaLessEqual;
using pnapi::formula::FormulaGreater;
using pnapi::formula::FormulaLess;
using pnapi::formula::Formula;
using std::set;
using std::map;
using std::vector;
using std::cerr;
using std::endl;

	/*****************************************************
	* Implementation of the methods of the class IMatrix *
	*****************************************************/

/** Constructor
	@param pn The Petri net for which the incidence matrix is to be built.
	@param log The JSON object saving the reduction properties
*/
IMatrix::IMatrix(PetriNet& pn, JSON& log) : petrinet(pn) {

	// for iteration over the places/transitions of the Petri net
	set<Transition*> tset(petrinet.getTransitions());
	set<Place*> pset(petrinet.getPlaces());
	set<Place*>::iterator it;
	const map<string,Port*>& portmap(petrinet.getInterface().getPorts());
	map<string,Port*>::const_iterator cpit;
	Vertex id;
	numtransitions = tset.size();
	numplaces = pset.size();
	numiolabels = 0;
	for(cpit=portmap.begin(); cpit!=portmap.end(); ++cpit)
		numiolabels += petrinet.getInterface().getInputLabels(*(cpit->second)).size()
					+ petrinet.getInterface().getOutputLabels(*(cpit->second)).size();
	numplaces += numiolabels;
	orignodes = numplaces+numtransitions;

	// make room for some additional transitions (may happen in some reductions)
	unsigned int numnodes(numplaces+2*numtransitions);

	// general locks for labels and new transitions
	pthread_rwlock_init(&labellock, NULL);
	pthread_rwlock_init(&unusedlock, NULL);

	// pre-/postset maps and initial marking
	pre = new Map[numnodes];
	post = new Map[numnodes];
    marking = new unsigned int[numnodes];

	// pointers from nodes to the PNAPI PetriNet object
	idvec = (Node**) malloc(SIZEOF_VOIDP * numnodes);

	// IO labels
	iovec = (Label**) malloc(SIZEOF_VOIDP * numiolabels);
	iotype = (unsigned int*) malloc(sizeof(unsigned int) * numiolabels);

	// space for names, action labels, and visibility status of nodes
	visibility = (unsigned int*) calloc(numnodes, sizeof(unsigned int));
	label = (unsigned int*) calloc(numnodes, sizeof(unsigned int));
	name = new string[numnodes];

	// locks for nodes and their checked reduction rules
	writing = (bool*) calloc(numnodes, sizeof(bool));
    rwlocks = (pthread_rwlock_t*) calloc(numnodes, sizeof(pthread_rwlock_t));
	modelocks = (pthread_rwlock_t*) calloc(numnodes, sizeof(pthread_rwlock_t));
    for (unsigned int i = 0; i < numnodes; ++i)
	{
        pthread_rwlock_init(rwlocks+i, NULL);
        pthread_rwlock_init(modelocks+i, NULL);
	}
	modes = (Mode*) calloc(numnodes, sizeof(uint32_t));

	// pointer from nodes to their equivalence class (regarding pre-/postset sizes), -1 = deleted node
	list = (int*) malloc(sizeof(int) * numnodes);

	// timestamps for nodes
	timestamp = (unsigned int*) malloc(sizeof(unsigned int) * numnodes);

	// locks for the equivalence classes list
    listlocks = (pthread_rwlock_t***) malloc(sizeof(pthread_rwlock_t**) * 2);
    listlocks[PL] = (pthread_rwlock_t**) malloc(sizeof(pthread_rwlock_t*) * (NODE_SET_LIMIT+1));
    listlocks[TR] = (pthread_rwlock_t**) malloc(sizeof(pthread_rwlock_t*) * (NODE_SET_LIMIT+1));
    for (unsigned int i=0; i <= NODE_SET_LIMIT; ++i)
    {
        listlocks[PL][i] = (pthread_rwlock_t*) calloc(NODE_SET_LIMIT+1, sizeof(pthread_rwlock_t));
        listlocks[TR][i] = (pthread_rwlock_t*) calloc(NODE_SET_LIMIT+1, sizeof(pthread_rwlock_t));
        for(unsigned int j=0; j <= NODE_SET_LIMIT; ++j)
        {
            pthread_rwlock_init(&listlocks[PL][i][j], NULL);
            pthread_rwlock_init(&listlocks[TR][i][j], NULL);
        }
    }

	// go through all input/output labels
	unsigned int lid(0);
	for(cpit=portmap.begin(); cpit!=portmap.end(); ++cpit)
	{
		set<Label*>::const_iterator sit;
		const set<Label*>& ilab(petrinet.getInterface().getInputLabels(*(cpit->second)));
		for(sit=ilab.begin(); sit!=ilab.end(); ++sit)
		{
			name[lid] = (*sit)->getName();
			iovec[lid] = *sit;
			timestamp[lid] = 1;
			ioToID[*sit] = lid;
//			visibility[lid] = PERSISTENT;
			iotype[lid] = INPUT;
			modes[lid] = Rules::ALLMODES;

			// insert the new place into a list according to its postset size
			unsigned int tmp((*sit)->getTransitions().size());
	        if (tmp > NODE_SET_LIMIT) tmp = NODE_SET_LIMIT;
	        list[lid] = tmp;
	        nodelists[PL][0][tmp].insert(lid);

			++lid;
		}	
		const set<Label*>& olab(petrinet.getInterface().getOutputLabels(*(cpit->second)));
		for(sit=olab.begin(); sit!=olab.end(); ++sit)
		{
			name[lid] = (*sit)->getName();
			iovec[lid] = *sit;
			timestamp[lid] = 1;
			ioToID[*sit] = lid;
//			visibility[lid] = PERSISTENT;
			iotype[lid] = OUTPUT;
			modes[lid] = Rules::ALLMODES;

			// insert the new place into a list according to its preset size
			unsigned int tmp((*sit)->getTransitions().size());
	        if (tmp > NODE_SET_LIMIT) tmp = NODE_SET_LIMIT;
	        list[lid] = tmp * (NODE_SET_LIMIT+1);
	        nodelists[PL][tmp][0].insert(lid);

			++lid;
		}	
	}

	// get the initial marking
    Marking m(pn,false);

	// go through all places
	unsigned int tidcnt(numplaces); // counting transition IDs
	for(it=pset.begin(), id=numiolabels; it!=pset.end(); ++it,++id)
	{
		// save ID and name
		idvec[id] = *it;
		nodeToID[*it] = id;
		name[id] = (*it)->getName();

		// for the postset of this place:
		set<Arc*>::iterator ait;
		set<Arc*> arcs = (*it)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected transitions with their arc weight
			Transition* t = &((*ait)->getTransition());
			int postweight = (*ait)->getWeight();
			unsigned int tid(nodeToID[t]);
			if (!tid) { // if we encounter the transition for the first time
				tid=nodeToID[t]=tidcnt++;
				idvec[tid] = t;
			}

			// save the arc weight for the transition as well as the place
			pre[tid][id] = postweight;
			post[id][tid] = postweight;
			// possibly to the JSON properties object as well
			if (Runtime::args_info.complexpath_given)
				log["xchange"][name[id]][t->getName()] = -(int)postweight;
		}

		// now for the preset of the place:
		arcs = (*it)->getPresetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected transitions with their arc weight
			Transition* t = &((*ait)->getTransition());
			int preweight = (*ait)->getWeight();
			unsigned int tid(nodeToID[t]);
			if (!tid) { // not yet existing?
				tid=nodeToID[t]=tidcnt++;
				idvec[tid] = t;
			}

			// and save the weight for the transition as well as the place
			post[tid][id] = preweight;
			pre[id][tid] = preweight;
			if (Runtime::args_info.complexpath_given) {
				if (log["xchange"][name[id]][t->getName()].type() == JSON::number)
					log["xchange"][name[id]][t->getName()] = (int)preweight + (int)log["xchange"][name[id]][t->getName()];
				else log["xchange"][name[id]][t->getName()] = (int)preweight;
			}
		}

        // obtain the initial marking on the place
        marking[id] = m[**it];
		if (Runtime::args_info.complexpath_given)
			if (marking[id]>0) log["xchange"][name[id]]["*init*"] = (int)marking[id];

		// insert the place into a list according to its pre/postset sizes
		unsigned int tmp1(pre[id].size());
		unsigned int tmp2(post[id].size());
        if (tmp1 > NODE_SET_LIMIT) tmp1 = NODE_SET_LIMIT;
        if (tmp2 > NODE_SET_LIMIT) tmp2 = NODE_SET_LIMIT;
        list[id] = tmp1 * (NODE_SET_LIMIT+1) + tmp2;
        nodelists[PL][tmp1][tmp2].insert(id);

		// lowest timestamp
		timestamp[id] = 1;
	}

	// check for unconnected transitions
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		if (nodeToID.find(*tit)==nodeToID.end())
		{
			idvec[tidcnt] = *tit;
			nodeToID[*tit] = tidcnt++;
		}

	// list transitions according to pre/postset sizes
	for(id=numplaces; id<numplaces+numtransitions; ++id)
	{
		// connect the transition to its IO label places
		set<Label*> ilab(((Transition*)idvec[id])->getInputLabels());
		set<Label*> olab(((Transition*)idvec[id])->getOutputLabels());
		set<Label*>::iterator sit2;
		for(sit2=ilab.begin(); sit2!=ilab.end(); ++sit2)
		{
			map<Label*,unsigned int>::const_iterator lit(((Transition*)idvec[id])->getLabels().find(*sit2));
			pre[id][ioToID[*sit2]] = lit->second;
			post[ioToID[*sit2]][id] = lit->second;
		}
		for(sit2=olab.begin(); sit2!=olab.end(); ++sit2)
		{
			map<Label*,unsigned int>::const_iterator lit(((Transition*)idvec[id])->getLabels().find(*sit2));
			post[id][ioToID[*sit2]] = lit->second;
			pre[ioToID[*sit2]][id] = lit->second;
		}

		// make transition unremovable if it has a sync label
		if (((Transition*)idvec[id])->isSynchronized())
			visibility[id] = PERSISTENT;

		// calculate the pre/postset sizes
		unsigned int tmp1(pre[id].size());
		unsigned int tmp2(post[id].size());
        if (tmp1 > NODE_SET_LIMIT) tmp1 = NODE_SET_LIMIT;
        if (tmp2 > NODE_SET_LIMIT) tmp2 = NODE_SET_LIMIT;
        list[id] = tmp1 * (NODE_SET_LIMIT+1) + tmp2;
        nodelists[TR][tmp1][tmp2].insert(id);
		timestamp[id] = 1;

		// get action label info if the user specified any visible labels
		if (Runtime::args_info.label_given) {
			const set<string>& roles(((Transition*)idvec[id])->getRoles());
			if (!roles.empty()) {
				label[id] = labelnum[*(roles.begin())];
				if (label[id]==0) {
					label[id] = labelnum.size();
					labelnum[*(roles.begin())] = label[id];
				}
				labelcount[label[id]]++;
				labeltext[label[id]] = *(roles.begin());
			}
		}

		// copy the name of the transition
		name[id] = idvec[id]->getName();
	}

	// find out which labels are visible in formulas
	for(unsigned int i=0; i<Runtime::args_info.label_given; ++i)
	{
		unsigned int j(labelnum[Runtime::args_info.label_arg[i]]);
		if (j>0) labelvis[j] = true;
	}
	// the tau/lambda label
	labeltext[0] = "";

	// check if the final condition of the net has to be respected
	if (Runtime::args_info.final_given)
	{
		set<const Place*> cpset(petrinet.getFinalCondition().concerningPlaces());
		set<const Place*>::iterator cpit;
		for(cpit=cpset.begin(); cpit!=cpset.end(); ++cpit)
			visibility[nodeToID[*cpit]] = VISIBLE;
	}

	// exclude "new" transitions until they are needed
	for(unsigned int i=numplaces+numtransitions; i<numnodes; ++i)
	{
		list[i] = -1;
		modes[i] = Rules::ALLMODES;
		unused.insert(i);
		timestamp[i] = 1;
	}
	numtransitions *= 2;

	propagated = (char*) calloc(numnodes,sizeof(char));
}

/** Destructor
*/
IMatrix::~IMatrix() {
	delete[] pre;
	delete[] post;
    delete[] marking;
	free(label);
	delete[] name;
	free(idvec);
	free(iovec);
	free(iotype);
    for (unsigned int i = 0; i < numplaces+numtransitions; ++i)
	{
        pthread_rwlock_destroy(rwlocks+i);
        pthread_rwlock_destroy(modelocks+i);
	}
    free(rwlocks);
	free(modelocks);
	free(modes);
	free(writing);
	free(list);
	free(timestamp);
    for (unsigned int i=0; i <= NODE_SET_LIMIT; ++i)
    {
        for(unsigned int j=0; j <= NODE_SET_LIMIT; ++j)
        {
            pthread_rwlock_destroy(&listlocks[PL][i][j]);
            pthread_rwlock_destroy(&listlocks[TR][i][j]);
        }
        free(listlocks[PL][i]);
        free(listlocks[TR][i]);
    }
    free(listlocks[PL]);
    free(listlocks[TR]);
    free(listlocks);
	free(visibility);
	pthread_rwlock_destroy(&labellock);
	pthread_rwlock_destroy(&unusedlock);
	free(propagated);
}

/** Check if a node is a place
	@param id The node.
	@return True if it is a place. False if it is a transition, NO_NODE or something else.
*/
bool IMatrix::isPlace(Vertex id) {
	return (id<numplaces);
}

/** Check if a node is an input or output label
	@param id The node.
	@return True if it is such a label. False if it is an internal place, a transition, NO_NODE or something else.
*/
bool IMatrix::isIO(Vertex id) {
	return (id<numiolabels);
}

/** Check if a node is in the original net
	@param id The node.
	@return True if so. False if it is newly created transition or NO_NODE.
*/
bool IMatrix::isOriginal(Vertex id) {
	return (id<orignodes);
}

/** Check if a node is in use (exists and has not been deleted by a reduction)
	@param id The ID of the node
	@return True if the node exists
*/
bool IMatrix::exists(Vertex id) {
	// check first if we are in range
    if (id>numplaces+numtransitions) return false;
	// lock the node and check if it is in use
	pthread_rwlock_rdlock(modelocks + id);
    bool retval(list[id]>=0);
	pthread_rwlock_unlock(modelocks + id);
	return retval;
}

/** Get the pre- or postset of a node.
	@param id The ID of the node
	@param prepost True for the postset 
	@return A map of edges incident with this node. Reading/writing this map requires the node to be locked.
*/
Map& IMatrix::getPrePost(Vertex id, bool prepost) {
    return (prepost ? post[id] : pre[id]);
}

/** Get the preset of a node
	@param id The ID of the node
	@return A map of the preset edges of this node. Reading/writing this map requires the node to be locked.
*/
Map& IMatrix::getPre(Vertex id) {
    return pre[id];
}

/** Get the postset of a node
	@param id The ID of the node
	@return A map of the postset edges of this node. Reading/writing this map requires the node to be locked.
*/
Map& IMatrix::getPost(Vertex id) {
    return post[id];
}

/** Get the number of tokens in the initial marking for a node. The node must be read-locked.
	@param id The ID of the node
	@return The number of tokens
*/
unsigned int& IMatrix::getTokens(Vertex id) {
    return marking[id];
}

/** Add a number of tokens to the initial marking of this node. The node must be write-locked.
	@param id The ID of the node
	@param tokens The number of tokens to be added
*/
void IMatrix::addTokens(Vertex id, unsigned int tokens) {
    marking[id] += tokens;
}

/** Get the time stamp of a node. The node must be read-locked.
	@param id The ID of the node
	@return The time stamp
*/
unsigned int IMatrix::getTimeStamp(Vertex id) {
	return timestamp[id];
}

/** Collect time stamps for those nodes in the map with a value of zero. The zero will be replaced
	by the actual time stamp.
	@param stamps A map from nodes to time stamps, where a value of zero indicates that the stamp should be collected
*/
void IMatrix::completeTimeStamps(Map& stamps) {
	// get stamps for unstamped nodes
	Map::iterator mit;
	for(mit=stamps.begin(); mit!=stamps.end(); ++mit)
		if (mit->second == 0) {
			mit->second = rdlock(mit->first);
			unlock(mit->first);
		}
}

/** Increment the time stamp of a node. The node must be write-locked.
	@param id The ID of the node
*/
void IMatrix::stamp(Vertex id) {
	++timestamp[id];
}

/** Get a read-lock on a node
	@param id The ID of the node
	@return The time stamp of the node
*/
unsigned int IMatrix::rdlock(Vertex id) {
    pthread_rwlock_rdlock(rwlocks + id);
	return getTimeStamp(id);
}

/** Get read-locks on two nodes
	@param id1 The ID of the first node
	@param id2 The ID of the second node
	@return The time stamp of the second node
*/
unsigned int IMatrix::rdlock(Vertex id1, Vertex id2) {
	if (id1<id2) {
		rdlock(id1);
		return rdlock(id2);
	}
	unsigned int timestamp(rdlock(id2));
	rdlock(id1);
	return timestamp;
}

/** Get a write-lock for a node
	@param id The ID of the node
*/
void IMatrix::wrlock(Vertex id) {
    pthread_rwlock_wrlock(rwlocks + id);
	writing[id] = true;
}

/** Get write-locks for a set of nodes if the time stamps of all these nodes are unchanged.
	Additionally, for all nodes also the locks for reduction rule checks are acquired.
	@param nodestamp A map from the nodes to be write-locked to their expected time stamps.
	@return True if all nodes could be locked with identical time stamps. As a result, all
		time stamps will be incremented. Otherwise, no locks
		will be acquired at all and the return value is false.
*/
bool IMatrix::wrlock(Map& nodestamp) {
	// get the locks for reduction rules first
	Map::iterator mit;
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
		pthread_rwlock_wrlock(modelocks + mit->first);

	// now get the node's locks and check the stamps
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
	{
		wrlock(mit->first);
		if (!mit->second) mit->second = 1;
		else if (mit->second != timestamp[mit->first]) break;
	}

	// if everything went well, increment the stamps and return true
	if (mit==nodestamp.end()) 
	{
		for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
			stamp(mit->first);
		return true;
	}

	// otherwise free the acquired locks and return false
	for(; mit!=nodestamp.begin(); --mit)
	{
		writing[mit->first] = false;
		unlock(mit->first);
	}
	writing[mit->first] = false;
	unlock(mit->first);
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
		pthread_rwlock_unlock(modelocks + mit->first);
	return false;
}

/** Unlock a node, mark it "checked" for a given reduction rule, and possibly delete it from the net
	@param id The ID of the node
	@param mode A reduction rule that has been checked/done (default: 0 = none)
	@param remove If the node should be deleted (default: false = no)
*/
void IMatrix::unlock(unsigned int id, Mode mode, bool remove) {
	// if we had a write lock, adapt the equivalence classes for the pre-/postsets
	// as the latter may have changed
	if (writing[id]) { 
		adaptList(id,remove);
		writing[id] = false;
	}
    pthread_rwlock_unlock(rwlocks + id);
	// set the reduction rule as "done"
	if (mode) setMode(id,mode);
}

/** Unlock a set of nodes as well as the locks for reduction rule checks.
	@param nodes A map from the nodes to be unlocked to a value. A value of zero indicates that
			a write-locked node should be deleted. For read-locked nodes the value is of no consequence.
*/
void IMatrix::unlock(Map& nodes) {
	Map::iterator mit;
	for(mit=nodes.begin(); mit!=nodes.end(); ++mit)
		unlock(mit->first,0,(mit->second==0));
	for(mit=nodes.begin(); mit!=nodes.end(); ++mit) {
		if (mit->second==0) modes[mit->first] = Rules::ALLMODES;
		pthread_rwlock_unlock(modelocks + mit->first);
	}
}

/** Set the flag for a given reduction rule check so it won't be done twice
	@param id The ID of the node
	@param flag A bit indicating the reduction rule
	@param lock If the reduction rule flags should be write-locked
*/
void IMatrix::setMode(Vertex id, Mode flag, bool lock) {
	if (lock) pthread_rwlock_wrlock(modelocks + id);
	modes[id] |= flag;
	if (lock) pthread_rwlock_unlock(modelocks + id);
}

/** Get all reduction rule flags for a given node. The ID's reduction rule lock must be acquired.
	@param id The ID of the node
	@return All reduction rule flags
*/
Mode IMatrix::getModes(Vertex id) {
	return modes[id];
}

/** Mark a node as unchecked for all reduction rules. The ID's reduction rule flags must be write-locked.
	@param id The ID of the node
	@param lock If the necessary locks (see above) should be acquired
*/
void IMatrix::clearModes(Vertex id, bool lock) {
	if (lock) pthread_rwlock_wrlock(modelocks + id);
	if (list[id]>=0) modes[id] = 0;
	if (lock) pthread_rwlock_unlock(modelocks + id);
}

/** Check if a given reduction rule has been done for a node. The node must not be locked.
	@param id The ID of the node
	@param flag The reduction rule for which to check
	@return If the reduction rule has already been checked for this node.
*/
bool IMatrix::isDone(unsigned int id, Mode flag) {
	pthread_rwlock_rdlock(modelocks + id);
	bool val(modes[id] & flag);
	pthread_rwlock_unlock(modelocks + id);
	return val;
}

/** Find a reduction rule for which at least one node has not been checked.
	@param rules The reduction rules definition, needed for an applicability check of a node against a rule
	@param start An arbitrary offset for nodes/rules so that different threads may start the check with
		different rules and nodes to increase chances of finding a result early.
	@return The rule that may be applied (its number, where its flag would be 1<<number)
*/
unsigned int IMatrix::findMode(Rules& rules, unsigned int start) {
	Mode maxmode(Rules::MAXMODE);

	// find starting point in the nodes
	unsigned int i(start);
	while (i>=numplaces+numtransitions) i-=numplaces+numtransitions;
	unsigned int istart(i);

	// run through all nodes
	do {
		// get the reduction rule modes
		pthread_rwlock_rdlock(modelocks + i);
		uint32_t mode(modes[i]);
		pthread_rwlock_unlock(modelocks + i);

		// find starting point in the modes
		unsigned int j(start);
		if (j>=maxmode) j=0;
		unsigned int jstart(j);

		// run through all modes and look for something to do
		do {
			if (!(mode & (1L<<j)) && rules.checkAppl(i,j)) return j;
			if (++j==maxmode) j=0;
		} while (j!=jstart);

		// increment and cross-over
		if (++i==numplaces+numtransitions) i=0;
	} while (i!=istart);

	return maxmode;
}

/** Find a node that meets certain conditions. The caller must not have any write-locks. The node found will be read-locked.
	@param id The ID of the node found, NO_NODE if none was found. If ID is given, search will commence after that ID.
	@param type The type of node (IMatrix::PL or IMatrix::TR)
	@param flag A reduction rule that still needs to be checked for the node found
	@param presize The size of the preset of the node to be found (all below NODE_SET_LIMIT are exact, NODE_SET_LIMIT is the union of all higher sizes)
	@param postsize The size of the postset of the node to be found (like presize)
	@return If such a node was found
*/
bool IMatrix::getFirstNode(unsigned int& id, unsigned int type, Mode flag, unsigned int presize, unsigned int postsize) {
/*
	// get the list of nodes with correct pre-/postset size
    pthread_rwlock_rdlock(&listlocks[type][presize][postsize]);
    set<unsigned int> thelist(nodelists[type][presize][postsize]);
    pthread_rwlock_unlock(&listlocks[type][presize][postsize]);

	// if empty, no node is found
    if (thelist.empty()) {
        id = NO_NODE;
        return false;    
    }

	// position an iterator on this and the next element
    set<unsigned int>::iterator it, itend;
    if (id==NO_NODE) it = thelist.end();
    else it = thelist.find(id);
    itend = it;
    id = NO_NODE;
    if (it == thelist.end()) it = thelist.begin();
    else if (++it == thelist.end()) it = thelist.begin();

	// run from it to itend (possibly crossing from end() to begin() in the process)
    bool retval(false);
    while (it != itend) {

		// lock the node and check its reduction rule mode and its pre/postset sizes
        pthread_rwlock_rdlock(&modelocks[*it]);
        retval = !(modes[*it] & flag);
        if (retval) {

			// recheck the pre-/postset sizes as the may have changed
			if (presize==list[*it]/(NODE_SET_LIMIT+1) &&
				postsize==list[*it]%(NODE_SET_LIMIT+1)) {
				// we have found our node
				rdlock(*it);
				pthread_rwlock_unlock(&modelocks[*it]);
	            id = *it;
				return true;
			}

			// the net structure has changed, call ourselves again
			pthread_rwlock_unlock(&modelocks[*it]);
			return getFirstNode(id,type,flag,presize,postsize);
        }
        pthread_rwlock_unlock(&modelocks[*it]);

		// increment iterator and possibly cross over from end() to begin()
        if (++it == thelist.end() && itend != it) it = thelist.begin();
    }

	// no node to be done
	return false;
/*/

	Vertex start(id);
	while (true) {

		// get the list of nodes with correct pre-/postset size
	    pthread_rwlock_rdlock(&listlocks[type][presize][postsize]);
	    set<unsigned int>& thelist(nodelists[type][presize][postsize]);

	    set<unsigned int>::iterator it(thelist.lower_bound(id+1));
		if (thelist.empty() || it==thelist.end()) {
		    pthread_rwlock_unlock(&listlocks[type][presize][postsize]);
			id = NO_NODE;
			return false;
		}
		id = *it;
	    pthread_rwlock_unlock(&listlocks[type][presize][postsize]);

		// lock the node and check its reduction rule mode and its pre/postset sizes
		pthread_rwlock_rdlock(&modelocks[id]);
	    if (!(modes[id] & flag)) {

			// recheck the pre-/postset sizes as the may have changed
			if (presize==list[id]/(NODE_SET_LIMIT+1) &&
				postsize==list[id]%(NODE_SET_LIMIT+1)) {
				// we have found our node
				rdlock(id);
				pthread_rwlock_unlock(&modelocks[id]);
				return true;
			}

			// the net structure has changed, call ourselves again
	        pthread_rwlock_unlock(&modelocks[id]);
			return getFirstNode(--id,type,flag,presize,postsize);
	    }
	    pthread_rwlock_unlock(&modelocks[id]);

    }

	// no node to be done
	return false;

}

/** Find a node that meets certain conditions. The caller must not have any write-locks. The node found will be read-locked.
	@param id The ID of the node found, NO_NODE if none was found.
	@param type The type of node (IMatrix::PL or IMatrix::TR)
	@param flag A reduction rule that still needs to be checked for the node found
	@param presizemin The minimal size of the preset of the node to be found. Will fail for minima above NODE_SET_LIMIT.
	@param presizemax The maximal size of the preset of the node to be found. NODE_SET_LIMIT means no maximum.
	@param postsizemin The minimal size of the postset of the node to be found. Will fail for minima above NODE_SET_LIMIT.
	@param postsizemax The maximal size of the postset of the node to be found. NODE_SET_LIMIT means no maximum.
	@return If such a node was found
*/
bool IMatrix::getFirstNode(unsigned int& id, unsigned int type, Mode flag, unsigned int presizemin, unsigned int presizemax, unsigned int postsizemin, unsigned int postsizemax) {
	// check the maxima
    if (presizemax > NODE_SET_LIMIT) presizemax = NODE_SET_LIMIT;
    if (postsizemax > NODE_SET_LIMIT) postsizemax = NODE_SET_LIMIT;

	// call the function for exact sizes
    for(unsigned int i=presizemin; i <= presizemax; ++i)
        for(unsigned int j=postsizemin; j <= postsizemax; ++j)
            if (getFirstNode(id,type,flag,i,j))
                return true;

    return false;
}

/** Find all isolated nodes of a given type as long as there is at least one node for which the reduction rule check still has to be made.
	All returned nodes will be write-locked, as well as their locks for reduction rule checks. 
	@param type The type of node (PL or TR)
	@param flag The reduction rule flag for the check to be made
	@return A map from isolated nodes to their time stamps. If no node can be found for which the reduction rule check is required then
		the returned map will be empty.
*/
Map IMatrix::getIsolated(unsigned int type, Mode flag) {
	// get all isolated nodes
    pthread_rwlock_rdlock(&listlocks[type][0][0]);
    set<unsigned int> thelist(nodelists[type][0][0]);
    pthread_rwlock_unlock(&listlocks[type][0][0]);

	// if empty: nothing to do
	Map res;
    if (thelist.empty()) return res;

	// go through the list, lock nodes, check if there is a node "todo"	
	bool todo(false);
	set<unsigned int>::iterator sit;
	for(sit=thelist.begin(); sit!=thelist.end(); ++sit)
	{
        pthread_rwlock_wrlock(modelocks + *sit);
		wrlock(*sit);
        if (!(modes[*sit] & flag)) todo=true;
		if (list[*sit]) break; // make sure we still have empty pre-/postsets
		res[*sit] = getTimeStamp(*sit);
	}

	// if there is no node "todo" or the net structure changed, release all locks (failure)
	if (!todo || sit!=thelist.end()) {
		if (sit != thelist.end()) --sit;
		do {
			unlock(*sit);
			pthread_rwlock_unlock(modelocks + *sit);
		} while (--sit != thelist.begin());
		res.clear();
	}

	// return the isolated nodes
	return res;
}

/** Adapt the equivalence class membership for pre-/postset size and handle deletion of a node. The node must be write-locked.
	@param id The ID of the node
	@param remove If the node should be deleted
*/
void IMatrix::adaptList(Vertex id, bool remove) {
    // changes to pre/postset of id must already have happened
    unsigned int type(id<numplaces ? PL : TR);
    unsigned int presize, postsize;

	// remove the node from its past equivalence class
    if (list[id]>=0) {
        presize = list[id]/(NODE_SET_LIMIT+1);
        postsize = list[id]%(NODE_SET_LIMIT+1);
        if (!remove && presize == pre[id].size() && postsize == post[id].size()) return;
        pthread_rwlock_wrlock(&listlocks[type][presize][postsize]);
        nodelists[type][presize][postsize].erase(id);
        pthread_rwlock_unlock(&listlocks[type][presize][postsize]);
    }

	// check if the node must be deleted
    if (remove) {
        list[id] = -1;
        return;
    }

	// reinsert the node into its new equivalence class
    presize = pre[id].size();
    postsize = post[id].size();
	if (presize > NODE_SET_LIMIT) presize = NODE_SET_LIMIT;
	if (postsize > NODE_SET_LIMIT) postsize = NODE_SET_LIMIT;
    list[id] = presize * (NODE_SET_LIMIT+1) + postsize;
    pthread_rwlock_wrlock(&listlocks[type][presize][postsize]);
    nodelists[type][presize][postsize].insert(id);
    pthread_rwlock_unlock(&listlocks[type][presize][postsize]);
}

/** Debug output: Print the equivalence classes for pre-/postset size
*/
void IMatrix::printLists() {
	set<unsigned int>::iterator it;
	for(unsigned int t=PL; t<=TR; ++t)
	{
		std::cout << "t=" << t << std::endl;
		for(unsigned int i=0; i<=NODE_SET_LIMIT; ++i)
			for(unsigned int j=0; j<=NODE_SET_LIMIT; ++j)
			{
				std::cout << "[" << i << ":" << j << "] ";
				for(it=nodelists[t][i][j].begin(); it!=nodelists[t][i][j].end(); ++it)
					std::cout << getName(*it) << " ";
				std::cout << endl;
			}
	}
}

/** Construct a PetriNet PNAPI object from the internal net representation. Not thread-safe.
	@param facts Knowledge base for the translation of the final condition
	@return A pointer to the Petri net
*/
PetriNet* IMatrix::exportNet(Facts& facts) {
	// not thread-safe
	PetriNet* net = new PetriNet();
	vector<Place*> places;
	vector<Transition*> transitions;
	map<const Place*, const Place*> fcond;

	// create ports;
	const map<string,Port*>& portmap(petrinet.getInterface().getPorts());
	map<string,Port*> portmap2;
	map<string,Port*>::const_iterator pmit;
	for(pmit=portmap.begin(); pmit!=portmap.end(); ++pmit)
		portmap2[pmit->first] = &(net->getInterface().addPort(pmit->first));

	// create IO labels
	vector<Label*> lablist;
	for(unsigned int i=0; i<numiolabels; ++i)
		if (list[i]>=0)
			lablist.push_back(&(portmap2[iovec[i]->getPort().getName()]->addLabel(iovec[i]->getName(),iovec[i]->getType())));
		else lablist.push_back(NULL);

	// create places, make a list with NULL entries for deleted places
	// so the numbering from the internal structure can be inherited for now
	for(unsigned int i=0; i<numplaces; ++i)
		if (i>=numiolabels && list[i]>=0) {
			places.push_back(&(net->createPlace(getName(i),marking[i])));
			fcond[((const Place*)idvec[i])] = places.back();
		} else places.push_back(NULL);

	// create the final condition
	if (Runtime::args_info.final_given) {
		net->getFinalCondition() = *(facts.transferFormula(petrinet.getFinalCondition().getFormula(),fcond));
		if (petrinet.getFinalCondition().getFormula().getEmptyPlaces().size()
			+ petrinet.getFinalCondition().getFormula().getPlaces().size()
			>= petrinet.getPlaces().size())
				net->getFinalCondition().allOtherPlacesEmpty(*net);
	}

	// create transitions now, including pre-/postsets and labels
	for(unsigned int i=0; i<numtransitions; ++i)
	{
		if (list[i+numplaces]>=0) {
			transitions.push_back(&(net->createTransition(getName(i+numplaces))));
			Map::iterator mit;
			Map& pmap(pre[i+numplaces]);
			for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
			{
				if (mit->first < numiolabels)
					transitions.back()->addLabel(*(lablist[mit->first]),mit->second);
				else
					net->createArc(*places[mit->first],*transitions.back(),mit->second);
			}
			Map& pmap2(post[i+numplaces]);
			for(mit=pmap2.begin(); mit!=pmap2.end(); ++mit)
			{
				if (mit->first < numiolabels)
					transitions.back()->addLabel(*(lablist[mit->first]),mit->second);
				else
					net->createArc(*transitions.back(),*places[mit->first],mit->second);
			}
			if (label[i+numplaces]>0)
				transitions.back()->addRole(labeltext[label[i+numplaces]]);
		}
	}
	return net;
}

/** Get the name of a node
	@param id The ID of a node
	@return Its name
*/
string IMatrix::getName(Vertex id) {
	return name[id];
}

/** Set the name of a node. Not thread-safe, use only for not-yet-existing nodes.
	@param id The ID of the node
	@param newname The name given to the node
*/
void IMatrix::setName(unsigned int id, string newname) {
	name[id] = newname;
}

/** Ensure that a node will never be removed
	@param node The name of the node
*/
void IMatrix::makePersistent(string node) {
	Node* nd(petrinet.findNode(node));
	if (nd == NULL) return;
	unsigned int tmp(nodeToID[nd]);
	if (tmp<numplaces+numtransitions) 
		visibility[tmp] = PERSISTENT;
}

/** Declare a node visible in formulas etc. so removal may require the production of additional information
	@param node The name of the node
*/
void IMatrix::makeVisible(string node) {
	Node* nd(petrinet.findNode(node));
	if (nd == NULL) return;
	unsigned int tmp(nodeToID[nd]);
	if (tmp<numplaces+numtransitions && visibility[tmp]!=PERSISTENT) 
		visibility[tmp] = VISIBLE;
}

/** Make a node visible in formulas etc. so removal may require the production of additional information
	@param id The ID of the node
*/
void IMatrix::makeVisible(Vertex id) {
	if (id<numplaces+numtransitions && visibility[id]!=PERSISTENT) 
		visibility[id] = VISIBLE;
}

/** Declare all places or all transitions visible in formulas
	@param t True for transitions, false for places
*/
void IMatrix::makeVisible(bool t) {
	unsigned int i;
	if (t) for(i=0; i<numtransitions; ++i) visibility[i+numplaces]=1;
	else for(i=0; i<numplaces; ++i) visibility[i]=1;
}

/** Get the visibility status of a node
	@param id The ID of the node
	@return Its visibility (including persistence)
*/
unsigned int IMatrix::visible(Vertex id) {
	return visibility[id];
}

/** If a node is invisible in formulas
	@param id The ID of the node
	@return If the node is invisible AND removable
*/
bool IMatrix::isInvisible(Vertex id) {
	return (visibility[id]==INVISIBLE);
}

/** If a node is visible in formulas
	@param id The ID of the node
	@return If it is visible (but not irremovable)
*/
bool IMatrix::isVisible(Vertex id) {
	return (visibility[id]==VISIBLE);
}

/** If a node is irremovable
	@param id The ID of the node
	@return If it is irremovable
*/
bool IMatrix::isPersistent(Vertex id) {
	return (visibility[id]==PERSISTENT);
}

/** Get the label of a transition
	@param id The ID of the node (transition)
	@return An ID for the label, with 0=no label/tau/lambda
*/
unsigned int IMatrix::getLabel(Vertex id) {
	return label[id];
}

/** Get the label of a transition if visible in formulas
	@param id The ID of the node (transition)
	@return An ID for the label if it is visible in formulas, 0 otherwise
*/
unsigned int IMatrix::getLabelVis(Vertex id) {
	if (labelvis[label[id]])
		return label[id];
	return 0;
}

/** Get the name of a label
	@param action The ID of the label
	@return Its name representation
*/
string& IMatrix::getLabeltext(unsigned int action) {
	return labeltext[action];
}

/** Set the label of a transition. Not thread-safe in general, use only for not-yet-existing transitions.
	@param id The ID of the node
	qparam action The ID of the label
*/
void IMatrix::setLabel(Vertex id, unsigned int action) {
	// a label can only be set while the corresponding node is deleted
	pthread_rwlock_wrlock(&labellock);
	labelcount[label[id]]--;
	label[id] = action;
	labelcount[action]++;
	pthread_rwlock_unlock(&labellock);
}

/** Find out how many transitions have a given label
	@param action The ID of the label
	@return How often it occurs
*/
unsigned int IMatrix::getLabelCount(unsigned int action) {
	pthread_rwlock_rdlock(&labellock);
	map<unsigned int,unsigned int>::iterator it(labelcount.find(action));
	unsigned int ret(0);
	if (it!=labelcount.end()) ret = it->second;
	pthread_rwlock_unlock(&labellock);
	return ret;
}

/** Remove the label from a transition. Not thread-safe in general, use only for not-yet-existing transitions.
	@param id The ID of the node (transition)
*/
void IMatrix::clearLabel(Vertex id) {
	pthread_rwlock_wrlock(&labellock);
	labelcount[label[id]]--;
	label[id] = 0;
	pthread_rwlock_unlock(&labellock);
}

/** Fire a transition and change the initial marking accordingly. All surrounding nodes of the transition must be write-locked.
	@param t The ID of the node (transition)
	@return If firing t was possible, otherwise the initial marking remains unchanged
*/
bool IMatrix::fire(Vertex t) {
	// all surrounding nodes of t must be write-locked

	// make sure t is a transition
	if (t<numplaces) return false;

	// check if t is firable
	Map::iterator mit;
	for(mit=pre[t].begin(); mit!=pre[t].end(); ++mit)
		if (mit->second>marking[mit->first]) return false;

	// fire t
	for(mit=pre[t].begin(); mit!=pre[t].end(); ++mit)
		marking[mit->first] -= mit->second;
	for(mit=post[t].begin(); mit!=post[t].end(); ++mit)
		marking[mit->first] += mit->second;
	return true;
}

/** Add a multiple of the preset of one node to the preset of another node. All surrounding nodes must be write-locked.
	@param id The ID of the first node
	@param id2 The ID of the node to which edges are to be added
	@param times How many edges are to be added for each original edge
*/
void IMatrix::addPre(Vertex id, Vertex id2, unsigned int times) {
	for(Map::iterator mit=pre[id].begin(); mit!=pre[id].end(); ++mit)
	{
		post[mit->first][id2] += times*mit->second;
		pre[id2][mit->first] += times*mit->second;
	}
}

/** Add a multiple of the postset of one node to the postset of another node. All surrounding nodes must be write-locked.
	@param id The ID of the first node
	@param id2 The ID of the node to which edges are to be added
	@param times How many edges are to be added for each original edge
*/
void IMatrix::addPost(Vertex id, Vertex id2, unsigned int times) {
	if (id>=numplaces+numtransitions) return;
	addPost(post[id],id2,times);
}

/** Add a multiple of a multiset of edges to the postset of a node. All surrounding nodes must be write-locked.
	@param pmap A map from nodes to edge multiplicity
	@param id2 The ID of the node to which edges are to be added
	@param times How many edges are to be added for each original edge
*/
void IMatrix::addPost(const Map& pmap, Vertex id2, unsigned int times) {
	for(Map::const_iterator mit=pmap.begin(); mit!=pmap.end(); ++mit)
	{
		pre[mit->first][id2] += times*mit->second;
		post[id2][mit->first] += times*mit->second;
	}
}

/** Remove all edges from the preset of a node. All surrounding nodes must be write-locked.
	@param id The ID of the node
*/
void IMatrix::removePre(Vertex id) {
	Map::iterator mit;
	for(mit=pre[id].begin(); mit!=pre[id].end(); ++mit)
		post[mit->first].erase(id);
	pre[id].clear();
}

/** Remove all edges from the postset of a node. All surrounding nodes must be write-locked.
	@param id The ID of the node
*/
void IMatrix::removePost(Vertex id) {
	Map::iterator mit;
	for(mit=post[id].begin(); mit!=post[id].end(); ++mit)
		pre[mit->first].erase(id);
	post[id].clear();
}

/** Remove all edges at some node. All surrounding nodes must be write-locked.
	@param id The ID of the node
*/
void IMatrix::removeArcs(Vertex id) {
	removePre(id);
	removePost(id);
}

/** Obtain new transitions from a pool (until it becomes empty). If the request can be handled,
		the new transitions will be write-locked together with their reduction rule locks.
		The new transitions will have empty pre-/postsets and will be marked unchecked regarding reduction rules.
	@param num How many transitions to obtain
	@param nodestamp A map for time stamps to which to add the new transitions (including stamp)
	@return The set of new transitions, either exactly num many or zero (in case of failure)
*/
set<Vertex> IMatrix::reserveTransitions(unsigned int num, Map& nodestamp) {
	set<Vertex> result;
	pthread_rwlock_wrlock(&unusedlock);
	if (unused.size()>=num)
	{
		while (num-->0) {
			Vertex tmp = *(unused.begin());
			pthread_rwlock_wrlock(modelocks + tmp);
			wrlock(tmp);
			stamp(tmp);
			nodestamp[tmp] = getTimeStamp(tmp);
			unused.erase(tmp);
			modes[tmp] = 0L;
			pre[tmp].clear();
			post[tmp].clear();
			result.insert(tmp);
		}
	}
	pthread_rwlock_unlock(&unusedlock);
	return result;
}

/** Return the PNAPI node given an ID
	@param id The ID of the node
	@return The node the ID belongs to
*/
Node* IMatrix::getNode(Vertex id) {
	return idvec[id];
}

/** Return the ID to a given PNAPI node
	@param n The node from the PNAPI PetriNet structure
	@return The internal ID of the node
*/
Vertex IMatrix::getID(const Node& n) {
	return nodeToID[&n];
}

/** Unlocks the given nodes ands marks all surrounding nodes so they will be checked again by all rules
    @param nodestamp A map with nodes as keys
*/
void IMatrix::propagateChange(Map& nodestamp) {
/*
	set<Vertex> tmp;
	Map::iterator mit,mit2;
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
	{
		clearModes(mit->first);
		Map& xpre(pre[mit->first]);
		for(mit2=xpre.begin(); mit2!=xpre.end(); ++mit2)
			if (nodestamp.find(mit2->first)==nodestamp.end())
				tmp.insert(mit2->first);
		Map& xpost(post[mit->first]);
		for(mit2=xpost.begin(); mit2!=xpost.end(); ++mit2)
			if (nodestamp.find(mit2->first)==nodestamp.end())
				tmp.insert(mit2->first);
	}
	unlock(nodestamp);
	set<Vertex>::iterator sit;
	for(sit=tmp.begin(); sit!=tmp.end(); ++sit)
		clearModes(*sit,true);
/*/
	vector<Vertex> tmp;
	tmp.reserve(16);
	Map::iterator mit,mit2;
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
	{
		clearModes(mit->first);
		propagated[mit->first]=1;
	}
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
	{
		Map& xpre(pre[mit->first]);
		for(mit2=xpre.begin(); mit2!=xpre.end(); ++mit2)
		{
			if (propagated[mit2->first]) continue;
			propagated[mit2->first]=1;
			tmp.push_back(mit2->first);
		}
		Map& xpost(post[mit->first]);
		for(mit2=xpost.begin(); mit2!=xpost.end(); ++mit2)
		{
			if (propagated[mit2->first]) continue;
			propagated[mit2->first]=1;
			tmp.push_back(mit2->first);
		}
	}
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
		propagated[mit->first]=0;
	unlock(nodestamp);
	for(unsigned int i=0; i<tmp.size(); ++i)
	{
		propagated[tmp[i]]=0;
		clearModes(tmp[i],true);
	}

}


// -*- C++ -*-

/*!
 * \file    witness.cc
 *
 * \brief   Class WItness
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/11/03
 *
 * \date    $Date: 2010-11-03 11:22:01 +0200 (Mi, 03. Nov 2010) $
 *
 * \version $Revision: 1.0 $
 */

#include "witness.h"
#include "lp_solve/lp_lib.h"

	/**********************************
	* Implementation of class WNode   *
	**********************************/

/** Standard constructor for the initial node. */
WNode::WNode() : P(NULL) {}

/** Standard constructor for other nodes. */
WNode::WNode(Place* p) {
	P = p;
}

/** Destructor. */
WNode::~WNode() {}

/** Compute a cycle-free allocation for the siphon represented by this node. */
void WNode::computeAllocation() {
	alloc.clear(); // remove the former allocation
	set<Place*>::iterator pit;
	set<Place*> mt(maxtrap);
	set<Place*> aset(siphon);
	set<Place*> diff; 
	for(pit=aset.begin(); pit!=aset.end(); ++pit)
		if (mt.find(*pit)==mt.end()) diff.insert(*pit);
	pit = diff.end();
	while (!diff.empty()) // go through the places in the siphon but not in its maximal trap
	{
		if (pit==diff.end()) pit=diff.begin(); // get the next such place
		else ++pit;
		Place* p(*pit);
		set<Node*> tset(p->getPostset()); // and check if its postset
		set<Node*>::iterator nit;
		for(nit=tset.begin(); nit!=tset.end(); ++nit) // contains at least one transition
		{
			set<Node*> pset((*nit)->getPostset()); // that does not put any token
			set<Node*>::iterator nit2;
			for(nit2=pset.begin(); nit2!=pset.end(); ++nit2)
			{
				if (aset.find((Place*)(*nit2))!=aset.end()) break; // into the remainder of the siphon
			}
			if (nit2==pset.end()) // if such a transition is found
			{
				alloc[p] = (Transition*)(*nit); // use it in the allocation
				diff.erase(p);
				aset.erase(p); // and remove the place from the remainder of the siphon
				pit = diff.end(); // finally, go to the next place, this one became invalid
				break;
			}
		}
	}		
}

/** Checks if the siphon of this node actively prohibits the firing of a transition.
	@param m The current marking.
	@param tpre The preset of the transition to fire.
	@return If the siphon would allow the firing of the transition.
*/
bool WNode::isActivated(map<Place*,unsigned int>& m, map<Place*,unsigned int>& tpre) {
	map<Place*,unsigned int>::iterator mit;
	for(mit=tpre.begin(); mit!=tpre.end(); ++mit)
		if (siphon.find(mit->first)!=siphon.end())
			if (m[mit->first]<mit->second) return false;
	return true;
}

/** Get the set of all transitions in the postset of the siphon whose firing is not
	prohibited by the siphon at the current marking.
	@param m The current marking.
	@param pre The collected presets of all transitions.
	@return The set of transitions the siphon would allow to fire.
*/
set<Transition*> WNode::getActivated(map<Place*,unsigned int>& m, map<Transition*,map<Place*,unsigned int> >& pre) {
	set<Transition*> result;
	map<Transition*,Place*>::iterator tit;
	for(tit=postsiphon.begin(); tit!=postsiphon.end(); ++tit)
		if (isActivated(m,pre[tit->first])) result.insert(tit->first);
	return result;
}

/** Compute all transitions belonging to the siphon of this node. */
void WNode::computeSiphonPostset() {
	postsiphon.clear();
	set<Place*>::iterator sit;
	for(sit=siphon.begin(); sit!=siphon.end(); ++sit)
	{
		const set<Node*>& nset((*sit)->getPostset());
		set<Node*>::const_iterator nit;
		for(nit=nset.begin(); nit!=nset.end(); ++nit)
			postsiphon[(Transition*)(*nit)] = (*sit);
	}	
}

/** Check if the current maximal trap of this node contains a token.
	@param m The current marking.
	@return If the maximal trap contains a token.
*/
bool WNode::checkMarkedMaxTrap(map<Place*,unsigned int>& m) {
	set<Place*>::iterator pit;
	for(pit=maxtrap.begin(); pit!=maxtrap.end(); ++pit)
		if (m[*pit]>0) return true;
	return false;
}

/** Create an arc from this node to another node. The arc is labelled with (t,p).
	@param t A transition in this node's siphon which is prohibited from firing by place p.
	@param p A place prohibiting the firing of t, contained in node w.
	@param w The node representing the siphon being the reason for the non-firability of t.
*/
void WNode::createArc(Transition* t, Place* p, WNode& w) {
	out[t]=&w;
	outP[t]=p;
	w.in[this][t]=p;
}

/** Remove all arcs starting at this node. */
void WNode::removeOutArcs() {
	map<Transition*,WNode*>::iterator wit;
	for(wit=out.begin(); wit!=out.end(); ++wit)
		wit->second->in.erase(this);
	out.clear();
	outP.clear();
}

/** Remove all arcs pointing to this node. */
void WNode::removeInArcs() {
	map<WNode*,map<Transition*,Place*> >::iterator wit;
	map<Transition*,Place*>::iterator tit;
	for(wit=in.begin(); wit!=in.end(); ++wit)
		for(tit=wit->second.begin(); tit!=wit->second.end(); ++tit)
		{
			wit->first->out.erase(tit->first);
			wit->first->outP.erase(tit->first);
		}
	in.clear();
}

/** Check if a transition produces a token on a defining place of a node. For the initial node all places of its
	siphon are defining, every other node has just one defining place.
	@param t The transition to check.
	@return If such a token was produced. In the positive case, all arcs from and to the node are deleted.
*/
bool WNode::producesDefiningToken(Transition& t) {
	bool result(false);
	PetriNet& net(t.getPetriNet());
	if (P) result = (net.findArc(t,*P)); // if P is the defining place, check if t produces a token on it
	else { // if we are at the initial node, check for each place of the siphon if t produces a token on it
		set<Place*>::iterator pit;
		for(pit=siphon.begin(); pit!=siphon.end(); ++pit)
			if (net.findArc(t,**pit)) { result=true; break; }
	}
	if (result) { // if so, isolate the node
		removeOutArcs();
		removeInArcs();
	}
	return result; // then return the result
}

/** Print the node.
	@param out The stream to print to.
*/
void WNode::show(ostream& out) {
	if (!P) out << "Initial Node" << endl;
	else out << "Node " << P->getName() << endl;
	set<Place*>::iterator pit;
	if (siphon.empty()) out << "Siphon not set" << endl;
	else if (!*(siphon.begin())) out << "No Siphon available" << endl;
	else {
		out << "Siphon:";
		for(pit=siphon.begin(); pit!=siphon.end(); ++pit)
			out << " " << (*pit)->getName();
		out << endl;
	}
	if (maxtrap.empty()) {
		if (siphon.empty()) out << "MaxTrap not set" << endl;
		else if (!*(siphon.begin())) out << "No MaxTrap available" << endl;
		else out << "MaxTrap empty" << endl;
	} else {
		out << "MaxTrap:";
		for(pit=maxtrap.begin(); pit!=maxtrap.end(); ++pit)
			out << " " << (*pit)->getName();
		out << endl;
	}
	if (alloc.empty()) out << "No Allocation given" << endl;
	else {
		map<Place*,Transition*>::iterator ptit;
		out << "Allocation:";
		for(ptit=alloc.begin(); ptit!=alloc.end(); ++ptit)
			out << " " << ptit->first->getName() << "->" << ptit->second->getName() << " ";
		out << endl;
	}
}

	/**********************************
	* Implementation of class Witness *
	**********************************/

/** Standard constructor. */ 
Witness::Witness(PetriNet& net) : pn(net),formula(net) {
	// create the nodes of the graph
	set<Place*>::const_iterator pit;
	const set<Place*> pset(pn.getPlaces());
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		if ((*pit)->getTokenCount()>0) am[*pit]=(*pit)->getTokenCount();
		node[*pit] = new WNode(*pit);
	}
	// create a formula for siphon and its maxtrap
	X = formula.createSetVar();
	Y = formula.createSetVar();
	formula.XSiphon(X);
	formula.XMaxTrap(X,Y);
	// compute the initially activated transitions
	const set<Transition*> tset(pn.getTransitions());
	set<Transition*>::const_iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		if (isActivated(*tit)) tact.insert(*tit);
}

/** Destructor. */
Witness::~Witness() {
	map<Place*,WNode*>::iterator wit;
	for(wit=node.begin(); wit!=node.end(); ++wit)
		if (wit->second!=&init) delete wit->second;
}

/** Check and set the given set of places as initial siphon without marked trap.
	@param swomt The alleged siphon without marked trap.
*/
void Witness::initSWOMT(const set<Place*>& swomt) {
	set<Place*>::const_iterator pit;
	for(pit=swomt.begin(); pit!=swomt.end(); ++pit)
		if (node[*pit]!=&init) // places of the initial node's siphon get no own node
			{ delete node[*pit]; node[*pit]=&init; }
	Formula f(pn); // create a formula checking the property
	f.copyFormula(formula); // non-empty siphon X & maxtrap Y
	f.XNMarked(Y); // with unmarked Y
	const set<Place*> pset(pn.getPlaces());
	for(pit=pset.begin(); pit!=pset.end(); ++pit) // set the assignment of X
		f.SetTo(f.getVar(X,**pit), swomt.find(*pit)!=swomt.end());
	if (!f.solve()) abort(21,"not a siphon without marked trap"); // and check X
	init.siphon = f.getAssignment(X); // everything is ok, so set the siphon
	init.maxtrap = f.getAssignment(Y); // and the maxtrap of the initial node
	init.computeAllocation(); // as well as its cycle-free allocation
	init.computeSiphonPostset(); // and the siphon's postset
}

/** Compute a minimal siphon without marked trap that contains a defining place.
	@param wn The node representing the defining place (not the initial node).
	@return If such a siphon exists.
*/
bool Witness::computeSWOMT(WNode& wn) {
	if (!wn.P) return false; // case: initial node
	Formula f(pn); // create the formula
	f.copyFormula(formula); // non-empty siphon X & maxtrap Y
	f.XNMarked(Y); // the latter unmarked
	f.SetTo(f.getVar(X,*(wn.P)),true); // X to contain the defining place P
	if (!f.solve()) { // if there is no such siphon
		wn.siphon.clear(); 
		wn.siphon.insert(NULL); // mark this node as failing permanently
		return false;
	}
	f.MinMax(X,false); // otherwise minimize the siphon
	wn.siphon = f.getAssignment(X); // and save the minimized siphon
	wn.maxtrap = f.getAssignment(Y); // and its maxtrap to the node
	wn.alloc.clear(); // a cacle-free allocation is not needed yet (lazy computation)
	wn.computeSiphonPostset(); // but we need the siphon's postset soon
	return true;
}

/** Check if a transition can fire under the current marking.
	@param t The transition to check for activation.
	@return If the transition can fire.
*/ 
bool Witness::isActivated(Transition* t) {
	if (pre.find(t)==pre.end()) { // we haven't computed the preset of t yet
		const set<pnapi::Arc*>& arcs(t->getPresetArcs());
		set<pnapi::Arc*>::const_iterator ait;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			pre[t][&((*ait)->getPlace())] = (*ait)->getWeight();
	}
	// now check if the preset has enough tokens to fire t
	map<Place*,unsigned int>::iterator mit;
	map<Place*,unsigned int> tpre(pre[t]);
	for(mit=tpre.begin(); mit!=tpre.end(); ++mit)
		if (mit->second>am[mit->first]) return false;
	return true;
}

/** Compute the siphon interdependency graph and return a deadlocking siphon with a firing sequence emptying that siphon.
	@param seq Return parameter: The firing sequence that empties the siphon as far as possible.
	@return The siphon that is emptied. No transitions in its postset can ever fire again.
*/
set<Place*> Witness::computeGraph(vector<Transition*>& seq) {
	set<WNode*> done;
	while (adaptGraph(init,done)==0) done.clear();
	seq = fseq;
	return res;
}

/** Fire a transition, changing the current marking.
	@param t A transition that must be firable.
*/
void Witness::fireTransition(Transition* t) {
	if (change.find(t)==change.end()) { // we haven't computed the token change for t yet
		map<Place*,int> tmp;
		set<pnapi::Arc*>::const_iterator ait;
		const set<pnapi::Arc*>& postarcs(t->getPostsetArcs());
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			tmp[&((*ait)->getPlace())] = (*ait)->getWeight();
		const set<pnapi::Arc*>& prearcs(t->getPresetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			tmp[&((*ait)->getPlace())] -= (*ait)->getWeight();
		map<Place*,int>::iterator mit; // make sure no zeros are contained in the map
		for(mit=tmp.begin(); mit!=tmp.end(); ++mit)
			if (mit->second!=0) change[t][mit->first]=mit->second;
	}
	// now change the current marking accordingly
	map<Place*,int>::iterator pit;
	for(pit=change[t].begin(); pit!=change[t].end(); ++pit)
	{
		am[pit->first] += pit->second;
		pit->first->setTokenCount(am[pit->first]);
	}
	// next, adapt the set of activated transitions
	for(pit=change[t].begin(); pit!=change[t].end(); ++pit)
	{
		set<Node*>::const_iterator tit;
		const set<Node*>& tset(pit->first->getPostset());
		if (pit->second<0) { // eliminate the just deactivated ones 
			for(tit=tset.begin(); tit!=tset.end(); ++tit)
				if (tact.find((Transition*)(*tit))!=tact.end())
					if (am[pit->first]<pre[(Transition*)(*tit)][pit->first])
						tact.erase((Transition*)(*tit));
		} else { // or look for newly activated transitions
			for(tit=tset.begin(); tit!=tset.end(); ++tit)
				if (tact.find((Transition*)(*tit))==tact.end())
					if (isActivated((Transition*)(*tit)))
						tact.insert((Transition*)(*tit));
		}
	}
	// finally, extend the final firing sequence
	fseq.push_back(t);
}

/** Print all nodes of the siphon dependency graph.
	@param out The stream to print to.
*/
void Witness::show(ostream& out) {
	init.show(out);
	map<Place*,WNode*>::iterator wit;
	for(wit=node.begin(); wit!=node.end(); ++wit)
		if (wit->second->P) wit->second->show(out);
}

/** Check a node of the siphon dependency graph for transitions that can fire or can be enabled, and fire them. This may influence
	other nodes, which have to be rechecked afterwards.
	@param w The node to be checked.
	@param done A list of nodes already checked before. These are excluded from rechecking to avoid infinite recursion.
	@return Success of this operation. A value of 0 means that all nodes have to be rechecked since the graph may have been changed.
		A value of 1 shows the end of the computation as a siphon (often this node's) has been emptied as far as possible. A value of 2 (not possible
		for the initial node) shows that this and all its depending nodes have been checked and found ok, the graph staying unchanged.
*/
int Witness::adaptGraph(WNode& w, set<WNode*>& done) {
	done.insert(&w); // don't go to this node during a recursive search
	if (&w!=&init) { // check if the siphon is still valid and replace it if not
		if (w.checkMarkedMaxTrap(am)) { // if the maxtrap is marked, the siphon is invalid
			w.alloc.clear(); // reset the allocation
			w.removeOutArcs(); // remove all outgoing arcs
			if (!computeSWOMT(w)) { // if a replacement siphon cannot be computed
				w.removeInArcs(); // remove the incoming arcs too (isolate the node)
				return 0; // and restart the computation at the initial node
			}
		}
	}
	// possibly print a message which node we are working at
	if (flag_verbose) {
		if (&w==&init) status("trying to empty initial siphon");
		else {
			string sname;
			bool comma(false);
			set<Place*>::iterator pit;
			for(pit=w.siphon.begin(); pit!=w.siphon.end(); ++pit,comma=true)
				sname += (comma?",":"") + (*pit)->getName();
			status("trying to empty siphon {%s}",sname.c_str()); 
		}
	}

	while (true) { // here we fire activated transitions in the postset of the node's siphon
		// check for activated transitions in the node's siphon
		map<Transition*,Place*>::iterator tit;
		for(tit=w.postsiphon.begin(); tit!=w.postsiphon.end(); ++tit)
			if (tact.find(tit->first)!=tact.end()) break;
		if (tit==w.postsiphon.end()) break; // if none exists, leave the loop
		Place* p(tit->second); // get a place from the siphon that is in the preset of the found transition
		if (w.alloc.empty()) w.computeAllocation(); // ensure that the allocation was computed
		Transition* t(w.alloc[p]); // get the transition that should fire according to the allocation (instead)
		fireTransition(t); // fire that transition
		status("firing %s",t->getName().c_str());
		if (w.producesDefiningToken(*t)) return 0; // restart if this produces a token on the defining place of the siphon
	}
	// now we need to handle the partially activated (w-activated) transitions
	// those transitions have enough tokens in the siphon, but at least one token is missing on a place
	// not belonging to the siphon
	set<Transition*>::iterator wtit;
	set<Transition*> wact(w.getActivated(am,pre));
	if (wact.empty()) { // there are no such transitions, we reached our goal 
		res=w.siphon; 
		status("all transitions deactivated for this siphon");
		return 1; 
	} 
	// otherwise we check if we can enable these partially activated transitions
	for(wtit=wact.begin(); wtit!=wact.end(); ++wtit)
	{
		Place* p(NULL); // a place that prohibits the firing of the transition
		const set<Node*>& tpre((*wtit)->getPreset()); // get the transition's preset
		set<Node*>::const_iterator pwtit;
		for(pwtit=tpre.begin(); pwtit!=tpre.end(); ++pwtit)
		{
			p = (Place*)(*pwtit);
			if (am[p]<pn.findArc(*p,**wtit)->getWeight()) // and check for missing tokens
			{ // if a token is missing, we check the node/siphon belonging to this place
				if (node[p]->siphon.empty()) computeSWOMT(*(node[p])); // if uninitialized, compute it
				if (*(node[p]->siphon.begin())) break; // if there is a siphon, we need to check that node
			}	
		}
		cout << endl;
		if (pwtit==tpre.end()) { // all missing tokens can be produced, so we do it
			vector<Transition*> saraseq(callSara(*wtit)); // find an enabling firing sequence
			string tnames;
			for(unsigned int i=0; i<saraseq.size(); ++i) // fire it as far as sensible
			{
				if (init.postsiphon.find(saraseq[i])!=init.postsiphon.end())
				{ // if the next transition touches the initial siphon
					p = init.postsiphon[saraseq[i]];
					Transition* t(init.alloc[p]);
					fireTransition(t); // fire the allocation transition instead
					tnames += " "+t->getName(); 
					break; // no further transitions may fire (we just changed the sequence)
				} else {
					fireTransition(saraseq[i]); // otherwise just fire the transition
					tnames += " " + saraseq[i]->getName();
				}
			}
			status("firing%s",tnames.c_str());
			return 0; // after the whole sequence, make a restart
		} else { // so far, this w-activated transition cannot be enabled, due to a siphon without marked trap
			if (w.outP.find(*wtit)!=w.outP.end()) // if the transition already points to a siphon/node
				p = w.outP[*wtit]; // we get the defining place for that siphon
			else w.createArc(*wtit,p,*(node[p])); // otherwise we create an edge to such a siphon, using the known place
			if (done.find(node[p])==done.end()) // if we haven't visited the node p points to, we need to do that
			{
				int retval(adaptGraph(*(node[p]),done));
				if (retval<2) return retval; // this may be a final solution (1) or an order to revisit all nodes (0)
			}
		}
	}
	if (&w!=&init) return 2; // this siphon is done, backtrack
	// we have a result, but we need to construct it (it is not contained in this siphon alone)
	status("interlocking cycle of siphons without enabled transitions");
	// first, get a starting place for our "empty" siphon from this siphon 
	set<Place*>::iterator pit;
	for(pit=w.siphon.begin(); pit!=w.siphon.end(); ++pit)
		if (w.maxtrap.find(*pit)==w.maxtrap.end()) // the place can't be in the maximal trap
			if (am[*pit]<pn.findArc(**pit,*(w.alloc[*pit]))->getWeight()) break; // and must not have enough tokens
	map<Place*,WNode*> sdiff; // places and corresponding nodes we have to visit
	res.insert(*pit); // the resulting siphon
	sdiff[*pit]=&w;
	while (!sdiff.empty()) // as long as there are new places
	{
		Place* p(sdiff.begin()->first); // get one of them
		WNode* wp(sdiff.begin()->second); // and its node
		sdiff.erase(p); // we're working on it
		const set<Node*>& tset(p->getPreset()); // for every transition in the place's preset
		set<Node*>::const_iterator nit;
		for(nit=tset.begin(); nit!=tset.end(); ++nit)
		{
			Place* p2(NULL); // one place must be added to our resulting siphon
			WNode* wp2(NULL); // we also remember the node this place can be found in
			if (wp->isActivated(am,pre[(Transition*)(*nit)])) { // if the transition is partially activated in this node
				p2 = wp->outP[(Transition*)(*nit)]; // the added place
				wp2 = wp->out[(Transition*)(*nit)]; // is in another node (the transition points to it)
			} else {
				const set<Node*>& tpre((*nit)->getPreset()); // otherwise we find an empty place in the transition's preset
				set<Node*>::const_iterator tnit;
				for(tnit=tpre.begin(); tnit!=tpre.end(); ++tnit)
					if (wp->siphon.find((Place*)(*tnit))!=wp->siphon.end()) // in this node's siphon
						if (am[(Place*)(*tnit)]<pn.findArc(*((Place*)(*tnit)),*((Transition*)(*nit)))->getWeight())
						{ p2 = (Place*)(*tnit); wp2=wp; break; } // so we mark the place and stay here
			}
			if (res.find(p2)==res.end()) { // if the found place is new in our resulting siphon
				res.insert(p2); // we add it
				sdiff[p2]=wp2; // and mark it as "to do"
			}
		}
	}
	return 1; // now the resulting siphon has been constructed, we are done
}

/** Get a firing sequence that activates a transition from the current marking.
	@param t The transition to enable, which must be possible.
	@return The firing sequence that activates t.
*/
vector<Transition*> Witness::callSara(Transition* t) {
	map<Place*,unsigned int>::iterator xit;
	Marking m0(pn,false); // construct the initial marking of the problem
	Marking mf(pn,true); // and the final marking
	for(xit=pre[t].begin(); xit!=pre[t].end(); ++xit)
		mf[*(xit->first)] = xit->second;
	map<Place*,int> cover; // the latter one may be covered
	const set<Place*> pset(pn.getPlaces());
	set<Place*>::const_iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		cover[*pit]=GE;
	// finally, call the tool Sara to solve this coverability problem
	return (sara::ReachabilityTest(pn,m0,mf,cover));
}


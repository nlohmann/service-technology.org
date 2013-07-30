// -*- C++ -*-

/*!
 * \file    rules.cc
 *
 * \brief   Class Rules
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2013-05-21
 *
 * \date    $Date: 2013-07-19 12:00:00 +0200 (Fr, 19. Jul 2013) $
 *
 * \version $Revision: 1.00 $
 */

#include <set>
#include <map>
#include <string>
#include <sstream>
#include "rules.h"
#include "facts.h"
#include "Runtime.h"
#include <pthread.h>

using std::set;
using std::map;
using std::vector;

	/***************************************************
	* Implementation of the methods of the class Rules *
	***************************************************/

/** Constructor
    @param mat Incidence matrix of the net
    @param fkt Property handler for the net
*/
Rules::Rules(IMatrix& mat, Facts& fkt) : im(mat), facts(fkt) {}

/** Apply a rule given by its rule number
    @param mode The number of the rule
	@param tid The ID of the calling thread
*/
void Rules::apply(unsigned int mode, unsigned int tid) {
	Mode flag(1L<<mode);
	switch (flag) {
		case PARPLACE:		parallelPlaces(tid); break;
		case PARTRANSITION:	parallelTransitions(tid); break;
		case ISOLATEDTRANS: isolatedTransitions(tid); break;
		case EQUIVPLACE:	equivalentPlaces(tid); break;
		case MELDTRANS1:	meldTransitions1(tid); break;
		case MELDTRANS2:	meldTransitions2(tid); break;
		case MELDTRANS3:	break; //meldTransitions3(tid); break;
		case MELDTRANS4:	meldTransitions4(tid); break;
		case MELDTRANS5:	meldTransitions5(tid); break;
		case LOOPPLACE:		loopPlace2(tid); break;
		case LOOPTRANS:		loopTransition(tid); break;
		case LIVETRANS:		liveTransitions(tid); break;
		case SERIESPLACE:	seriesPlace(tid); break;
		case FINALPLACE:	finalPlace(tid); break;
		case FINALTRANS:	finalTransition(tid); break;
		case INITDEADPL2:	initiallyDeadPlace2(tid); break;
	}
}

/** Check the applicability of a rule to a node
    @param node The ID of the node
    @param mode The number of the rule
    @return If the rule should be checked
*/
bool Rules::checkAppl(Vertex node, unsigned int mode) {
	// check first if debug options exclude this rule
	if (Runtime::args_info.debug_arg)
		if ((Runtime::args_info.debug_arg & (1L<<mode)) == 0)
			return false;

	bool place(im.isPlace(node));
	switch (1L<<mode) {
		case LIVETRANS:		if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::REVERSE))
								return false;
							if (im.getPre(node).size()>0) place = true;
							return !place;
		case INITDEADPL2:	return place;
		case PARPLACE:		return place;
		case LOOPTRANS:		if (facts.checkCondition(Facts::CTL | Facts::LTL))
								return false;
							if (im.getPre(node).size()==0) place = true;
							if (im.getPost(node).size()==0) place = true;
							return !place;
		case PARTRANSITION: return !place;
		case ISOLATEDTRANS:	if (im.getPost(node).size()>0) place = true;
							if (im.getPre(node).size()>0) place = true;
							return !place;
		case MELDTRANS1:	if (facts.checkCondition(Facts::CTL | Facts::LTL))
								return false;
							if (im.getPre(node).size()==0) place = false;
							if (im.getPost(node).size()!=1) place = false;
							return place;
		case EQUIVPLACE:	if (facts.checkCondition(Facts::PATHS)) return false;
							if (im.getPre(node).size()==0) place = false;
							if (im.getPost(node).size()!=1) place = false;
							return place;
		case MELDTRANS4:
		case MELDTRANS2:	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::INVARIANT))
								return false;
							if (im.getPost(node).size()==0) place = false;
							if (im.getPre(node).size()!=1) place = false;
							return place;
		case MELDTRANS3:	return false;
		case MELDTRANS5:	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::INVARIANT))
								return false;
							if (im.getPost(node).size()<2) place = false;
							if (im.getPre(node).size()<2) place = false;
							return place;
		case LOOPPLACE:		if (im.getPost(node).size()==0) place = false;
							if (im.getPre(node).size()==0) place = false;
							return place;
		case SERIESPLACE:	if (facts.checkCondition(Facts::CTL | Facts::LTL))
								return false;
							if (im.getPre(node).size()!=1) place = false;
							return place;
		case FINALPLACE:	if (facts.checkCondition(Facts::REVERSE | Facts::INVARIANT))
								return false;
							if (facts.checkCondition(Facts::BOUNDEDNESS) && !facts.checkFact(Facts::UNBOUNDED))
								return false;
							if (im.getPre(node).size()==0) place = false;
							if (im.getPost(node).size()>0) place = false;
							return place;
		case FINALTRANS:	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::CTLX | Facts::LTLX 
													| Facts::BISIM | Facts::REVERSE | Facts::INVARIANT))
								return false;
							if (facts.checkCondition(Facts::LIVENESS)
								&& !facts.checkFact(Facts::NONLIVE)) return false;
							if (im.getPost(node).size()>0) place = true;
							if (im.getPre(node).size()==0) place = true;
							return !place;
	}
}

/** Starke's rule 1: Transitions with empty presets are removed together with their postsets
    @param The ID of the calling thread
*/
void Rules::liveTransitions(unsigned int tid) {
	// if the rule is inapplicable we are done
	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::REVERSE)) return;

    // properties for later checks
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));

	// find a starting transition "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::TR,LIVETRANS,0,0,0,NODE_SET_LIMIT))
	{

        // if we cannot remove mainid, get out
		if (im.isPersistent(mainid)
		|| (condition && (im.isVisible(mainid) || im.getLabelVis(mainid)))
		|| (conditio4 && (im.isVisible(mainid) || im.getLabel(mainid)))
		|| (conditio2 && im.getLabel(mainid))) {
			im.unlock(mainid,LIVETRANS);
			continue;
		}

		// get postset and marking of mainid, in form of a copy
	    Map post(im.getPost(mainid));

		// collect IDs of affected nodes and save the timestamp of mainid
		// if anyone tampers with the transition or its adjacent edges, we will know
	    Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		im.unlock(mainid);
		bool safe(true);
		Map::iterator mit;
	    for(mit=post.begin(); mit!=post.end(); ++mit)
		{
            // Must the place remain in the net?
			if (im.isPersistent(mit->first) || (conditio3 && im.isVisible(mit->first))) break;
			if (im.isIO(mit->first)) break;

			// all places in mainid's postset are affected
			nodestamp[mit->first] = im.rdlock(mit->first);
			collectNeighbors(mit->first, nodestamp);
			im.unlock(mit->first);
		}
		if (mit!=post.end()) {
			im.setMode(mainid,LIVETRANS);
			continue;
		}

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(LIVETRANS,mainid,nodestamp); // logging only

		// calculate path prefix (option --path or --complexpath)
		Map factor;
		for(mit=post.begin(); mit!=post.end(); ++mit)
			for(Map::iterator mit2=im.getPost(mit->first).begin(); mit2!=im.getPost(mit->first).end(); ++mit2)
				if (factor[mit2->first] < (mit2->second-1) / mit->second + 1)
					factor[mit2->first] = (mit2->second-1) / mit->second + 1;

		// change inheritable properties
		if (!post.empty()) {
			facts.addFact(Facts::UNBOUNDED,false);
			facts.addFact(Facts::UNSAFE,false);
		}
		facts.removeFact(Facts::CTL | Facts::LTL | Facts::REVERSE);

		// apply the rule and change properties
		for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
		{
			if (mit->first == mainid) { // the live transition
				mit->second = 0; // remove mainid
				facts.setStatus(Facts::PATH, mainid, Facts::UNUSABLE);
				facts.setStatus(Facts::LIVE, mainid, Facts::ISTRUE);
				if (im.isVisible(mainid) || im.getLabelVis(mainid))
					facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
				else if (im.getLabel(mainid))
					facts.removeFact(Facts::ALTL | Facts::BISIM);
				im.clearLabel(mainid);
			} else if (im.isPlace(mit->first)) { // a live place in the postset
				// erase the place with all its edges
				im.removeArcs(mit->first);
				mit->second = 0; // remove the place

				// export formula rule if the node is visible
				map<Vertex,int> tmp;
				facts.setMarking(tmp,mit->first,-1); // any number of tokens possible
				facts.setStatus(Facts::MARKING,mit->first,Facts::UNUSABLE);
				facts.setStatus(Facts::BOUNDED,mit->first,Facts::ISFALSE);
				facts.setStatus(Facts::SAFE,mit->first,Facts::ISFALSE);
			} else { // transitions after one of the places must be prefixed in paths with the removed transition
				if (factor[mit->first] > 0)
					facts.addPath(mit->first, Path(factor[mit->first],mainid));
			}
		}

		// mark surrounding area as changed and unlock node
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 3, places only: Two places have identical pre- and postsets (including arc weights)
    @param The ID of the calling thread
*/
void Rules::parallelPlaces(unsigned int tid) {

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,PARPLACE,0,NODE_SET_LIMIT,0,NODE_SET_LIMIT)) {

		// get pre/postset and marking of mainid
	    Map& post1(im.getPost(mainid));
		Map& pre1(im.getPre(mainid));
		unsigned int tokens1(im.getTokens(mainid));

		// select any transition in pre- or postset
		Vertex t;
		bool inPre(false);
		if (!pre1.empty()) { t = pre1.begin()->first; inPre = true; }
		else if (!post1.empty()) t = post1.begin()->first;
		else {
			im.unlock(mainid,PARPLACE);
			continue;
		}

		// save timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		im.unlock(mainid);

		// obtain the selected transition's post/preset (which contains mainid)
		im.rdlock(t);
		Map par(inPre ? im.getPost(t) : im.getPre(t));
		im.unlock(t);

		// compare pre- and postset of mainid with all other places in par
		unsigned int tokens2,stampp2;
		Map::iterator mit;
		for(mit=par.begin(); mit!=par.end(); ++mit)
		{
			if (mit->first == mainid) continue; // do not compare mainid with itself
			// read-lock the two places in correct order to avoid deadlocks
			stampp2 = im.rdlock(mainid,mit->first);

			// flag if we got a wrong node
			bool flag(false);

			// if one of the places is an IO label, we cannot act
			if (im.isIO(mit->first) || im.isIO(mainid)) flag=true;

			// if one of the places is persistent it must be the one with less tokens
			tokens2 = im.getTokens(mit->first);
			if (im.isPersistent(mit->first)) {
				if (tokens1<tokens2) flag=true;
				if (tokens1==tokens2 && im.isPersistent(mainid)) flag=true; 
			} 
			if (tokens1>tokens2 && im.isPersistent(mainid)) flag=true;

			// check if we can transfer the safety check to the reduced net
			if (tokens1+tokens2==1 && facts.checkCondition(Facts::SAFETY)
				&& !facts.checkFact(Facts::UNSAFE)) flag=true;
			if (tokens1+tokens2==1 && facts.checkCondition(Facts::SAFETY)
				&& im.isVisible(tokens1==1 ? mainid : mit->first)) flag=true;

			// check if the places have the same pre- and postset
			if (!flag && checkParallel(mainid,mit->first)) break;

			// wrong places, unlock them
			im.unlock(mit->first);
			im.unlock(mainid);
		}
		if (mit==par.end()) {
			// no place parallel to mainid found
			im.setMode(mainid,PARPLACE);
			continue;
		}

		// this is the parallel place
		Vertex secid(mit->first);
		nodestamp[secid] = stampp2;

		// collect IDs of affected nodes
		collectNeighbors(mainid, nodestamp);
		im.unlock(secid);
	    im.unlock(mainid);
	
		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(PARPLACE,mainid,nodestamp);

		// determine which place to remove (depends on token number and possibly on visibility)
		bool removemain(tokens1>tokens2);
		if (tokens1==tokens2)
			if (im.isPersistent(secid) || im.isInvisible(mainid)) 
					removemain = true;
		unsigned int delid(removemain ? mainid : secid);
		unsigned int keepid(removemain ? secid : mainid);
		unsigned int tokendiff(removemain ? tokens1-tokens2 : tokens2-tokens1);

		// apply the rule
		im.removeArcs(delid);
		nodestamp[delid] = 0; // remove the node
		map<Vertex,int> tmp;
		tmp[delid] = 1;
		tmp[NO_NODE] = -tokendiff;
		facts.setMarking(tmp, keepid, 1);
//		facts.setMarking(delid, keepid, tokendiff);
		facts.setBounded(delid, keepid);
		if (im.isVisible(delid) && im.isInvisible(keepid)) im.makeVisible(keepid);
		if (tokens1>1 && tokens2>1)
			facts.setStatus(Facts::SAFE, keepid, Facts::ISFALSE);
		if (tokens1>1 || tokens2>1) {
			facts.setStatus(Facts::SAFE, delid, Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		} else if (!tokendiff) {
			facts.setSafe(delid, keepid);
			if (!im.isInvisible(delid) && im.isInvisible(keepid)) im.makeVisible(keepid);
		}

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 3, transitions only: Two transitions have identical pre- and postsets
    @param The ID of the calling thread
*/
void Rules::parallelTransitions(unsigned int tid) {
	// conditions for later checks
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::BISIM));

	// find a starting transition "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::TR,PARTRANSITION,0,NODE_SET_LIMIT,0,NODE_SET_LIMIT)) {

		// get pre/postset of mainid
	    Map& post1(im.getPost(mainid));
		Map& pre1(im.getPre(mainid));

		// select any place in pre- or postset
		Vertex p;
		bool inPre(false);
		if (!pre1.empty()) { p = pre1.begin()->first; inPre = true; }
		else if (!post1.empty()) p = post1.begin()->first;
		else {
			im.unlock(mainid,PARTRANSITION);
			continue;
		}

		// save timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		im.unlock(mainid);

		// obtain the selected place's post/preset (which contains mainid)
		im.rdlock(p);
		Map par(inPre ? im.getPost(p) : im.getPre(p));
		im.unlock(p);

		// compare pre- and postset of mainid with all other transitions in par
		unsigned int stampt2;
		Map::iterator mit;
		for(mit=par.begin(); mit!=par.end(); ++mit)
		{
			if (mit->first == mainid) continue; // do not compare mainid with itself

			// read-lock the two transitions in correct order to avoid deadlocks
			stampt2 = im.rdlock(mainid,mit->first);

			// flag for wrong node
			bool flag(false);

			// none of the transitions can be persistent
			if (im.isPersistent(mit->first)	|| im.isPersistent(mainid)) flag=true;

			if (conditio2) { // check if we would hurt action-based LTL (lose next)
				if (im.getLabel(mainid)==0 && im.getLabel(mit->first)!=0) flag=true;
				if (im.getLabel(mainid)!=0 && im.getLabel(mit->first)==0) flag=true;
			} 
			if (condition) { // check if we would hurt action-based LTL-X
				if (!im.isInvisible(mit->first) || !im.isInvisible(mainid)) flag=true;
				else if (im.getLabelVis(mainid)!=im.getLabelVis(mit->first)) flag=true;
			}
			if (conditio3) { // check if we would lose bisimulation
				if (!im.isInvisible(mit->first) || !im.isInvisible(mainid)) flag=true;
				if (im.getLabel(mainid) != im.getLabel(mit->first)) flag=true;
			}

			// check if the transitions have the same pre- and postset
			if (!flag && checkParallel(mainid,mit->first)) break;

			// unlock the transitions, we chose the wrong ones
			im.unlock(mit->first);
			im.unlock(mainid);
		}
		if (mit==par.end()) {
			// no transition parallel to mainid found
			im.setMode(mainid,PARTRANSITION);
			continue;
		}

		// this is the parallel transition
		unsigned int secid(mit->first);
		nodestamp[secid] = stampt2;

		// collect IDs of affected nodes
		collectNeighbors(mainid, nodestamp);
		im.unlock(secid);
	    im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(PARTRANSITION,mainid,nodestamp);

		// determine which transition to remove (depends on visibility)
		bool removemain(im.isInvisible(mainid));
		unsigned int delid(removemain ? mainid : secid);
		unsigned int keepid(removemain ? secid : mainid);

		// apply the rule
		im.removeArcs(delid);
		nodestamp[delid] = 0; // remove the node
		facts.setStatus(Facts::PATH, delid, Facts::UNUSABLE);
		if (!im.isInvisible(delid) || !im.isInvisible(keepid))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabelVis(delid)!=im.getLabelVis(keepid))
			facts.removeFact(Facts::ALTL | Facts::ALTLX);
		else if (im.getLabel(delid)==0 && im.getLabel(keepid)!=0)
			facts.removeFact(Facts::ALTL);
		else if (im.getLabel(delid)!=0 && im.getLabel(keepid)==0)
			facts.removeFact(Facts::ALTL);
		if (im.getLabel(delid)!=im.getLabel(keepid))
			facts.removeFact(Facts::BISIM);
		facts.setLive(delid,keepid);
		if (im.isVisible(delid) && im.isInvisible(keepid)) im.makeVisible(keepid);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 3, for isolated transitions
    @param The ID of the calling thread
*/
void Rules::isolatedTransitions(unsigned int tid) {
	// a condition for later checks
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTL | Facts::LTL));
	bool conditio4(facts.checkCondition(Facts::BISIM));

	Map nodestamp(im.getIsolated(IMatrix::TR,ISOLATEDTRANS));
	printApply(ISOLATEDTRANS,nodestamp.begin()->first,nodestamp); // logging only

    // in general, at least one transition must remain, and any of two transitions
    // with the same label, at least one can be removed. The next operator for
    // temporal logics and visible transitions/labels complicate things.

    // check every node against ...
	Map::iterator mit(nodestamp.begin());
	while (mit != nodestamp.end()) {
		im.setMode(mit->first,ISOLATEDTRANS,false);
		if (mit->second == 0 || im.isPersistent(mit->first)) 
			{ ++mit; continue; } // already deleted this one or cannot merge it

        // ... every other node
		Map::iterator mit2(mit);
        // count how many other nodes with the same (visible) label there are
		unsigned int samelabel(0), samevislabel(0), remaining(nodestamp.size());
		while (++mit2 != nodestamp.end()) {
			if (mit2->second == 0) continue; // already deleted this one
			if (im.getLabel(mit->first)==im.getLabel(mit2->first)) ++samelabel;
			if (im.getLabelVis(mit->first)==im.getLabelVis(mit2->first)) ++samevislabel;

            // check if we must not delete this transition:
			if (im.isPersistent(mit2->first)) continue;
			if (condition && im.isVisible(mit2->first)) continue;
			if (conditio2 && im.getLabel(mit->first)==0 
						  && im.getLabel(mit2->first)!=0) continue;
			if (condition && im.getLabelVis(mit->first)!=im.getLabelVis(mit2->first) 
						  && im.getLabel(mit2->first)>0) continue; 
			if (im.getLabel(mit->first)!=im.getLabel(mit2->first)
				|| im.isVisible(mit2->first) || im.isVisible(mit->first)) {
					if (conditio4) continue;
					else facts.removeFact(Facts::BISIM);
			}

			if (im.getLabel(mit->first)==im.getLabel(mit2->first)) --samelabel;
			if (im.getLabelVis(mit->first)==im.getLabelVis(mit2->first)) --samevislabel;
			mit2->second = 0; // delete the transition
			facts.setStatus(Facts::PATH,mit2->first,Facts::UNUSABLE);
			facts.setStatus(Facts::LIVE,mit2->first,Facts::ISTRUE);
			--remaining;
		}

        // check if we can delete the "outer" transition
		bool flag(true);
		if (condition && im.isVisible(mit->first)) flag=false;
		if (condition && im.getLabelVis(mit->first)>0 && samevislabel==0) flag=false;
		if (conditio2 && im.getLabel(mit->first)>0 && samelabel==0) flag=false;
		if (conditio3 && remaining==1) flag=false;
		if (conditio4 && (im.isVisible(mit->first) || im.getLabel(mit->first))) flag=false;
		if (flag) { 
			mit->second = 0; 			
			facts.setStatus(Facts::PATH,mit->first,Facts::UNUSABLE);
			facts.setStatus(Facts::LIVE,mit->first,Facts::ISTRUE);
			if (im.isVisible(mit->first) || im.getLabel(mit->first))
				facts.removeFact(Facts::BISIM);
			--remaining; 
			if (!remaining) facts.removeFact(Facts::CTL | Facts::LTL);
		}
		++mit;
	}

	im.unlock(nodestamp);
}

/** Starke's rule 4: Equivalent places (two transitions with identical pre-/postsets except for one place in each preset)
    @param The ID of the calling thread
*/
void Rules::equivalentPlaces(unsigned int tid) {
    // We cannot uphold strict paths, only complex ones (with or-operators in them)
	if (facts.checkCondition(Facts::PATHS)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX));
	bool conditio3(facts.checkCondition(Facts::ALTL));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::REVERSE));
	bool conditio6(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));
	bool conditio7(facts.checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED));
	bool conditio8(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,EQUIVPLACE,1,NODE_SET_LIMIT,1,1)) {

		// get pre/postset of mainid for this
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int tokens1(im.getTokens(mainid));

		// check for postset singleton edge and invisibility in formulas
		// as well as IO
		if (postp1.begin()->second != 1 || im.isIO(mainid) || im.isPersistent(mainid)
			|| (conditio5 && tokens1>0) 
			|| (conditio8 && im.isVisible(mainid) && tokens1<2)
			|| ((conditio2 || conditio7) && !im.isInvisible(mainid))) {
			im.unlock(mainid,EQUIVPLACE);
			continue;
		}

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// get info about the postset transition
		Vertex t1(postp1.begin()->first);
		im.unlock(mainid);
		nodestamp[t1] = im.rdlock(t1);
	    Map& postt1(im.getPost(t1));
		Map& pret1(im.getPre(t1));
		
		// select any place in pre- or postset of t1 (other than mainid)
		Vertex p;
		bool inPre(false);
		if (!pret1.empty()) { 
			p = pret1.begin()->first;
			if (p==mainid) p = pret1.rbegin()->first;
			if (p!=mainid) inPre = true;
		} 
		if (!inPre && !postt1.empty()) p = postt1.begin()->first;
		else if (!inPre) {
			im.unlock(t1);
			im.setMode(mainid,EQUIVPLACE);
			continue;
		}
		im.unlock(t1);

		// obtain the selected place's post/preset (which contains t1)
		im.rdlock(p);
		Map par(inPre ? im.getPost(p) : im.getPre(p));
		im.unlock(p);

		// compare pre- and postset of t1 with all other transitions in par
		Map::iterator mit;
		unsigned int stampt2, stampp2, tokens2;
		Vertex p2;
		for(mit=par.begin(); mit!=par.end(); ++mit)
		{
			if (mit->first == t1) continue; // do not compare t1 with itself

			// read-lock the two transitions in correct order to avoid deadlocks
			stampt2 = im.rdlock(t1,mit->first);

			/// flag for wrong nodes
			bool flag(false);

			// none of the transitions can be persistent(visible)
			if (im.isPersistent(mit->first)	|| im.isPersistent(t1)) flag=true;
			if (conditio3) { // check if we would hurt action-based LTL (next)
				if (im.getLabel(t1)==0 && im.getLabel(mit->first)!=0) flag=true;
				if (im.getLabel(t1)!=0 && im.getLabel(mit->first)==0) flag=true;
			}
			if (condition) { // check if we would hurt action-based LTL-X
				if (!im.isInvisible(mit->first)	|| !im.isInvisible(t1)) flag=true;
				if (im.getLabelVis(t1)!=im.getLabelVis(mit->first)) flag=true;
			}
			if (conditio4) { // check if we would hurt bisimulation equivalence
				if (!im.isInvisible(mit->first)	|| !im.isInvisible(t1)) flag=true;
				if (im.getLabel(t1)!=im.getLabel(mit->first)) flag=true;
			}
			if (conditio6) { // check if single transition liveness gets lost
				if (!im.isInvisible(mit->first) || !im.isInvisible(t1)) flag=true;
			}

			// check if the transitions have the same pre- and postset (except for mainid, p2)
			if (!flag) p2 = checkParallel(t1,mit->first,mainid);

			// unlock the transitions before checking the place p2
			im.unlock(mit->first);
			im.unlock(t1);

			// check if the transitions are parallel (except for mainid, p2)
			if (flag || p2 == NO_NODE) continue;

			// the place must be removable among other things
			// check also for simple edge from p2 to t2 and initial tokens
			stampp2 = im.rdlock(p2);
			tokens2 = im.getTokens(p2);
			if (im.getPost(p2).size()==1 && im.getPost(p2)[mit->first]==1
				&& !im.isIO(p2) && !im.isPersistent(p2)
				&& (!conditio5 || tokens2==0)
				&& (!conditio8 || im.isInvisible(p2) || tokens2>1)
				&& ((!conditio2 && !conditio7) || im.isInvisible(p2))
				&& ((tokens1>1 && tokens2>1) || !facts.checkCondition(Facts::UNSAFE))) break;
			im.unlock(p2);
		}
		if (mit==par.end()) {
			// no transition parallel to t1 found
			im.setMode(mainid,EQUIVPLACE);
			continue;
		}

		// this is the parallel transition
		Vertex t2(mit->first);

		// collect IDs of affected nodes
		nodestamp[p2] = stampp2;
		collectNeighbors(p2, nodestamp);
		im.unlock(p2);
		im.rdlock(t2);
		nodestamp[t2] = stampt2;
		collectNeighbors(t2, nodestamp);
	    im.unlock(t2);
		im.rdlock(mainid);
		collectNeighbors(mainid, nodestamp);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(EQUIVPLACE,mainid,nodestamp);

		// determine which transition to remove (depends on visibility)
		bool removemain(im.isInvisible(mainid));

		// construct path translation
		facts.addFixedPath(removemain?t2:t1, Path(1,removemain?mainid:p2), false);
		facts.addFixedPath(removemain?t2:t1, Path(1,removemain?mainid:p2));
		facts.addPath(removemain?t2:t1, Path(1,removemain?t1:t2));
		facts.addFixedPath(removemain?t2:t1, Path(1,removemain?mainid:p2));
		facts.addPath(removemain?p2:mainid, Path(1,removemain?mainid:p2));
		facts.removeFact(Facts::PATHS);

		// apply the rule, first shift the edges of the place to be deleted
		movePre(removemain ? mainid : p2, removemain ? p2 : mainid);
		im.addTokens(removemain ? p2 : mainid,im.getTokens(removemain ? mainid : p2));
		im.removeArcs(removemain ? mainid : p2);
		im.removeArcs(removemain ? t1 : t2);
		nodestamp[removemain ? mainid : p2] = 0; // remove the place
		nodestamp[removemain ? t1 : t2] = 0; // remove the transition

		// check and deduce properties
		if (!im.isInvisible(t1) || !im.isInvisible(t2))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		if (removemain && im.isVisible(t1)) im.makeVisible(t2);
		if (!removemain && im.isVisible(t2)) im.makeVisible(t1);
		if (removemain && im.isVisible(mainid)) im.makeVisible(p2);
		if (!removemain && im.isVisible(p2)) im.makeVisible(mainid);
		facts.setStatus(Facts::LIVE, t1, Facts::UNUSABLE);
		facts.setStatus(Facts::LIVE, t2, Facts::UNUSABLE);
		set<Vertex> tmp;
		tmp.insert(t1);
		tmp.insert(t2);
		facts.setLive(tmp,removemain?t2:t1);
		if (im.getLabelVis(t1)!=im.getLabelVis(t2))
			facts.removeFact(Facts::ALTL | Facts::ALTLX);
		if (im.getLabel(t1)==0 && im.getLabel(t2)!=0)
			facts.removeFact(Facts::ALTL);
		if (im.getLabel(t1)!=0 && im.getLabel(t2)==0)
			facts.removeFact(Facts::ALTL);
		if (im.getLabel(t1)!=im.getLabel(t2))
			facts.removeFact(Facts::BISIM);
		if (!im.isInvisible(mainid) || !im.isInvisible(p2))
			facts.removeFact(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX);
		facts.setStatus(Facts::MARKING, mainid, Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED, removemain ? p2 : mainid, Facts::UNUSABLE);
		facts.setStatus(Facts::SAFE, mainid, tokens1>1 ? Facts::ISFALSE : Facts::UNUSABLE);
		facts.setStatus(Facts::MARKING, p2, Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED, removemain ? mainid : p2, Facts::UNUSABLE);
		facts.setStatus(Facts::SAFE, p2, tokens2>1 ? Facts::ISFALSE : Facts::UNUSABLE);
		set<Vertex> tmp2;
		tmp2.insert(mainid);
		tmp2.insert(p2);
		map<Vertex,int> tmp3;
		tmp3[mainid] = 1;
		tmp3[p2] = 1;
		facts.setMarking(tmp3,removemain?p2:mainid,1);
		facts.setBounded(tmp2,removemain?p2:mainid);
		if (tokens1>1 || tokens2>1)
			facts.addFact(Facts::UNSAFE,false);
		if (tokens1>0 || tokens2>0)
			facts.removeFact(Facts::REVERSE);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 5, n=1: Melding of preset and postset of one place, here: singleton postset
    @param The ID of the calling thread
*/
void Rules::meldTransitions1(unsigned int tid) {
	// the rule will destroy state-based next-operators
	if (facts.checkCondition(Facts::CTL | Facts::LTL)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::REVERSE));
	bool conditio6(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));
	bool conditio7(facts.checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED));
	bool conditio8(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,MELDTRANS1,1,NODE_SET_LIMIT,1,1)) {

		// check for invisibility in formulas
		if (im.isPersistent(mainid) || ((conditio3 || conditio7) && im.isVisible(mainid))) 
		{
			im.unlock(mainid,MELDTRANS1);
			continue;
		}

		// get pre/postset of mainid, weight of the postset edge, and token number
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int v(postp1.begin()->second);
		unsigned int tokens(im.getTokens(mainid));

		// all preset edges must have a multiple of v as weight
		Map::iterator mit;
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			if (mit->second % v) break;
		// if not, or if state-based logic would get hurt,
		// or pre/postset are not disjoint, choose another place
		if (mit!=prep1.end() || 
			((conditio3 || conditio5) && tokens>=v) || 
			(v>1 && tokens<2 && facts.checkCondition(Facts::SAFETY) && !facts.checkFact(Facts::UNSAFE)) ||
			(conditio8 && tokens<2 && im.isVisible(mainid)) ||
			!disjoint(prep1,postp1)) 
		{
			im.unlock(mainid,MELDTRANS1);
			continue;
		}

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// get info about the postset transition
		Vertex t1(postp1.begin()->first);
		nodestamp[t1] = im.rdlock(t1);
	    Map& postt1(im.getPost(t1));
		Map& pret1(im.getPre(t1));

		// if the transition does not fulfill the prerequisites
		bool flag(false);
		if (postt1.empty() && facts.checkCondition(Facts::BOUNDEDNESS) 
			&& !facts.checkFact(Facts::UNBOUNDED)) flag=true;
		if (pret1.size()>1 || pret1.begin()->first!=mainid) flag=true;
		if (im.isPersistent(t1)) flag=true;
		if (condition && im.getLabelVis(t1)) flag=true;
		if (condition && !im.isInvisible(t1)) flag=true;
		if (conditio2 && im.getLabel(t1)) flag=true;
		if (conditio4 && (!im.isInvisible(t1) || im.getLabel(t1))) flag=true;
		if (flag) {
			im.unlock(t1);
			im.unlock(mainid,MELDTRANS1);
			continue;
		}
		
		// collect IDs of affected nodes
		collectNeighbors(mainid, nodestamp);
		collectNeighbors(t1, nodestamp);
	    im.unlock(t1);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(MELDTRANS1,mainid,nodestamp);

		// deduced properties
		set<Vertex> tmp;
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			tmp.insert(mit->first);
		facts.setLive(t1,tmp);
		if (conditio6 && !im.isInvisible(t1)) {
			for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
				if (im.isInvisible(mit->first)) im.makeVisible(mit->first);
		}
		for(mit=postt1.begin(); mit!=postt1.end(); ++mit) {
			if (tokens>=v) 
				facts.setStatus(Facts::MARKING,mit->first,Facts::UNUSABLE);
			map<Vertex,int> tmp2;
			tmp2[mit->first] = v;
			tmp2[mainid] = mit->second;
			tmp2[NO_NODE] = (tokens%v) * mit->second;
			facts.setMarking(tmp2, mit->first, v);
		}
		facts.setStatus(Facts::MARKING,mainid,Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::UNUSABLE);
		if (tokens>1) {
			facts.setStatus(Facts::SAFE,mainid,Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		} else facts.setStatus(Facts::SAFE,mainid,Facts::UNUSABLE);
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			facts.addPath(mit->first, Path(mit->second/v,t1), false);
		facts.setStatus(Facts::PATH,t1,Facts::UNUSABLE);
		if (postt1.empty())
			facts.removeFact(Facts::BOUNDEDNESS);

		// apply the rule
		while (im.fire(t1))  // fire t1 as long as possible
			facts.addPath(NO_NODE, Path(1,t1), false); 
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			im.addPost(t1,mit->first,mit->second/v);
		im.removeArcs(mainid);
		im.removeArcs(t1);
		nodestamp[mainid] = 0; // remove the place
		nodestamp[t1] = 0; // remove the transition

		// destroyed properties
		facts.removeFact(Facts::CTL | Facts::LTL);
		if (!im.isInvisible(t1) || im.getLabelVis(t1))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabel(t1))
			facts.removeFact(Facts::ALTL | Facts::BISIM);
		if (!im.isInvisible(mainid) || tokens>=v)
			facts.removeFact(Facts::CTLX | Facts::LTLX);
		if (tokens>=v)
			facts.removeFact(Facts::REVERSE);

		// mark surrounding area as changed
		im.propagateChange(nodestamp);
//		im.unlock(nodestamp);
	}
}

/** Starke's rule 5, n>1, k=1: Melding of preset and postset of one place, here: singleton preset
    @param The ID of the calling thread
*/
void Rules::meldTransitions2(unsigned int tid) {
	// the rule will destroy state-based next-operators
	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::INVARIANT)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));
	bool conditio6(facts.checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED));
	bool conditio7(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,MELDTRANS2,1,1,1,NODE_SET_LIMIT)) {

		// check for invisibility in formulas
		if (im.isPersistent(mainid) || ((conditio3 || conditio6) && im.isVisible(mainid))) 
		{
			im.unlock(mainid,MELDTRANS2);
			continue;
		}

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int v(prep1.begin()->second);
		unsigned int tokens(im.getTokens(mainid));

		// all postset edges must have v as weight
		Map::iterator mit;
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (mit->second != v) break;
		// if not, or if state-based logic would get hurt,
		// or pre/postset are not disjoint, choose another place
		if (mit!=postp1.end() || 
			tokens>=v || 
			(v>1 && tokens<2 && facts.checkCondition(Facts::SAFETY) && !facts.checkFact(Facts::UNSAFE)) ||
			(tokens<2 && conditio7 && im.isVisible(mainid)) ||
			!disjoint(prep1,postp1)) 
		{
			im.unlock(mainid,MELDTRANS2);
			continue;
		}

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// get info about the postset transitions
		bool flag(false),flag2(true);
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
		{
			nodestamp[mit->first] = im.rdlock(mit->first);
			if (im.getPre(mit->first).size()>1) break;
			if (!im.getPost(mit->first).empty()) flag2=false;
			if (im.isPersistent(mit->first)) break;
			if (condition && im.getLabelVis(mit->first)) break;
			if (condition && !im.isInvisible(mit->first)) break;
			if (conditio2 && im.getLabel(mit->first)) break;
			if (conditio4 && (!im.isInvisible(mit->first) || im.getLabel(mit->first))) break;
			im.unlock(mit->first);
		}
		if ((flag2 && facts.checkCondition(Facts::BOUNDEDNESS) && !facts.checkFact(Facts::UNBOUNDED)) 
			|| mit!=postp1.end()) {
				if (mit!=postp1.end()) im.unlock(mit->first);
				im.unlock(mainid,MELDTRANS2);
				continue;
		}

		// check the pre-transition of mainid
		Vertex t1(prep1.begin()->first);
		nodestamp[t1] = im.rdlock(t1);

		// if the transition does not fulfill the prerequisites
		if (im.isPersistent(t1)) flag=true;
		if (condition && im.getLabelVis(t1)) flag=true;
		if (condition && !im.isInvisible(t1)) flag=true;
		if (conditio2 && im.getLabel(t1)) flag=true;
		if (conditio4 && (!im.isInvisible(t1) || im.getLabel(t1))) flag=true;
		if (flag) {
			im.unlock(t1);
			im.unlock(mainid,MELDTRANS2);
			continue;
		}
		
		// collect IDs of affected nodes
		collectNeighbors(t1, nodestamp);
	    im.unlock(t1);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(MELDTRANS2,mainid,nodestamp);

		// path modifications
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			facts.addPath(mit->first, Path(1,t1));
		facts.setStatus(Facts::PATH,t1,Facts::UNUSABLE);

		// marking invariant projection
		Map& pret1(im.getPre(t1));
		for(mit=pret1.begin(); mit!=pret1.end(); ++mit) {
			map<Vertex,int> tmp2;
			tmp2[mit->first] = v;
			tmp2[mainid] = mit->second;
			tmp2[NO_NODE] = tokens * mit->second;
			facts.setMarking(tmp2, mit->first, v);
		}

		// apply the rule
		Vertex save(postp1.begin()->first);
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
		{
			if (!im.isInvisible(mit->first)) save=mit->first;
			im.addPre(t1,mit->first,1);
			im.addPost(t1,mit->first,1);
		}
		im.removeArcs(mainid);
		im.removeArcs(t1);
		nodestamp[mainid] = 0; // remove the place
		nodestamp[t1] = 0; // remove the transition

        // change properties
		facts.removeFact(Facts::CTL | Facts::LTL);
		if (!im.isInvisible(t1) || im.getLabelVis(t1))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabel(t1))
			facts.removeFact(Facts::ALTL | Facts::BISIM);
		if (!im.isInvisible(mainid))
			facts.removeFact(Facts::CTLX | Facts::LTLX);
		facts.setStatus(Facts::MARKING,mainid,Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::UNUSABLE);
		facts.setLive(t1,save);
		if (conditio5 && im.isVisible(t1) && im.isInvisible(save)) 
			im.makeVisible(save);
		if (tokens>1) {
			facts.setStatus(Facts::SAFE,mainid,Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		} else facts.setStatus(Facts::SAFE,mainid,Facts::UNUSABLE);
		if (flag2)
			facts.removeFact(Facts::BOUNDEDNESS);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 6: t1 with only p in its postset, p with only t1 in its preset, preset of t1 has t1 as
	its only post-transition, t1 not in postset of p; all arcs around p have same weight, initial marking
	on p is less; remove p and t1
    @param The ID of the calling thread
*/
void Rules::meldTransitions4(unsigned int tid) {
	// the rule will destroy state-based next-operators
	if (facts.checkCondition(Facts::CTL | Facts::LTL)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));
	bool conditio6(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,MELDTRANS4,1,1,1,NODE_SET_LIMIT)) {

		// check for invisibility in formulas
		if (im.isPersistent(mainid) || (conditio3 && im.isVisible(mainid))) 
		{
			im.unlock(mainid,MELDTRANS4);
			continue;
		}

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int v(prep1.begin()->second);
		unsigned int tokens(im.getTokens(mainid));

		// all postset edges must have v as weight
		Map::iterator mit;
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (mit->second != v) break;
		// if not, or if state-based logic would get hurt,
		// or pre/postset are not disjoint, choose another place
		if (mit!=postp1.end()
			|| tokens>=v 
			|| (v>1 && tokens<2 && facts.checkCondition(Facts::SAFETY) && !facts.checkFact(Facts::UNSAFE))
			|| (tokens<2 && conditio6 && im.isVisible(mainid))
			|| !disjoint(prep1,postp1)) 
		{
			im.unlock(mainid,MELDTRANS4);
			continue;
		}

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		collectNeighbors(mainid,nodestamp);

		// check the pre-transition of mainid
		Vertex t1(prep1.begin()->first);
		nodestamp[t1] = im.rdlock(t1);
		Map pret1(im.getPre(t1)); // make a copy

		// if the transition does not fulfill the prerequisites
		bool flag(false);
		if (pret1.empty()) flag=true;
		if (im.getPost(t1).size()>1) flag=true;
		if (im.isPersistent(t1)) flag=true;
		if (condition && im.getLabelVis(t1)) flag=true;
		if ((condition || conditio5) && !im.isInvisible(t1)) flag=true;
		if (conditio2 && im.getLabel(t1)) flag=true;
		if (conditio4 && (!im.isInvisible(t1) || im.getLabel(t1))) flag=true;
		if (flag) {
			im.unlock(t1);
			im.unlock(mainid,MELDTRANS4);
			continue;
		}
		
		// unlock the nodes before locking others
	    im.unlock(t1);
		im.unlock(mainid);

		// check that t1 is the only transition in its pre-places' postsets
		for(mit=pret1.begin(); mit!=pret1.end(); ++mit)
		{
			nodestamp[mit->first] = im.rdlock(mit->first);
			if (im.getPost(mit->first).size()!=1) break;
			im.unlock(mit->first);
		}
		if (mit!=pret1.end()) {
			im.unlock(mit->first);
			im.setMode(mainid,MELDTRANS4);
			continue;
		}

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(MELDTRANS4,mainid,nodestamp);

		// path modifications
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			facts.addPath(mit->first, Path(1,t1));
		facts.setStatus(Facts::PATH, t1, Facts::UNUSABLE);

        // change properties
		facts.removeFact(Facts::CTL | Facts::LTL);
		if (!im.isInvisible(t1) || im.getLabelVis(t1))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabel(t1))
			facts.removeFact(Facts::ALTL | Facts::BISIM);
		if (!im.isInvisible(mainid))
			facts.removeFact(Facts::CTLX | Facts::LTLX);
		// the place is unbounded if all pre-places are unbounded
		set<Vertex> tmp;
		for(mit=pret1.begin(); mit!=pret1.end(); ++mit) {
			tmp.insert(mit->first);
			if (im.isVisible(mainid) && im.isInvisible(mit->first)) im.makeVisible(mit->first);
		}
		facts.setBounded(mainid,tmp);
		facts.setStatus(Facts::MARKING,mainid,Facts::UNUSABLE);
		// marking invariant projection
		for(mit=pret1.begin(); mit!=pret1.end(); ++mit) {
			map<Vertex,int> tmp2;
			tmp2[mit->first] = v;
			tmp2[mainid] = mit->second;
			tmp2[NO_NODE] = tokens * mit->second;
			facts.setMarking(tmp2, mit->first, v);
		}
		// safety cannot be inherited, but possibly decided
		if (tokens>1) {
			facts.setStatus(Facts::SAFE, mainid, Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		} else facts.setStatus(Facts::SAFE, mainid, Facts::UNUSABLE);

		// apply the rule
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			im.addPre(t1,mit->first,1);
		im.removeArcs(mainid);
		im.removeArcs(t1);
		nodestamp[mainid] = 0; // remove the place
		nodestamp[t1] = 0; // remove the transition

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 5, n>1, k>1: Melding of preset and postset of one place, here: by creating new transitions
    @param The ID of the calling thread
*/
void Rules::meldTransitions5(unsigned int tid) {
	// the rule will destroy state-based next-operators
	if (facts.checkCondition(Facts::CTL | Facts::LTL | Facts::INVARIANT)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));
	bool conditio6(facts.checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED));
	bool conditio7(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,MELDTRANS5,2,NODE_SET_LIMIT,2,NODE_SET_LIMIT)) {

		// check for invisibility in formulas
		if (im.isPersistent(mainid) || ((conditio3 || conditio6) && im.isVisible(mainid))) 
		{
			im.unlock(mainid,MELDTRANS5);
			continue;
		}

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map postp1(im.getPost(mainid));
		Map prep1(im.getPre(mainid));
		unsigned int v(prep1.begin()->second);
		unsigned int tokens(im.getTokens(mainid));

		// all pre/postset edges must have v as weight
		// if not, or if state-based logic would get hurt,
		// or pre/postset are not disjoint, choose another place
		Map::iterator mit;
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			if (mit->second != v) break;
		if (mit==prep1.end())
			for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
				if (mit->second != v) break;
		if (mit!=postp1.end() || 
			tokens>=v || 
			(v>1 && tokens<2 && facts.checkCondition(Facts::SAFETY) && !facts.checkFact(Facts::UNSAFE)) ||
			(tokens<2 && conditio7 && im.isVisible(mainid)) ||
			!disjoint(prep1,postp1)) 
		{
			im.unlock(mainid,MELDTRANS5);
			continue;
		}

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// get info about the postset transitions
		bool flag(true); // if all transitions have empty postsets
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
		{
			nodestamp[mit->first] = im.rdlock(mit->first);
			if (im.getPre(mit->first).size()>1) break;
			if (!im.getPost(mit->first).empty()) flag=false;
			if (im.isPersistent(mit->first)) break;
			if (condition && im.getLabelVis(mit->first)) break;
			if (condition && !im.isInvisible(mit->first)) break;
			if (conditio2 && im.getLabel(mit->first)) break;
			if (conditio4 && (!im.isInvisible(mit->first) || im.getLabel(mit->first))) break;
			collectNeighbors(mit->first,nodestamp);
			im.unlock(mit->first);
		}
		if ((flag && facts.checkCondition(Facts::BOUNDEDNESS) && !facts.checkFact(Facts::UNBOUNDED)) 
			|| mit!=postp1.end()) {
				if (mit!=postp1.end()) im.unlock(mit->first);
				im.unlock(mainid,MELDTRANS5);
				continue;
		}

		// get info about the preset transitions
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
		{
			nodestamp[mit->first] = im.rdlock(mit->first);
			if (im.isPersistent(mit->first)) break;
			if ((condition || conditio4) && !im.isInvisible(mit->first)) break;
			collectNeighbors(mit->first,nodestamp);
			im.unlock(mit->first);
		}
		if (mit!=prep1.end()) {
			im.unlock(mit->first);
			im.unlock(mainid,MELDTRANS5);
			continue;
		}
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(MELDTRANS5,mainid,nodestamp);

		// get new transitions if possible
		set<Vertex> tnew(im.reserveTransitions(tid,prep1.size()*postp1.size(), nodestamp));
		if (tnew.empty()) { // we could not get enough new transitions
			im.unlock(nodestamp);
			im.setMode(mainid,MELDTRANS5);
			continue;
		}

		// apply the rule, delete the place
		im.removeArcs(mainid);
		nodestamp[mainid] = 0;

		// next, collect one set of transitions that eventually will all stem from different pre-transitions
		set<Vertex> precollection;
		unsigned int pos(prep1.size());
		for(set<Vertex>::iterator sit=tnew.begin(); pos>0; ++sit,--pos)
			precollection.insert(*sit);

		// set pre/postsets etc. of the new transitions
		bool makevis(false);
		mit = prep1.begin();
		Map::iterator mit2(postp1.begin());
		for(set<Vertex>::iterator sit=tnew.begin(); sit!=tnew.end(); ++sit)
		{
			im.setName(*sit,im.getName(mit->first)+"_"+im.getName(mit2->first));
			im.setLabel(*sit,im.getLabel(mit->first));
			if (im.isVisible(mit->first) || im.isVisible(mit2->first))
				im.makeVisible(*sit);
			im.addPre(mit->first,*sit,1);
			im.addPost(mit->first,*sit,1);
			im.addPost(mit2->first,*sit,1);
//			facts.setPath(*sit, Path());
			facts.addPath(*sit, Path(1,mit2->first), true, true);
			facts.addPath(*sit, Path(1,mit->first));
			if (mit2==postp1.begin())
				facts.setLive(mit->first,*sit);
			if (mit==prep1.begin()) {
				set<Vertex> tmp(precollection);
				facts.setLive(mit2->first,tmp);
				makevis = true;
			}
			if (++mit == prep1.end()) { mit = prep1.begin(); ++mit2; }
		}
		if (makevis && conditio5)
			for(set<Vertex>::iterator sit=precollection.begin(); sit!=precollection.end(); ++sit)
				if (im.isInvisible(*sit)) im.makeVisible(*sit);

		// check other facts
		facts.removeFact(Facts::CTL | Facts::LTL);
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
			if (!im.isInvisible(mit->first)) break;
		if (mit!=prep1.end()) facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);

		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (!im.isInvisible(mit->first) || im.getLabelVis(mit->first)) break;
		if (mit!=postp1.end()) facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);

		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (im.getLabel(mit->first)) break;
		if (mit!=postp1.end()) facts.removeFact(Facts::ALTL | Facts::BISIM);

		if (!im.isInvisible(mainid))
			facts.removeFact(Facts::CTLX | Facts::LTLX);

		facts.setStatus(Facts::MARKING,mainid,Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::UNUSABLE);

		if (tokens>1) {
			facts.setStatus(Facts::SAFE, mainid, Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		} else facts.setStatus(Facts::SAFE, mainid, Facts::UNUSABLE);

		if (flag)
			facts.removeFact(Facts::BOUNDEDNESS);

		// remove the old pre/post-transitions
		for(mit=prep1.begin(); mit!=prep1.end(); ++mit)
		{
			im.removeArcs(mit->first);
			nodestamp[mit->first] = 0;
			facts.setStatus(Facts::PATH,mit->first,Facts::UNUSABLE);
		}
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
		{
			im.removeArcs(mit->first);
			nodestamp[mit->first] = 0;
			facts.setStatus(Facts::PATH,mit->first,Facts::UNUSABLE);
		}
		
		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 7: A place on which all transitions loop with a weight of at most its initial marking. Place is removed
    @param The ID of the calling thread
*/
void Rules::loopPlace(unsigned int tid) {
	// Starke Rule 7

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTL | Facts::LTL));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,LOOPPLACE,1,NODE_SET_LIMIT,1,NODE_SET_LIMIT)) {

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int tokens(im.getTokens(mainid));

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// all edges must be loops with weight <= tokens
		Map::iterator mit,mit2;
		for(mit=postp1.begin(),mit2=prep1.begin(); mit!=postp1.end()&&mit2!=prep1.end(); ++mit,++mit2)
		{
			if (mit->first!=mit2->first || mit->second!=mit2->second || mit->second > tokens) break;
			nodestamp[mit->first] = 0;
		}
		if (mit!=postp1.end() || mit2!=prep1.end() || im.isPersistent(mainid))
		{
			im.unlock(mainid,LOOPPLACE);
			continue;
		}

		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(LOOPPLACE,mainid,nodestamp);

		// change properties
		bool flag(true);
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (im.getPre(mit->first).size()==1 && im.getPost(mit->first).size()==1) {
				if (im.isPersistent(mit->first) || conditio3) flag=false;
				else if (!im.isInvisible(mit->first) || im.getLabelVis(mit->first)) {
					if (condition) flag=false;
					else facts.removeFact(Facts::ALTL | Facts::ALTLX);
				} else if (im.getLabel(mit->first)) {
					if (conditio2) flag=false;
					else facts.removeFact(Facts::ALTL);
				}
				if (flag) { // a transition that becomes isolated may be deleted
					nodestamp[mit->first]=0; // delete node
					facts.setStatus(Facts::PATH, mit->first, Facts::UNUSABLE);
					facts.removeFact(Facts::CTL | Facts::LTL);
					facts.setStatus(Facts::LIVE, mit->first, Facts::ISTRUE);
				}
			}
//		set<Vertex> tmp;
//		facts.setMarking(mainid,tmp,tokens);
		map<Vertex,int> tmp;
		facts.setMarking(tmp,mainid,tokens);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::ISTRUE);
		if (tokens>1) {
			facts.setStatus(Facts::SAFE, mainid, Facts::ISFALSE);
			facts.addFact(Facts::UNSAFE,false);
		}

		// apply the rule
		im.removeArcs(mainid);
		nodestamp[mainid] = 0; // remove the place

		// mark surrounding area as changed
		im.propagateChange(nodestamp);
//		im.unlock(nodestamp);
	}
}

/** Starke's rule 7 extended: A place on which all transitions loop. Place is removed together with dead neighbors.
    @param The ID of the calling thread
*/
void Rules::loopPlace2(unsigned int tid) {
	// Starke Rule 7 extended

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTL | Facts::LTL));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,LOOPPLACE,1,NODE_SET_LIMIT,1,NODE_SET_LIMIT)) {

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int tokens(im.getTokens(mainid));

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// all edges must be loops, if weight > tokens, the transition is dead
		Map::iterator mit,mit2;
		bool flag(false);
		for(mit=postp1.begin(),mit2=prep1.begin(); mit!=postp1.end()&&mit2!=prep1.end(); ++mit,++mit2)
		{
			if (mit->first!=mit2->first || mit->second!=mit2->second) break;
			nodestamp[mit->first] = im.rdlock(mit->first);
			bool flag(false);
			if (tokens < mit->second) {
				collectNeighbors(mit->first,nodestamp);
				if (im.isPersistent(mit->first)) flag=true;
			}
			im.unlock(mit->first);
		}
		if (mit!=postp1.end() || mit2!=prep1.end() || flag || im.isPersistent(mainid))
		{
			im.unlock(mainid,LOOPPLACE);
			continue;
		}

		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(LOOPPLACE,mainid,nodestamp);

		// delete dead transitions
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			if (tokens < mit->second) {
				im.removeArcs(mit->first);
				nodestamp[mit->first] = 0; // delete dead transition
				facts.addFact(Facts::NONLIVE,false);
				facts.setStatus(Facts::LIVE, mit->first, Facts::ISFALSE);
				facts.setStatus(Facts::PATH, mit->first, Facts::UNUSABLE);
			}

		// change properties
//		set<Vertex> tmp;
//		facts.setMarking(mainid,tmp,tokens);
		map<Vertex,int> tmp;
		facts.setMarking(tmp,mainid,tokens);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::ISTRUE);
		facts.setStatus(Facts::SAFE, mainid, tokens>1 ? Facts::ISFALSE : Facts::ISTRUE);
		if (tokens>1) facts.addFact(Facts::UNSAFE,false);

		// apply the rule
		im.removeArcs(mainid);
		nodestamp[mainid] = 0; // remove the place

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 8 extended: A looping transition may be removed
    @param The ID of the calling thread
*/
void Rules::loopTransition(unsigned int tid) {
	// the rule will ruin next in CTL/LTL
	if (facts.checkCondition(Facts::CTL | Facts::LTL)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::BISIM));
	bool conditio4(facts.checkCondition(Facts::LIVENESS));
	bool conditio5(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::TR,LOOPTRANS,1,NODE_SET_LIMIT,1,NODE_SET_LIMIT)) {

		if (im.isPersistent(mainid)
			|| (condition && im.getLabelVis(mainid))
			|| (conditio2 && im.getLabel(mainid))
			|| ((condition || conditio5) && im.isVisible(mainid))
			|| (conditio3 && (im.isVisible(mainid) || im.getLabel(mainid))))
		{
			im.unlock(mainid,LOOPTRANS);
			continue;
		}

		// get pre/postset of mainid, weight of the preset edge, and token number
	    Map& postt1(im.getPost(mainid));
		Map& pret1(im.getPre(mainid));

		// save the timestamp of mainid
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);

		// all edges must be loops
		Map::iterator mit,mit2;
		for(mit=postt1.begin(),mit2=pret1.begin(); mit!=postt1.end()&&mit2!=pret1.end(); ++mit,++mit2)
		{
			if (mit->first!=mit2->first || mit->second!=mit2->second) break;
			nodestamp[mit->first] = 0;
		}
		if (mit!=postt1.end() || mit2!=pret1.end())
		{
			im.unlock(mainid,LOOPTRANS);
			continue;
		}

		// select any neighbor
		Vertex p(pret1.begin()->first);
		im.unlock(mainid);
		im.rdlock(p);

		// get the postset of p and compare its transitions with mainid
		Map& postp(im.getPost(p));
		bool contained(false);
		for(mit=postp.begin(); mit!=postp.end(); ++mit)
		{
			if (mit->first == mainid) continue;
			im.rdlock(mainid,mit->first);
			if (contains(im.getPre(mit->first),pret1)) 
			{
				contained = true;
				nodestamp[mit->first] = im.getTimeStamp(mit->first);
				im.unlock(mit->first);
				im.unlock(mainid);
				break;
			}
			im.unlock(mit->first);
			im.unlock(mainid);
		}
		im.unlock(p);
		if (!contained 
			&& facts.checkCondition(Facts::LIVENESS) 
			&& !facts.checkFact(Facts::NONLIVE)) {
				im.setMode(mainid,LOOPTRANS);
				continue;
		}

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(LOOPTRANS,mainid,nodestamp);

		// apply the rule
		im.removeArcs(mainid);
		nodestamp[mainid] = 0; // delete node
		facts.setStatus(Facts::PATH, mainid, Facts::UNUSABLE);
		facts.removeFact(Facts::CTL | Facts::LTL);
		if (!contained) facts.removeFact(Facts::LIVENESS);
		if (im.getLabelVis(mainid) || im.isVisible(mainid))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabel(mainid))
			facts.removeFact(Facts::ALTL | Facts::BISIM);
		facts.setStatus(Facts::LIVE,mainid,Facts::UNUSABLE);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Murata's Series Place: Place with singleton preset, preset with singleton pre- and postset
    @param The ID of the calling thread
*/
void Rules::seriesPlace(unsigned int tid) {
	// the rule will destroy state-based next-operators
	if (facts.checkCondition(Facts::CTL | Facts::LTL)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::CTLX | Facts::LTLX));
	bool conditio4(facts.checkCondition(Facts::BISIM));
	bool conditio5(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,SERIESPLACE,1,1,0,NODE_SET_LIMIT)) {

		// get pre/postset of mainid and its pretransition
	    Map& postp1(im.getPost(mainid));
		Map& prep1(im.getPre(mainid));
		unsigned int tokens(im.getTokens(mainid));

		// check for invisibility in formulas
		// and singleton preset edge with weight 1
		// and absence of tokens
		if (im.isIO(mainid) || im.isPersistent(mainid) || (conditio3 && im.isVisible(mainid))
			|| prep1.begin()->second != 1 || tokens>0) 
		{
			im.unlock(mainid,SERIESPLACE);
			continue;
		}

		// get mainid's time stamp and neighbors
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		collectNeighbors(mainid, nodestamp);

		// get the preset transition
		Vertex t1(prep1.begin()->first);
		nodestamp[t1] = im.rdlock(t1);
	    Map& postt1(im.getPost(t1));
		Map& pret1(im.getPre(t1));

		// check that transition's pre-/postsets are singletons
		// and check visibility of the transition
		if (pret1.size()!=1 || postt1.size()!=1
			|| pret1.begin()->second!=1
			|| im.isPersistent(t1)
			|| (conditio2 && im.getLabel(t1))
			|| (condition && (im.isVisible(t1) || im.getLabelVis(t1)))
			|| (conditio4 && (im.isVisible(t1) || im.getLabel(t1))))
		{
			im.unlock(t1);
			im.unlock(mainid,SERIESPLACE);
			continue;
		}

		// get the transition's pre-place and check its visibility
		Vertex p2(pret1.begin()->first);
		nodestamp[p2] = 0; // no locking required yet
		if (im.isPersistent(p2) 
			|| (conditio4 && im.isIO(p2))
			|| (conditio3 && im.isVisible(p2)))
		{
			im.unlock(t1);
			im.unlock(mainid,SERIESPLACE);
			continue;
		}		

		// release read-locks
	    im.unlock(t1);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(SERIESPLACE,mainid,nodestamp);

		// path modifications
	    Map& prep2(im.getPre(p2));
	    Map& postp2(im.getPost(p2));
		Map::iterator mit;
		for(mit=postp1.begin(); mit!=postp1.end(); ++mit)
			facts.addPath(mit->first, Path(1,t1));
		facts.setStatus(Facts::PATH,t1,Facts::UNUSABLE);

		// apply the rule
		im.addPost(mainid,p2,1);
		im.removeArcs(mainid);
		im.removeArcs(t1);
		nodestamp[mainid] = 0; // remove the place
		nodestamp[t1] = 0; // remove the transition

        // change properties
		if (!im.isInvisible(t1) || im.getLabelVis(t1))
			facts.removeFact(Facts::ALTL | Facts::ALTLX | Facts::BISIM);
		else if (im.getLabel(t1))
			facts.removeFact(Facts::ALTL | Facts::BISIM);
		facts.removeFact(Facts::CTL | Facts::LTL);
		if (!im.isInvisible(mainid) || !im.isInvisible(p2))
			facts.removeFact(Facts::CTLX | Facts::LTLX);
		if (im.isIO(p2))
			facts.removeFact(Facts::BISIM);
		map<Vertex,int> tmp;
		tmp[mainid] = 1;
		tmp[p2] = 1;
		facts.setMarking(tmp,p2,1);
		facts.setBounded(mainid,p2);
		facts.setSafe(mainid,p2);
		if (im.isVisible(mainid) && im.isInvisible(p2)) im.makeVisible(p2);
		set<Vertex> tmp2;
		for(mit=prep2.begin(); mit!=prep2.end(); ++mit) {
			tmp2.insert(mit->first);
			if (conditio5 && im.isVisible(t1) && im.isInvisible(mit->first)) 
				im.makeVisible(mit->first);
		}
		facts.setLive(t1,tmp2);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Invisible place with empty postset
    @param The ID of the calling thread
*/
void Rules::finalPlace(unsigned int tid) {

	// the rule may remove an unbounded place
	if (facts.checkCondition(Facts::REVERSE | Facts::INVARIANT)) return;
	if (facts.checkCondition(Facts::BOUNDEDNESS) && !facts.checkFact(Facts::UNBOUNDED)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::CTL | Facts::LTL | Facts::CTLX | Facts::LTLX | Facts::BOUNDEDNESS | Facts::UNBOUNDED));
	bool conditio2(facts.checkCondition(Facts::SAFETY | Facts::UNSAFE));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,FINALPLACE,1,NODE_SET_LIMIT,0,0)) {

		unsigned int tokens(im.getTokens(mainid));

		// check if the rule must be excluded for the place
		if (im.isIO(mainid) || im.isPersistent(mainid) || 
			(tokens<2 && facts.checkCondition(Facts::SAFETY) && !facts.checkFact(Facts::UNSAFE)) ||
			(condition && im.isVisible(mainid)) ||
			(conditio2 && tokens<2 && im.isVisible(mainid)))
		{
			im.unlock(mainid,FINALPLACE);
			continue;
		}

		// get mainid's time stamp and neighbors
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		collectNeighbors(mainid, nodestamp);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(FINALPLACE,mainid,nodestamp);

		// apply the rule
		im.removeArcs(mainid);
		nodestamp[mainid] = 0; // remove the place

        // change properties
		if (!im.isInvisible(mainid))
			facts.removeFact(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX | Facts::UNBOUNDED);
		facts.setStatus(Facts::MARKING,mainid,Facts::UNUSABLE);
		facts.setStatus(Facts::BOUNDED,mainid,Facts::UNUSABLE);
		facts.removeFact(Facts::BOUNDEDNESS | Facts::REVERSE);
		facts.setStatus(Facts::SAFE, mainid, tokens>1 ? Facts::ISFALSE : Facts::UNUSABLE);
		if (tokens>1) facts.addFact(Facts::UNSAFE,false);
		else if (!facts.checkFact(Facts::UNSAFE))
			facts.removeFact(Facts::SAFETY);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Transition with empty postset
    @param The ID of the calling thread
*/
void Rules::finalTransition(unsigned int tid) {
	// the rule may remove an unbounded place
	if (facts.checkCondition(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX 
							| Facts::BISIM | Facts::REVERSE | Facts::INVARIANT)) return;
	if (facts.checkCondition(Facts::LIVENESS) && !facts.checkFact(Facts::NONLIVE)) return;

	// conditions that need to be checked later
	bool condition(facts.checkCondition(Facts::ALTL | Facts::ALTLX));
	bool conditio2(facts.checkCondition(Facts::ALTL));
	bool conditio3(facts.checkCondition(Facts::LIVENESS | Facts::NONLIVE));

	// find a starting transition "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::TR,FINALTRANS,1,NODE_SET_LIMIT,0,0)) {

		// check for invisibility in formulas
		if (im.isPersistent(mainid) 
			|| (conditio2 && im.getLabel(mainid))
			|| (conditio3 && !im.isInvisible(mainid))
			|| (condition && (im.isVisible(mainid) || im.getLabelVis(mainid))))
		{
			im.unlock(mainid,FINALTRANS);
			continue;
		}

		// get mainid's time stamp and neighbors
		Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
		collectNeighbors(mainid, nodestamp);
		im.unlock(mainid);

		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(FINALTRANS,mainid,nodestamp);

        // change properties
	    Map& pret1(im.getPre(mainid));
		Map::iterator mit;
		for(mit=pret1.begin(); mit!=pret1.end(); ++mit)
			facts.setStatus(Facts::MARKING,mit->first,Facts::UNUSABLE);
		if (!im.isInvisible(mainid) || im.getLabelVis(mainid))
			facts.removeFact(Facts::ALTL | Facts::ALTLX);
		else if (im.getLabel(mainid))
			facts.removeFact(Facts::ALTL);
		facts.removeFact(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX 
						| Facts::LIVENESS | Facts::BISIM | Facts::REVERSE);
		facts.setStatus(Facts::PATH,mainid,Facts::UNUSABLE);
		facts.setStatus(Facts::LIVE,mainid,Facts::UNUSABLE);

		// apply the rule
		im.removeArcs(mainid);
		nodestamp[mainid] = 0; // remove the place

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Starke's rule 2 modified: Place with preset contained in postset (weighted) and its postset transitions are removed, if none of them can fire
    @param The ID of the calling thread
*/
void Rules::initiallyDeadPlace2(unsigned int tid) {
	// for later checks
	bool condition(facts.checkCondition(Facts::LIVENESS));

	// find a starting place "mainid" for the rule if possible
	Vertex mainid(NO_NODE);
	while (im.getFirstNode(mainid,tid,IMatrix::PL,INITDEADPL2,0,NODE_SET_LIMIT,0,NODE_SET_LIMIT))
	{
		// get postset and marking of mainid
	    Map& post(im.getPost(mainid));
	    Map& pre(im.getPre(mainid));
	    unsigned int tokens(im.getTokens(mainid));

		// check the rule: all postset transitions need more tokens than available on mainid
	    Map::iterator mit;
	    for(mit=post.begin(); mit!=post.end(); ++mit)
	        if (mit->second<=tokens) break;

		// if the rule is not fulfilled look for a new first place
	    if (mit!=post.end() || im.isIO(mainid) || im.isPersistent(mainid) || !contains(post,pre))
		{
			im.unlock(mainid,INITDEADPL2);
			continue;
		}

		// collect IDs of affected nodes and save the timestamp of mainid
		// if anyone tampers with the place or its adjacent edges, we will know
	    Map nodestamp;
		nodestamp[mainid] = im.getTimeStamp(mainid);
	    for(mit=post.begin(); mit!=post.end(); ++mit)
		{
			// all transitions in mainid's postset are affected
			// transitions may be locked after places
			nodestamp[mit->first] = im.rdlock(mit->first);
			if (im.isPersistent(mit->first)) break;
			collectNeighbors(mit->first, nodestamp);
			im.unlock(mit->first);
		}
		// if any postset transition is persistent, look for a new place
	    if (mit!=post.end())
		{
			im.unlock(mit->first);
			im.unlock(mainid,INITDEADPL2);
			continue;
		}
	    im.unlock(mainid);
	
		// get write-locks and check all time-stamps
		if (!im.wrlock(nodestamp,tid)) continue;
		printApply(INITDEADPL2,mainid,nodestamp); // logging only

		// apply the rule
		for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit)
		{
			if (mit->first == mainid) { // the dead place
				mit->second = 0; // remove mainid
//				set<Vertex> tmp;
//				facts.setMarking(mainid, tmp, tokens);
				map<Vertex,int> tmp;
				facts.setMarking(tmp, mainid, tokens);
				facts.setStatus(Facts::BOUNDED, mainid, Facts::ISTRUE);
				facts.setStatus(Facts::SAFE, mainid, tokens>1 ? Facts::ISFALSE : Facts::ISTRUE);
			} else if (!im.isPlace(mit->first)) { // a dead transition
				// erase the transition with all its edges
				im.removeArcs(mit->first);
				mit->second = 0; // remove the transition
				facts.setStatus(Facts::PATH, mit->first, Facts::UNUSABLE);

				// export formula rule for liveness
				facts.setStatus(Facts::LIVE,mit->first,Facts::ISFALSE);
				unsigned int label(im.getLabel(mit->first));
				if (label) im.clearLabel(mit->first);
			}
		}

		// change inheritable properties (if there are transitions)
		if (nodestamp.size()>1)
			facts.addFact(Facts::NONLIVE,false);
		if (tokens>1)
			facts.addFact(Facts::UNSAFE,false);

		// mark surrounding area as changed and unlock nodes
		im.propagateChange(nodestamp);
	}
}

/** Shift the preset of a node to another node (adding to its preset). All surrounding nodes must be write-locked.
    @param id The ID of the node which shall have an empty preset
    @param id2 The ID of the node to which the preset will be added
*/
void Rules::movePre(Vertex id, Vertex id2) {
	Map::iterator mit2;
	Map& pmap(im.getPre(id));
	for(mit2=pmap.begin(); mit2!=pmap.end(); ++mit2)
	{
		im.getPre(id2)[mit2->first] += mit2->second;
		im.getPost(mit2->first)[id2] += mit2->second;
		im.getPost(mit2->first).erase(id);
	}
	pmap.clear();
}

/** Check if two nodes have the same pre- and postsets, include edge multiplicity. Both nodes must be read-locked.
    @param id1 The ID of the first node
    @param id2 The ID of the second node
    @return If both have the same pre- and postsets
*/
bool Rules::checkParallel(unsigned int id1, unsigned int id2) {
	Map::iterator mit1,mit2;

	Map& pre1(im.getPre(id1));
	Map& pre2(im.getPre(id2));
	for(mit1=pre1.begin(), mit2=pre2.begin(); mit1!=pre1.end() && mit2!=pre2.end(); ++mit1,++mit2)
		if (mit1->first!=mit2->first || mit1->second!=mit2->second) break;
	if (mit1!=pre1.end() || mit2!=pre2.end()) return false;

	Map& post1(im.getPost(id1));
	Map& post2(im.getPost(id2));
	for(mit1=post1.begin(), mit2=post2.begin(); mit1!=post1.end() && mit2!=post2.end(); ++mit1,++mit2)
		if (mit1->first!=mit2->first || mit1->second!=mit2->second) break;
	if (mit1!=post1.end() || mit2!=post2.end()) return false;

	return true;
}

/** Check if the pre- and postsets of two nodes are identical except for one pre-node. The two nodes must be read-locked.
    @param id1 The ID of the first node
    @param id2 The ID of the node to compare with
    @param prediff A node in the preset of id1 which MUST NOT have a match in the preset of id2
    @return NO_NODE if no or more than one node in the preset of id2 cannot be matched to the preset of id1.
        Otherwise, the ID of the single non-matching node in the preset of id2 will be returned.
*/
Vertex Rules::checkParallel(Vertex id1, Vertex id2, Vertex prediff) {
	Map::iterator mit1,mit2;
	Vertex id2diff(NO_NODE);

    // check the presets
	Map& pre1(im.getPre(id1));
	Map& pre2(im.getPre(id2));
	for(mit1=pre1.begin(), mit2=pre2.begin(); mit1!=pre1.end() && mit2!=pre2.end(); ++mit1,++mit2)
	{
        // the must-not-match
		if (mit1->first==prediff && mit2->first!=prediff) 
			if (++mit1==pre1.end()) break;

        // the first non-matching node in id2's preset
		if (mit1->first!=mit2->first && id2diff==NO_NODE) {
			id2diff = mit2->first;
			if (++mit2==pre2.end()) break;
		}

        // otherwise equality must be given
		if (mit1->first!=mit2->first || mit1->second!=mit2->second) break;
	}

    // make sure we reached the end in both presets
	if (mit1!=pre1.end() && mit1->first==prediff) ++mit1;
	if (mit2!=pre2.end() && id2diff==NO_NODE) id2diff = (mit2++)->first;
	if (mit1!=pre1.end() || mit2!=pre2.end()) return NO_NODE;

    // now check the postset, which must be equal
	Map& post1(im.getPost(id1));
	Map& post2(im.getPost(id2));
	for(mit1=post1.begin(), mit2=post2.begin(); mit1!=post1.end() && mit2!=post2.end(); ++mit1,++mit2)
		if (mit1->first!=mit2->first || mit1->second!=mit2->second) break;
	if (mit1!=post1.end() || mit2!=post2.end()) return NO_NODE;

    // return the spurious node from id2
	return id2diff;
}

/** Collect all the neighbor nodes of a given node. The node must be read-locked.
    @param id The ID of the node
    @param stamps A container in which the neighbors are collected by key, their values set to zero
*/
void Rules::collectNeighbors(Vertex id, Map& stamps) {
	Map::iterator mit;

	// go through the preset
	Map& pmap(im.getPre(id));
	for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
		if (stamps.find(mit->first)==stamps.end()) 
			stamps[mit->first] = 0;

	// and the postsets
	Map& pmap2(im.getPost(id));
	for(mit=pmap2.begin(); mit!=pmap2.end(); ++mit)
		if (stamps.find(mit->first)==stamps.end())
			stamps[mit->first] = 0;
}

/** Debug info for rule application
    @param mode The rule's flag
    @param id The ID of the node to operate on
    @param nodestamp A map of all nodes affected by the operation of the rule
*/
void Rules::printApply(Mode mode, Vertex id, const Map& nodestamp) {
	if (!Runtime::args_info.debug_given) return;
	std::cerr << "apply " << mode << "[" << im.getName(id) << "] to ";
	Map::const_iterator mit;
	for(mit=nodestamp.begin(); mit!=nodestamp.end(); ++mit) 
		std::cerr << im.getName(mit->first) << " ";
	std::cerr << std::endl;
}

/** Check that two pre-/postsets are disjoint (have no common nodes)
    @param map1 A map of the first pre-/postset
    @param map1 A map of the second pre-/postset
    @return If the maps are disjoint
*/
bool Rules::disjoint(const Map& map1, const Map& map2) {
	Map::const_iterator mit1,mit2;
	for(mit1=map1.begin(), mit2=map2.begin(); mit1!=map1.end() && mit2!=map2.end(); )
	{
		if (mit1->first == mit2->first) return false;
		if (mit1->first < mit2->first) ++mit1;
		else ++mit2;
	}
	return true;
}

/** Check that a pre-/postset fully contains another, i.e. has the same or more edges to any node
    @param map1 A map of the first pre-/postset
    @param map1 A map of the second pre-/postset
    @return If map1 fully encompasses map2. If map2 has k edges to some node, map1 must have at least k edges to that node.
*/
bool Rules::contains(const Map& map1, const Map& map2) {
	Map::const_iterator mit1,mit2;
	for(mit1=map1.begin(), mit2=map2.begin(); mit1!=map1.end() && mit2!=map2.end(); ++mit1,++mit2)
	{
		while (mit1!=map1.end() && mit1->first!=mit2->first) ++mit1;
		if (mit1==map1.end()) break;
		if (mit1->second < mit2->second) break;
	}
	return (mit2==map2.end());
}


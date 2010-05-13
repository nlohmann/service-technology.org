#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

#include "lp_solve/lp_lib.h"
#include "InvariantFinder.h"

#include <map>
#include <set>
#include <vector>

using std::map;
using std::set;
using std::vector;

extern vector<Transition*> transitionorder;
extern vector<Place*> placeorder;
extern map<Transition*,int> revtorder;
extern map<Place*,int> revporder;
extern set<pnapi::Arc*> arcs;

InvariantFinder::InvariantFinder(PetriNet* net, unsigned int cols) : _net(net), lpw(cols){

}

void InvariantFinder::findTInvariant(){
	calcPTOrder();
	lpw.calcTInvariant(false);
}

void InvariantFinder::findPInvariant(){
	calcPTOrder();
	lpw.calcPInvariant(false);
}

void InvariantFinder::findTrap(){
	calcPTOrder();
	lpw.calcTrap(false);
}

/** Calculate the global ordering of transitions and places for this problem.
	@author Harro Wimmel
	@return If the ordering is deterministic.

*/
bool InvariantFinder::calcPTOrder() {
	set<Transition*> tset(_net->getTransitions());
	set<Place*> pset(_net->getPlaces());
	set<Transition*>::iterator tit;
	set<Place*>::iterator pit;
	map<Transition*,int> tval;
	map<Place*,int> pval;
	map<int,set<Transition*> > tord,ttmp;
	map<int,set<Place*> > pord,ptmp;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		tval[(*tit)] = 1;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		pval[(*pit)] = 1;
	tord[0] = tset;
	pord[0] = pset;
	map<int,set<pnapi::Transition*> >::iterator tmit;
	map<int,set<pnapi::Place*> >::iterator pmit;
	set<pnapi::Arc*>::iterator ait;
	int min;
	for(int i=0; i<6; ++i) // Probably five rounds are enough to make the order deterministic, but we can't be sure!
	{
		min = 0;
		for(tmit=tord.begin(); tmit!=tord.end(); ++tmit)
		{
			int minval = 0;
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			{
				int val = 0;
				for(ait=(*tit)->getPresetArcs().begin(); ait!=(*tit)->getPresetArcs().end(); ++ait)
					val += 2*pval[&((*ait)->getPlace())]*(*ait)->getWeight();
				for(ait=(*tit)->getPostsetArcs().begin(); ait!=(*tit)->getPostsetArcs().end(); ++ait)
					val -= pval[&((*ait)->getPlace())]*(*ait)->getWeight();
				tval[(*tit)] = val;
				if (val<minval) minval=val;
			}
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			{
				tval[(*tit)] -= minval;
				ttmp[min+tval[(*tit)]].insert(*tit);
			}
			min = ttmp.rbegin()->first + 1;	
		}
		tord.clear();
		for(tmit=ttmp.begin(),min=1; tmit!=ttmp.end(); ++tmit,++min)
		{
			tord[min]=tmit->second;
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
				tval[(*tit)]=min;
		}
		ttmp.clear();
		min = 0;
		for(pmit=pord.begin(); pmit!=pord.end(); ++pmit)
		{
			int minval = 0;
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			{
				int val = 0;
				for(ait=(*pit)->getPresetArcs().begin(); ait!=(*pit)->getPresetArcs().end(); ++ait)
					val += tval[&((*ait)->getTransition())]*(*ait)->getWeight();
				for(ait=(*pit)->getPostsetArcs().begin(); ait!=(*pit)->getPostsetArcs().end(); ++ait)
					val -= 2*tval[&((*ait)->getTransition())]*(*ait)->getWeight();
				pval[(*pit)] = val;
				if (val<minval) minval=val;
			}
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			{
				pval[(*pit)] -= minval;
				ptmp[min+pval[(*pit)]].insert(*pit);
			}
			min = ptmp.rbegin()->first + 1;	
		}
		pord.clear();
		for(pmit=ptmp.begin(),min=1; pmit!=ptmp.end(); ++pmit,++min)
		{
			pord[min]=pmit->second;
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
				pval[(*pit)] = min;
		}
		ptmp.clear();
	}

	// push the result into the global vectors
	bool deterministic(true);
	transitionorder.clear();
	for(tmit=tord.begin(); tmit!=tord.end(); ++tmit)
	{
		if (tmit->second.size()>1) deterministic=false;
		for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			transitionorder.push_back(*tit);
	}
	placeorder.clear();
	for(pmit=pord.begin(); pmit!=pord.end(); ++pmit)
	{
		if (pmit->second.size()>1) deterministic=false;
		for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			placeorder.push_back(*pit); 
	}
	revtorder.clear();
	for(unsigned int i=0; i<transitionorder.size(); ++i)
		revtorder[transitionorder[i]]=i;
	revporder.clear();
	for(unsigned int i=0; i<placeorder.size(); ++i)
		revporder[placeorder[i]]=i;

	arcs = _net->getArcs();
	
	return deterministic;
}

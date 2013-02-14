/*!
\file FirelistStubbornDeletion.cc
\author Harro
\status new

\brief Class for firelist generation by the deletion algorithm for stubborn sets.
*/

#include <config.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Exploration/FirelistStubbornDeletion.h>
#include <Exploration/StatePredicateProperty.h>
#include <Formula/AtomicStatePredicate.h>
#include <vector>
#include <set>
#include <map>
//#include <iostream>

using std::vector;
using std::set;
using std::map;
//using std::endl;
//using std::cout;

#ifndef INT32_MAX
#define INT32_MAX (2147483647)
#endif

/*!
 * \brief A constructor for firelists of stubborn sets using the deletion algorithm.
 *        Used for deadlock checks.
 */
FirelistStubbornDeletion::FirelistStubbornDeletion()
{
	sp = NULL;

	// determine the number of <t,s>-pairs that may exists in the and-or-graph
	fieldsize = 0;
	for(index_t t=0; t<Net::Card[TR]; fieldsize += Transition::CardDeltaT[PRE][t++]) ;

	// compute space needed for predecessors of nodes of the and-or-graph
	// nodes are ordered as follows: transitions, places, <t,s>-pairs, <t,s,2>, <t,s,3>
	size = Net::Card[TR] + Net::Card[PL] + 3*fieldsize;

	// build those parts of the graph that can be built at construction time
	buildStaticGraph();
}

/*!
 * \brief A constructor for firelists of stubborn sets using the deletion algorithm.
 *        Used for state predicates.
 * \param sp The state predicate to check for.
 */
FirelistStubbornDeletion::FirelistStubbornDeletion(StatePredicate* sp)
{
	// save the state predicate
	this->sp = sp;

	// determine the number of <t,s>-pairs that may exists in the and-or-graph
	fieldsize = 0;
	for(index_t t=0; t<Net::Card[TR]; fieldsize += Transition::CardDeltaT[PRE][t++]) ;

	// compute space needed for predecessors of nodes of the and-or-graph
	// nodes are ordered as follows: transitions, places, <t,s>-pairs, <t,s,2>, <t,s,3>
	size = Net::Card[TR] + Net::Card[PL] + 3*fieldsize;

	// now compute the number of nodes needed for the formula and memorise the successors
	formula_offset = size;
	const StatePredicate* const* subnodes;
	f_nodes.push_back(sp);
	map<index_t,vector<index_t> > f_or_nodes;
	for(index_t f_nodes_ptr=0; f_nodes_ptr<f_nodes.size(); ++f_nodes_ptr)
	{
		index_t subnr(f_nodes[f_nodes_ptr]->getSubs(&subnodes));
		if (subnr==0) {
				// atomic formula (not TRUE/FALSE!)
				if (f_nodes[f_nodes_ptr]->countAtomic()==1)
					f_leaf_nodes.push_back(f_nodes_ptr);
		} else if (f_nodes[f_nodes_ptr]->isOrNode()) {
				// or-node, add successors
				for(index_t i=0; i<subnr; ++i)
				{
					f_or_nodes[f_nodes_ptr].push_back(f_nodes.size());
					f_nodes.push_back(subnodes[i]);
				}
		} else {
				// and-node, add successors
				for(index_t i=0; i<subnr; ++i)
				{
					f_and_nodes[f_nodes_ptr].push_back(f_nodes.size());
					f_nodes.push_back(subnodes[i]);
				}
		}
	}

	// compute the total size of the graph
	size += f_nodes.size();

	// build the static parts of the graph
	buildStaticGraph();

	// add the or-nodes of the formula (they become and-nodes in the graph)
	for(map<index_t,vector<index_t> >::iterator it=f_or_nodes.begin(); it!=f_or_nodes.end(); ++it)
		for(index_t i=0; i<it->second.size(); ++i)
		{
			index_t ptr(it->second[i]+formula_offset);
			predecessor[ptr] = (index_t*) malloc(SIZEOF_INDEX_T);
			predecessor[ptr][predcnt[ptr]++] = it->first+formula_offset;
		}

	// make the and-nodes of the formula (they become or-nodes in the graph)
	for(map<index_t,vector<index_t> >::iterator it=f_and_nodes.begin(); it!=f_and_nodes.end(); ++it)
		for(index_t i=0; i<it->second.size(); ++i)
		{
			index_t ptr(it->second[i]+formula_offset);
			predecessor[ptr] = (index_t*) malloc(SIZEOF_INDEX_T);
			predecessor[ptr][predcnt[ptr]++] = it->first+formula_offset;
			andor[ptr] = true;
		}

}

/*!
 * \brief Destructor.
 */
FirelistStubbornDeletion::~FirelistStubbornDeletion()
{
	free(enabled);
	free(counter);
	free(maxcounter);
	free(andor);
	free(color);
	free(root);
	free(protect);
	delete[] consumer;
	delete[] consumption;
	delete[] producer;
	free(transition);
	free(offset);
	for(index_t i=0; i<size; ++i) free(predecessor[i]);
	free(predecessor);
	free(predcnt);
}

/*!
 * \brief Construct the makring independet parts of the graph that is needed for the stubborn
 *        set generation. State predicates are not considered.
 */
void FirelistStubbornDeletion::buildStaticGraph()
{
	// this function must be called exactly once in each constructor
	// otherwise memory leaks or errors will occur

	// allocate memory for the graph structure
	predecessor = (index_t**) malloc(size * SIZEOF_VOIDP);
	// we count how many predecessors each node has:
	predcnt = (index_t*) calloc(SIZEOF_INDEX_T, size);
	// allocate memory for flags/counters in the deletion algorithm
	protect = (unsigned char*) malloc(Net::Card[TR] * SIZEOF_CHAR);
	root = (unsigned char*) malloc(Net::Card[TR] * SIZEOF_CHAR);
	color = (unsigned char*) malloc(size * SIZEOF_CHAR);
	andor = (unsigned char*) calloc(SIZEOF_CHAR, size);
	counter = (index_t*) malloc(size * SIZEOF_INDEX_T);
	maxcounter = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
	enabled = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);

	// determine for each transition t the offset of the first <t,s>-pair (if it exists)
	// the number of such pairs is Transition::CardDeltaT[PRE][t]
	offset = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
	offset[0] = Net::Card[TR] + Net::Card[PL];
	for(index_t t=1; t<Net::Card[TR]; ++t) offset[t] = offset[t-1] + Transition::CardDeltaT[PRE][t-1];

	// create a shortcut from each <t,s>-pair to its transition t
	transition = (index_t*) malloc(size * SIZEOF_INDEX_T);
	for(index_t i=offset[0]+fieldsize-1,t=Net::Card[TR]-1; i>=offset[0]; --i)
	{
		transition[i]=t;
		if (offset[t]==i) --t;
	}
	for(index_t i=offset[0]+fieldsize; i<size; ++i)
		transition[i] = transition[i-fieldsize];

	// for each place determine the transitions that decrease/increase its token number upon firing
	consumer = new vector<index_t>[Net::Card[PL]];
	consumption = new vector<index_t>[Net::Card[PL]];
	producer = new set<index_t>[Net::Card[PL]];
	for(index_t t=0; t<Net::Card[TR]; ++t)
	{
		for(index_t i=0; i<Transition::CardDeltaT[PRE][t]; ++i)
		{
			consumer[Transition::DeltaT[PRE][t][i]].push_back(t);
			consumption[Transition::DeltaT[PRE][t][i]].push_back(Transition::MultDeltaT[PRE][t][i]);
		}
		for(index_t i=0; i<Transition::CardDeltaT[POST][t]; ++i)
			producer[Transition::DeltaT[POST][t][i]].insert(t);
	}

	// allocate enough memory for the predecessors of each node in the and-or-graph
	// (except for the formula nodes which are handled in the constructor)
	// and initialise the constant elements in the graph
	index_t i;
	for(i=0; i<Net::Card[TR]; ++i) 
	{
		index_t count(0);
		for(index_t j=0; j<Net::CardArcs[TR][PRE][i]; ++j)
			count += consumer[Net::Arc[TR][PRE][i][j]].size();
		for(index_t j=0; j<Net::CardArcs[TR][POST][i]; ++j)
			count += consumer[Net::Arc[TR][POST][i][j]].size();
		predecessor[i] = (index_t*) malloc(count * SIZEOF_INDEX_T);
	}
	for(; i<offset[0]; ++i) 
		predecessor[i] = (index_t*) malloc(Net::CardArcs[PL][POST][i-Net::Card[TR]] * SIZEOF_INDEX_T);
	for(; i<offset[0]+fieldsize; ++i) 
	{ 
		predecessor[i] = (index_t*) malloc(SIZEOF_INDEX_T); 
		predcnt[i]=1; 
		predecessor[i][0]=transition[i]; 
		andor[i] = true; // or-node!
	}
	for(; i<offset[0]+2*fieldsize; ++i) 
	{ 
		predecessor[i] = (index_t*) malloc(SIZEOF_INDEX_T); 
		predcnt[i]=1; 
		predecessor[i][0]=i-fieldsize; 
	}
	for(; i<offset[0]+3*fieldsize; ++i) 
	{ 
		predecessor[i] = (index_t*) malloc(SIZEOF_INDEX_T); 
		predcnt[i]=1; 
		predecessor[i][0]=i-2*fieldsize; 
	}
}

/*!
 * \brief The function to be called when a stubborn set at a given marking
 *        should be constructed
 * \param ns The marking for which the stubborn set should be built.
 * \param result A pointer to NULL. Will be replaced by a pointer to an array
 *               containing the stubborn set.
 * \return The number of element in the stubborn set.
 */
index_t FirelistStubbornDeletion::getFirelist(NetState &ns, index_t** result)
{
	// check which transitions are enabled = and-nodes / disabled = or-nodes
	// for disabled transitions count the disabling places
	index_t ecnt(0);
	for(index_t t=0; t<Net::Card[TR]; ++t)
	{
		maxcounter[t] = 0;
		if ((andor[t] = !ns.Enabled[t]))
		{
			for(index_t i=0; i<Net::CardArcs[TR][PRE][t]; ++i)
				if (ns.Current[Net::Arc[TR][PRE][t][i]] < Net::Mult[TR][PRE][t][i])
					++maxcounter[t];
		} else enabled[ecnt++] = t;
	}

	// initialise graph properties that have a general start value
	init();

	// the offset of the first place in the graph
	index_t poffset(Net::Card[TR]);

	// clear all predecessor lists for places and transitions (not for pairs!)
	for(index_t i=0; i<offset[0]; ++i) predcnt[i]=0;

	// construct place-predecessors of transitions (type <p,t'>)
	for(index_t p=0; p<Net::Card[PL]; ++p)
		for(index_t i=0; i<Net::CardArcs[PL][POST][p]; ++i)
			if (ns.Current[p] < Net::Mult[PL][POST][p][i])
			{
				for(set<index_t>::iterator it=producer[p].begin(); it!=producer[p].end(); ++it)
				{
					mult_t wpu(0); // weight from p to producer t'
					for(index_t j=0; j<Net::CardArcs[PL][POST][p]; ++j)
					{
						if (Net::Arc[PL][POST][p][j]==*it)
						{
							wpu = Net::Mult[PL][POST][p][j];
							break;
						}
					}
					if (ns.Current[p] >= wpu)
						predecessor[*it][predcnt[*it]++] = p+poffset;
				}
				break;
			}

	// construct complex predecessors of transitions (type <<t,s,i>,t'>)
	for(index_t p=0; p<Net::Card[PL]; ++p)
		for(index_t i=0; i<consumer[p].size(); ++i)
		{
			index_t t(consumer[p][i]);
			if (ns.Enabled[t]) 
			{
				// get the consumption of t on p and the location of <t,p,2>
				index_t c(consumption[p][i]), pos(offset[t]+fieldsize);
				for(index_t j=0; j<Transition::CardDeltaT[PRE][t]; ++j)
					if (Transition::DeltaT[PRE][t][j]==p) 
					{ 
						pos += j;
						break; 
					}
				// check condition E2
				for(index_t j=0; j<Net::CardArcs[PL][POST][p]; ++j)
				{
					index_t u(Net::Arc[PL][POST][p][j]);
					if (c+Net::Mult[PL][POST][p][j] > ns.Current[p])
						predecessor[u][predcnt[u]++] = pos;
					else
						for(index_t x=0; x<consumer[p].size(); ++x)
							if (consumer[p][x]==u)
							{
								predecessor[u][predcnt[u]++] = pos;
								break;
							}
				}

				// get the weight of the arc t->p
				mult_t wtp(0);
				for(index_t j=0; j<Net::CardArcs[PL][PRE][p]; ++j)
					if (Net::Arc[PL][PRE][p][j]==t) 
						{ wtp=Net::Mult[PL][PRE][p][j]; break; }
				// check condition E3
				for(index_t j=0; j<Net::CardArcs[PL][PRE][p]; ++j)
				{
					index_t u(Net::Arc[PL][PRE][p][j]);
					mult_t wup(Net::Mult[PL][PRE][p][j]);
					mult_t wpu(0);
					for(index_t k=0; k<Net::CardArcs[PL][POST][p]; ++k)
						if (Net::Arc[PL][POST][p][k]==u) 
							{ wpu=Net::Mult[PL][POST][p][k]; break; }
					if (ns.Current[p] >= wpu && (wup > wpu || wup > wtp))
						predecessor[u][predcnt[u]++] = pos+fieldsize; // location of <t,p,3>
				}
			}
		}

	// construct transition-predecessors of places (type <t,s>)
	for(index_t p=0; p<Net::Card[PL]; ++p)
	{
		for(index_t j=0; j<Net::CardArcs[PL][POST][p]; ++j)
			if (ns.Current[p] < Net::Mult[PL][POST][p][j])
				predecessor[p+poffset][predcnt[p+poffset]++] = Net::Arc[PL][POST][p][j]; 
	}

	// construct predecessors in the formula-part of the graph
	if (sp) { // do we have a state predicate?

		// or-nodes of the graph (= and-nodes of the formula)
		for(map<index_t,vector<index_t> >::iterator it=f_and_nodes.begin(); it!=f_and_nodes.end(); ++it)
			for(index_t i=0; i<it->second.size(); ++i)
				predcnt[it->second[i]+formula_offset] = (f_nodes[it->second[i]]->value ? 0 : 1);

		// atomic nodes, add their upsets as successors if they evaluate to false
		for(index_t i=0; i<f_leaf_nodes.size(); ++i)
		{
			const AtomicStatePredicate* aspptr((const AtomicStatePredicate*)(f_nodes[f_leaf_nodes[i]]));
			if (!aspptr->value)
				for(index_t j=0; j<aspptr->cardUp; ++j)
					predecessor[aspptr->up[j]][predcnt[aspptr->up[j]]++] = f_leaf_nodes[i]+formula_offset;
		}		
	}

	// the graph is ready, apply the deletion algorithm
/*
	index_t rescnt(deletion(ns,result));
	cout << "STUBBORN " << rescnt << ":";
	for(index_t j=0; j<rescnt; ++j) cout << " " << Net::Name[TR][(*result)[j]];
	cout << endl << "ENABLED";
	for(index_t j=0; j<Net::Card[TR]; ++j) if (ns.Enabled[j]) cout << " " << Net::Name[TR][j];
	cout << endl << "WHITE";
	for(index_t j=0; j<Net::Card[TR]; ++j) if (color[j]==WHITE) cout << " " << Net::Name[TR][j];
	cout << endl;
	return rescnt;
*/
	return deletion(ns,result);
}

/*!
 * \brief The deletion algorithm (including minimisation).
 * \param ns The marking for which the algorithm should be run.
 * \param result The resulting stubborn set in a newly allocated array.
 * \return The size of the stubborn set.
 */
index_t FirelistStubbornDeletion::deletion(NetState& ns, index_t** result)
{
	// run the inner deletion algorithm once (main run)
	cnstr(ns,NULL,0);

	// reserve space for the result
	*result = (index_t*) malloc(ns.CardEnabled * SIZEOF_INDEX_T);

	// check if all enabled transitions are white or only one white transition is enabled
	index_t rescnt(0); // counter for elements in *result
	bool allwhite(true);
	for(index_t i=0; i<ns.CardEnabled; ++i)
	{
		if (color[enabled[i]]!=WHITE) allwhite = false;
		else (*result)[rescnt++] = enabled[i];
	}
	if (allwhite || rescnt<2) return rescnt;

	// complete minimisation upto size 5 only
	index_t L(rescnt);
	if (ns.CardEnabled > 5) L = 2; 

	// subset of transitions (size L required)
	index_t subset[5] = {0,0,0,0,0};
	// pointers to the enabled list (same transitions)
	index_t subsetptr[5] = {0,0,0,0,0};
	// flag showing if the active position in subset is uninitialised
	bool newpos(true);

	for(index_t i=1,active=0; i < L; --active)
	{
		// advance to the next subset of enabled transitions of size i
		index_t next;
		while (active < i)
		{
			// obtain the previously selected transition (previous element or previous step), advance one transition index
			if (newpos) next = (active==0 ? 0 : subsetptr[active-1]+1);
			else next = subsetptr[active]+1;

			// if no more enabled transitions, retrace to previous element
			if (next==ns.CardEnabled)
			{
				if (active==0) break;
				--active; 
				newpos = false;
			} else { 
				// found an enabled transition, save it and move on to next element
				subsetptr[active] = next;
				subset[active++] = enabled[next];
				newpos = true; 
			}
		}

		// if we have checked all subsets of size i, increment i and start over
		if (active==0) { ++i; active=1; newpos = true; continue; }
		else newpos = false; // for the next run of the while loop

		// run the deletion algorithm on the chosen subset
		init();
		for(index_t j=0; j<i; ++j) protect[subset[j]] = true;
		cnstr(ns,subset,i);

		// check if all enabled white transitions are protected, return the subset if so
		index_t t, wecnt(0);
		allwhite = true;
		for(t=0; t<ns.CardEnabled; ++t)
			if (color[enabled[t]]==WHITE) 
			{
				++wecnt;
				if (protect[enabled[t]]==UNPROTECTED) allwhite = false;
			}
		if (allwhite)
		{
			for(index_t j=0; j<i; ++j) (*result)[j] = subset[j];
			return i;
		}

		// check if we have less enabled white transitions, decrease L if so
		if (L > wecnt)
		{
			rescnt = 0;
			for(t=0; t<ns.CardEnabled; ++t)
				if (color[enabled[t]]==WHITE)
					(*result)[rescnt++] = enabled[t];
			L = rescnt; // same as wecnt at this point
		}
	}
	return rescnt;
}

/*!
 * \brief Initialise the graph for one run of the inner deletion algorithm,
 *        i.e. reset protection, root, and color flags and reinit counters.
 */
void FirelistStubbornDeletion::init()
{
	// initialise graph properties that have a general start value
	memset(protect,UNPROTECTED,Net::Card[TR]);
	memset(root,PENDING,Net::Card[TR]);
	memset(color,WHITE,size);

	// check which transitions are enabled = and-nodes / disabled = or-nodes
	// for disabled transitions count the disabling places
	for(index_t t=0; t<Net::Card[TR]; ++t)
		counter[t] = maxcounter[t];

	// set counters at <t,s>-nodes
	for(index_t i=offset[0]; i<offset[0]+fieldsize; ++i)
		counter[i] = 2;

	// set counters in the formula-part of the graph
	if (sp) {
		// or-nodes of the graph (= and-nodes of the formula)
		for(map<index_t,vector<index_t> >::iterator it=f_and_nodes.begin(); it!=f_and_nodes.end(); ++it)
			counter[it->first+formula_offset] = f_nodes[it->first]->countUnsatisfied();
	}
}

/*!
 * \brief The inner deletion algorithm without minimisation. The graph must be initialised.
 * \param ns The current marking.
 * \param prt An array of protected transitions.
 * \param prtnr The number of protected transitions
 */
void FirelistStubbornDeletion::cnstr(NetState& ns, index_t* prt, index_t prtnr)
{
	// mark all protected transitions as unselectable/undeletable
	for(index_t i=0; i<prtnr; ++i)
		root[prt[i]] = DONE;

	while (true)
	{
		// continue only as long as there are at least two enabled white transitions
		// and one of them has root flag PENDING,
		// of these take the one with the minimal progress value
		index_t whites(0), minprogt;
		int32_t minprogress(INT32_MAX);
		index_t t;
		for(t=0; t<ns.CardEnabled; ++t)
		{
			index_t ts(enabled[t]); // make sure we select the transition with the lowest progress value first
			if (color[ts]==WHITE)
			{ 
				++whites;
				if (root[ts]==PENDING && Transition::ProgressMeasure[ts]<minprogress) {
					minprogress = Transition::ProgressMeasure[ts];
					minprogt = ts;
				}
			}
		}
		if (whites<2 || minprogress == INT32_MAX) return;

		// try to remove the selected transition from the stubborn set
		root[minprogt] = DONE;
		speculate(minprogt);

		// check if all protected transitions and the formula root are still white
		for(t=0; t<prtnr; ++t)
			if (color[prt[t]]!=WHITE) break;
		if (t < prtnr || (sp && color[formula_offset]!=WHITE)) {
			rehabilitate(minprogt); 
			continue; 
		}

		// check if there is a white key transition
		bool keyfound(false);
		for(index_t k=0; !keyfound && k<ns.CardEnabled; ++k)
		{
			index_t t(enabled[k]);
			if (color[t]!=WHITE) continue;
			keyfound = true;
			for(index_t i=0; keyfound && i<Net::CardArcs[TR][PRE][t]; ++i)
			{
				index_t p(Net::Arc[TR][PRE][t][i]);
				for(index_t j=0; keyfound && j<consumer[p].size(); ++j)
					if (color[consumer[p][j]]!=WHITE) keyfound=false;
			}
		}

		// on success mark all gray nodes black, i.e. exclude them from the stubborn set permanently
		if (keyfound)
		{
			for(index_t i=0; i<size; ++i)
				if (color[i]==GRAY) color[i]=BLACK;
		} else rehabilitate(minprogt);
	}
}

/*!
 * \brief Mark a node as temporarily removed from the stubborn set. Work out the
 *        consequences recursively.
 * \param node The node to be excluded.
 */
void FirelistStubbornDeletion::speculate(index_t node)
{
	// mark the node gray
	color[node] = GRAY;
	// check all predecessors
	for(index_t i=0; i<predcnt[node]; ++i)
	{
		index_t y(predecessor[node][i]);
		if (color[y]!=WHITE) continue; // only WHITE nodes are checked
		if (andor[y]) {
			if (!--counter[y]) speculate(y);
		} else speculate(y);
	}
}

/*!
 * \brief Revert a node to its original status if exclusion from the stubborn
 *        set is not possible. Also unmark all consequences recursively.
 * \param node The node to revert.
 */
void FirelistStubbornDeletion::rehabilitate(index_t node)
{
	// mark the node white again
	color[node] = WHITE;
	// check all predecessors
	for(index_t i=0; i<predcnt[node]; ++i)
	{
		index_t y(predecessor[node][i]);
		if (color[y]==BLACK) continue;
		if (andor[y]) ++counter[y];
		if (color[y]==GRAY) rehabilitate(y);
	}
}

/*!
 * \brief Make a copy of the firelist generator (possibly for concurrent threads?).
 * \param property Ignored for deadlock checking, otherwise the (possibly modified) state predicate.
 */
Firelist* FirelistStubbornDeletion::createNewFireList(SimpleProperty* property)
{
    if (sp) return new FirelistStubbornDeletion(((StatePredicateProperty*)property)->getPredicate());
    return new FirelistStubbornDeletion();
}


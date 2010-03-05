// -*- C++ -*-

/*!
 * \file    jobqueue.cc
 *
 * \brief   Class JobQueue
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/02/26
 *
 * \date    $Date: 2010-02-26 12:00:00 +0100 (Fr, 26. Feb 2010) $
 *
 * \version $Revision: -1 $
 */

#include "jobqueue.h"
#include "cmdline.h"
#include <iostream>
#include <set>
#include <map>
#include <deque>

using std::set;
using std::map;
using std::deque;
using std::cout;
using std::endl;

extern gengetopt_args_info args_info;

	/******************************************************
	* Implementation of the methods of the class JobQueue *
	******************************************************/

/** Constructor for the empty queue
	@param pn The Petri net for which the incidence matrix is to be built.
*/
JobQueue::JobQueue() : cnt(0),active(NULL) {}

/** Constructor for the initialized queue
	@param job The first job
*/
JobQueue::JobQueue(PartialSolution* job) : cnt(1),active(job) {}

/** Destructor. Frees the memory for all jobs in the queue
*/
JobQueue::~JobQueue() {
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
	{
		for(unsigned int i=0; i<jit->second.size(); ++i)
			delete jit->second[i];
	}
	delete active;
}

/** Test whether the job queue is empty.
	@return If it is empty.
*/
bool JobQueue::empty() {
	return (!active);
}

/** Test whether the job queue contains at most the active job.
	@return If the queue is "almost" empty.
*/
bool JobQueue::almostEmpty() {
	return (queue.empty());
}

/** Get the number of jobs in a queue.
	@return Size of the queue.
*/
int JobQueue::size() { return cnt; }

/** Get the first job in the queue.
	@return The first job.
*/
PartialSolution* JobQueue::first() { return active; }

/** Remove the first job in the queue.
	@return True if a job could be removed.
*/
bool JobQueue::pop_front() {
	if (empty()) return false;
	delete active;
	active=NULL;
	--cnt;
	if (!almostEmpty()) 
	{ 
		active = queue.begin()->second.front();
		queue.begin()->second.pop_front();
		if (queue.begin()->second.empty())
			queue.erase(queue.begin());
	}
	return true;
}

/** Add a job to the queue. The job will be at the earliest position available according to its
	priority. The job will be added even if it is already in the queue.
	@param job The job to be added to the queue.
*/
void JobQueue::push_front(PartialSolution* job) {
	if (empty()) active=job;
	else queue[priority(job)].push_front(job);
	++cnt;
}

/** Add a job to the queue. The job will be at the latest position available according to its
	priority. The job will be added even if it is already in the queue.
	@param job The job to be added to the queue.
*/
void JobQueue::push_back(PartialSolution* job) {
	if (empty()) active=job;
	else queue[priority(job)].push_back(job);
	++cnt;
}

/** Add a job to the queue. The job will be at the latest position available according to its
	priority. The job will not be added if there is an equivalent one in the queue.
	@param job The job to be added to the queue.
	@return If the job has been added.
*/
bool JobQueue::insert(PartialSolution* job) {
	if (empty()) { active=job; ++cnt; return true; }
	int pri = find(job);
	if (pri<0) return false;
	queue[pri].push_back(job);
	++cnt;
	return true;
}

/** Find out if there is an equivalent job in the queue.
	@param The job to be checked.
	@return -1 if the job was found in the queue, its priority otherwise.
*/
int JobQueue::find(PartialSolution* job) {
	int pri(priority(job));
	if (queue.find(pri)==queue.end()) return pri;
	// count the Parikh vector of the transition sequence of job
	map<Transition*,int> tmap;
	vector<Transition*> tseq(job->getSequence());
	if (tseq.size()>0)
		 for(unsigned int i=0; i<tseq.size(); ++i) ++tmap[tseq[i]];
	// go through the jobs with the same priority, but leave out first()
	deque<PartialSolution*> deq(queue[pri]);
	for(unsigned int i=0; i<deq.size(); ++i)
	{
		// count Parikh vector for queued job
		map<Transition*,int> tmap2;
		vector<Transition*> tseq2(deq[i]->getSequence());
		if (tseq2.size()>0)
			for(unsigned int j=0; j<tseq2.size(); ++j) ++tmap2[tseq2[j]];
		if (tmap==tmap2 && job->getRemains()==deq[i]->getRemains())
		{
			// test if the constraints are equal
			map<set<Place*>,int>::iterator cit;
			// the following vectors are assumed to have their elements in the same order.
			// In most cases(?) this will be garantueed at creation time.
			set<Constraint>& cs(job->getConstraints());
			set<Constraint>& cs2(deq[i]->getConstraints());
			if (cs==cs2) return -1;
/*			if (cs.size()!=cs2.size()) continue;
			int k;
			for(k=0; k<cs.size(); ++k)
				if (cs[k].getRHS()!=cs2[k].getRHS() || cs[k].compare(cs2[k])==0) break;
			if (k>=cs.size()) return -1;
*/
		}
	}
	return pri;
}

/** Calculate the priority of a job. The number is calculated from the total number of transitions
	to be fired, the number of non-firable transitions, and the number of constraints in the job.
	Lower numbers mean higher priorities. The result will always be non-negative.
	@param job The job for which the priority is to be calculated.
	@return The priority.
*/
int JobQueue::priority(PartialSolution* job) const {
	int priority(0);
	map<Transition*,int>::iterator it;
	for(it=job->getFullVector().begin(); it!=job->getFullVector().end(); ++it)
		priority += it->second;
	for(it=job->getRemains().begin(); it!=job->getRemains().end(); ++it)
		priority += it->second;
	priority += job->getConstraints().size();
	set<Constraint>::iterator cit;
	for(cit=job->getConstraints().begin(); cit!=job->getConstraints().end(); ++cit)
		if (cit->isJump()) ++priority;
	return priority;
}

/** Search the job queue for a full solution.
	@return A full solution or NULL if none is found.
*/
PartialSolution* JobQueue::findSolution() {
	if (active->isSolved()) return active;
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
			if (jit->second[i]->isSolved()) return jit->second[i];
	return NULL;
}

/** Check in a failure job queue whether the marking equation was infeasible.
	@return True if the marking equation is infeasible.	
*/
bool JobQueue::checkMEInfeasible() {
	bool infeasible = true;
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
			if (jit->second[i]->isFeasible()) infeasible=false;
	return infeasible;
}

/** Add a job to a failure queue. The job will be matched against the queue, only new
	portions will be inserted.
	@param job The job to be added to the failure queue.
*/
void JobQueue::push_fail(PartialSolution* job) {
	if (!args_info.verbose_given && queue.size()>1) return;
	if (!almostEmpty()) job->setFeasibility(true); // first entry: marking equation may be infeasible
	// check if there is an entry in the queue with an equivalent marking
	bool found = false; // no equivalent entry so far
	map<const Place*,unsigned int> map1(job->getMarking().getMap()); // the marking of job
	map<const Place*,unsigned int>::const_iterator mit;
	map<int,deque<PartialSolution*> >::iterator jit;
	unsigned int i; // we must remember i after the loop, so declare it here
	for(jit=queue.begin(); jit!=queue.end(); ++jit) // iterate through the failure queue
	{
		for(i=0; i<jit->second.size(); ++i) // inner iteration
		{
			// get the marking of a failure queue entry
			map<const Place*,unsigned int> map2(jit->second[i]->getMarking().getMap());
			// compare the maps which can have additional zero entries, so == is not advised
			for(mit=map1.begin(); mit!=map1.end(); ++mit)
				if (mit->second!=map2[mit->first]) break;
			if (mit==map1.end())
			{
				for(mit=map2.begin(); mit!=map2.end(); ++mit)
					if (mit->second!=map1[mit->first]) break;
				if (mit==map2.end()) { found=true; break; } // markings are identical
			}
		}
		if (found) break; // leave both loops
	}
	set<Constraint> ctmp;
	set<Constraint>::iterator cit;
	if (!found) // no equivalent entry found, create a new one
	{
		set<Constraint> cv;
		ctmp = job->getConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent()) if (cit->cleanConstraintSet(cv)) cv.insert(*cit);
		ctmp = job->getFailureConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent()) if (cit->cleanConstraintSet(cv)) cv.insert(*cit);
		job->setConstraints(cv);
		map<Transition*,int> parikh(job->calcParikh());
		queue[priority(job)].push_back(job); 
		++cnt;
		if (cleanFailure(parikh)) job->setSolved(); // if job is obsolete
		return;
	}
	// add our failed constraints into the list
	ctmp = job->getConstraints();
	for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
		if (cit->isRecent()) 
			if (cit->cleanConstraintSet(jit->second[i]->getConstraints())) 
				jit->second[i]->setConstraint(*cit);
	ctmp = job->getFailureConstraints();
	for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
		if (cit->isRecent()) 
			if (cit->cleanConstraintSet(jit->second[i]->getConstraints())) 
				jit->second[i]->setConstraint(*cit);
	// update status of the failed job
	jit->second[i]->setFeasibility(true);
	map<Transition*,int> parikh(job->calcParikh());
	if (job->getSequence().size()<jit->second[i]->getSequence().size()) jit->second[i]->setSequence(job->getSequence());
	if (cleanFailure(parikh)) jit->second[i]->setSolved();
}

/** (Pseudo-)Cleans a failure queue from obsolete entries regarding one parikh image.
	@param The parikh image to check against.
*/
bool JobQueue::cleanFailure(map<Transition*,int>& p) {
	bool result(false);
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
		{ // walk all failure entries
			vector<map<Transition*,int> >& parikh(jit->second[i]->getParikh());
			for(unsigned int j=0; j<parikh.size(); ++j)
			{ // go through all parikh images in an entry
				map<Transition*,int>::iterator mit,pit;
				bool smaller(false); // compare if queue entry is smaller than p
				for(mit=parikh[j].begin(),pit=p.begin(); mit!=parikh[j].end(); ++mit)
				{
					if (pit==p.end()) break;
					if (mit->first!=pit->first) continue;
					if (mit->second>pit->second) break;
					if (mit->second<pit->second) smaller=true;
					++pit;
				}
				// on a positive result, mark queue entry as obsolete
				if (mit==parikh[j].end() && smaller) { jit->second[i]->setSolved(); break; }
				if (!result) {
					smaller=false; // compare if p is smaller than queue entry
					for(mit=parikh[j].begin(),pit=p.begin(); pit!=p.end(); ++pit)
					{
						if (mit==parikh[j].end()) break;
						if (mit->first!=pit->first) continue;
						if (mit->second<pit->second) break;
						if (mit->second>pit->second) smaller=true;
						++mit;
					}
					// on a positive result mark p as obsolete
					if (pit==p.end() && smaller) { result=true; break; }
				}
			}
		}
	return result;
}

/** Prints a failure queue.
	@param im Incidence matrix of the Petri net
*/
void JobQueue::printFailure(IMatrix& im) {
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
		{
			PartialSolution* ps(jit->second[i]);
			if (ps->isSolved()) continue; // this failure is obsolete
			cout << "After the firing sequence \"";
			if (ps->getSequence().size()>0)
			{
				bool blank = false;
				for(unsigned int j=0; j<ps->getSequence().size(); ++j)
				{
					if (blank) cout << " ";
					blank = true;
					cout << ps->getSequence()[j]->getName();
				}
			}
			else cout << "<empty>";
			cout << "\":" << endl;
			set<Constraint>::iterator cit;
			for(cit=ps->getConstraints().begin(); cit!=ps->getConstraints().end(); ++cit)
			{
				int diff = (cit->getRHS()-ps->getActualRHS(*cit)); // needed tokens (at least)
				cout << " - need at least " << diff
						<< " more token" << (diff==1?"":"s") << " on place" 
						<< (cit->getPlaces().size()==1?" ":" set {");
				bool comma = false;
				// get the set of undermarked places
				map<Place*,int> pmap(cit->getPlaces());
				map<Place*,int>::iterator mit;
				for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
				{
					if (comma) cout << ",";
					comma = true;
					cout << mit->first->getName() << ":" << ps->getMarking()[*(mit->first)];
				}
				cout << (cit->getPlaces().size()==1?"":"}") << " to fire ";
				comma = false;
				// get the set of hindered transitions in the SCC
				set<Transition*> tset(cit->getSubTransitions());
				// if it is empty, fall back to the transitions just relying on the SCC
				if (tset.empty()) 
				{ 
					tset=ps->disabledTransitions(pmap,im); 
					if (tset.size()==1) cout << "transition ";
					else cout << "all transitions in {"; 
				}
				else
				{ 
					if (tset.size()==1) cout << "transition ";
					else cout << "a transition in {";
				}
				set<Transition*>::iterator tit;
				for(tit=tset.begin(); tit!=tset.end(); ++tit)
				{
					if (comma) cout << ",";
					comma = true;
					cout << (*tit)->getName();
				}
				cout << (tset.size()==1?"":"}") << endl;
			}
		}
}

/** Counts the size of a failure queue which may contain obsolete entries.
	@return The size.
*/
int JobQueue::trueSize() {
	int counter=0;
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
			if (!jit->second[i]->isSolved()) ++counter;
	return counter;
}


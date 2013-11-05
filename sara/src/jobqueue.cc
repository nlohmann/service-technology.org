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
 * \date    $Date: 2013-11-05 12:00:00 +0100 (Di, 5. Nov 2013) $
 *
 * \version $Revision: 1.14 $
 */

#include "jobqueue.h"
#include "verbose.h"
#include <iostream>
#include <set>
#include <map>
#include <deque>
#include <sstream>
#include <fstream>

using std::set;
using std::map;
using std::deque;
using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::ofstream;
using pnapi::Node;


	/******************************************************
	* Implementation of the methods of the class JobQueue *
	******************************************************/

namespace sara {

// command line switches and values
extern bool flag_droppast, flag_joborder, flag_verbose, flag_show, flag_forceprint, flag_continue;
extern int val_droppast;

// if there is more than one thread
extern bool multithreaded;
extern pthread_mutex_t print_mutex;

/** Constructor for the empty queue.
*/
JobQueue::JobQueue() : cnt(0),active(NULL) {}

/** Constructor for the initialized queue
	@param job The first job
*/
JobQueue::JobQueue(PartialSolution* job) : cnt(1),active(job) {}

/** Destructor. Frees the memory for all jobs in the queue, past, active, or future.
*/
JobQueue::~JobQueue() {
	flush();
}

/** Empties the queue.
*/
void JobQueue::flush() {
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
	{
		for(unsigned int i=0; i<jit->second.size(); ++i)
			delete jit->second[i];
	}
	for(jit=past.begin(); jit!=past.end(); ++jit)
	{
		for(unsigned int i=0; i<jit->second.size(); ++i)
			delete jit->second[i];
	}
	queue.clear();
	past.clear();
	delete active;
	active = NULL;
	cnt = 0;
}

/** Test whether the job queue is empty.
	@return If it is empty.
*/
bool JobQueue::empty() const {
	return (!active);
}

/** Test whether the job queue contains at most the active job and past jobs, but no future jobs.
	@return If the queue is "almost" empty.
*/
bool JobQueue::almostEmpty() {
	return (queue.empty());
}

/** Get the number of jobs in a queue, excluding past jobs.
	@return Size of the queue.
*/
int JobQueue::size() const { return cnt; }

/** Get the first, i.e. active, job in the queue.
	@return The first job.
*/
PartialSolution* JobQueue::first() { return active; }

/** Remove the active job from the queue, possibly pushing it into the past.
	@param kill Forbid this job's entry into the past list (e.g. if it is there already).
			In this case, the job is freed from memory.
	@return True if a job could be removed.
*/
bool JobQueue::pop_front(bool kill) {
	if (empty()) return false;

	// delete or push to the past?
	if (kill) delete active;
	else past[priority(active)].push_back(active); // active job is now past
	active=NULL;
	--cnt;

	// a job from the future must now become active
	if (!almostEmpty()) 
	{ 
		active = queue.begin()->second.front();
		queue.begin()->second.pop_front();
		if (queue.begin()->second.empty())
			queue.erase(queue.begin());
	}

	// clean up the past if there are too many entries there
	if (flag_droppast && (unsigned int)(val_droppast)<past.size()) // drop things that are too far in the past and will probably not come up again
	{
		cerr << "\r";
		status("warning: dropped all past jobs with priority %d",past.begin()->first);
		for(unsigned int i=0; i<past.begin()->second.size(); ++i)
			delete past.begin()->second[i];
		past.erase(past.begin());
	}

	return true;
}

/** Retrieve the active job from the queue and return it to the caller. The job is removed
	from the JobQueue and its memory management and will not be deleted upon the JobQueue's
	destruction.
	@return The active job (if any).
*/
PartialSolution* JobQueue::pop_front() {
	if (empty()) return NULL;

	// delete or push to the past?
	PartialSolution* res(active);
	active=NULL;
	--cnt;

	// a job from the future must now become active
	if (!almostEmpty()) 
	{ 
		active = queue.begin()->second.front();
		queue.begin()->second.pop_front();
		if (queue.begin()->second.empty())
			queue.erase(queue.begin());
	}

	return res;
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

/** Add a job to the past of a queue. The job will be at the latest position available according to its
	priority. The job will be added even if it is already in the queue.
	@param job The job to be added to the past of a queue.
*/
void JobQueue::push_past(PartialSolution* job) {
	past[priority(job)].push_back(job);

	// check if the past became too large and needs tidying
	if (flag_droppast && (unsigned int)(val_droppast)<past.size()) // drop things that are too far in the past and will probably not come up again
	{
		cerr << "\r";
		status("warning: dropped all past jobs with priority %d",past.begin()->first);
		for(unsigned int i=0; i<past.begin()->second.size(); ++i)
			delete past.begin()->second[i];
		past.erase(past.begin());
	}
}

/** Appends the queue jbg (but no past jobs) to this queue.
	@param jbq The queue to append to this one.
*/
void JobQueue::transfer(JobQueue& jbq) {
	// transfer the active job
	if (jbq.active && find(jbq.active)>=0) {
//		push_back(new PartialSolution(*(jbq.active)));
		push_back(jbq.active);
		jbq.active = NULL;
	}

	// transfer the future jobs
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=jbq.queue.begin(); jit!=jbq.queue.end(); ++jit) // walk all internal queues
		for(unsigned int i=0; i<jit->second.size(); ++i) // and their entries
			if (jit->second[i] && find(jit->second[i])>=0) {
//					push_back(new PartialSolution(*(jit->second[i])));
					push_back(jit->second[i]);
					jit->second[i] = NULL;
					// and copy them to this queue
			}
}

/** Find out if there is an equivalent job in the queue.
	@param job The job to be checked.
	@return -1 if the job was found in the queue, its would-be priority otherwise.
*/
int JobQueue::find(PartialSolution* job) {
	// check if the queue contains a job with the same priority
	int pri(priority(job));
	if (queue.find(pri)==queue.end()) return pri; // it's not in the queue

	// get job's constraints for later comparison
	set<Constraint>& cs(job->getConstraints()); 

	// count the Parikh vector of the transition sequence of job
	map<Transition*,int> tmap;
	vector<Transition*>& tseq(job->getSequence());
	for(unsigned int i=0; i<tseq.size(); ++i) ++tmap[tseq[i]];

	// go through the jobs with the same priority, but leave out active job
	deque<PartialSolution*>& deq(queue[pri]);
	for(unsigned int i=0; i<deq.size(); ++i)
	{
		// count Parikh vector for queued job
		map<Transition*,int> tmap2;
		vector<Transition*>& tseq2(deq[i]->getSequence());
		for(unsigned int j=0; j<tseq2.size(); ++j) ++tmap2[tseq2[j]];
		if (tmap==tmap2 && job->getRemains()==deq[i]->getRemains())
		{
			// test if the constraints are equal
			set<Constraint>& cs2(deq[i]->getConstraints());
			if (cs==cs2) return -1;
		}
	}
	return pri;
}

/** Find out if there was an equivalent job (having the same constraints) in the past.
	@param job The job to be checked.
	@param excl A job that may be in the queue but doesn't count as equivalent (NULL if none).
	@return True if the job was found in the past.
*/
bool JobQueue::findPast(PartialSolution* job, PartialSolution* excl) {
	// get priority and constraints of the job
	int pri(priority(job));
	set<Constraint>& cs(job->getConstraints());

	// if jobs of the same priority exist, check them
	if (past.find(pri)!=past.end())
	{
		// go through the past jobs with the same priority
		deque<PartialSolution*>& deq(past[pri]);
		for(unsigned int i=0; i<deq.size(); ++i)
		{
				// don't check the excluded job
				if (deq[i]==excl) continue;

				// compare the constraints
				set<Constraint>& cs2(deq[i]->getConstraints());
				if (cs==cs2) return true;
		}
	}
	return false;
}

/** Calculate the priority of a job. The number is calculated from the total number of transitions
	to be fired, the number of non-firable transitions, or the number of constraints in the job.
	Lower numbers mean higher priorities. The result will always be non-negative.
	@param job The job for which the priority is to be calculated.
	@return The priority.
*/
int JobQueue::priority(PartialSolution* job) const {
	int priority(0);
	if (flag_joborder)
	{
		// the alternative ordering by size of the potential solution
		map<Transition*,int>::iterator it;
		for(it=job->getFullVector().begin(); it!=job->getFullVector().end(); ++it)
			priority += it->second;
	} else {
		// the standard ordering by number of constraints (jumps count twice)
		priority += (job->getConstraints().size());
		set<Constraint>::iterator cit;
		for(cit=job->getConstraints().begin(); cit!=job->getConstraints().end(); ++cit)
			if (cit->isJump()) priority+=1;
	}
	return priority;
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
	// if the user doesn't want to know about failure reasons, forget about the job
	if (!flag_verbose && !flag_show && queue.size()>1) { delete job; return; } 
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
		bool done(false); // if there is either at least one "real" constraint or none at all
		set<Constraint> cv; // a copy of the necessary "real" (= place) constraints
		ctmp = job->getConstraints();
		if (ctmp.empty()) done=true; // none at all case
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent() && !cit->getPlaces().empty()) // check for new "real" constraints 
				if (cit->cleanConstraintSet(cv)) { cv.insert(*cit); done=true; }
		ctmp = job->getFailureConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent() && !cit->getPlaces().empty()) // also in the failure queue
				if (cit->cleanConstraintSet(cv)) { cv.insert(*cit); done=true; }
		if (done) { // we should save this job as a failure ...
			job->setConstraints(cv);
			map<Transition*,int> parikh(job->calcParikh());
			queue[priority(job)].push_back(job); 
			++cnt;
			if (cleanFailure(parikh)) job->setSolved(); // check if job is obsolete
		} else delete job; // ... or delete it
		return;
	}
	// we found an earlier version
	// check if the job has "real" constraints and a shorter firing sequence than the old one
	ctmp = job->getConstraints();
	for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
		if (cit->isRecent() && !cit->getPlaces().empty()) break;
	if (cit==ctmp.end()) {
		ctmp = job->getFailureConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent() && !cit->getPlaces().empty()) break;
	}
	if (cit!=ctmp.end() && job->getSequence().size()<jit->second[i]->getSequence().size()) {
		// if so, copy the necessary constraints
		jit->second[i]->clearConstraints();
		ctmp = job->getConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent() && !cit->getPlaces().empty()) 
				if (cit->cleanConstraintSet(jit->second[i]->getConstraints())) 
					jit->second[i]->setConstraint(*cit);
		ctmp = job->getFailureConstraints();
		for(cit=ctmp.begin(); cit!=ctmp.end(); ++cit)
			if (cit->isRecent() && !cit->getPlaces().empty()) 
				if (cit->cleanConstraintSet(jit->second[i]->getConstraints())) 
					jit->second[i]->setConstraint(*cit);

		// update the status of the failed job
		jit->second[i]->setFeasibility(true);
		map<Transition*,int> parikh(job->calcParikh());

		// and update the firing sequence
		jit->second[i]->setSequence(job->getSequence());
		jit->second[i]->addParikh(parikh);
		if (cleanFailure(parikh)) jit->second[i]->setSolved();
	}
	delete job;
}

/** (Pseudo-)Clean a failure queue from obsolete entries regarding one parikh image
	of a partial firing sequence.
	@param p The parikh image to check against.
	@return If the parikh images itself is made obsolete by the queue.
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
				for(mit=parikh[j].begin(),pit=p.begin(); pit!=p.end(); ++pit)
				{
					if (mit==parikh[j].end()) break;
					if (mit->first!=pit->first) continue;
					if (mit->second>pit->second) break;
					if (mit->second<pit->second) smaller=true;
					++mit;
				}
				for(; pit!=p.end(); ++pit) // check if there are still positive entries
					if (pit->second>0) smaller=true;
				// on a positive result, mark queue entry as obsolete
				if (mit==parikh[j].end() && smaller) { jit->second[i]->setSolved(); break; }
				if (!result) {
					smaller=false; // compare if p is smaller than queue entry
					for(mit=parikh[j].begin(),pit=p.begin(); mit!=parikh[j].end(); ++mit)
					{
						if (pit==p.end()) break;
						if (mit->first!=pit->first) continue;
						if (mit->second<pit->second) break;
						if (mit->second>pit->second) smaller=true;
						++pit;
					}
					for(; mit!=parikh[j].end(); ++mit) // check if there are still positive entries
						if (mit->second>0) smaller=true;
					// on a positive result mark p as obsolete
					if (pit==p.end() && smaller) { result=true; break; }
				}
			}
		}
	return result;
}

/** Print a failure queue.
	@param im Incidence matrix of the Petri net
	@param pb The problem instance (for saving a visually colored net).
	@param pbnr An identifying number for the problem instance (used for filename construction)
	@param json The JSON object to which the failure information will additionally be written.
*/
void JobQueue::printFailure(IMatrix& im, Problem& pb, int pbnr, JSON& json) {
	int failcnt(0); // counting the failures for adapting filenames
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
		{
			PartialSolution* ps(jit->second[i]);
			if (ps->isSolved()) continue; // this failure is obsolete
			cout << "After the firing sequence \"";
			JSON tmp,tmp3;
			if (ps->getSequence().size()>0)
			{
				bool blank = false;
				for(unsigned int j=0; j<ps->getSequence().size(); ++j)
				{
					if (blank) cout << " ";
					blank = true;
					cout << ps->getSequence()[j]->getName();
					tmp += ps->getSequence()[j]->getName();
				}
				resetColors(*(pb.getPetriNet()));
				colorSequence(ps->getSequence());
			}
			else { cout << "<empty>"; tmp += "<empty>"; }
			cout << "\":" << endl;
			tmp3["AFTER SEQUENCE"] = tmp;

			// At this point color the remainder of transitions blue but do not print anything
			map<Transition*,int> tmx(ps->getRemains());
			set<Transition*> tmxset;
			map<Transition*,int>::iterator tmxit;
			map<Place*,int> ppmap; // for coloring the postset of the transitions
			for(tmxit=tmx.begin(); tmxit!=tmx.end(); ++tmxit)
				if (tmxit->second>0) { 
					tmxset.insert(tmxit->first);
					set<pnapi::Arc*> aset(tmxit->first->getPostsetArcs());
					set<pnapi::Arc*>::iterator ait;
					for(ait=aset.begin(); ait!=aset.end(); ++ait)
						ppmap[&((*ait)->getPlace())]=1;
				}
			colorTransitions(tmxset,true);
			colorPlaces(ppmap,true);

			// Return to printing the counterexample
			if (ps->getConstraints().empty()) cout << " - final marking reached, no more transitions to fire" << endl;
			set<Constraint>::iterator cit;
			for(cit=ps->getConstraints().begin(); cit!=ps->getConstraints().end(); ++cit)
			{
				// get the undermarked places
				map<Place*,int> pmap(cit->getPlaces());
				map<Place*,int>::iterator mit;
				// get the set of hindered transitions in the SCC
				set<Transition*> tset(cit->getSubTransitions());
				set<Transition*>::iterator tit;
				int diff = (cit->getRHS()-ps->getActualRHS(*cit)); // needed tokens (at least)
				if (diff==0 || !tset.empty()) // calculate the missing tokens directly in this case
				{
					if (tset.empty()) { // recalculate places if the constraint was not helpful
						pmap.clear();
						for(tmxit=tmx.begin(); tmxit!=tmx.end(); ++tmxit)
							if (tmxit->second>0)
								for(mit=im.getPreset(*tmxit->first).begin(); mit!=im.getPreset(*tmxit->first).end(); ++mit)
									if (ps->getMarking()[*(mit->first)]<(unsigned int)(mit->second)) {
										pmap[mit->first] = mit->second-ps->getMarking()[*(mit->first)];						
										tset.insert(tmxit->first);
									}
					}
					else { // recalculate places if the constraint was not helpful
						pmap.clear();
						for(tit=tset.begin(); tit!=tset.end(); ++tit)
							for(mit=im.getPreset(**tit).begin(); mit!=im.getPreset(**tit).end(); ++mit)
								if (ps->getMarking()[*(mit->first)]<(unsigned int)(mit->second))
									pmap[mit->first] = mit->second-ps->getMarking()[*(mit->first)];						
					}
					if (diff==0) {
						diff=-1;
						for(tit=tset.begin(); tit!=tset.end(); ++tit)
						{
							int miss=0;
							for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
								if (ps->getMarking()[*(mit->first)]<(unsigned int)(im.getPreset(**tit)[mit->first]))
									miss += im.getPreset(**tit)[mit->first]-ps->getMarking()[*(mit->first)];
							if (diff<0 || miss<diff) diff=miss;
						}
					}
				}
				JSON tmp2;
				tmp2["NEED"] = diff;
				cout << " - need at least " << diff
						<< " more token" << (diff==1?"":"s") << " on place" 
						<< (pmap.size()==1?" ":" set {");
				bool comma = false;
				for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
				{
					if (comma) cout << ",";
					comma = true;
					cout << mit->first->getName() << ":" << ps->getMarking()[*(mit->first)];
					tmp2["ON"][mit->first->getName()] = (int)(ps->getMarking()[*(mit->first)]);
				}
				colorPlaces(pmap,false);
				cout << (pmap.size()==1?"":"}") << " to fire ";
				comma = false;
				// if tset is empty, fall back to the transitions just relying on the SCC
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
				for(tit=tset.begin(); tit!=tset.end(); ++tit)
				{
					if (comma) cout << ",";
					comma = true;
					cout << (*tit)->getName();
					tmp2["TO FIRE"] += (*tit)->getName();
				}
				colorTransitions(tset,false);
				cout << (tset.size()==1?"":"}") << endl;
				tmp3["DEADLOCKS"] += tmp2;
			}
			saveColoredNet(pb,pbnr,++failcnt);
			json[pb.getName()]["COUNTEREXAMPLE"] += tmp3;
		}
}

/** Count the size of a failure queue which may contain obsolete entries.
	@return The size.
*/
int JobQueue::trueSize() {
	int counter(0);
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
			if (!jit->second[i]->isSolved()) ++counter;
	return counter;
}

/** Add a job to a solution queue. The job will be matched against the queue, only incomparable
	jobs will be inserted, bigger ones from the queue are eliminated.
	@param job The job to be added to the solution queue.
	@return If the job was added to the queue.
*/
bool JobQueue::push_solved(PartialSolution* job) {
	map<Transition*,int> p(job->calcParikh());
	bool result(false); // if we should refuse to add this solution
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
	{
		bool changed(false); // whether we have changed this deque
		for(unsigned int i=0; i<jit->second.size(); ++i)
		{ // walk all solution entries
			vector<map<Transition*,int> >& parikh(jit->second[i]->getParikh());
			for(unsigned int j=0; j<parikh.size(); ++j)
			{ // go through all parikh images in an entry
				map<Transition*,int>::iterator mit,pit;
				bool smaller(false); // compare if queue entry is smaller or equal to p
				if (!result) {
					for(mit=parikh[j].begin(),pit=p.begin(); pit!=p.end(); ++pit)
					{
						if (mit==parikh[j].end()) break;
						if (mit->first!=pit->first) continue;
						if (mit->second>pit->second) break;
						if (mit->second<pit->second) smaller=true;
						++mit;
					}
					for(; pit!=p.end(); ++pit) // check if there are still positive entries
						if (pit->second>0) smaller=true;
					// on a positive result, mark p as not to be added
					if (mit==parikh[j].end()) { result=true; break; }
				}
				smaller=false; // compare if p is smaller than queue entry
				for(mit=parikh[j].begin(),pit=p.begin(); mit!=parikh[j].end(); ++mit)
				{
					if (pit==p.end()) break;
					if (mit->first!=pit->first) continue;
					if (mit->second<pit->second) break;
					if (mit->second>pit->second) smaller=true;
					++pit;
				}
				for(; mit!=parikh[j].end(); ++mit) // check if there are still positive entries
					if (mit->second>0) smaller=true;
				// on a positive result delete the entry and mark the deque as faulty
				if (pit==p.end() && smaller) 
				{ 
					delete jit->second[i];
					jit->second[i] = NULL;
					changed = true;
					--cnt;
					break;
				}
			}
		}
		if (changed) { // replace the deque by one without null pointers
			deque<PartialSolution*> dtmp;
			for(unsigned int i=0; i<jit->second.size(); ++i)
				if (jit->second[i]) dtmp.push_back(jit->second[i]);
			jit->second = dtmp;
		}
	}
	// if there is no smaller (or equal) solution in the queue, insert this one
	if (!result) { ++cnt; queue[priority(job)].push_back(job);}
	else delete job; // otherwise forget this solution
	return !result;
}

/** Print a solution queue.
	@param sum On Return: The sum over all solution lengths.
	@param pb The problem instance for saving a colored solution.
	@param pbnr An identifying number for the problem instance (used for filename construction)
	@param json The JSON object to which the solution will be written (additionally).
	@return The maximal length of a solution.
*/
int JobQueue::printSolutions(int& sum, Problem& pb, int pbnr, JSON& json) {
	int sollength(-1); // solution length
	int colcnt(0); // counting the solutions for adapting filenames
	vector<Transition*> tmpsolv;
	if (flag_forceprint) return -1; // solutions were printed earlier
	if (queue.empty()) abort(14,"error: solved, but no solution found -- this should not happen");
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
		for(unsigned int i=0; i<jit->second.size(); ++i)
		{
			PartialSolution* ps(jit->second[i]);
			vector<Transition*>& solution(ps->getSequence());
			if (flag_continue)
			{
				cout << "sara: SOLUTION(" << solution.size() << "): ";
				if ((int)(solution.size())>sollength) sollength=(int)(solution.size());
				sum += solution.size();
				JSON tmp;
				for(unsigned int j=0; j<solution.size(); ++j)
				{
					cout << solution[j]->getName() << " ";
					tmp += solution[j]->getName();
				}
				cout << endl;
				json[pb.getName()]["SOLUTION"] += tmp;
				resetColors(*(pb.getPetriNet()));
				colorSequence(solution);
				saveColoredNet(pb,pbnr,++colcnt);
			} else if (sollength<0 || solution.size()<tmpsolv.size()) {
				tmpsolv = solution;
				sollength = (int)(tmpsolv.size());
			}
		}
	if (!flag_continue) {
				cout << "sara: SOLUTION(" << tmpsolv.size() << "): ";
				sum += tmpsolv.size();
				for(unsigned int j=0; j<tmpsolv.size(); ++j)
				{
					cout << tmpsolv[j]->getName() << " ";
					json[pb.getName()]["SOLUTION"] += tmpsolv[j]->getName();
				}
				cout << endl;
				resetColors(*(pb.getPetriNet()));
				colorSequence(tmpsolv);
				saveColoredNet(pb,pbnr,++colcnt);
	}
	return sollength;
}

#ifdef SARALIB
/** Get a solution if one exists.
	@return One solution (even if more were found), or a vector containing one NULL pointer if there is no solution.
*/
vector<Transition*> JobQueue::getOneSolution() {
	vector<Transition*> noresult;
	if (queue.empty()) { noresult.push_back(NULL); return noresult; }
	return (queue.begin()->second[0]->getSequence());
}
#endif

/** Print the queue including past, active, and future jobs for debug purposes.
	@param past Whether the past jobs and the active job should be printed at all.
		Choose false for solution and failure queues as these do not have past and
		active jobs.
*/
void JobQueue::show(bool past) {
	if (past) {
		cerr << "+++++ Past entries:" << endl;
		map<int,deque<PartialSolution*> >::iterator jit;
		for(jit=this->past.begin(); jit!=this->past.end(); ++jit)
		{
			cerr << "++++ Priority " << jit->first << endl;
			for(unsigned int i=0; i<jit->second.size(); ++i)
			{
				cerr << "++ Next job:" << endl;
				jit->second[i]->show();
				cerr << endl;
			}	
		}
		cerr << "++++++ Active entry:" << endl;
		if (active) {
			cerr << "++++ Priority " << priority(active) << endl;
			active->show(); 
			cerr << endl;
		} else cerr << "++ No active job" << endl;
	}
	cerr << "++++++ Future entries:" << endl;
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit)
	{
		cerr << "+++++ Priority " << jit->first << endl;
		for(unsigned int i=0; i<jit->second.size(); ++i)
		if (jit->second[i])
		{
			if (jit->second[i]->isSolved() && !past) cerr << "++ Next job is obsolete:" << endl;
			else cerr << "++ Next job:" << endl;
			jit->second[i]->show();
			cerr << endl;
		}	
	}
}

/** Delete all jobs that have an lp_solve solution less or equal to vec.
	JobQueues using this method may not use any find operations,  as these
	cannot handle the potential null pointers. push_back() is allowed and
	the queue can be shifted to a failure queue via append().
	@param vec The vector to compare the jobs in this JobQueue to.
*/
void JobQueue::deleteCovered(map<Transition*,int>& vec) {
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=queue.begin(); jit!=queue.end(); ++jit) // walk all queues
		for(unsigned int i=0; i<jit->second.size(); ++i) // and entries in the queues
			if (jit->second[i]) // if an entry exists
				if (jit->second[i]->compareVector(vec)) // and is less than vec
				{ delete jit->second[i]; jit->second[i]=NULL; } // delete it
}

/** Appends the queue jbg to a failure queue, sieving out null pointers.
	@param jbq The queue to append to this one.
*/
void JobQueue::append(JobQueue& jbq) {
	map<int,deque<PartialSolution*> >::iterator jit;
	for(jit=jbq.queue.begin(); jit!=jbq.queue.end(); ++jit) // walk all internal queues
		for(unsigned int i=0; i<jit->second.size(); ++i) // and their entries
			if (jit->second[i]) push_fail(new PartialSolution(*(jit->second[i])));
			// and copy them to a failure queue
}

/** Colors the transitions of the firing sequence according to their number of
	occurrences -- more occurrences = darker grey. Used for dot output.
	@param tvec The firing sequence.
*/
void JobQueue::colorSequence(vector<Transition*>& tvec) {
	if (!flag_show) return;
	map<Transition*,int> tmap;
	for(unsigned int i=0; i<tvec.size(); ++i)
		tmap[tvec[i]]++;
	
	int sz(0);
	map<Transition*,int>::iterator mit;
	for(mit=tmap.begin(); mit!=tmap.end(); ++mit)
		if (sz<mit->second) sz=mit->second;
	if (sz==0) return;
	bool mode(false);
	if (sz>60) { sz = sz/60; mode=true; } 
	else if (sz==1) sz=60; 
	else sz = 60/(sz-1);
	int color;
	for(mit=tmap.begin(); mit!=tmap.end(); ++mit)
	{
		stringstream sstr;
		string scol, colname;
		if (mode) color = 80 - (mit->second-1) / sz;
		else color = 80 - (mit->second-1) * sz;
		sstr << color;
		sstr >> scol;
		colname = "gray" + scol;
		mit->first->setColor(colname);
		set<Node*>::iterator nit;
		set<Node*> pset(mit->first->getPreset());
		for(nit=pset.begin(); nit!=pset.end(); ++nit)
			(*nit)->setColor("gray80");
		pset = mit->first->getPostset();
		for(nit=pset.begin(); nit!=pset.end(); ++nit)
			(*nit)->setColor("gray80");
	}
}

/** Colors the transitions of a given set red. Used for dot output.
	@param tset The set of transitions.
	@param blue Use color blue instead of red.
*/
void JobQueue::colorTransitions(set<Transition*>& tset, bool blue) {
	if (!flag_show) return;
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		(*tit)->setColor((blue?"blue":"red"));
}

/** Colors the places occurring in a given map red. Used for dot output.
	@param pmap The map containing the places.
	@param blue Use color blue instead of red.
*/
void JobQueue::colorPlaces(map<Place*,int>& pmap, bool blue) {
	if (!flag_show) return;
	map<Place*,int>::iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		pit->first->setColor((blue?"blue":"red"));
}

/** Removes all coloring from the net. Used for dot output.
	@param pn The Petri net to uncolor.
*/
void JobQueue::resetColors(PetriNet& pn) {
	if (!flag_show) return;
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		(*pit)->setColor();
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		(*tit)->setColor();
}

/** Write the painted Petri net as dot output.
	@param pb The problem instance containing the Petri net.
	@param pbnr An identifying number for the problem instance (appears in the filename)
	@param nr The number of this solution/counterexample (appears in the filename).
*/
void JobQueue::saveColoredNet(Problem& pb, int pbnr, int nr) {
	if (!flag_show) return;
	string name(pb.getFilename());
	size_t found = name.rfind('.');
	if (found!=string::npos) name = name.substr(0,found);
	stringstream sstr;
	string snr;
	sstr << "_P" << pbnr << "S" << nr << ".dot";
	sstr >> snr;
	name += snr;
	ofstream outfile(name.c_str());
	if (!outfile.is_open()) abort(6,"error while writing Petri net to file '%s'",name.c_str());
	outfile << pnapi::io::dot << *(pb.getPetriNet());
	outfile.close();
}

} // end namespace sara

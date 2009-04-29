#include "helpers.h"


const std::string intToStr(const int x) {
	std::ostringstream o;
	if (!(o << x)) return "ERROR";
	return o.str();
}

void findMaxDependenciesR(EBV candidate, vector<EventSetBound>& results, set<PPS>& alreadyTested, lprec *lp, std::map<EVENT,unsigned int> EventID) {

	static int counter = 0;

	if (candidate.size() <= 1) return;

	bool minUnbounded = false;
	bool maxUnbounded = false;

	int objCol[candidate.size()];
	REAL objVal[candidate.size()];

	PPS thisOne;

	for (int i = 0; i < candidate.size(); ++i) {

		thisOne.insert(candidate.at(i).key);

		if (candidate.at(i).min == LINDA_UNBOUNDED) {
			minUnbounded = true;
		}
		if (candidate.at(i).max == LINDA_UNBOUNDED) {
			maxUnbounded = true;
		}

		objCol[i] = EventID[candidate.at(i).key];
		objVal[i] = 1;

	}

	if (alreadyTested.find(thisOne) != alreadyTested.end()) {
		return;
	} else {
		alreadyTested.insert(thisOne);
		++counter;
		//std::cerr << "another test: " << counter << std::endl;
	}

	set_obj_fnex(lp, candidate.size(), objVal, objCol);

	int min = LINDA_UNBOUNDED;
	int max = LINDA_UNBOUNDED;
	set_minim(lp);

	int result = solve(lp);
	if (result == 0) {
		min = (int) get_objective(lp);
	}

	set_maxim(lp);
	result = solve(lp);
	if (result == 0) {
		max = (int) get_objective(lp);
	}

	EventSetBound b(thisOne);
	b.min = min;
	b.max = max;
	results.push_back(b);

	if (candidate.size() == 2) return;

	// Step 2: Recursive call.

	for (int i = 0; i < candidate.size(); ++i) {
		EBV newCandidate = candidate;
		newCandidate.erase(newCandidate.begin()+i);
		findMaxDependenciesR(newCandidate, results, alreadyTested, lp, EventID);
	}
}

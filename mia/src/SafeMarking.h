/*
 * SafeMarking.h
 *
 *  Created on: Apr 15, 2011
 *      Author: darsinte
 */

#pragma once

#include <bitset>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

//using std::bitset;
//using std::vector;
//using std::pair;
//using std::priority_queue;
//using std::cout;

class SafeMarking {
private:
	vector<SafeMarking> leafs, children;
	bitset<100000> m_old, m_new;
	int type;

public:

	enum NODE_TYPE {
		LEAF = 0,
		INNER = 1
	};

	SafeMarking();
	SafeMarking(bitset<100000> m_old, bitset<100000> m_new, int type);
	~SafeMarking();

	void setMarking(bitset<100000> m_old, bitset<100000> m_new);

	void setSize(int size);
	int getSize();

	void setType(int type);
	int getType();

	bitset<100000> getM_Old() const;
	bitset<100000> getM_New() const;

	SafeMarking intersect(SafeMarking other);

	void addLeaf(SafeMarking leaf);
	void addChildren(SafeMarking child);
	bool isEmpty();

	bool operator<(const SafeMarking &other) const;
	friend ostream& operator<<(ostream&, const SafeMarking&);
};

void build_tree(vector<SafeMarking> global_states);

/*
- compute all intersections, remember for each intersection which leaf is source
- insert intersection in priority queue - see stl::priority_queue
- extract intersection one by one
- add link between leafs and check existing fathers
 */


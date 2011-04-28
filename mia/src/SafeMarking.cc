/*
 * SafeMarking.cpp
 *
 *  Created on: Apr 15, 2011
 *      Author: darsinte
 */

#include "SafeMarking.h"

SafeMarking::SafeMarking() {
	type = SafeMarking::LEAF;
}

SafeMarking::SafeMarking(bitset<100000> m_old, bitset<100000> m_new, int type) {
	this->m_old = m_old;
	this->m_new = m_new;
	this->type = type;
}

SafeMarking::~SafeMarking() {
}

void SafeMarking::setMarking(bitset<100000> m_old, bitset<100000> m_new) {
	this->m_old = m_old;
	this->m_new = m_new;
}

bitset<100000> SafeMarking::getM_Old() const{
	return m_old;
}

bitset<100000> SafeMarking::getM_New() const{
	return m_new;
}

SafeMarking SafeMarking::intersect(SafeMarking other) {
	bitset<100000> inters_old = m_old & other.getM_Old();
	bitset<100000> inters_new = m_new & other.getM_New();

	SafeMarking result(inters_old, inters_new, SafeMarking::INNER);

	return result;
}

void SafeMarking::addLeaf(SafeMarking leaf) {
	leafs.push_back(leaf);
}

void SafeMarking::addChildren(SafeMarking child) {
	children.push_back(child);
}

bool SafeMarking::isEmpty() {
	return (m_old.count() == 0) && (m_new.count() == 0);
}

void build_tree(vector<SafeMarking> global_states) {
	priority_queue<SafeMarking> inners;
	for (int i = 0; i < global_states.size(); i++) {
		for (int j = 0; j < i; j++) {
			SafeMarking inner = global_states[i].intersect(global_states[j]);
			inners.push(inner);
		}
	}

	while (!inners.empty()) {
		SafeMarking inner = inners.top();
		inners.pop();
		cout << inner.getM_Old().count() << " " << inner.getM_New().count() << "\n";
	}
}

bool SafeMarking::operator <(const SafeMarking& other) const {
	return ((m_old.count() <= other.getM_Old().count()) && (m_new.count() <= other.getM_New().count()));
}

ostream& operator<<(ostream& out, const SafeMarking& m)
{
	out << "(" << m.m_old.to_ulong() << "," << m.m_new.to_ulong() << ")";
	return out;
}

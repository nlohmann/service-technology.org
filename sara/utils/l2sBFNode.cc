// -*- C++ -*-

/*!
 * \file    BFNode.cc
 *
 * \brief   Class BFNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/03/28
 *
 * \date    $Date: 2011-03-28 14:47:01 +0200 (Mo, 28. Mar 2011) $
 *
 * \version $Revision: 1.05 $
 */

#include <vector>
#include <map>
#include <set>
#include <iostream>

#include "l2sBFNode.h"

using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::endl;

	/*********************************
	* Implementation of class BFNode *
	*********************************/

/** Standard node constructor.
*/
BFNode::BFNode() : cp(BFINVALID),op(0) {}

/** Node destructor. Subnode deletion is included.
*/
BFNode::~BFNode() {
//	set<BFNode*>::iterator bfit;
//	for(bfit=succ.begin(); bfit!=succ.end(); ++bfit)
//		delete (*bfit);
}

/** Node collector. Get all nodes in the tree/acyclic graph.
	@param all Returns the set of all nodes (initialize with the empty set).
*/
void BFNode::allNodes(set<BFNode*>& all) {
	all.insert(this);
	set<BFNode*>::iterator bfit;
	for(bfit=succ.begin(); bfit!=succ.end(); ++bfit)
		(*bfit)->allNodes(all);
}

/** Clear the successor list, so a delete will only affect this node.
*/
void BFNode::clear() { succ.clear(); }

BFNode* BFNode::copy() {
	BFNode* node = new BFNode();
	node->cp = cp;
	node->op = op;
	node->p = p;
	set<BFNode*>::iterator sit;
	for(sit=succ.begin(); sit!=succ.end(); ++sit)
		node->succ.insert((*sit)->copy());
	return node;
}

/** Make a node an inner node.
	@param bop The boolean operator representing the inner node.
	@return If the operation succeeded. Will not succeed for leaves.
*/
bool BFNode::setInner(int bop) {
	if (cp>BFFORM) return false;
	cp = BFFORM;
	op = bop;
	return true;
}

/** Make a node a leaf with an atomic formula.
	@param place The place name for the atomic formula.
	@param comp The compare operator of the atomic formula.
	@param val The number of tokens for the comparison.
	@return If the operation succeeded. Will not succeed for inner nodes.
*/
bool BFNode::setLeaf(map<string,int> place, int comp, int val) {
	if (cp==BFFORM) return false;
	p = place;
	cp = comp;
	op = val;
	return true;
}

/** Check if the node is a leaf.
	@return BFFORM (false) for an inner node, the comparator otherwise.
*/
int BFNode::isLeaf() { return cp; }

/** Get the boolean operator of this leaf.
	@return BFLE, BFEQ, BFNE, or BFGE.
*/
int BFNode::getOp() { return op; }

/** Get the place name for this leaf.
	@return Name of the place in the atomic formula.
*/
map<string,int>& BFNode::getPlace() { return p; }

/** Add a successor (son) to a node. Each inner node needs at least one son, AND and
	OR nodes allow for more than one.
	@param son Pointer to the new son. In case of a NOT, it will replace the old one.
	@return If the operation succeeded.
*/
bool BFNode::addSon(BFNode* son) {
	if (cp!=BFFORM) return false;
	if (op==BFNOT) succ.clear();
	succ.insert(son);
	return true;
}

/** Set all sons of a node at once.
	@param sons The set of sons.
	@return If the operation was successful.
*/
bool BFNode::setSons(set<BFNode*> sons) {
	if (cp!=BFFORM) return false;
	if (op==BFNOT && sons.size()>1) return false;
	if (op!=BFAND && op!=BFOR && sons.size()>2) return false;
	succ = sons;
	return true;
}

/** Get all sons of a node at once.
	@return The set of sons.
*/
set<BFNode*> BFNode::getSons() { return succ; }

/** Recursively convert formula to DNF.
	@param If this node's formula has to be negated (false at first entry).
*/
void BFNode::toDNF(bool neg) {
	if (cp!=BFFORM) { // negate atomic formulae
		if (neg) {
			bool split(false);
			map<string,int>::iterator mit;
			switch(cp) {
				case BFLE: cp=BFGE; ++op; break; 
				case BFGE: cp=BFLE; --op; break;
				case BFNE: cp=BFEQ; break;
				case BFEQ: 
					for(mit=p.begin(); mit!=p.end(); ++mit)
						if (mit->second<0) split=true;
					if (op>0 || split) { // OR-split, Sara can't do inequalities
						BFNode* n1 = new BFNode();
						n1->setLeaf(p,(op>1?BFLE:BFEQ),op-1); 
						BFNode* n2 = new BFNode();
						n2->setLeaf(p,BFGE,op+1);
						cp=BFFORM; 
						op=BFOR;
						addSon(n1);
						addSon(n2);
					} else { cp=BFGE; op=1; }
					break;
				default: break;
			}
		} else if (cp==BFNE) {
			bool split(false);
			map<string,int>::iterator mit;
			for(mit=p.begin(); mit!=p.end(); ++mit)
				if (mit->second<0) split=true;
			if (op>0 || split) { // OR-split, Sara can't do inequalities
				BFNode* n1 = new BFNode();
				n1->setLeaf(p,(op>1?BFLE:BFEQ),op-1); 
				BFNode* n2 = new BFNode();
				n2->setLeaf(p,BFGE,op+1);
				cp=BFFORM; 
				op=BFOR;
				addSon(n1);
				addSon(n2);
			} else { cp=BFGE; op=1; }
		}
		return;
	}
	if (op==BFNOT) neg = !neg; // negate subformulae
	if (neg) {
		if (op==BFAND) op=BFOR;
		else if (op==BFOR) op=BFAND;
	}
	set<BFNode*>::iterator sit;
	for(sit=succ.begin(); sit!=succ.end(); ++sit)
	{
		(*sit)->toDNF(neg); // step recursively thru subformulae
	}
/*
	sit = succ.begin(); // eliminate superfluous TRUE AND ... / FALSE OR ...
	while (sit!=succ.end()) {
		if ((*sit)->p.empty() && (*sit)->cp==BFFORM) {
			if ((*sit)->op==1 && op==BFOR) { succ.erase(++sit); continue; } // ... OR FALSE
			if ((*sit)->op==0 && op==BFAND) { succ.erase(++sit); continue; } // ... AND TRUE
			// we have ... AND FALSE / ... OR TRUE and can eliminate the whole subtree
			for(sit=succ.begin(); sit!=succ.end(); ++sit) delete (*sit);
			succ.clear();
			if (op==BFAND) op=1; else op=0; // make this node FALSE/TRUE
			cp = BFEQ;
			return;			
		}
		++sit;
	}
*/

	set<BFNode*> add,sub;
	for(sit=succ.begin(); sit!=succ.end(); ++sit)
	{
		// pull up subformulae with the same operator
		if (op==(*sit)->op && (*sit)->cp==BFFORM) {
			sub.insert(*sit);	
			for(set<BFNode*>::iterator ssit=(*sit)->succ.begin(); ssit!=(*sit)->succ.end(); ++ssit)
				add.insert(*ssit);
			(*sit)->succ.clear();
		}
	}
	for(sit=sub.begin(); sit!=sub.end(); ++sit)
	{
		succ.erase(*sit);
		delete (*sit);
	}
	for(sit=add.begin(); sit!=add.end(); ++sit)
		succ.insert(*sit);

	// next remove NOT as well as AND/OR with a single argument
	if (succ.size()==1) { 
		BFNode* obs(*(succ.begin()));
		succ = obs->succ;
		cp = obs->cp;
		op = obs->op;
		p = obs->p;
		obs->succ.clear();
		obs->p.clear();
		delete obs;
		return;
	}
	if (op!=BFAND) return;
	// at this point we have an AND with all sons either being ORs or atomic
	// now we pull up the ORs (distributive law)
	map<BFNode*,set<BFNode*>::iterator> point; // collects all OR-sons
	map<BFNode*,set<BFNode*>::iterator>::iterator mit;
	set<BFNode*> atoms; // collects all atomic sons of this node
	for(sit=succ.begin(); sit!=succ.end(); ++sit)
		if ((*sit)->cp>BFFORM) atoms.insert(*sit);
		else if ((*sit)->cp==BFFORM)
			point[*sit] = (*sit)->succ.begin();
	if (point.empty()) return; // no OR below this AND, nothing to do
	succ.clear();
	op = BFOR; // make this node an OR clause
	mit = point.begin(); // dummy for entry into the while-loop
	while (mit!=point.end()) {
		// create a new subnode for a new AND clause
		BFNode* node = new BFNode();
		succ.insert(node);
		node->succ = atoms;
		node->op = BFAND;
		node->cp = BFFORM;
		// select one item from each OR
		for (mit=point.begin(); mit!=point.end(); ++mit)
			node->succ.insert(*(mit->second));
		// advance to the next clause
		for (mit=point.begin(); mit!=point.end(); ++mit)
		{ 
			if (++(mit->second)!=mit->first->succ.end()) break;
			mit->second = mit->first->succ.begin();
		}
	}
	for(mit=point.begin(); mit!=point.end(); ++mit)
	{
		mit->first->succ.clear();
		delete mit->first;
	}	
}


/*****************************************************************************\
 Maxis -- Construct maximal services

 Copyright (C) 2010  Jarungjit Parnjai <parnjai@informatik.hu-berlin.de>

 Maxis is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Maxis is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Maxis.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <string>
#include <set>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <iostream>
#include <sstream>
#include <cassert>

#include "dnf.h"
#include "util.h"

using std::string;
using std::set;
using std::map;
using std::stack;
using std::queue;
using std::vector;
using std::cerr;
using std::endl;
using std::pair;
using std::ostream;
using std::ostringstream;

/*!
 * \brief constructor
 *
 * \note  if left or right child is also a conjunction,
 *        it will be merged with this node.
 */
PartialOrder::PartialOrder(Formula * f, set<string> & cl) :
		aFormula(f->clone()), clause(cl) {
}
/*!
 * \brief destructor
 *
 * \note  children will also be deleted.
 */
PartialOrder::~PartialOrder() {
	delete(aFormula);
}

set<string> PartialOrder::get_conjunctive_string() const  { return clause; }

set<PartialOrder *> PartialOrder::get_poset() const { return poset; }

bool PartialOrder::has_child() { return (!poset.empty()); }

int PartialOrder::number_of_clauses() { return (clause.size()); }

void PartialOrder::insert_poset(PartialOrder *p) { poset.insert(p); }

void PartialOrder::remove_poset(PartialOrder *p)
{
	set<PartialOrder *>::iterator findit = poset.find(p);
	if (findit!=poset.end())
		poset.erase(p);
}

Formula * PartialOrder::get_aFormula() { return aFormula->clone(); }

std::ostream & PartialOrder::out(std::ostream & os) {
  os << "(";
  string delim = "";
  for(set<string>::iterator it=clause.begin(); it!=clause.end(); ++it) {
	  os << delim << it->data();
	  delim = ", ";
  }
  os << ") ";
  return os;
}

/// insert an element into DNF structure
bool DNF::search_po_map(int depth, PartialOrder * element) {

	bool found = false;

	map<int, set<PartialOrder *> >::iterator pit;
	pit = po_map.find(depth);

	if (pit != po_map.end()) {
		set<PartialOrder *> element_set = pit->second;
		if (!element_set.empty()) {
			set<PartialOrder *>::iterator eit;
			eit = element_set.find(element);
			if (eit != element_set.end()) {
				found = true;
			}
		}
	}
	return found;
}

/// insert an element into DNF structure
void DNF::insert_po_map(int depth, PartialOrder * element) {
	map<int, set<PartialOrder *> >::iterator pit;
	pit = po_map.find(depth);

	set<PartialOrder *> element_set;
	if (pit != po_map.end()) {
		element_set = pit->second;
		element_set.insert(element);
		po_map[depth] = element_set;
	}
	else {
		element_set.insert(element);
		po_map.insert( pair<int, set<PartialOrder *> > (depth, element_set) );
	}
}


/* looks up search_string in the po structure
 * if the po structure already contains exact search string, then
 *    return the po pointers for both found_element and parent_element
 * otherwise,
 *    return found_element and parent element pointers for placing the search element
 */
pair<set<PartialOrder *>, set<PartialOrder *> > DNF::lookup(PartialOrder * search_element,
												std::ostream  & os ) const {
	typedef pair<set<PartialOrder *>, set<PartialOrder *>::iterator > TraversePair;

	// assert : search_po_map(child) != true
	set<PartialOrder *> parent_elements;
	set<PartialOrder *> child_elements;

	stack< PartialOrder *> parentStack;
	stack< TraversePair > lookupStack;

	parentStack.push(po);

	set<PartialOrder *> neighbors = po->get_poset();
	set<PartialOrder *>::iterator neighbors_iterator = neighbors.begin();
	lookupStack.push(TraversePair (neighbors, neighbors_iterator));

	set<string> temp_string = po->get_conjunctive_string();
	set<string> search_string = search_element->get_conjunctive_string();
	int search_depth = search_element->number_of_clauses();

	os << "======================" << endl;
	os << "search_string = (";
	for (set<string>::iterator sit = search_string.begin(); sit != search_string.end(); ++sit)
		os << *sit << ", ";
	os << ")" << endl;

	bool duplicated = false;
	PartialOrder * temp_element = NULL;
	PartialOrder * temp_parent = po;
	while (!lookupStack.empty() && !duplicated )
	{
		TraversePair tpair = lookupStack.top();
		lookupStack.pop();
		set<PartialOrder *> temp_set = tpair.first;
		set<PartialOrder *>::iterator temp_iterator = tpair.second;

		os << "temp_set = ";
		os << "(";
		for (set<PartialOrder *>::iterator tsit = temp_set.begin(); tsit != temp_set.end(); ++tsit)
			(*tsit)->out(os) << ", ";
		os << ")" << endl; // << "temp_iterator = ";

		while (*temp_iterator != *temp_set.end() )
		{
			temp_element = * temp_iterator;
			temp_string = temp_element->get_conjunctive_string();
			int temp_depth = temp_element->number_of_clauses();

			pair<bool, bool> result = util::setRelation(temp_string, search_string);
			bool superset = result.first;
			bool subset = result.second;
			if (superset && subset) {
				// found a duplicate conjunction of literals
				os << "duplicated!" <<endl;
				duplicated = true;
				break;
			}
			if (!superset && subset) {
				// temp_string is a subset of search_string,
				os << "temp_string is a subset of search_string" << endl;
				if (search_depth-temp_depth==1) {
					parent_elements.insert( temp_element );
//					parentStack.push(temp_element);
//					temp_element = NULL;
					temp_iterator++;
					continue;
				}

				// otherwise, go deeper
				parentStack.push( temp_element );
				// push current lookup entry back into lookupStack
				lookupStack.push( TraversePair (temp_set, temp_iterator) );

				// push a children into lookupStack
				set<PartialOrder *> children = temp_element->get_poset();
				if (!children.empty()) {
					set<PartialOrder *>::iterator children_iterator = children.begin();
					lookupStack.push(TraversePair (children, children_iterator));
				}
				temp_parent = temp_element;
				break;
			}
			if (superset && !subset) {
				// temp_string is a superset of search_string,
				os << "temp_string is a superset of search_string" << endl;
				if (temp_depth-search_depth==1) {
					child_elements.insert( temp_element );
//					parentStack.push(temp_element);
					temp_iterator++;
					continue;
//					break;
				}
				break;
			}
			if (!superset && !subset) {
				// wrong path, search next neighbors
				os << "search next neighbor" << endl;
				temp_iterator++;
				continue;
			}
		} /* end while (*temp_iterator != *temp_set.end() ) */

		// back track
		if (!parentStack.empty()) {
			parentStack.pop();
		}
	}

	if (duplicated) {
		parent_elements.clear();
		child_elements.clear();
	} else {
		if (parent_elements.empty()) {
			parent_elements.insert( temp_parent );
		}
	}

	return  pair<set<PartialOrder *>, set<PartialOrder *> > (parent_elements, child_elements);
}

void DNF::update_DNF_structure(PartialOrder * element, set<string> element_clause, std::ostream & os) {

	// if it is a duplicated element, do nothing
	if (search_po_map(element_clause.size(), element)) {
		return;
	}

	pair<set<PartialOrder *>, set<PartialOrder *> > results = lookup(element, os);
	set<PartialOrder *> parent_results = results.first;
	set<PartialOrder *> child_results = results.second;

	set<PartialOrder *>::iterator peit = parent_results.begin();

	os << "parents :: ";
	for (set<PartialOrder *>::iterator peit = parent_results.begin(); peit != parent_results.end(); peit++) {
		PartialOrder * parent = * peit;
		parent->insert_poset( element );
		 parent->out(os) << ", ";
	}
	os << "children :: ";
	for (set<PartialOrder *>::iterator ceit = child_results.begin(); ceit != child_results.end(); ceit++) {
		PartialOrder * child = * ceit;
		element->insert_poset( child );
		 child->out(os) << ", ";
	}

	// remove redundant links
	for (set<PartialOrder *>::iterator pit = parent_results.begin(); pit != parent_results.end(); pit++) {
		PartialOrder * parent = * pit;
		set<PartialOrder *> parent_poset = parent->get_poset();
		set<PartialOrder *>::iterator parent_it;
		os << endl << "   check parent @"; parent->out(os) << "@ (" << parent_poset.size() << ") ";
		for (set<PartialOrder *>::iterator cit = child_results.begin(); cit != child_results.end(); cit++) {
			PartialOrder * child = * cit;
			parent_it = parent_poset.find(child);
			os << endl << "   check child @"; child->out(os) << "@";
			if (parent_it != parent_poset.end()) {
				parent->remove_poset(child);
				os << endl << "   remove redundant link from @";
				parent->out(os) << " to @";
				child->out(os) << endl;
			}
		}
	}

	os << endl;

//	os << "inserting... :: ";
//	element->out(os) << " (" << element->number_of_clauses() << ")" << endl;

	insert_po_map(element->number_of_clauses(), element);
}


/// constructor
DNF::DNF(Formula * f, set<string> & literals, std::ostream & os) :
		//minimal(false), maximal(false),
		liberal_flag(normal), originalFormula(f->clone()), all_literals(literals), po(NULL)  {

	// root or partial order structure is always Literal "false"
	string str = "false";
	Literal * l = new Literal(& str);
	// l_clause of root node is always a "false" string
	set<string> l_clause;
	l_clause.insert(str);
	po = new PartialOrder(static_cast<Formula *> (l), l_clause);

	// the root always has depth = 0
	insert_po_map(0, po);

	// assert : originalFormula is in DNF
	os << "original formula = ";
	originalFormula->out(os) << endl;

	// get all children elements of an original Formula
	set<string> child_clause;
	Conjunction * c = dynamic_cast<Conjunction *> (originalFormula);
	if (c != NULL) {
		// assert : originalFormula contains only 1 clause of Conjunction in
		child_clause = conjunction_string_literals(c);

		if (!child_clause.empty()) {
			PartialOrder * child = new PartialOrder(f, child_clause);
			os << "update_DNF_stustructure :";  child->out(os) << " (" << child->number_of_clauses() << ")" << endl;
			update_DNF_structure(child, child_clause, os);

			child_clause.clear();
			disjuncts.insert(originalFormula);
		}
	} else {
		set<Formula *> children = originalFormula->getElements();

		for(set<Formula *>::iterator it=children.begin(); it!=children.end(); ++it) {

			// assert : each children is either Literal or Conjunction
			Formula * f = * it;
			Literal * l = dynamic_cast<Literal *> (f);
			if (l != NULL) {
				child_clause.insert( l->getString() );
			} else {
				c = dynamic_cast<Conjunction *> (f);
				if (c != NULL) {
					child_clause = conjunction_string_literals(c);
				}
			}

			if (!child_clause.empty()) {
				// alternatively, one can check if it is a multiple conjunction
				PartialOrder * child = new PartialOrder(f, child_clause);

				os << "update_DNF_stustructure :";  child->out(os) << " (" << child->number_of_clauses() << ")" << endl;
				update_DNF_structure(child, child_clause, os);

				child_clause.clear();
				disjuncts.insert(f);
			}
		}  /* end for each iterator */
	}

}

// destructor
DNF::~DNF(){
	//traverse_delete(po);
	set<PartialOrder *> first_children = po->get_poset();
	set<PartialOrder *> delete_nodes;

	for(set<PartialOrder *>::iterator it=first_children.begin(); it!=first_children.end(); ++it) {
		Formula * aFormula = (*it)-> get_aFormula();
		set<PartialOrder *> grandChildren = (*it)->get_poset();
		for(set<PartialOrder *>::iterator git=grandChildren.begin(); git!=grandChildren.end(); ++git) {
			(*it)->remove_poset( *git );
			delete_nodes.insert( *git );
		}
	}

	delete_DNF_structure( delete_nodes );
	po_map.clear();
	disjuncts.clear();
	all_literals.clear();
}

void DNF::delete_DNF_structure(set<PartialOrder *> delete_nodes) {
	for(set<PartialOrder *>::iterator it=delete_nodes.begin(); it!=delete_nodes.end(); ++it) {
	    delete(*it);
	}
}

/*
void DNF::traverse_delete(PartialOrder *node)
{
	set<PartialOrder *> children = node->get_poset();
	for(set<PartialOrder *>::iterator it=children.begin(); it!=children.end(); ++it) {
		traverse_delete( *it );
	}
    delete(node);
}
*/

set<string> DNF::conjunction_string_literals(const Conjunction * c) const {
	set<string> clause;

	bool sat = false;
	bool unsat = false;

	set<Formula *> children = c->getElements();
	for(set<Formula *>::iterator it=children.begin(); it!=children.end(); ++it)  {
		Literal * l = dynamic_cast<Literal *> (* it);
		if (l != NULL) {
			string literal = l->getString();
			clause.insert(literal);
			if (literal == "true") {
				sat = true;
				continue;
			}
			if (literal == "false") {
				unsat = true;
				break;
			}
		}
	}

	// (a * false) = false, (false * true) = false, (a * true) = a
	// these normalization requires an update of DNF structure as depth is changed
	if (clause.size() > 1) {
		if (unsat) {
			clause.clear();
			clause.insert("false");
		} else {
			if (sat) {
				clause.clear();
				clause.insert("true");
			}
		}
	}

	return clause;
}

Formula * normalize_conjunct(set<string> clause) {
	set<Formula *> conjunct_clauses;
	bool assign = false;
	set<string>::iterator cit = clause.begin();
	while (cit != clause.end()) {
		if ((*cit) == "false") {
			assign = true;
			break;
		}
		if ((*cit) != "true") {
			Literal * l = new Literal(*cit);
			conjunct_clauses.insert( static_cast<Formula *>(l) );
		}
		cit++;
	}

	Formula * aFormula;
	if (assign) {
		aFormula = static_cast<Formula *>( new Literal("false") );
	} else {
		if (conjunct_clauses.empty()) {
			Literal * l = new Literal("true");
			conjunct_clauses.insert( static_cast<Formula *>(l) );
		}
		aFormula = static_cast<Formula *>( new Conjunction(conjunct_clauses) );
	}

 	return aFormula;
}

/// construct a minimal DNF,  maintain only the first level children
void DNF::minimize(std::ostream & os) {

	disjuncts.clear();

	set<PartialOrder *> children = po->get_poset();
    os << "ps = ";
	po->out(os) << endl;
	os << "children = (";

	set<PartialOrder *> delete_nodes;
	for(set<PartialOrder *>::iterator it=children.begin(); it!=children.end(); ++it) {
		(*it)->out(os) << ", ";
		Formula * aFormula = (*it)-> get_aFormula();

		disjuncts.insert(aFormula);
		set<PartialOrder *> grandChildren = (*it)->get_poset();
		for(set<PartialOrder *>::iterator git=grandChildren.begin(); git!=grandChildren.end(); ++git) {
			os << "git = ";
			(*git)->out(os) << endl;

			(*it)->remove_poset( *git );
			delete_nodes.insert( *git );
		}
	}
	delete_DNF_structure( delete_nodes );
	os << ")" << endl;
}


void DNF::fill_DNF_structure(PartialOrder * parent, ostream & os)
{
	os << "for each ";
	(parent)->out(os) << " do : " << endl;
	set<string> element_set = parent->get_conjunctive_string();
	set<string>::iterator sit = element_set.find("true");
	if (sit == element_set.end())
		return;
	sit = element_set.find("false");
	if (sit == element_set.end())
		return;

	set<string> diff_elements = util::setDifference( all_literals, element_set );


	set<string> parent_elements = parent->get_conjunctive_string();
	Formula * parent_formula = parent->get_aFormula();

	set<Formula *> conjuncts;
	set<string>::iterator dit = diff_elements.begin();

	os << "diff = ";
	while( dit != diff_elements.end() ) {
		os << (*dit) << ", ";

		Literal * l = new Literal(*dit);

		conjuncts.insert( parent->get_aFormula() );
		conjuncts.insert( static_cast<Formula *> (l) );
		Conjunction * aConjunct = new Conjunction( conjuncts );
		set<string> temp_elements = parent_elements;
		temp_elements.insert( *dit );
		PartialOrder * conjunct_po = new PartialOrder( static_cast<Formula *>(aConjunct), temp_elements );

		conjunct_po->out(os) << ", " << endl;

		set<string> conjunct_clause = conjunct_po->get_conjunctive_string();
		update_DNF_structure(conjunct_po, conjunct_clause, os);

		disjuncts.insert( conjunct_po->get_aFormula() );

		temp_elements.clear();
		conjuncts.clear();
		++dit;
/*
		int depth = conjunct_po->number_of_clauses();
		if (depth < all_literals.size()) {
			os << "next depth = " << depth << endl;
			fill_DNF_structure(conjunct_po, os);
		}
*/
	}
	os << endl;

}

/// construct a complete DNF
void DNF::maximize(std::ostream & os)
{
	disjuncts.clear();

	set<string>::iterator it = all_literals.begin();
	set<PartialOrder *> poset = po->get_poset();
	for(set<PartialOrder *>::iterator it = poset.begin(); it != poset.end(); ++it) {
		Formula * f = (*it)->get_aFormula();
		disjuncts.insert( f );

		fill_DNF_structure(*it, os);
	}
}


/// is the DNF structure minimal
bool DNF::is_maximal() const {  return (liberal_flag == maximal); }

// is the DNF structure maximal
bool DNF::is_minimal() const { return (liberal_flag == minimal);  }

/* write DNF output
 */
Disjunction * DNF::get_DNF()
{
 	Disjunction * aFormula = new Disjunction(disjuncts);
    return aFormula;
}

/* write to stream
 */
ostream & DNF::out(ostream & os)
{
	// po->out(os);
	std::ostringstream oss;
	os << "[" << traverse_output(oss, po).str() << "]" << endl;
    return os;
}

/* preorder traversal
 */
ostringstream & DNF::traverse_output(ostringstream & oss, PartialOrder * node)
{
	(node)->out(oss) << ", ";
    set<PartialOrder *> child = node->get_poset();
	for(set<PartialOrder *>::iterator cit=child.begin(); cit!=child.end(); ++cit) {
		traverse_output(oss, *cit);
	}
	return oss;
}

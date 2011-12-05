/****************************************************************************\
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



/// constructor
Choice::Choice(Formula *f, std::set<std::string> & literals, liberal & flag, std::ostream & os, dbg_flag & dbg) :
	dnf(f->clone()),  allow_literals(literals), liberal_flag(flag), satisfy_flag(possible), debug(dbg) {

	// include final as an allowed literal
	allow_literals.insert("final");

	if (debug == on) {
		os << endl << "processing DNF : ";	   dnf->out(os);	   os<<endl;
		os << "allow literals : (";
		for(std::set<std::string>::iterator ait = allow_literals.begin(); ait != allow_literals.end(); ++ait)
			os << *ait << ",";
		os << ")" << endl;
	}

	std::set<std::string> child_clause;
    bool single_child = false;
	Conjunction * c = dynamic_cast<Conjunction *> (dnf);
	if (c != NULL) {
		// collect all literal strings in conjunction c
		child_clause = conjunction_string_literals(c);
		single_child = true;
	} else {

		Literal * l = dynamic_cast<Literal *> (dnf);
		if (l != NULL) {
			std::string literal = l->getString();
			// check if literal is in allowed
			std::set<std::string>::iterator it	= allow_literals.find(literal);
			if (it != allow_literals.end()) { // if allowed
				child_clause.insert(literal);
				single_child = true;
			}
		}
	}

	if (single_child) {

		std::set< std::set<std::string> > str_set_holder;
		str_set_holder.insert( child_clause );
		all_choices[child_clause.size()] = str_set_holder;

	} else  { // if not single_child)

		// assert : f is of dnf form
		// get each child element of dnf
		std::set<Formula *> children = dnf->getElements();

		std::set<std::string> child_clause;
		for (std::set<Formula *>::iterator cit = children.begin(); cit != children.end(); ++cit) {

			Conjunction * c = dynamic_cast<Conjunction *> (* cit);
			if (c != NULL) {
				if (debug == on) { os << "Conjunction : ";			c->out(os); 		os << endl; }

				// collect all literal strings in conjunction c
				child_clause = conjunction_string_literals(c);

			} else {

				Literal * l = dynamic_cast<Literal *> (* cit);
				if (l != NULL) {
					if (debug == on) { os << "Literal : ";			l->out(os); 		os << endl; }
					std::string literal = l->getString();
					if (literal == "true") {   // if one disjunct is true ,then dnf is always satisfied
						satisfy_flag = always;
						if (debug == on) { os << "set satisfy_flag to true" << endl; }

						std::set<std::set<std::string> > true_choice;
						for(std::set<std::string>::iterator alit = allow_literals.begin();
								alit != allow_literals.end(); ++alit) {
							std::set<std::string> str_holder;
							str_holder.insert( *alit );
							true_choice.insert( str_holder );
						}
						int index = 1;
						all_choices.clear();
						all_choices[index] = true_choice;
						break;
					}
					child_clause.insert(literal);
				}
			}

			// if there is a new choice to insert,
			if (!child_clause.empty()) {
				// check if there exists the set with the same number of elements
				int index = child_clause.size();
				std::map<int, std::set<std::set<std::string> > >::iterator  it = all_choices.find(index);

				if (it != all_choices.end()) {  // if there exists a choice of the same size
					std::set< std::set<std::string> > str_set_holder = it->second;
					// search a set that contains the same choice
					bool foundit = false;
					for(std::set< std::set<std::string> >::iterator sit = str_set_holder.begin(); sit != str_set_holder.end(); ++sit) {
						if (foundit = (child_clause == *sit)) {
							break;
						}
					}
					if (!foundit) { // none of the choice found
						str_set_holder.insert( child_clause );
						all_choices[it->first] = str_set_holder;
					}
				} else { // none of choices with the same size exists
					std::set< std::set<std::string> > str_set_holder;
					str_set_holder.insert( child_clause );
					all_choices[child_clause.size()] = str_set_holder;
				}
			}
			child_clause.clear();
		}
	}   // end if not single_child

}

// destructor
Choice::~Choice(){
	all_choices.clear();
	allow_literals.clear();
}

/// construct a minimal DNF,  maintain only the first level children
void Choice::minimize(std::ostream & os) {
	liberal_flag = minimal;

	int current_level = allow_literals.size()+1;
	int last_level = current_level;
	do {
		current_level--;
		std::map<int, std::set< std::set<std::string> > >::iterator lit = all_choices.find(current_level);
		if (lit != all_choices.end()) {
 			last_level = current_level;
			break;
		}
	} while(current_level > 0);

	int first_level = last_level;
	current_level = 0;
	do {
		current_level++;
		std::map<int, std::set< std::set<std::string> > >::iterator lit = all_choices.find(current_level);
		if (lit != all_choices.end()) {
 			first_level = current_level;
			break;
		}
	} while( current_level < last_level);

	if (debug == on) { os << "minimize : first = " << first_level << " last = " << last_level << endl; }


	// otherwise check the choices for removal
	current_level = last_level;
	int prev_level = current_level;
	if (first_level < last_level) {   // remove only if there is more than one level

		do {  // while first_level < current_level

			if (debug == on) { os << "         :: first = " << first_level << " current = " << current_level << endl; }

			std::map<int, std::set< std::set<std::string> > >::iterator lit = all_choices.find(current_level);
			if (lit != all_choices.end()) { // if found a choice of the given size
				//  get all choices of the current level
			  	std::set<std::set<std::string> > current_level_choices = lit->second;

			    std::map<int, std::set< std::set<std::string> > >::iterator plit = all_choices.find(--prev_level);
			    if (plit != all_choices.end()) {   // there exists choices at an immediate prev level

			    	std::set< std::set<std::string> > prev_level_choices =  plit->second;

					for(std::set<std::set<std::string> >::iterator prit = prev_level_choices.begin();
							prit != prev_level_choices.end(); ++prit)  {  // for each choice in the prev level

					  	std::set<std::string> prev_ch = * prit;   // get the prev choice
					  	for(std::set<std::string>::iterator prev_ptr = prev_ch.begin();
					  			prev_ptr != prev_ch.end(); ++prev_ptr) { // for each of the prev choice

					  		// check if there is a curr_ch that is a subset of the prev_ch
						  	bool subset = true;
						  	std::set<std::set<std::string> > curr;
						  	std::copy( current_level_choices.begin(), current_level_choices.end(),
						  			std::inserter( curr, curr.begin() ) );

							for(std::set<std::set<std::string> >::iterator cit = curr.begin();
									cit != curr.end(); ++cit)  {  // for each choice in the current level
								std::set<std::string>  curr_ch = *cit;
								std::set<std::string>::iterator pptr = prev_ptr;   // copy prev_ptr
								for(std::set<std::string>::iterator curr_ptr = curr_ch.begin(); curr_ptr != curr_ch.end(); ++curr_ptr) {
									if (debug == on) { os << "         ::: prev_ptr = " << *pptr << " curr_ptr = " << *curr_ptr << endl;}

									if (*curr_ptr != *pptr) {
										if (debug == on) { os << "         ::: subset = false" << endl; }
										subset = false;
										break;
									}
									else {
										if (debug == on) { os << "         ::: subset = true"  << endl; }
										if (++pptr == prev_ch.end()) {
											break;
										}
										if (debug == on) {	os << "         ::: ++pptr"  << endl; }
									}
								}
								if (subset) {  // if prev_ch is a subset of curr_ch
									// delete the curr_ch
									current_level_choices.erase(curr_ch);
								}
								curr_ch.clear();
							}  // end for each choice in the current level

							curr.clear();
					  	}  // for each of the prev choice

					}    // end for each choice in the prev level
			    }   // end if there exists choices at an immediate prev level

			    // update the current level choices back
				all_choices[current_level--] = current_level_choices;

			}   // end if found a choice of the given size

		} while( first_level < current_level );
	}

/*
	// if the formula is always satisfied, then generate the minimum set of choices
	if (satisfy_flag == always) {
		os << "satisfy always................ " << endl;
		current_level = 1;
	  	std::set< std::set<std::string> > ch;
	   	for(set<string>::iterator it=allow_literals.begin(); it!=allow_literals.end(); ++it) {
		  	std::set<std::string> temp_ch;
	   	    temp_ch.insert( *it );
	   	    ch.insert(temp_ch);
	   	}
	   	all_choices[current_level] = ch;
	   	for(current_level=2; current_level<=allow_literals.size(); current_level++) {
			std::map<int, std::set< std::set<std::string> > >::iterator cit = all_choices.find(current_level);
			if (cit != all_choices.end())
  	   		   all_choices.erase(current_level);
	   	}
	   	return;
	}
*/

}

/// construct a complete DNF
void Choice::maximize(std::ostream & os) {

	if (debug == on) {
		os << "-----------------------" << endl;
		os << "maximize................ " << endl;
		os << "allow literals :  " << allow_literals.size() << ": (";
		for(std::set<string>::iterator chi=allow_literals.begin(); chi != allow_literals.end(); ++chi) {
			os << *chi <<",";
		}
		os << ")" << endl;
	}

	liberal_flag = maximal;
	int level =0;
	int first_level = 0;

	do {
		level++;
		std::map<int, std::set< std::set<std::string> > >::iterator lit = all_choices.find(level);
		if (lit != all_choices.end()) {
			first_level = level;
			break;
		}
	} while(level < allow_literals.size());

	if (debug == on) {
		os << "first level : " << first_level << endl;
		os << "allow literals :  ";
			for(std::set<string>::iterator lsi = allow_literals.begin(); lsi != allow_literals.end(); ++lsi) {
				os << *lsi <<",";
			}
			os << endl;
	}

	level = first_level;
	int next_level = level;
	do {
		std::map<int, std::set< std::set<std::string> > >::iterator lit = all_choices.find(level);
		if (lit != all_choices.end()) { // if found a choice of the given size

    		std::set< std::set<string> > level_choices = all_choices[level];

    		if (debug == on) {
    			os << "   * current_level :  " << level; // << endl;
    			for(std::set< std::set<string> >::iterator i = level_choices.begin();
    					i != level_choices.end(); ++i) {
    				os << "(";
    				std::set<string>  tmp = * i;
    				for(std::set<string>::iterator si = tmp.begin(); si != tmp.end(); ++si) {
    					os << *si <<",";
    				}
    				os << ")";
				}
    			os << endl;
    		}

		    std::map<int, std::set< std::set<std::string> > >::iterator nlit = all_choices.find(++next_level);
	    	std::set< std::set<std::string> > next_level_choices;
		    if (nlit != all_choices.end()) {   // there exists choices at an immediate next level
		    	next_level_choices = nlit->second;
		    }

//			os << "     next_level :  " << next_level  << endl;

			for(std::set<std::set<std::string> >::iterator cit = level_choices.begin();  cit != level_choices.end(); ++cit) {
				// for each choice in the current level
			  	std::set<std::string> ch = * cit;


	    		std::set<std::string> diff;
			  	// get difference of all literals with currently level choice
			   	std::set_difference(allow_literals.begin(), allow_literals.end(), ch.begin(), ch.end(),
			    	            std::inserter( diff, diff.begin() ) );

			   	if (debug == on) {
			   		os << "     diff size :  " << diff.size() << ": (";
			   		for(std::set<string>::iterator chi=diff.begin(); chi != diff.end(); ++chi) {
	        			os << *chi <<",";
			   		}
			   		os << ")" << endl;

			   		os << "     processing diff : ";
			   	}
	    		// for additional literal in the diff set
			   	for(set<string>::iterator dit=diff.begin(); dit!=diff.end(); ++dit) {
			   	    //  insert each additional literal into next level
				  	std::set<std::string> temp_ch = ch;
				  	if (debug == on) {	os << *dit <<","; }
			   	    temp_ch.insert( *dit );
				    next_level_choices.insert( temp_ch );
			   	}
			   	if (debug == on) { os << endl; }
	    		all_choices[next_level] = next_level_choices;
			}
		}
		level++;
	} while( level <= allow_literals.size() );

	if (debug == on) { os << "-----------------------" << endl; }

}

// is the DNF structure minimal
bool Choice::is_maximal() const {  return (liberal_flag == maximal); }

// is the DNF structure maximal
bool Choice::is_minimal() const { return (liberal_flag == minimal);  }

// is always satisfied
bool Choice::is_always_satisfied() const { return (satisfy_flag == always); }

// contains a choice
bool Choice::contains(std::set<std::string> ch) { //, std::ostream & os)  {
	int index = ch.size();
	bool found = true;

	std::map<int, std::set<std::set<std::string> > >::iterator it = all_choices.find(index);
	if (it != all_choices.end())  {  // search only if all choices contains set of string of the same size
		std::set<std::set<std::string> > ch_holder = it->second;
		for(std::set< std::set<std::string> > ::iterator hit= ch_holder.begin();
				hit!= ch_holder.end(); ++hit)  {

			std::set<std::string> curr_ch = *hit;
/*
			if (debug == on) {
		   		os << "     compare with : (";
		   		for(std::set<string>::iterator chi= curr_ch.begin(); chi != curr_ch.end(); ++chi) {
        			os << *chi <<",";
		   		}
		   		os << ") ";
		   	}
*/
			std::set<std::string>::iterator ptr = ch.begin();
			for(std::set<std::string>::iterator curr_ptr = curr_ch.begin(); curr_ptr != curr_ch.end(); ++curr_ptr) {
				if (*curr_ptr != *ptr) {
 //       			os << ": [" << *curr_ptr <<"] ";
					found = false;	break;	}
				else {
					if (++ptr == ch.end()) {
						break;
					}
				}
			}
			if (found) { break; }
		}
	}

//	if (debug == on) { os << (found ? ": found! " : ": final not found!") <<endl; }
	return found;
}

// write to stream
std::ostream & Choice::out(std::ostream & os) {
	os << "..." << endl;
    return os;
}

/// get all choices
std::set< std::set<std::string> > Choice::get_all_choices()  {
	std::set< std::set<std::string> > ch;
	for(std::map<int, std::set< std::set<std::string> > >::iterator it= all_choices.begin();
			it!= all_choices.end(); ++it)  {
		std::set<std::set<std::string> > ch_holder = it->second;
		for(std::set< std::set<std::string> > ::iterator hit= ch_holder.begin();
				hit!= ch_holder.end(); ++hit)  {
			ch.insert(*hit);
		}
	}
	return ch;
}


std::set<std::string> Choice::conjunction_string_literals(const Conjunction * c)  {

	std::set<std::string> clause;
	bool t_flag = false;
	bool f_flag = false;

	std::set<Formula *> children = c->getElements();
	for(std::set<Formula *>::iterator it=children.begin(); it!=children.end(); ++it)  {
		Literal * l = dynamic_cast<Literal *> (* it);
		if (l != NULL) {
			std::string literal = l->getString();

			// check if literal is in allowed
			std::set<std::string>::iterator it	= allow_literals.find(literal);
			if (it == allow_literals.end()) {
				// if not allowed and it is neither true nor false, check the next one
				if (literal != "true" && literal != "true")
				    break;
			}

			if (literal == "true") {
				t_flag = true;
				continue;
			}
			clause.insert(literal); 		// (a * b * true) = a * b
			if (literal == "false") {       // (a * b * false) = false
				f_flag = true;
				break;
			}
		}
	}

	if (f_flag) {  // conjunction contains one "false" is equivalent to "false"
		clause.clear();
		clause.insert("false");
		satisfy_flag = never;
	} else  {
		if (clause.size() == 0) {    // in case there is no updated clause
			// in case of (true) = true
			if (t_flag) {  // no other clause but "true"
				clause.clear();
				clause.insert("true");
				satisfy_flag = always;
			}
		}
	}

	return clause;
}


/*
 * * outdated classes
//
 * \brief constructor
 *
 * \note  if left or right child is also a conjunction,
 *        it will be merged with this node.
///
PartialOrder::PartialOrder(Formula * f, set<string> & cl) :
		aFormula(f->clone()), clause(cl) {
}
//
 * \brief destructor
 *
 * \note  children will also be deleted.
//
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

	map<int, set<PartialOrder *> >::iterator pit = po_map.find(depth);

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

//	if (po_map == NULL) {   // if first time for everything
//		set<PartialOrder *> element_set;
//		element_set.insert(element);
//		po_map[depth] = element;
//	}
//	else {
		map<int, set<PartialOrder *> >::iterator pit;
	 	pit = po_map.find(depth);

		set<PartialOrder *> element_set;
		if (pit != po_map.end()) {   // if there is already a set of element(s) at depth
			element_set = pit->second;
			element_set.insert(element);
			po_map[depth] = element_set;
		}
		else {  // if there is no element set at depth
			element_set.insert(element);
			po_map.insert( pair<int, set<PartialOrder *> > (depth, element_set) );
		}
//	}

}


// looks up search_string in the po structure
 * if the po structure already contains exact search string, then
 *    return the po pointers for both found_element and parent_element
 * otherwise,
 *    return found_element and parent element pointers for placing the search element
 //
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
		} // end while (*temp_iterator != *temp_set.end() )

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
// purpose : create the representation of Disjunctive Boolean Formula
//   input : (1) original formula * f, (2) set of all literal string
//
DNF::DNF(Formula * f, set<string> & literals, std::ostream & os) :
	    liberal_flag(normal), originalFormula(f->clone()), all_literals(literals) { //, po_map(NULL)  {


	// If a DNF original formula contains 'true' as one disjunct,
	//    then  the root or partial order structure is set with Literal "true"
	// Otherwise, the root or partial order structure is set with Literal "false"
	Literal * l = new Literal(true);

	os << "dnf formula = ";
	f->dnf()->out(os) << endl;



//	const Operator * dnf = static_cast<const Operator *> (f->dnf());
//	const Literal * lit = static_cast<Literal *>(l);
//
//   if(! dnf->isIn(lit))
//	    l = new Literal(false);


	set<string> l_clause;
	l_clause.insert(l->getString());
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
		}  // end for each iterator
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


//void DNF::traverse_delete(PartialOrder *node)
//{
//	set<PartialOrder *> children = node->get_poset();
//	for(set<PartialOrder *>::iterator it=children.begin(); it!=children.end(); ++it) {
//		traverse_delete( *it );
//	}
//   delete(node);
//}

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
//
//		int depth = conjunct_po->number_of_clauses();
//		if (depth < all_literals.size()) {
//			os << "next depth = " << depth << endl;
//			fill_DNF_structure(conjunct_po, os);
//		}
//
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

//* write DNF output
Disjunction * DNF::get_DNF()
{
 	Disjunction * aFormula = new Disjunction(disjuncts);
    return aFormula;
}

// write to stream
ostream & DNF::out(ostream & os)
{
	// po->out(os);
	std::ostringstream oss;
	os << "[" << traverse_output(oss, po).str() << "]" << endl;
    return os;
}

// preorder traversal
ostringstream & DNF::traverse_output(ostringstream & oss, PartialOrder * node)
{
	(node)->out(oss) << ", ";
    set<PartialOrder *> child = node->get_poset();
	for(set<PartialOrder *>::iterator cit=child.begin(); cit!=child.end(); ++cit) {
		traverse_output(oss, *cit);
	}
	return oss;
}
*/

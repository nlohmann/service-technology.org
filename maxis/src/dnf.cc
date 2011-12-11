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
	for(std::map<int, std::set<std::set<std::string> > >::iterator it = all_choices.begin();
			it != all_choices.end(); ++it) {
		std::set<std::set<std::string> > level = it->second;
		os << "index " << it->first << ": [ " << endl;
		for(std::set<std::set<std::string> >::iterator lit= level.begin();
				lit != level.end(); ++lit) {
			std::set<std::string> ch = *lit;
			os <<  "[ ";
			bool first = true;
			for(std::set<std::string>::iterator slit = ch.begin();
					slit != ch.end(); ++slit) {
				if (!first) {
					os << ", ";
				}
				os << *slit;
				first = false;
			}
			os <<  " ]" << endl;
		}
		os << " ]" << endl;
	}
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

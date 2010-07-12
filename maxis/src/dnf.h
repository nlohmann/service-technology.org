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

//#ifndef DNF_H_
//#define DNF_H_

#include <string>
#include <set>
#include <map>
#include <stack>

#include "formula.h"

enum liberal { normal, minimal, maximal };

class PartialOrder {
private:
  // either literal or conjunction
  Formula * aFormula;
  // a set of all dnf clauses
  std::set<std::string> clause;
  // a set of all partial order references
  std::set<PartialOrder *> poset;

public:
  // constructor
  PartialOrder(Formula *, std::set<std::string> & );
  // destructor
  ~PartialOrder();
   /// get all dnf clauses
   std::set<std::string> get_conjunctive_string() const;
   /// get partial order set
   std::set<PartialOrder *> get_poset() const;
   // get a formula
   Formula * get_aFormula();
   /// has a child
   bool has_child();
   /// number of all dnf clauses
   int number_of_clauses();
   /// insert a partial order element
   void insert_poset(PartialOrder *);
   /// remove a partial order element
   void remove_poset(PartialOrder *);
   // write to stream
   std::ostream & out(std::ostream &);
};

class DNF {
private:
	Formula * originalFormula;
	std::set<Formula *> disjuncts;
	std::set<std::string> all_literals;
	std::map<int, std::set<PartialOrder *> > po_map;
	PartialOrder * po;
	liberal liberal_flag;

	bool search_po_map(int, PartialOrder *);
    void insert_po_map(int, PartialOrder *);
    void fill_DNF_structure(PartialOrder *, std::ostream &);
    void update_DNF_structure(PartialOrder *, std::set<std::string>, std::ostream &);
    void delete_DNF_structure(std::set<PartialOrder *>);
//    void traverse_delete(PartialOrder *);
    Disjunction * normalize_conjuct(std::set<std::string> conjuncts);
 	std::ostringstream & traverse_output(std::ostringstream &, PartialOrder *);
	std::set<std::string> conjunction_string_literals(const Conjunction *) const;
	std::pair< std::set<PartialOrder *>, std::set<PartialOrder *> > lookup(PartialOrder *, std::ostream &) const;

public:
    // constructor
    DNF(Formula *, std::set<std::string> &, std::ostream &);
    // destructor
    ~DNF();
	/// construct a minimal DNF
	void minimize(std::ostream &);
	/// construct a complete DNF
	void maximize(std::ostream & os);
	/// is the partial order set of DNF minimal
	bool is_minimal() const;
	/// is the partial order set of DNF maximal
	bool is_maximal() const;
    // write DNF output
	Disjunction * get_DNF();
    // write to stream
	std::ostream & out(std::ostream &);
};

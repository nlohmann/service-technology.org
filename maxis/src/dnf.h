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

enum dbg_flag { on, off };
enum liberal { normal, minimal, maximal };
enum satisfy { possible, always, never };


class Choice {
private:
	Formula * dnf;
	std::set<std::string> allow_literals;
	std::map<int, std::set<std::set<std::string> > > all_choices;
	liberal liberal_flag;
	satisfy satisfy_flag;
	dbg_flag debug;

	std::set<std::string> conjunction_string_literals(const Conjunction *);

public:
    // constructor : formula, all_literals, liberal_flag
	Choice(Formula *, std::set<std::string> &, liberal &, std::ostream &, dbg_flag & dbg);
   // destructor
    ~Choice();
	// construct a minimal DNF
	void minimize(std::ostream &);
	// construct a complete DNF
	void maximize(std::ostream & os);
	// is the partial order set of DNF minimal
	bool is_minimal() const;
	// is the partial order set of DNF maximal
	bool is_maximal() const;
	// is always satisfied
	bool is_always_satisfied() const;
	// contains a choice
	bool contains(std::set<std::string>);
	/// get all choices
	std::set<std::set<std::string> > get_all_choices();
	// write to stream
	std::ostream & out(std::ostream &);
};


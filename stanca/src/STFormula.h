/*****************************************************************************\
 Stanca --  Siphon/Trap Analysis and siphon-trap property Checking using the sAtisfiability solver MINISAT
 
 Copyright (c) 2010 Olivia Oanea
 
 Stanca is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.
 
 Stanca is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.
 
 You should have received a copy of the GNU Affero General Public License
 along with Stanca.  If not, see <http://www.gnu.org/licenses/>.
 \*****************************************************************************/

#include "pnapi.h"
#include <sstream>
#include "System.h"
#include "ParseUtils.h"
#include "Options.h"
#include "Dimacs.h"
#include "Solver.h"
#include "SimpSolver.h"

using namespace std;
using namespace Minisat;


using std::set;
using std::vector;

struct PlaceLit{
	int id;
	int level;
	string namep;
	string toString() const{
		stringstream ss;
		if (id) ss <<namep<<"^"<<level;
		else ss <<"not "<<namep<<"^"<<level;
	        string s(ss.str());
		return s;
	}
};

string getPlaceString(vector<PlaceLit> pl, int id);


struct STStructure{
	//int szs;
	vector<bool> set;
	map<unsigned int, string> st;//maps identifiers to placenames
	//PetriNet pn;

bool operator == (STStructure sp) const {
	for (unsigned i=0; i<set.size(); ++i) {
		if (set.at(i)!=sp.set.at(i)) {
			return false;
		}
	}
	return true;
}
bool operator != (STStructure sp) const {
	for (unsigned i=0; i<set.size(); ++i) {
		if (set.at(i)==sp.set.at(i)) {
			return false;
		}
	}
	return true;
}
bool operator < (STStructure sp) const {
	for (unsigned i=0; i<set.size(); ++i) {
		if (set.at(i)==sp.set.at(i)) {
			continue;
		}
		else if(!set.at(i)&& sp.set.at(i)) continue;
		else return false;
	}
	return true;
}
STStructure operator + (STStructure sp) const {
	STStructure retv;
	for (unsigned i=0; i<set.size(); ++i) {
		if (set.at(i)||sp.set.at(i)) {
			retv.set.at(i)=true;
		}
		else retv.set.at(i)=false;
	}
	return retv;
}
STStructure operator - (STStructure sp) const {
	STStructure retv;
	for (unsigned i=0; i<set.size(); ++i) {
		if (set.at(i)&& !sp.set.at(i)) {
			retv.set.at(i)=true;
		}
		else retv.set.at(i)=false;
	}
	return retv;
}
};
inline  string toString(STStructure sp){
	string out="";
	for (unsigned i=0; i<sp.set.size(); ++i) {
		if(sp.set.at(i)) {
			out=out+sp.st[i]+" ";
		}
	}
	return out;
}




/*! \brief Generic formula with literals as places for the 
	computation of specific place subsets.
 
 Class for formulas with places as literals of a Petri net.
 There may be more literals for a place have unique identifiers. 
  */

class PFormula  {
protected:
	 /// maximal id for for minisat
	unsigned int maxid;
	///number of iterations for the place literals
	unsigned int mit;
	///underlying Petri net
	pnapi::PetriNet *net;
	///input for Minisat conjuction of clauses (disjunction of literals or their negations)
	vector<vector<int> > formula;
	///replaced the old map with a struct allowing access to id names
	vector<PlaceLit> pl;
	///place mapping to ids and iterations
	map<string, vector<unsigned int> > mp;
	///satisfying assignment in case there is one
	STStructure* assign;
	virtual ~PFormula(){}
public: 
	PFormula(){}
	//PFormula(pnapi::PetriNet *net1);
	//get the internal representation as a string
	string toString();
	vector<vector<int> > getFormula(){return formula;}
	map<string, vector<unsigned int> > getMapping(){return mp;}
	pnapi::PetriNet getNet();
	void setNet(pnapi::PetriNet * net);
	//void setMap();//setting place literals
	virtual void setFormula()=0;//construct the formula
	//adding clauses for partial formulas (some places are known, e.g. siphons, traps) 
	void addUnitClause(string plsrt);
	//eventually minimize it using minisat
	/// print formula with minisat identifiers
	void printFormula();
	/// print formula with place literals
	void printPlaceFormula();
};

/*! \brief Siphon formula
 
 */
class SiphonFormula : public PFormula{
public:
	SiphonFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

/*! \brief Trap formula
 
 */

class TrapFormula : public PFormula{
public:
	TrapFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

/*! \brief Siphon with trap inside formula
 
 */
class SwTFormula : public PFormula{
public:
	SwTFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

/*! \brief Siphon-trap property formula
 
 */

class STFormula : public PFormula{
public:
	STFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

//extern  int minisat(vector<vector<int> > &);vector<bool>*
extern bool  minisat(vector<vector<int> > &, vector<bool> &, vector<int> &);//returns one solution - the initial procedure or a clause which represnts the conflict
//this is called in case the formula 
//extern vector<bool>*  minisatconflict(vector<vector<int> > &);
extern vector<bool>*  minisat2(vector<vector<int> > &);//returns a non-empty solution (at most two calls)
extern vector<vector<bool> *>*  minisatall(vector<vector<int> > &);// iterates all solutions
extern vector<vector <bool>* >*  minisatsimp(vector< vector< int > > & in); //calls the solver for minimal assignments
//map<string, vector<int> > mp;

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

using namespace std;
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



//typedef STStructure Siphon;
//typedef STStructure Trap;


//formula class specially built for MINISAT with places as literals
class PFormula  {
protected:
	unsigned int maxid; // used for minisat 
	unsigned int mit;//number of iterations for the place literals
	pnapi::PetriNet *net;//underlying Petri net
	vector<vector<int> > formula;//
	vector<PlaceLit> pl;//replaced the old map with a struct allowing access to id names
	//void addClause(vector<int> clause);//adds a clause to the formula not necessary any more
	map<string, vector<unsigned int> > mp;//place mapping to ids and iterations
	STStructure* assign;//satisfying assignment in case there is one
	virtual ~PFormula(){}
public: 
	PFormula(){}
	//PFormula(pnapi::PetriNet *net1);
	string toString();//get the internal representation as a string
	pnapi::PetriNet getNet();
	void setNet(pnapi::PetriNet * net);
	void setMap();//setting place literals
	virtual void setFormula()=0;//construct the formula
	//eventually minimize it using minisat
	//virtual STStructure* getSatisfyingAssignment();//? just one assignment directly check that it is non-empty
	void printFormula();
	void printPlaceFormula();
};

//siphon formula is a special kind of formula with its own set function
//and a siphon structure in it mit=1
class SiphonFormula : public PFormula{
public:
	SiphonFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

class TrapFormula : public PFormula{
public:
	TrapFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

//siphon with trap inside
class SwTFormula : public PFormula{
public:
	SwTFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

class STFormula : public PFormula{
public:
	STFormula(pnapi::PetriNet *net1);
	void setFormula();//construct the formula from the net
};

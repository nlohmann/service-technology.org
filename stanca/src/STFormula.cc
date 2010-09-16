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

#include "STFormula.h"


//PFormula::addClause(vector<int> clause){
//}

//prints formula with the ids 
void PFormula::printFormula(){
	string out;
	// check whether it is set
	for (unsigned int i=0; i<formula.size(); ++i) {
		vector<int> vct=formula.at(i) ;
		out+=" ( ";
		for (unsigned int j=0; j<vct.size(); ++j) {
			stringstream ss;
			out+=" or ";
			if(vct.at(j)){
				ss<<vct.at(j);out+=ss.str();
			}  
		}
		out+=" ) ";
	}
	std::cout<<out<<endl;
}

void PFormula::setMap(){
	//initialize place literals from PN 
	//vector<PlaceLit> xpl(net->getPlaces().size());
	//for(int=0;i<net->getPlaces().size();++i){
	//	id;
	//int level;
	//string namep;
}
//prints formula with the corresponding literals as places from the net
void PFormula::printPlaceFormula(){
	string out;
	for (unsigned int i=0; i<formula.size(); ++i) {
		vector<int> vct=formula.at(i) ;
		out+=" ( ";
		for (unsigned int j=0; j<vct.size(); ++j) {

			if(vct.at(j)){
				out+=" or ";
				out+=getPlaceString(pl, vct.at(j));
			}
			
		}
		out+=" ) ";
	}
	std::cout<<out<<endl;
}

//SP* PFormula::getSatisfyingAssignment(){
//? call minisat	
//}
//PFormula::PFormula(pnapi::PetriNet *net1){
//	net=net1;
//}

SiphonFormula::SiphonFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		mp[(*it)->getName()][0]=i+1;
		++it;
	}
	mit=1;
}


TrapFormula::TrapFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		mp[(*it)->getName()][0]=i+1;
		++it;
	}	
	mit=1;
}

SwTFormula::SwTFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=2*net->getPlaces().size();
    set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(2);
	for (unsigned int i=0; i<maxid; ++i) {
		mp[(*it)->getName()][0]=i+1;
		++it;
	}	
	mit=2;
}


STFormula::STFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=2*net->getPlaces().size()+2;
	mit=2*net->getPlaces().size()+2;
	//initialize the mapping
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	//vector<int> vi(2);
	unsigned int k=0;
	for (unsigned int i=0; i<maxid; ++i) {
		unsigned imodk=i % net->getPlaces().size();
		if(k== imodk){
			mp[(*it)->getName()][0]=i+1;
			if(it==net->getPlaces().end())it=net->getPlaces().begin();
			else ++it;
		}
		++k;
	}
}


void SiphonFormula::setFormula(){
//set the max id  the cardinality of the set of places
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		vector<int> vct(maxid,0);
		//pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));
		int pid=mp[(*tit)->getName()][0];// the first id for the place; we have to create more
		vct[pid]=1;
		formula.push_back(vct);
	}
	//siphon formula
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		pnapi::Transition &t=*net->findTransition((*tit)->getName());
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			vector<int> vct(maxid,0);
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[pl->getName()][0];// the first id for the place; we have to create more
			vct[pid]=-1;
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				pnapi::Place * pli = dynamic_cast<pnapi::Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				vct[ppid]=1;
			}
			//add clause
			formula.push_back(vct);
		}
	
	}
}

void TrapFormula::setFormula(){
	//non=empty
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		vector<int> vct(maxid,0);
		pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));//?spurious
		int pid=mp[pl->getName()][0];// the first id for the place
		vct[pid]=1;
		formula.push_back(vct);
	}
	//siphon formula
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		//first
		vector<int> vct(maxid,0);
		pnapi::Transition &t=*net->findTransition((*tit)->getName());
		//int pid=mp[pl->getName()][0];// the first id for the place;
		//vct[pid]=1;
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[(*nit)->getName()][0];// the first id for the place
			vct[pid]=-1;
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				pnapi::Place * pli = dynamic_cast<pnapi::Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				vct[ppid]=1;
			}
			//add clause
			formula.push_back(vct);
		}
		
	}
	

}

void SwTFormula::setFormula(){
	//create  ids first
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		vector<int> vct(maxid,0),vctp(maxid,0);
		pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));
		int pid=mp[pl->getName()][0];// the first id for the place;
		int pidp=mp[pl->getName()][1];// the second id for the place;
		vct[pid]=1;
		vctp[pidp]=1;
		formula.push_back(vct);//non-empty siphon
		formula.push_back(vctp);//non-empty trap
	}
	//formula for siphon
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		//first
		vector<int> vct(maxid,0);
		pnapi::Transition &t=*net->findTransition((*tit)->getName());
		//int pid=mp[pl->getName()][0];// the first id for the place; we have to create more
		//vct[pid]=1;
		//const std::string s=t->getName();std::set<Node *> sn;//
		//sn=net.findTransition((*tit)->getName())->getPostset();
		//vector<int> clause(net.getPlaces().size());
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[pl->getName()][0];// the first id for the place
			vct[pid]=-1;
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				pnapi::Place * pli = dynamic_cast<pnapi::Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				vct[ppid]=1;
			}
			//add clause
			formula.push_back(vct);
		}
		
	}
	//formula for trap
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		//first
		vector<int> vct(maxid,0);
		pnapi::Transition &t=*net->findTransition((*tit)->getName());
		//int pid=mp[pl->getName()][0];// the first id for the place;
		//vct[pid]=1;
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[(*nit)->getName()][0];// the first id for the place
			vct[pid]=-1;
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				pnapi::Place * pli = dynamic_cast<pnapi::Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				vct[ppid]=1;
			}
			//add clause
			formula.push_back(vct);
		}
		
	}
	
}

void STFormula::setFormula(){
	//creating the ids for the places iteratively
	//non-empty
	vector<int> vct(maxid,0);
//creates ids for each iteration
	//first formula (disjunction of literals)
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));
		int pid=mp[pl->getName()][0];// the first id for the place;
		vct[pid]=1;
		formula.push_back(vct);//non-empty 
	}
	//siphon formula
	vector<int> vcts(maxid,0);//first and second part 
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		pnapi::Transition &t=*net->findTransition((*tit)->getName());
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			//get the id of the place
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
			/// here find the corresponding place in the mapping  and build an implication 
			int pid=mp[pl->getName()][0];// the first id for the place; we have to create more
			vcts[pid]=-1;
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				pnapi::Place * pli = dynamic_cast<pnapi::Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				vcts[ppid]=1;
			}
			//add clause
			formula.push_back(vcts);
		}
	}
	vector<int> vctt(maxid,0);
	//third formula 
	for (int i=0; i<net->getPlaces().size()+1; ++i) { //for all levels
		//creates ids for next iteration iteration if any not here 
		if(i<net->getPlaces().size()){
			for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
				pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));
				int pid=mp[pl->getName()][i+1];// the id number i+1 for the place;
				vctt[pid]=1;
				formula.push_back(vct);//non-empty siphon
			}
		}		
		//for all places build equivalences
		//formula.push_back(notf);		
		//direct  and reverse implication 
		for (std::set<pnapi::Place *>::iterator it=net->getPlaces().begin(); it!= net->getPlaces().end(); ++it) {
			//pi+1 equivalent pi and (si and t,spre, s'post s'i)
			vector<int> notf(maxid,0),notfr(maxid,0),sirev(maxid,0);
			//get id for i and i+1
			pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*it));
			int pid=mp[pl->getName()][i];// the id number i+1 for the place;
			int pidnext=mp[pl->getName()][i+1];// the id number i+1 for the place;
			//not s i+1 or s i
			notf[pidnext]=-1;
			notf[pid]=1;
			notfr[pidnext]=-1;
			sirev[pid]=1;
			formula.push_back(notf);
			formula.push_back(notfr);
			formula.push_back(sirev);
			//formula with trap not s i+1  or s prime i
			for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
				pnapi::Transition &t=*net->findTransition((*tit)->getName());
				//int pid=mp[pl->getName()][0];// the first id for the place;
				vector<int> vctrev(maxid,0);
				for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
					pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*nit));
					//get the id of the place
					/// here find the corresponding place in the mapping  and build an Implication 
					int pidprime=mp[(*nit)->getName()][i];// the first id for the place
					vector<int> vct(maxid,0);
					vct[pidprime]=1;
					vct[pidnext]=-1;
					formula.push_back(vct);
					vctrev[pidprime]=1;
				}
				formula.push_back(vctrev);
			}
		}
	}
 // forth formula (marked places)
	vector<int> forthvct(maxid,0);
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		pnapi::Place * pl = dynamic_cast<pnapi::Place *> ((*tit));
		if(pl->getTokenCount()>0) {
			int pid=mp[pl->getName()][net->getPlaces().size()+1];// the last id for the place;
			forthvct[pid]=-1;
			formula.push_back(forthvct);//non-empty 
		}
	}
	
}

string getPlaceString(vector<PlaceLit> pl, int id){
	string sgs;
 	for (unsigned i=0; i<pl.size(); ++i) {
		if (pl.at(i).id==id) 
			sgs=pl.at(i).toString();
	}
	return sgs;
}

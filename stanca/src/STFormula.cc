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


void PFormula::addUnitClause(string plstr){
	int pid=mp[plstr][0];
	vector<int> vct; 
	vct.push_back(pid);
	formula.push_back(vct);
}

//prints formula with the ids 
void PFormula::printFormula(){
	string out;
	std::cout<<"Places: "<< net->getPlaces().size()<<endl;
	std::cout<<"MaxId: "<< maxid<<endl;
	std::cout<<"Formula: ";
	for (unsigned int i=0; i<formula.size(); ++i) {
		vector<int> vct=formula.at(i) ;
		if(i!=0) out+=" and ";
		bool first=true;
		for (unsigned int j=0; j<vct.size(); ++j) {
			stringstream ss;
				if(first) {
					out+=" ( ";
					first=false;
					ss<<vct.at(j);
					out+=ss.str();
					continue;
				}
				else if(!(out=="") ) {
					out+=" or ";
					ss<<vct.at(j);
					out+=ss.str();
				}
		}
		if(!first) out+=" ) ";
	}
	std::cout<<out<<endl;
}

//prints subformula with the ids 
void PFormula::printSFormula(vector<vector<int> > subformula){
	string out;
	std::cout<<"Subformula: ";
	for (unsigned int i=0; i<subformula.size(); ++i) {
		vector<int> vct=subformula.at(i) ;
		if(i!=0) out+=" and ";
		bool first=true;
		for (unsigned int j=0; j<vct.size(); ++j) {
			stringstream ss;
			if(first) {
				out+=" ( ";
				first=false;
				ss<<vct.at(j);
				out+=ss.str();
				continue;
			}
			else if(!(out=="") ) {
				out+=" or ";
				ss<<vct.at(j);
				out+=ss.str();
			}
		}
		if(!first) out+=" ) ";
	}
	std::cout<<out<<endl;
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

bool PFormula::evaluateFormula(vector<int> & conflict){
	vector<bool> vb;
	Solver      S;
	S.verbosity = 0;
	IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 0, IntRange(0, 2));
	IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
	IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", INT32_MAX, IntRange(0, INT32_MAX));
	
	double cpu_time = cpuTime();
	double initial_time = cpuTime();
	S.verbosity = verb;
	
	solver = &S;
	parse_DIMACS_main(formula, S);
	if (!S.simplify()){
		fprintf(stdout,"UNSAT\n");// fclose(res);
		if (S.verbosity > 0){
			fprintf(stdout,"\n"); }
		fprintf(stdout,"MINISAT: UNSATISFIABLE, %d\n",S.conflict.size());
		conflict= vector<int>(S.conflict.size());
		for (int i = 0; i < S.conflict.size(); i++)
		{
			conflict.at(i)=toInt(S.conflict[i]);
			std::cout<< conflict.at(i)<<std::endl;
		}
		
		return false;
	}
	vec<Lit> dummy;
	lbool ret = S.solveLimited(dummy);
	if (S.verbosity > 0){
		fprintf(stdout,"\n"); }
	printf(ret == l_True ? "MINISAT: SATISFIABLE\n" : ret == l_False ? "MINISAT: UNSATISFIABLE\n" : "INDETERMINATE\n");
	//if (res != NULL){
	
	if (ret == l_True){
		//conflict=NULL;
		fprintf(stdout,"SAT\n");
		vb=vector<bool> (S.model.size());
		for (int i = 0; i < S.nVars(); i++)
			if (S.model[i] != l_Undef){
				//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
				vb.at(i)=toInt(S.model[i])-1;std::cout<< vb.at(i)<<std::endl;
			}
			else vb.at(i)=-1;
		map<unsigned int, string> x;int kk=0;
		for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
			x.insert(pair<unsigned int, string>(kk,(*tit)->getName()));
			++kk;
		}
		assign= new STStructure(vb,x);
		return true;
	}
	else if (ret == l_False){
		fprintf(stdout,"MINISAT: UNSAT\nConflicting clause");
		//vb=NULL;
		conflict= vector<int>(S.conflict.size());
		for (int i = 0; i < S.conflict.size(); i++)
			//if (S.conflict[i] != l_Undef)
		{
			conflict.at(i)=toInt(S.conflict[i]);
			//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
			//vb->at(i)=toInt(S.model[i])-1;
			std::cout<< conflict.at(i)<<std::endl;
		}
		//else vb->at(i)=-1;
		//fprintf(stdout," 0\n");//return vb;
		return false;
	}
	
	//return NULL;
	else
		fprintf(stdout, "INDET\n");return false;
	
	return false;
}

/** Evaluates a formula iteratively, until it becomes unsatisfiable  
 @param maybe not needed vector<bool> &  solution, vector<int> & conflict
 **/

bool PFormula::evaluateNontriviallyFormula(){
	Solver      S;
    S.verbosity = 0;
	
	double cpu_time = cpuTime();
	
    solver = &S;
    //signal(SIGINT,SIGINT_handler);
    //signal(SIGHUP,SIGINT_handler);
	
    parse_DIMACS_main(formula, S);
    //gzclose(in);
    if (!S.simplify()){
		printf("MINISAT: UNSATISFIABLE\n");
        return false;//(0); // UNSAT
    }
	//first check
	vec<Lit> dummy;
	lbool ret = S.solveLimited(dummy);
	printf(ret == l_True ? "MINISAT: SATISFIABLE\n" : ret == l_False ? "MINISAT: UNSATISFIABLE\n" : "INDETERMINATE\n");

	if (ret == l_True){
		vector<bool> *vb=new vector<bool> (S.model.size());
		for (int i = 0; i < S.nVars(); i++)
			//if (S.model[i] != l_Undef)
		{
			//cout<<endl;
			//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
			vb->at(i)=toInt(S.model[i])-1;std::cout<< vb->at(i)<<std::endl;
		}
		//else vb->at(i)=-1;
		vec<Lit> cl;
		cl.clear();
		vec<lbool> cc;
		int mt;
		Lit ml;
		S.model.copyTo(cc);
		for (int r=0; r<cc.size()/2; ++r) {
			mt=toInt(cc[r]);
			//negate solution
			ml=cc[r]==l_True ?~mkLit(r): mkLit(r); 
			cl.push(ml);
		}
		S.addClause(cl);
		//vec<Lit> dummy;
		//lbool 
	ret = S.solveLimited(dummy);
		printf(ret == l_True ? "MINISAT: SATISFIABLE\n" : ret == l_False ? "MINISAT: UNSATISFIABLE\n" : "INDETERMINATE\n");		
	//vector<bool> *
		vb=new vector<bool> (S.model.size());
	for (int i = 0; i < S.nVars(); i++)
		//if (S.model[i] != l_Undef)
	{
		//cout<<endl;
		//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
		vb->at(i)=toInt(S.model[i])-1;std::cout<< vb->at(i)<<std::endl;
	}
	}
    //bret=S.solve(); // result of SOLVE

	return false;
}

/// print satisfying assignment
void PFormula::printSatAssignment(bool x){
	cout << assign->set.size()<<endl;
	for (unsigned int k=0; k<assign->set.size(); ++k) {
		bool xb;
		if(x) xb=assign->set.at(k);
		else xb=!assign->set.at(k);
		if(xb){ //cout << "rs["<<k<<"] = "<<assign->set.at(k)<<endl;
			for (set<pnapi::Place *>::iterator it=net->getPlaces().begin(); it!= net->getPlaces().end(); ++it) {
				// cout << (*it)->getName()<< " ";
				for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
					if(mp.find((*it)->getName())->second[kk]==k+1)
						cout << (*it)->getName()<<" ";//<<"^"<<kk;
			}
		}
	}
	cout << endl;
}



SiphonFormula::SiphonFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		vector<unsigned int> vi(1);
		vi[0]=i+1;
		mp[(*it)->getName()]=vi;
		pair<unsigned int, string> ps(i,(*it)->getName());
		++it;
	}
}

MinSiphonFormula::MinSiphonFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=2*net1->getPlaces().size();
	set<pnapi::Place *>::iterator it=net1->getPlaces().begin();
	for (unsigned int i=0; i<net1->getPlaces().size(); ++i) {
		vector<unsigned int> vi(2);
		vi[0]=i+1;
		vi[1]=net1->getPlaces().size()+i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}	
}



TrapFormula::TrapFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	for (unsigned int i=0; i<maxid; ++i) {
		vector<unsigned int> vi(1);
		vi[0]=i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}	
}

MaxSiphonFormula::MaxSiphonFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		vector<unsigned int> vi(1);
		vi[0]=i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}
	//superset=superset1;
}

MaxTrapFormula::MaxTrapFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		vector<unsigned int> vi(1);
		vi[0]=i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}
	//superset=superset1;
}



//SwMTFormula::SwMTFormula(pnapi::PetriNet *net1){
	/*net=net1;
	maxid=net->getPlaces().size();
	set<pnapi::Place *>::iterator it=net->getPlaces().begin();
	vector<int> vi(1);
	for (unsigned int i=0; i<maxid; ++i) {
		vector<unsigned int> vi(1);
		vi[0]=i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}
	mit=1;
	 */
	//superset=superset1;
//}


SwTFormula::SwTFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=2*net1->getPlaces().size();
    set<pnapi::Place *>::iterator it=net1->getPlaces().begin();
	for (unsigned int i=0; i<net1->getPlaces().size(); ++i) {
		vector<unsigned int> vi(2);
		vi[0]=i+1;
		vi[1]=net1->getPlaces().size()+i+1;
		mp[(*it)->getName()]=vi;
		++it;
	}	
}

/**STFormula constructor 
 **/

STFormula::STFormula(pnapi::PetriNet *net1){
	net=net1;
	maxid=net->getPlaces().size()*(net->getPlaces().size()+1);
	//initialize the mapping
	set<pnapi::Place *>::iterator it;
	it=net->getPlaces().begin();
	for (unsigned int i=0; i<net->getPlaces().size(); ++i) {
		//for each iteration
		vector<unsigned int> vi(net->getPlaces().size()+1);
		for (unsigned int k=0; k<net->getPlaces().size()+1; ++k) {
			//compute the starting id 
			vi[k]=k*net->getPlaces().size()+i+1;
		}
		mp[(*it)->getName()]=vi;
		++it;
	}	
}

/**Set the Siphon  formula 
 **/


void SiphonFormula::setFormula(){
//set the max id  the cardinality of the set of places
	vector<int> vct;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place; we have to create more
		vct.push_back(pid);//[pid]=1;
	}
	formula.push_back(vct);
	//siphon formula
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		std::set<pnapi::Node *> xp=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an implication 
			int pid=mp[(*nit)->getName()][0];// the first id for the place; 
			vct.push_back(-pid);
			set<pnapi::Node *> xxn=(*tit)->getPreset();
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(ppid);
			}
			//add clause
			formula.push_back(vct);
		}
	
	}
}

/**Set the MinSiphon formula 
 **/
void MinSiphonFormula::setFormula(){
	SiphonFormula sf(net);
	sf.setFormula();
	formula=sf.getFormula();
	//add second siphon
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		std::set<pnapi::Node *> xp=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an implication 
			int pid=mp[(*nit)->getName()][1];// the first id for the place; 
			vct.push_back(-pid);
			set<pnapi::Node *> xxn=(*tit)->getPreset();
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][1];
				vct.push_back(ppid);
			}
			//add clause
			formula.push_back(vct);
		}
	}
	//inclusion
	vector<int> vct;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place
		int pidnext=mp[(*tit)->getName()][1];// the second id for the place
		vct.push_back(-pid);
		vct.push_back(pidnext);
		formula.push_back(vct);	
	}
	//minimality: the maximal included siphon is the empty set 
	vector<int> vctc;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pidnext=mp[(*tit)->getName()][1];// the second id for the place
		int pid=mp[(*tit)->getName()][0];// the first id for the place
		vctc.push_back(-pid);
		vctc.push_back(pidnext);
	}
	formula.push_back(vctc);		
}

/**Set the Trap formula 
 **/
void TrapFormula::setFormula(){
	//set the max id  the cardinality of the set of places
	vector<int> vct;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place
		vct.push_back(pid);
	}
	formula.push_back(vct);
	//siphon formula
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		std::set<pnapi::Node *> xp=(*tit)->getPreset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			vector<int> vct;
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[(*nit)->getName()][0];// the first id for the place
			vct.push_back(-pid);
			set<pnapi::Node *> xxn=(*tit)->getPostset();
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(ppid);
			}
			//add clause
			formula.push_back(vct);
		}
		
	}
}

/**Set the Max Trap formula 
 **/

void MaxTrapFormula::setFormula(){
	//non-empty
	//trap formula
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place;
		//vct[pid]=1;
		std::set<pnapi::Node *> xn=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xn.begin(); nit!=xn.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			//int pid=mp[(*nit)->getName()][0];//the first id for the place
			vct.push_back(-pid);
			///here build a disjunction
			set<pnapi::Node *> xxn=(*nit)->getPostset();
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				///cout<"nnit "<<(*nnit)->getName()<<endl;
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(-ppid);//vct[ppid]=1;
			}
			//add clause
			//if(vct.size()>0) 
			formula.push_back(vct);
		}
	}
}

/**Set the Siphon with Max Trap formula 
 **/

void SwMTFormula::setFormula(){
	SiphonFormula sf(net);
	sf.setFormula();
	formula=sf.getFormula();
	//non-empty
	//trap formula
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place;
		//vct[pid]=1;
		std::set<pnapi::Node *> xn=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xn.begin(); nit!=xn.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			//int pid=mp[(*nit)->getName()][0];//the first id for the place
			vct.push_back(-pid);
			///here build a disjunction
			set<pnapi::Node *> xxn=(*nit)->getPostset();
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				///cout<"nnit "<<(*nnit)->getName()<<endl;
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(-ppid);//vct[ppid]=1;
			}
			//add clause
			//if(vct.size()>0) 
			formula.push_back(vct);
		}
	}
}

/**Set the Max Siphon Trap formula 
 **/

void MaxSiphonFormula::setFormula(){
	//non-empty
	//vector<int> vct;//(maxid,0);
	//for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
	//	int pid=mp[(*tit)->getName()][0];// the first id for the place
	//	vct.push_back(pid);//=1;
	//}
	//formula.push_back(vct);
	//trap formula
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		std::set<pnapi::Node *> xn=(*tit)->getPreset();
		for (std::set<pnapi::Node *>::iterator nit=xn.begin(); nit!=xn.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			int pid=mp[(*tit)->getName()][0];//the first id for the place
			vct.push_back(pid);
			///here build a disjunction
			set<pnapi::Node *> xxn=(*nit)->getPreset();
			for (std::set<pnapi::Node *>::iterator nnit=xxn.begin(); nnit!=xxn.end(); ++nnit){
				///cout<"nnit "<<(*nnit)->getName()<<endl;
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(-ppid);//vct[ppid]=1;
			}
			//add clause
			//if(vct.size()>0) 
			formula.push_back(vct);
		}
	}
}

/**Set the Siphon with Trap  inside formula 
 **/
void SwTFormula::setFormula(){
	//create  ids first
	vector<int> vct,vctp;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// get id for the place;
		vct.push_back(pid);
	}
	formula.push_back(vct);//non-empty siphon
	//formula for trap
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		//int pid=mp[pl->getName()][0];// the first id for the place;
		//vct[pid]=1;
		std::set<pnapi::Node *> xp=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			vector<int> vct;//(maxid,0);
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[(*nit)->getName()][0];// the first id for the place
			vct.push_back(-pid);
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPreset().begin(); nnit!=(*tit)->getPreset().end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][0];
				vct.push_back(ppid);
			}
			//add clause
			formula.push_back(vct);
		}
	}
	
	
	//formula for trap
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		vector<int> vct;//(maxid,0);
		std::set<pnapi::Node *> xp=(*tit)->getPreset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			vector<int> vct;
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an Implication 
			int pid=mp[(*nit)->getName()][1];
			vct.push_back(-pid);
			
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=(*tit)->getPostset().begin(); nnit!=(*tit)->getPostset().end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][1];
				vct.push_back(ppid);
			}
			//cout << endl;
			//add clause
			formula.push_back(vct);
		}
	}
	//trap inclusion
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pids=mp[(*tit)->getName()][0];// siphon pid
		int pidt=mp[(*tit)->getName()][1];// trap pid
		vector<int> vct;
		vct.push_back(-pidt);
		vct.push_back(pids);
		formula.push_back(vct);
	}	
}

///adds non-empty trap condition to the standard formula
void  SwTFormula::setMarkedTrap(){
	vector<int>  trap;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][1];
		trap.push_back(pid);
		vector<int> forthvct, trap;
		if((*tit)->getTokenCount()>0) {
			trap.push_back(pid);
		}
	}
	formula.push_back(trap);	
}

///for detecting whether a siphon contains only the empty trap
/// sat means the siphon contains only the empty trap unsatisfiable means that the siphon contains a non-trivial trap
void  SwTFormula::setEmptyTrap(){
	vector<int>  trap;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][1];
		vector<int> forthvct, trap;
		if((*tit)->getTokenCount()>=0) {
			trap.push_back(-pid);
		}
		formula.push_back(trap);	
	}
}


///adds the marked condition to the standard formula
void  SwTFormula::setNonemptyTrap(){
	vector<int>  trap;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][1];
		trap.push_back(pid);
	}
	formula.push_back(trap);

}

/**Prints out a net with a trap & siphon place assignment 
 @param x true when the trap is not maximal
 **/
void SwTFormula::printSatAssignment(bool x){
	//build separate structures for siphons and traps
	STStructure siphon, trap;
	unsigned int k=0;
	vector<bool> vb(net->getPlaces().size(),false);
	std::map<unsigned int, string> mapp;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		std::pair<unsigned int, string> xx(k,(*tit)->getName());
		++k;
		mapp.insert(xx);
	}
	
	cout << assign->set.size()<<endl;
	cout << "Siphon: ";//assign->set.size()
	for (unsigned int k=0; k<net->getPlaces().size(); ++k) {
		bool xb;
		if(x) xb=assign->set.at(k);
		else xb=!assign->set.at(k);
		if(xb){ //cout << "rs["<<k<<"] = "<<assign->set.at(k)<<endl;
			for (set<pnapi::Place *>::iterator it=net->getPlaces().begin(); it!= net->getPlaces().end(); ++it) {
				// cout << (*it)->getName()<< " ";
				for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
					if(mp.find((*it)->getName())->second[kk]==k+1)
						cout << (*it)->getName()<<" ";//<<"^"<<kk;
			}
		}
	}
	cout<<endl<<"Trap: ";
	for (unsigned int k=net->getPlaces().size(); k<assign->set.size(); ++k) {
		bool xb;
		if(x) xb=assign->set.at(k);
		else xb=!assign->set.at(k);
		if(xb){ //cout << "rs["<<k<<"] = "<<assign->set.at(k)<<endl;
			for (set<pnapi::Place *>::iterator it=net->getPlaces().begin(); it!= net->getPlaces().end(); ++it) {
				// cout << (*it)->getName()<< " ";
				for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
					if(mp.find((*it)->getName())->second[kk]==k+1)
						cout << (*it)->getName()<<" ";
			}
		}
	}
	cout << endl;
}

/**Set the Siphon Trap formula 
 **/
void STFormula::setFormula(){
	//non-empty siphon
	vector<int> vct;
	//first formula (disjunction of literals)
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][0];// the first id for the place;
		vct.push_back(pid);
	}
	formula.push_back(vct);//non-empty 
	for (set<pnapi::Transition*>::iterator tit=net->getTransitions().begin(); tit!=net->getTransitions().end(); ++tit) {
		std::set<pnapi::Node *> xp=(*tit)->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=xp.begin(); nit!=xp.end(); ++nit){
			//get the id of the place
			/// here find the corresponding place in the mapping  and build an implication 
			vector<int> vcts;//first and second part 
			int pid=mp[(*nit)->getName()][0];
			vcts.push_back(-pid);
			///here build a disjunction
			std::set<pnapi::Node *> xpp=(*tit)->getPreset();
			for (std::set<pnapi::Node *>::iterator nnit=xpp.begin(); nnit!=xpp.end(); ++nnit){
				int ppid=mp[(*nnit)->getName()][0];
				vcts.push_back(ppid);
			}
			//add clause
			formula.push_back(vcts);
		}
	}
	//third formula 
	unsigned i=0;
	for (unsigned int i=0; i<net->getPlaces().size(); ++i) { 
		//for all places build equivalences
		vector<vector<int> > equiv;
		//formula.push_back(notf);		
		for (std::set<pnapi::Place *>::iterator it=net->getPlaces().begin(); it!= net->getPlaces().end(); ++it) {
			//pi+1 equivalent pi and (si and t,spre, s'post s'i)
			//get id for i and i+1
			int pid=mp[(*it)->getName()][i];// the id number i+1 for the place;
			cout <<(*it)->getName()<<"  "<< pid<<" \n";
			int pidnext=mp[(*it)->getName()][i+1];// the id number i+1 for the place;
			
			//direct has two clauses
			vector<int> notf;
			notf.push_back(-pidnext);
			notf.push_back(pid);
			formula.push_back(notf);
			//printFormula();
			std::set<pnapi::Node *> xp=(*it)->getPostset();//transition postset of a  place
			for (std::set<pnapi::Node *>::iterator nit= xp.begin(); nit!=xp.end(); ++nit){
				//compute the place postset of the transition
				std::set<pnapi::Node *> tp=(*nit)->getPostset();
				vector<int>  notfr;
				notfr.push_back(-pidnext);
				for (std::set<pnapi::Node *>::iterator nnit= tp.begin(); nnit!=tp.end(); ++nnit){
					//get the id of the place
					int pidprime=mp[(*nnit)->getName()][i];// the first id for the place
					notfr.push_back(pidprime);				
				}
				if(tp.size()) 
					formula.push_back(notfr);
			}
			//reverse
			//previous is in CNF 
			//current is CNF of previous sau postt (also in cnf)
			vector<vector<int> >  previous,current,first;
			bool empty=false;//whether the empty clause exists
			for (std::set<pnapi::Node *>::iterator nnit= xp.begin(); nnit!=xp.end(); ++nnit){
				vector<int>  postt;
				//compute the place postset of the transition
				std::set<pnapi::Node *> tp=(*nnit)->getPostset();
				for (std::set<pnapi::Node *>::iterator nit= tp.begin(); nit!=tp.end(); ++nit){
					//get the id of the place
					int pidprime=mp[(*nit)->getName()][i];
					postt.push_back(-pidprime);
					vector<int>  firstv;firstv.push_back(-pidprime);
					first.push_back(firstv);
				}
				postt.push_back(pid);
				if(postt.size()==0) empty=true;
				if(nnit!= xp.begin()){//compute the cartezian product with the previously computed set
					for(int k=0;k<previous.size();++k){//for each conjunction
						//and make a disjunction with each element from post
						for (int kk=0; kk<postt.size(); ++kk) {
							//receive first the vector from k
							vector<int> xxx=previous.at(k);
							xxx.push_back(postt.at(kk));
							current.push_back(xxx);
						}
					}
					//printSFormula(current);
					//printSFormula(previous);
					//cout<<current.size()<<postt.size()<<endl;
					if(postt.size()!=0) previous=current;
					//printSFormula(current);
					//printSFormula(previous);

				}
				else{
					//previous.push_back(postt);
					previous=first;
				}
				//printSFormula(previous);
			}
			//the last set contains the final product in DNF
			for(int k=0;k<previous.size();++k){
				//replace 
				vector<int> vkk=previous.at(k);
				vkk.push_back(-pid);
				vkk.push_back(pidnext);
				formula.push_back(vkk);					
			}	
			if(empty){
				vector<int> vkk;
				vkk.push_back(-pid);
				vkk.push_back(pidnext);
				formula.push_back(vkk);				
			}
			//formula.push_back(sirev);
		}
		//printFormula();cout<<endl;
		//if(i==2) break;
	}
	

 // forth formula (marked places)
	vector<int> trap;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		vector<int> forthvct,counter;
		int pid=mp[(*tit)->getName()][net->getPlaces().size()];
		if((*tit)->getTokenCount()>0) {
			forthvct.push_back(-pid);
			if(forthvct.size())formula.push_back(forthvct);
		}
		counter.push_back(mp[(*tit)->getName()][net->getPlaces().size()]);
		//formula.push_back(counter);
		trap.push_back(pid);
		//trap.push_back(mp[(*tit)->getName()][net->getPlaces().size()]);
		//else {forthvct.push_back(pid);formula.push_back(forthvct);}
	}
//	formula.push_back(trap);
	//if the trap is empty, it should still give a solution
	//if(trap.size())formula.push_back(trap);
}

/// make the trap be non-empty
void STFormula::setNonemptyTrap(){
	vector<int> trap;
	for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
		int pid=mp[(*tit)->getName()][net->getPlaces().size()];
		//formula.push_back(counter);
		trap.push_back(pid);
	}
		formula.push_back(trap);
}

	
string getPlaceString(vector<PlaceLit> pl, int id){
	string sgs;
 	for (unsigned i=0; i<pl.size(); ++i) {
		if (pl.at(i).id==id) 
			sgs=pl.at(i).toString();
	}
	return sgs;
}


///after SAT is called, the solution, if any, is mapped back internally 
void PFormula::setSatAssignment(vector<bool> vb){
	if(vb.size()){
		//first do the mapping of places 
		unsigned int k=0;
		std::map<unsigned int, string> mapp;
		for (set<pnapi::Place*>::iterator tit=net->getPlaces().begin(); tit!=net->getPlaces().end(); ++tit) {
			std::pair<unsigned int, string> xx(k,(*tit)->getName());
			++k;
			mapp.insert(xx);
		}
		assign=new STStructure(vb,mapp);

	}
}



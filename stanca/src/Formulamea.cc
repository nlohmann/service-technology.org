/*
 *  Formulamea.cc
 *  
 *
 *  Created by Olivia Oanea on 12/10/09.
 *  Copyright 2009 University of Rostock. All rights reserved.
 *
 */

#include <config.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <set>
#include <map>
#include <list>
#include <ctime>
#include <sys/types.h>
#include <sys/wait.h>
#include "Formulamea.h"
#include "verbose.h"
#include <stdio.h>
#include <string.h>

/*const MFormula* MNegation::computeNFF() const{
	if (typeid(*subf) == typeid(MNegation)) { // neg neg \phi
		const MNegation *negf=dynamic_cast<const MNegation *>(subf);
		const MFormula *gf=negf->subf;
		return gf->computeNFF();
	}
	else if(typeid(*subf) == typeid(MConjunction)){
		const MConjunction *cj=dynamic_cast<const MConjunction *>(subf);
		MDisjunction *res=new MDisjunction(dynamic_cast<const MNegation *>(cj->left),dynamic_cast<const MNegation *>(cj->right));
		return res->computeNFF();
	}
	else if((typeid(*subf) == typeid(MDisjunction))) {
		const MDisjunction *cj=dynamic_cast<const MDisjunction *>(subf);
		MConjunction *res=new MConjunction(dynamic_cast<const MNegation *>(cj->left),dynamic_cast<const MNegation *>(cj->right));
		return res->computeNFF();			
	}
	
}*/

///help function for nff2cnf
const MFormula* distr(const MFormula * f1, const MFormula * f2){ 
	const MFormula* rr;
	if(typeid(*f1)==typeid (MConjunction)) {
		const MConjunction conj1=MConjunction(dynamic_cast<const MConjunction &> (*f1));
		rr = new MConjunction(distr(conj1.left, f2),distr(conj1.right, f2));
	}
	else if(typeid(*f2)==typeid (MConjunction)){
		const MConjunction conj2=dynamic_cast<const MConjunction &> (*f2);
		rr= new MConjunction(distr(f1, conj2.left),distr(f1, conj2.right));
	}
	else rr = new MDisjunction(f1,f2);
	//if(rr!=NULL) cout<<endl <<"distr" << rr->toString()<<endl;
	return rr;
}

///translation to cnf form from nff
const MFormula* NFF2CNF(const MFormula *f){ 
	//cout<<endl<<"start nff2cnf "<<f->toString()<<endl;
	const MFormula* rr;
//if ((typeid(f)==typeid((MConjunction)) cout<<"strange";
	//if literal or negation e ok
	if (typeid(*f)==typeid(PlLit)){
		rr=f;
		//const PlLit pl=PlLit(dynamic_cast<const PlLit &> (*f));
		//return new PlLit(pl);
	}
	if (typeid(*f)==typeid(MNegation)){
		//const MNegation mn=MNegation(dynamic_cast<const MNegation &> (*f));
		//if(typeid(*mn.subf)==typeid(PlLit))
			rr=f;
	}
	if (typeid(*f)==typeid(MConjunction)){
		const MConjunction conj=MConjunction(dynamic_cast<const MConjunction &> (*f));
		rr= new MConjunction(NFF2CNF(conj.left), NFF2CNF(conj.right));
	}
	if (typeid(*f)==typeid(MDisjunction)){

		const MDisjunction conj=MDisjunction(dynamic_cast<const MDisjunction &> (*f));
		rr=distr(NFF2CNF(conj.left), NFF2CNF(conj.right));
	}
	//if(rr!=NULL) cout<<endl <<"return cnf"<< rr->toString()<<endl;
	return rr;
}

/// compute cnf form

const MFormula * computeCNF(const MFormula *inf){
	cout << "start EFF"<<endl;
	const MFormula *nef=inf->computeEFF();
	//cout <<endl<<" EEF "<< nef->toString();
	//nef->toString();
		cout << "start IFF"<<endl;
	const MFormula *nif=nef->computeIFF();
		cout << "NFF"<<endl;
	//cout <<endl<<" IFF "<< nif->toString();
	const MFormula *negf=nif->computeNFF();
		cout << "CNF"<<endl;
	//cout << negf->toString();
	const MFormula *cnfnumerge=NFF2CNF(negf);
	//cout <<"CNF"<< cnfnumerge->toString()<<endl;
	//return NFF2CNF(negf);
	//return nef;
	return cnfnumerge;
}


vector<vector<int> > CNF2MSF(const MFormula *fcnf){
	//build a conjunction of disjunctions
	vector<vector<int> > msf;
	set<const MFormula *> vc;
	cout <<endl<< "enter"<<endl;
	
	///initially we have a conjunction
	//if (typeid(*fcnf)==typeid(MFormula)){typeid(derived).name() 
		//} 
	//if (strstr(typeid(*fcnf).name() ,"MConjunction")!=NULL ){
		
	//}
	//size_t pos=typeid(*fcnf).name().find("MConjunction");
	if (strstr(typeid(*fcnf).name() ,"MConjunction")!=NULL) {//the most difficult case
		///
		//cout << "first conjunction";//cout <<endl<<" "<<typeid(*fcnf).name() <<"dis"<<endl;
		const MConjunction *conj = new MConjunction(dynamic_cast<const MConjunction&>(*fcnf));
		//if (conj!=NULL) {
		//	cout <<endl<<conj->toString()<<endl;
		//}
		list<const MConjunction*> sd;
		sd.push_front(conj);int i=0;
		while (!sd.empty()) {
			const MConjunction *cconj=sd.front();
			//cout << endl<<sd.size()<<endl;++i;
			/*if (i>2) {
				cout <<endl ; break;
			}*/
			//cout <<"begin of schleife for "<<cconj->right->toString()<<endl <<cconj->left->toString()<<endl;
			//cout <<"begin of schleife for "<<typeid(*cconj->left).name()<<endl <<typeid(*cconj->left).name()<<endl;
			//const MFormula *left=(*sd.begin())->left;
			//const MFormula *right=(*sd.begin())->right;
			if  (strstr(typeid(*cconj->left).name() ,"MConjunction")!=NULL) {//(typeid(*(*sd.begin())->left)==typeid(MConjunction)) {
				//cout << "conjunction left";
				const MConjunction *pk=dynamic_cast<const MConjunction *>(cconj->left);
				sd.push_back(pk);//cout<<endl<<pk->toString()<<endl;
			}
			if  (strstr(typeid(*cconj->right).name() ,"MConjunction")!=NULL) {//if (typeid(*(*sd.begin())->right)==typeid(MConjunction)) {
				//cout << "conjunction right";
				const MConjunction *pk=dynamic_cast<const MConjunction *>(cconj->right);
				sd.push_back(pk);	//cout<<endl<<pk->toString()<<endl;
			}
			//both "and" children were inserted; hence we might as well remove the node
			//we don't need to wait until the very end
			//cout << endl<<sd.size()<<endl;
			sd.pop_front();//cout << endl<<sd.size()<<endl;
			//cout <<"after erase "<<cconj->right->toString()<<endl <<cconj->left->toString()<<endl;
			if  (strstr(typeid(*cconj->left).name() ,"MDisjunction")!=NULL) {//if (typeid(*(*sd.begin())->left)==typeid(MDisjunction)) {
				//cout << "disjunction left "<<typeid(cconj->left).name()<<endl;
				const MDisjunction *pk=dynamic_cast<const MDisjunction *>(cconj->left);
				vc.insert(pk);//cout<<endl<<pk->toString()<<endl<<(*sd.begin())->toString()<<endl;
				if (pk!=NULL) {
					//cout <<endl<<pk->toString()<<endl;
				}
				//continue;
			}
			if  (strstr(typeid(*cconj->right).name() ,"MDisjunction")!=NULL) {//if (typeid(*(*sd.begin())->right)==typeid(MDisjunction)) {
				const MDisjunction *pk=dynamic_cast<const MDisjunction *>(cconj->right);
				vc.insert(pk);
				//cout << "disjunction right";
				//continue;
			}
			if (strstr(typeid(*cconj->right).name() ,"MNegation")!=NULL){//((typeid(*(*sd.begin())->right)==typeid(MNegation))) {
				const MNegation *pk=dynamic_cast<const MNegation *>(cconj->right);
				vc.insert(pk);
				//continue;
			}
			if (strstr(typeid(*cconj->left).name() ,"MNegation")!=NULL){//if ((typeid(*(*sd.begin())->left)==typeid(MNegation))) {
				const MNegation *pk=dynamic_cast<const MNegation *>(cconj->left);
				vc.insert(pk);
				//continue;
			}
			if (strstr(typeid(*cconj->right).name() ,"PlLit")!=NULL){//((typeid(*(*sd.begin())->right)==typeid(PlLit))) {
				const PlLit *pk=dynamic_cast<const PlLit *>(cconj->right);
				vc.insert(pk);
				//continue;
			}
			if (strstr(typeid(cconj->left).name() ,"PlLit")!=NULL){//((typeid(*(*sd.begin())->left)==typeid(PlLit))) {
				const PlLit *pk=dynamic_cast<const PlLit *>(cconj->left);
				vc.insert(pk);
				//continue;
			}
			//cout <<"End of schleife for "<<cconj->toString()<< endl<<sd.size()<<endl;
			
			
			
			//pl literals and negations are missing are missing
			
		}
	}
	else{//it is a clause keep it that way
		vc.insert(fcnf);//cout <<endl<< "what is it then"<<endl;
	}
	//cout<<endl <<"number of clauses" << vc.size()<<endl;
	if (vc.size()>0) {//cout <<endl<< (*vc.begin())->toString()<<endl;
		
	}
	
	
	//here we get the literals from disjunctions
	
	///iterate the vector
	//vector<Formula *>::size_type sz = vc.size();
	int itt=0;
	for (set<const MFormula *>::iterator it=vc.begin(); it!=vc.end(); ++it){
		
		/// new vector 
		vector<int> vi; //cout<<"clause"<<itt<<endl;
		///check the type of the formula for each clause and replace it by their identifier
		if(typeid(**it)==typeid(MDisjunction)){
			
			const MDisjunction *cj = new MDisjunction(dynamic_cast<const MDisjunction&>(**it));
			//cout << endl<<cj->toString()<<endl;
			list<const MDisjunction*> sd; sd.push_back(cj);
			while (!sd.empty()) {
				const MDisjunction *pk=sd.back();sd.pop_back();
				if (typeid(*pk->left)==typeid(MDisjunction)) {//cout<<"Disj";
					const MDisjunction *cj = new MDisjunction(dynamic_cast<const MDisjunction&>(*pk->left));
					sd.push_back(cj);
				}
				if (typeid(*pk->right)==typeid(MDisjunction)) {//cout<<"Disj";
					const MDisjunction *cj = new MDisjunction(dynamic_cast<const MDisjunction&>(*pk->right));
					sd.push_back(cj);
				}
				if (typeid(*pk->left)==typeid(PlLit)) {//cout<<"lit";
					const PlLit *xpl = new PlLit(dynamic_cast<const PlLit&> (*pk->left));
					vi.push_back(xpl->number);
					delete xpl;
				}
				if (typeid(*pk->right)==typeid(PlLit)) {//cout<<"lit";
					const PlLit *xpl = new PlLit(dynamic_cast<const PlLit&> (*pk->right));
					vi.push_back(xpl->number);
					delete xpl;
				}
				if (typeid(*pk->left)==typeid(MNegation)) {//cout<<"lit";
					//const MNegation *xplnr = new MNegation(dynamic_cast<const MNegation&> (*pk->left));
					const MNegation *xplnr = static_cast<const MNegation *> (pk->left);
					//cout << xplnr->toString();
					//if (typeid(*xplnr->subf)==typeid(PlLit)) cout<<"not literal";
					//const PlLit *xpl = new PlLit(dynamic_cast<const PlLit&> (*xplnr->subf));
					const PlLit *xpl = new PlLit(static_cast<const PlLit&> (*xplnr->subf));
					vi.push_back(-xpl->number);//cout<<-xpl->number;
					//delete xplnr;
					delete xpl;//}
					//cout << xplnr->toString();
					//delete xplnr;
				}
				if (typeid(*pk->right)==typeid(MNegation)) {//cout<<"lit";
					const MNegation *xplnr = new MNegation(dynamic_cast<const MNegation&> (*pk->right));
					const PlLit *xpr = new PlLit(dynamic_cast<const PlLit&> (*xplnr->subf));
					vi.push_back(-xpr->number);
					delete xplnr;delete xpr;
				}
				
			}
		}
		else  ///it is a literal
			if (typeid(**it)==typeid(PlLit)) {
				
				const PlLit *xpl = new PlLit(dynamic_cast<const PlLit&> (**it));
				//cout << endl<<xpl->toString()<<endl;
				vi.push_back(xpl->number);
				//vi.insert((*it)->number);
			    delete xpl;
			}
			else if(typeid(**it)==typeid(MNegation)) {
				//cout << endl<<cj-toString()<<<endl;
				const MNegation *xplnr = new MNegation(dynamic_cast<const MNegation&> (**it));
				//cout << endl<<xplnr->toString()<<endl;
				const PlLit *xpr = new PlLit(dynamic_cast<const PlLit&> (*xplnr->subf));				
				vi.push_back(-xpr->number);
				delete xpr;delete xplnr;
				//vi.insert((*it)->number);
			} 
		msf.push_back(vi);++itt;
		/*for (int j=0; j<vi.size(); ++j) {
			cout <<" " <<vi.at(j);
		}
		cout << endl;*/
		//cout<<endl<<vi.size()<<endl;
	}
	
	return msf;
}

int isSatisfiable(const MFormula *f){
	bool sat=false;
	/// compute cnf
	
	return sat;
}

string toPl(map<string, vector<int> > mp, int id){
	pair<string, int> help;
	cout << mp.size()<<"size";
	//const map<string, vector<int> >::iterator it;
	for (map<string, vector<int> >::iterator it=mp.begin() ; it != mp.end(); it++ ){
		for (int i=0; i<=(*it).second.size(); ++i) {
			if ((*it).second[i]==id) {
				//pair<(*it).first,(*it).second> pp;
				//cout <<"hey"<< (*it).first<<i+1;
				help=pair<string,int>((*it).first,i+1);
			}
		}
		
	}
	stringstream ss;
	ss <<  help.second;
	string s(ss.str());
	return help.first + "^" + s;
}

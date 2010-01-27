/*
 *  Formula.h
 *  
 *
 *  Created by Olivia Oanea on 12/9/09.
 *  Copyright 2009 University of Rostock. All rights reserved.
 *
 */

#ifndef _MFORMULA_H
#define _MFORMULA_H
#include <string>
#include <vector>
#include <set>
#include <verbose.h>
#include <typeinfo>
#include <utility>
//#include <types.h>


using namespace std;

extern map<string, vector<int> > mp;
string toPl(map<string, vector<int> > mp, int id);



class MFormula{

public:
	
	int isSatisfiable();
	
	//vector<vector<int> > computeCNF();
	
	///destructor
	virtual ~MFormula(){}
	
	///basic constructor
	MFormula(){}
	//copy constructorFormula (const Formula & formula);
	MFormula(const MFormula *f){}

	/// compute implication-free form
	virtual const MFormula* computeEFF() const =  0;
	
	/// compute implication-free form
	virtual const MFormula* computeIFF() const =  0;
	
	/// compute negation-free form
	virtual const MFormula* computeNFF() const =  0;
	
	
	/// for printing
	virtual string toString() const = 0;
};

class MConjunction : public MFormula {
public:	
	const MFormula *left;
	const MFormula *right;
	string ft;
	
	MConjunction(const MFormula *lleft, const MFormula *rright) : left(lleft), right (rright){
		
		assert(lleft);
		assert(rright);
	}
	MConjunction(const MConjunction *conj) : MFormula(*conj), left(conj->left),right(conj->right){}
	~MConjunction(){
		
	}	
	const MFormula* computeEFF() const{
		return new MConjunction(left->computeEFF(), right->computeEFF());
	}
	
	const MFormula* computeIFF() const{
		return new MConjunction(left->computeIFF(), right->computeIFF());
	}
	const MFormula* computeNFF() const{
		return new MConjunction(left->computeNFF(), right->computeNFF());
	}
	
	
	string toString() const{
		//assert(left);assert(right);
		return ("(" + left->toString() + " and " + right->toString() + ")");
	}
	
};

class MDisjunction : public MFormula {
public:	
	const MFormula *left;
	const MFormula *right;
	std::string ft;
	//public:	
	MDisjunction(const MFormula *lleft, const MFormula *rright) : left(lleft), right (rright){
		assert(lleft);
		assert(rright);
		ft = "OR";
	}	
	MDisjunction(const MDisjunction *dis) :  left(dis->left),right(dis->right){}
	~MDisjunction(){
		//delete left;
		//delete right;
	}	
	const MFormula* computeEFF() const{
		//MDisjunction* md= new MDisjunction(left->computeNFF(), right->computeNFF());
		return new MDisjunction(left->computeEFF(), right->computeEFF());
	}
	const MFormula* computeIFF() const{
		return new MDisjunction(left->computeIFF(), right->computeIFF());
	}
	const MFormula* computeNFF() const{
		return new MDisjunction(left->computeNFF(), right->computeNFF());
	}
	
	string toString() const{
		return ("(" + left->toString() + " or " + right->toString() + ")");
	}
	
};

class PlLit : public MFormula {
public:
	const int number; /// id of the place literal
	const string namep;// name of the place
	const int itr;//iteration number
	std::string ft;
	/// there may be more literals for a place this is determined at runtime
	//public:
	PlLit(const int n,const string np,const int it) : number(n), namep(np), itr(it){
	ft = "LIT";
	}
	PlLit(const PlLit &formula): MFormula(formula), number(formula.number),namep(formula.namep), itr(formula.itr){}
	~PlLit(){}
	string toString() const{		
		//string result;
		//pair<string, int> here=toPl(number);
		stringstream ss;
		ss <<namep<<"^"<<itr;
        string s(ss.str());
		//result=here.first+"^"+s;cout<<result;*/
		return s;
	}
	const MFormula* computeEFF() const{
		return this;
	}	
	const MFormula* computeIFF() const{
		return this;
	}
	const MFormula* computeNFF() const{
		return this;
	}
	
};


class MNegation : public MFormula {
public:	
	const MFormula *subf;
	
	//public:	

	MNegation(const MFormula *ff) : subf(ff){
		assert(ff);
	}	
	MNegation(const MNegation *neg) : MFormula(*neg), subf(neg->subf){assert(neg);
	}
	~MNegation(){
		delete subf;
	}
	const MFormula* computeEFF() const{
		return new MNegation(subf->computeEFF());
	}
	const MFormula* computeIFF() const{
		return new MNegation(subf->computeIFF());
	}
	const MFormula* computeNFF() const{//dubla negatie returneaza phi
										 if (typeid(*subf) == typeid(MNegation)) { // neg neg \phi
										 const MNegation *negf=dynamic_cast<const MNegation *>(subf);
										 const MFormula *gf=negf->subf;
										 return gf->computeNFF();
										 }
										 else if(typeid(*subf) == typeid(MConjunction)){//conj
											 const MConjunction *cj=dynamic_cast<const MConjunction *>(subf);
											 const MNegation *nleft=new MNegation(cj->left); const MNegation *nright=new MNegation(cj->right);
											 MDisjunction *res=new MDisjunction(nleft->computeNFF(),nright->computeNFF());//dynamic_cast<const MNegation *>(cj->left),dynamic_cast<const MNegation *>(cj->right));
										 return res;
										 }
										 else if((typeid(*subf) == typeid(MDisjunction))) {
											 const MDisjunction *cj=dynamic_cast<const MDisjunction *>(subf);
											 const MNegation *nleft=new MNegation(cj->left); const MNegation *nright=new MNegation(cj->right);
											 MConjunction *res=new MConjunction(nleft->computeNFF(),nright->computeNFF());//dynamic_cast<const MNegation *>(cj->left),dynamic_cast<const MNegation *>(cj->right));
											 return res;			
										 }
										 else if(typeid(*subf) == typeid(PlLit)) {
											 return this;
										 }
	}

	string toString() const{
		return (" not (" + subf->toString() + ")");
	}
};





/// the workflow is that first the negation free proc is called before implication free procedure
class MImplication : public MFormula {
public:	
	const MFormula *left;
	const MFormula *right;
	
	
	MImplication(const MFormula *lleft, const MFormula *rright)  : left(lleft), right(rright){
		assert(lleft);
		assert(rright);
	}	
	~MImplication(){
		//delete left;
		//delete right;
	}
	const MFormula* computeEFF() const{
		//MImplication *mi=new MImplication(left->computeEFF(), right->computeEFF());
		return new MImplication(left->computeEFF(), right->computeEFF());
	}
	const MFormula* computeIFF() const{//Negation(const Formula *ff)
		//const Negation* hh=new Negation(left->computeIFF());
		//Negation* hh;
		//MDisjunction *md= new MDisjunction(new MNegation(left->computeIFF()), right->computeIFF());
		//cout<<md->toString()<<endl;
		return new MDisjunction(new MNegation(left->computeIFF()), right->computeIFF());
	}
	const MFormula* computeNFF() const{
		//do nothing 
	}
	
	string toString() const{
		return ("(" + left->toString() + " => " + right->toString() + ")");
	}
	
};

class Equivalence : public MFormula {
public:	
	const MFormula *left;
	const MFormula *right;
	std::string ft;

	Equivalence(const MFormula *lleft, const MFormula *rright) : left(lleft), right (rright){
		assert(lleft);
		assert(rright);
	}	
	~Equivalence(){
		//delete left;
		//delete right;
	}
	const MFormula* computeEFF() const{
		MConjunction mc= new MConjunction(new MImplication(left->computeEFF(), right->computeEFF()), 
					 new MImplication(right->computeEFF(), left->computeEFF()));
		return new MConjunction(new MImplication(left->computeEFF(), right->computeEFF()), 
						   new MImplication(right->computeEFF(), left->computeEFF()));
	}
	const MFormula* computeIFF() const{
		return this;//new Equivalence(left->computeIFF(), right->computeIFF());
	}
	const MFormula* computeNFF() const{
		return this;
		//do nothing because it is not necessary here
	}
	
	string toString() const{
		return ("(" + left->toString() + " <=> " + right->toString() + ")");
	}
	
};




//not needed but pay attention to 
class MFalse :public MFormula {
public:
	MFalse();
	~MFalse(){	}	
	const MFormula* computeIFF() const;
	const MFormula* computeNFF() const;
	string toString() const {return ("false"); }
};

class True :public MFormula {
public:
	True();
	~True(){}	
	const MFormula* computeIFF() const;
	const MFormula* computeNFF() const;
	string toString() const {return ("true"); }
};


const MFormula* distr(const MFormula * f1, const MFormula * f2);
const MFormula* NFF2CNF(const MFormula * f);
const MFormula * computeCNF(const MFormula *inf);
vector<vector<int> > CNF2MSF(const MFormula *fcnf);
int isSatisfiable(const MFormula *f);
extern  int minisat(vector<vector<int> > &);
//map<string, vector<int> > mp;

#endif